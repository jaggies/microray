/*
 * ray11.c
 *
 * A program to show ray progress using X11 while rendering.
 *
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <X11/Intrinsic.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/FileSB.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include "util.h"
#include "sphere.h"
#include "triangle.h"
#include "pointlight.h"
#include "hit.h"
#include "phongshader.h"
#include "checkerboardshader.h"
#include "perspectivecamera.h"
#include "world.h"
#include "raytrace.h"
#include "loader.h"
#include "netpbm.h"
#include "testload.h"
#include "dither.h"

#define MAXDEPTH 4 /* max number of reflected rays */
#define USE_ERROR_DIFFUSION
#define DITHER_IN_FULL_COLOR
#define DEFAULT_WIDTH 1
#define DEFAULT_HEIGHT 1
#define RBITS 3
#define GBITS 3
#define BBITS 2
#define RMASK (((1 << RBITS) - 1) << (8 - RBITS))
#define GMASK (((1 << GBITS) - 1) << (8 - GBITS))
#define BMASK (((1 << BBITS) - 1) << (8 - BBITS))

#ifdef PROFILE
    long intersections = 0;
#endif /* PROFILE */

XtAppContext app;
Widget topLevel;
Widget drawingArea;
Pixel cur_color;
GC gc;
Pixmap pixmap; // backing store for XmDrawingArea
Colormap cmap;

static char* ABOUT_MSG = "MicroRay (c) 2018 Jim Miller";
static char* HELP_MSG = "Help yourself.";
static void startRender(const char* path);
static const char *vic_name[] = { "StaticGray", "GrayScale", "StaticColor",
        "PseudoColor", "TrueColor", "DirectColor" };
static unsigned char pixelMap[256]; // mapping from pixels to cmap entries

void load_cb(Widget dialog, XtPointer client_data, XtPointer call_data)
{
    char *file = NULL;
    XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct *) call_data;

    if (cbs) {
        if (!XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &file)) {
            return; /* internal error */
        }
        startRender(file);
        XtFree(file); /* free allocated data from XmStringGetLtoR() */
    }
}

void expose_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
    short width, height;
    if (!pixmap) return;
    XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
    Window root;
    int x, y;
    unsigned int p_width, p_height, p_border, p_depth;
    Status status = XGetGeometry(XtDisplay(widget), pixmap, &root, &x, &y, &p_width, &p_height,
            &p_border, &p_depth);
    assert(status && width <= p_width && height <= p_height);
    XCopyArea(XtDisplay(widget), pixmap, XtWindow(widget), gc,
            0 /*srcx*/, 0 /*srcy*/, width, height, 0 /*dstx*/, 0 /*dsty*/);
}

void resize_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
    short width, height;
    assert(widget == drawingArea);
    XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
    /* create a pixmap the same size as the drawing area. */
    if (pixmap) {
        unsigned int p_width, p_height, p_border, p_depth;
        int x, y;
        Window root;
        Status status = XGetGeometry(XtDisplay(widget), pixmap, &root, &x, &y, &p_width, &p_height,
                &p_border, &p_depth);
        assert(status);
        if (p_width < width || p_height < height) {
            XmDestroyPixmap(XtScreen(widget), pixmap);
            pixmap = 0;
        }
    }
    if (!pixmap) {
        XWindowAttributes attr; /*= {0}; */
        XGetWindowAttributes(XtDisplay(widget), XtWindow(widget), &attr);
        printf("Allocating pixmap: %dx%d@%dbpp\n", width, height, attr.depth);
        pixmap = XCreatePixmap(XtDisplay(drawingArea), XtWindow(drawingArea), width+1, height+1, attr.depth);
        assert(pixmap);
        XSetForeground(XtDisplay(widget), gc, WhitePixelOfScreen(XtScreen(widget)));
        XFillRectangle (XtDisplay(widget), pixmap, gc, 0, 0, width, height);
    }
}

void edit_cb(Widget widget, XtPointer client_data, XtPointer call_data) {

}

void unmanage_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
    XtUnmanageChild(widget);
}

void file_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    static Widget dialog; /* make it static for reuse */
    int item_no = (int) client_data;

    if (item_no == 1) /* the "quit" item */
        exit(0);

    /* "Open" was selected.  Create a Motif FileSelectionDialog w/callback */
    if (!dialog) {
        dialog = XmCreateFileSelectionDialog(topLevel, "file_sel", NULL, 0);
        XtAddCallback(dialog, XmNokCallback, load_cb, NULL);
        XtAddCallback(dialog, XmNokCallback, unmanage_cb, NULL);
        XtAddCallback(dialog, XmNcancelCallback, unmanage_cb, NULL);
    }
    XtManageChild(dialog);
    XtPopup(XtParent(dialog), XtGrabNone);
}

void help_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget dialog;
    Arg args[5];
    int n = 0;
    XmString msg =  XmStringCreateLtoR(((int) client_data) == 0 ?
            HELP_MSG : ABOUT_MSG, XmFONTLIST_DEFAULT_TAG);
    XtSetArg(args[n], XmNmessageString, msg); n++;
    dialog = XmCreateInformationDialog(topLevel, "help_dialog", args, n);
    XtManageChild(dialog);
    XtPopup(XtParent(dialog), XtGrabNone);
    XmStringFree(msg);
}

int min_(int a, int b) { return a < b ? a : b; }
int max_(int a, int b) { return a > b ? a : b; }

int dither(int r, int g, int b, int x, int y, int depth) {
    #ifdef USE_ERROR_DIFFUSION
    static int rerr, berr, gerr;
    #endif
    switch(depth) {
        /* case theVisual->class == TrueColor || theVisual->class == DirectColor */
        case 24: {
            #ifdef DITHER_IN_FULL_COLOR
                #ifdef USE_ERROR_DIFFUSION
                int rx = diffusion_dither(1<<8, 1<<RBITS, r, &rerr);
                int gx = diffusion_dither(1<<8, 1<<GBITS, g, &gerr);
                int bx = diffusion_dither(1<<8, 1<<BBITS, b, &berr);
                #else
                int rx = ordered_dither(1<<8, 1<<RBITS, x, y, r);
                int gx = ordered_dither(1<<8, 1<<GBITS, x, y, g);
                int bx = ordered_dither(1<<8, 1<<BBITS, x, y, b);
                #endif
                return (rx << (24-RBITS)) | (gx << (16 - GBITS)) | (bx << (8 - BBITS));
            #else
                return (uint32_t) (r << 16) | (g << 8) | b;
            #endif
        } break;
        case 8: {
            int index;
            #ifdef USE_ERROR_DIFFUSION
            int rx = diffusion_dither(1<<8, 1<<RBITS, r, &rerr);
            int gx = diffusion_dither(1<<8, 1<<GBITS, g, &gerr);
            int bx = diffusion_dither(1<<8, 1<<BBITS, b, &berr);
            #else
            int rx = ordered_dither(1<<8, 1<<RBITS, x, y, r);
            int gx = ordered_dither(1<<8, 1<<GBITS, x, y, g);
            int bx = ordered_dither(1<<8, 1<<BBITS, x, y, b);
            #endif
            index = (rx << (GBITS + BBITS)) | (gx << BBITS) | bx;
            return pixelMap[index];
        }
        default:
            return 0; /* oops */
    }
}

void allocVisual(int rbits, int gbits, int bbits) {
    Display* dpy = XtDisplay(drawingArea);

    /* find a visual */
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(dpy, 0, 8, PseudoColor, &vinfo)) {
        fprintf(stderr, "Can't find 8-bit visual\n");
        return;
    }

    cmap = DefaultColormap(dpy, 0); /* XCreateColormap(dpy, XtWindow(topLevel), vinfo.visual, AllocNone); */
    assert(vinfo.depth == 8);

    printf("Rmask=%02x, Gmask=%02x, Bmask=%02x\n", RMASK, GMASK, BMASK);
    /* printf("Using 8-bit visual %ld, class=%s, depth=%d\n", vinfo.visualid, vic_name[vinfo.class], vinfo.depth); */

    int rlevels = 1 << rbits;
    int glevels = 1 << gbits;
    int blevels = 1 << bbits;
    for (int r = 0; r < rlevels; r++) {
        unsigned short red = 0xffff * r / (rlevels - 1);
        for (int g = 0; g < glevels; g++) {
            unsigned short green = 0xffff * g / (glevels - 1);
            for (int b = 0; b < blevels; b++) {
                unsigned short blue = 0xffff * b / (blevels - 1);
                XColor color;
                color.red = red;
                color.green = green;
                color.blue = blue;
                color.flags = DoRed | DoGreen | DoBlue;
                Status status = XAllocColor(dpy, cmap, &color);
                if (status) {
                    int index = (r << (GBITS + BBITS)) | (g << BBITS) | b;
                    assert(index < 256);
                    pixelMap[index] = color.pixel;
                    /* printf("Alloc (%d,%d,%d) as %08lx\n", red, green, blue, color.pixel); */
                } else {
                    printf("Couldn't allocate (%d,%d,%d) => index=%lu\n", r, g, b, color.pixel);
                }
            }
        }
    }
}

void createHierarchy(XtAppContext app, Widget top) {
    Widget main_w = XtVaCreateManagedWidget("main_window", xmMainWindowWidgetClass, top,
            XmNscrollBarDisplayPolicy, XmSTATIC /* XmAS_NEEDED */,
            XmNscrollingPolicy, XmSTATIC /* XmAUTOMATIC */,
	    #ifdef MOTIF2
            XmNscrolledWindowChildType, XmWORK_AREA,
	    #endif
            NULL);

    XmString file = XmStringCreateLocalized("File");
    XmString edit = XmStringCreateLocalized("Edit");
    XmString view = XmStringCreateLocalized("View");
    XmString help = XmStringCreateLocalized("Help");
    Widget menubar = XmVaCreateSimpleMenuBar(main_w, "menubar",
            XmVaCASCADEBUTTON, file, 'F',
            XmVaCASCADEBUTTON, edit, 'E',
            XmVaCASCADEBUTTON, view, 'V',
            XmVaCASCADEBUTTON, help, 'H',
            NULL);
    XmStringFree(file);
    XmStringFree(edit);
    XmStringFree(view);
    XmStringFree(help);

    Widget widget = XtNameToWidget(menubar, "button_3");
    if (widget) {
        XtVaSetValues(menubar, XmNmenuHelpWidget, widget, NULL);
    }

    XmString open_ = XmStringCreateLocalized("Open...");
    XmString quit = XmStringCreateLocalized("Quit");
    XmVaCreateSimplePulldownMenu(menubar, "file_menu", 0, file_cb,
            XmVaPUSHBUTTON, open_, 'O', NULL, NULL,
            XmVaSEPARATOR,
            XmVaPUSHBUTTON, quit, 'Q', NULL, NULL,
            NULL);
    XmStringFree(open_);
    XmStringFree(quit);

    /* Second menu is the Edit menu  */
    Widget menu = XmVaCreateSimplePulldownMenu(menubar, "edit_menu", 1, edit_cb, NULL);

    if ((widget = XtNameToWidget(menu, "button_0"))) {
        XtVaSetValues(widget, XmNset, True, NULL);
    }

    XmString help_item = XmStringCreateLocalized("Help...");
    XmString about_item = XmStringCreateLocalized("About...");
    XmVaCreateSimplePulldownMenu(menubar, "about_menu", 3, help_cb,
            XmVaPUSHBUTTON, help_item, 'H', NULL, NULL,
            XmVaPUSHBUTTON, about_item, 'A', NULL, NULL,
            NULL);

    XtManageChild(menubar);

    /* Create a ScrollArea as the "work area" of the main window */
    Widget scroll = XtVaCreateManagedWidget("scrolledWindow", xmScrolledWindowWidgetClass,
            main_w, NULL);

    /* Create a DrawingArea as the "work area" of the main window */
    drawingArea = XtVaCreateManagedWidget("drawingArea", xmDrawingAreaWidgetClass, scroll,
            XmNwidth, DEFAULT_WIDTH,
            XmNheight, DEFAULT_HEIGHT, NULL);
    XtAddCallback(drawingArea, XmNresizeCallback, resize_cb, NULL);
    XtAddCallback(drawingArea, XmNexposeCallback, expose_cb, NULL);

    XtVaSetValues(main_w,
            XmNmenuBar, menubar,
            XmNworkWindow, scroll,
            NULL);

    Screen* screen = XtScreen(top);
    Display* dpy = XtDisplay(top);
    XGCValues gcv; /* = {0};*/
    gcv.foreground = WhitePixelOfScreen(screen);
    gc = XCreateGC(dpy, RootWindowOfScreen(screen), GCForeground, &gcv);
}

static void renderImage(World* world, const char* outpath) {
    int h, w;
    NetPBM* pbm;
    float du = 1.0f / world->width, dv = 1.0f / world->height;
    float v = 0.0f + dv * 0.5f; /* emit ray from pixel centers */
    pbm = createNetPBM(outpath);

    world->depth = 255;
    if (!pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        printf("Can't write image '%s'\n", outpath);
        return;
    }

    printf("Rendering scene (%dx%d)\n", world->width, world->height);
    Display* dpy = XtDisplay(drawingArea);
    const int depth = DefaultDepthOfScreen(XtScreen(drawingArea));
    for (h = 0; h < world->height; h++, v += dv) {
        float u = 0.0f + du * 0.5f;
        /* printf("Line %d (%d%%)\n", h, 100 * h / world->height); */
        for (w = 0; w < world->width; w++, u += du) {
            Ray ray;
            Vec3 color;
            unsigned char rgb[3];
            world->camera->op->makeRay(world->camera, u, 1.0f - v, &ray);
            trace(&ray, 0 /* ignore */, world, &color, MAXDEPTH);
            rgb[0] = min(255, max(0, (int) round(color.x * 255)));
            rgb[1] = min(255, max(0, (int) round(color.y * 255)));
            rgb[2] = min(255, max(0, (int) round(color.z * 255)));
            pbm->write(pbm, rgb);
            XSetForeground(dpy, gc, dither(rgb[0], rgb[1], rgb[2], w, h, depth));
            XDrawPoint(dpy, pixmap, gc, w, h);
        }
	// Force an expose event
        #ifdef SHOW_PROGRESS
        XClearArea(XtDisplay(drawingArea), XtWindow(drawingArea), 0, 0, 1, 1, 1);
        while (XPending(dpy)) {
            XEvent event;
            XNextEvent(dpy, &event);
            XtDispatchEvent(&event);
        }
        #endif
    }
    /* Force an expose event */
    XClearArea(XtDisplay(drawingArea), XtWindow(drawingArea), 0, 0, 1, 1, 1);
    pbm->close(pbm);

    #ifdef PROFILE
        printf("%ld intersections\n", intersections);
        intersections = 0;
    #endif /* PROFILE */
}

Boolean render_proc(XtPointer client_data)
{
    World* world = (World*) client_data;
    printf("Render\n");
    renderImage(world, "out.ppm"); /* TODO: save as same <filename>.ppm */
    return 1;
}

static void startRender(const char* path) {
    printf("Loading %s\n", path);
    World* world = loadFile(path);
    if (!world) {
        printf("Failed to load %s\n", path);
        return;
    }
    if (world->nShapes == 0) {
        printf("World contains no shapes\n");
        return;
    }
    if (world->nLights == 0) {
        printf("World contains no lights\n");
        return;
    }
    if (!world->camera) {
        printf("World contains no camera\n");
        return;
    }
    XtVaSetValues(drawingArea,
            XmNwidth, world->width,
            XmNheight, world->height, NULL);

    XtAppAddWorkProc(app, render_proc, world);
}

int main(int argc, char **argv) {
    XtSetLanguageProc(NULL, NULL, NULL);
    topLevel = XtVaAppInitialize(&app, "MainWindow", NULL, 0, &argc, argv, NULL, NULL);
    createHierarchy(app, topLevel);
    XtRealizeWidget(topLevel);

    allocVisual(RBITS, GBITS, BBITS);

    if (argc > 1) {
        startRender(argv[1]);
    }

    XtAppMainLoop(app);
    return 0;
}

