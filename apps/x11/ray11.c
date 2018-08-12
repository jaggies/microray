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
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/FileSB.h>
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

#define MAXDEPTH 4 /* max number of reflected rays */
#ifdef PROFILE
    long intersections = 0;
#endif /* PROFILE */

XtAppContext app;
Widget topLevel;
Pixel cur_color;

// Remove these
Widget label;
static const String colors[] = { "black", "red", "green", "blue" };
char cur_bitmap[] = { 0 };
static char* MSG =
        "Use the FileSelection dialog to find bitmap files to\n"
        "display in the scrolling area in the main window.  Use\n"
        "the edit menu to display the bitmap in different colors.";

void edit_cb(Widget widget, XtPointer client_data, XtPointer call_data) {

}

void unmanage_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
    XtUnmanageChild(widget);
}

void file_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    static Widget dialog; /* make it static for reuse */
    extern void load_pixmap();
    int item_no = (int) client_data;

    if (item_no == 1) /* the "quit" item */
        exit(0);

    /* "Open" was selected.  Create a Motif FileSelectionDialog w/callback */
    if (!dialog) {
        dialog = XmCreateFileSelectionDialog(topLevel, "file_sel", NULL, 0);
        XtAddCallback(dialog, XmNokCallback, load_pixmap, NULL);
        XtAddCallback(dialog, XmNcancelCallback, unmanage_cb, NULL);
    }
    XtManageChild(dialog);
    XtPopup(XtParent(dialog), XtGrabNone);
}

void help_cb(widget, client_data, call_data)
    Widget widget;XtPointer client_data;XtPointer call_data; {
    static Widget dialog;

    if (!dialog) {
        Arg args[5];
        int n = 0;
        XmString msg = XmStringCreateLtoR(MSG, XmFONTLIST_DEFAULT_TAG);
        XtSetArg(args[n], XmNmessageString, msg);
        n++;
        dialog = XmCreateInformationDialog(topLevel, "help_dialog", args, n);
    }
    XtManageChild(dialog);
    XtPopup(XtParent(dialog), XtGrabNone);
}


void createHierarchy(XtAppContext app, Widget top) {

    Widget main_w = XtVaCreateManagedWidget("main_window", xmMainWindowWidgetClass, top,
            XmNscrollBarDisplayPolicy, XmAS_NEEDED,
            XmNscrollingPolicy, XmAUTOMATIC,
            NULL);

    XmString file = XmStringCreateLocalized("File");
    XmString edit = XmStringCreateLocalized("Edit");
    XmString help = XmStringCreateLocalized("Help");
    Widget menubar = XmVaCreateSimpleMenuBar(main_w, "menubar",
            XmVaCASCADEBUTTON, file, 'F',
            XmVaCASCADEBUTTON, edit, 'E',
            XmVaCASCADEBUTTON, help, 'H',
            NULL);
    XmStringFree(file);
    XmStringFree(edit);

    Widget widget = XtNameToWidget(menubar, "button_2");
    if (widget) {
        XtVaSetValues(menubar, XmNmenuHelpWidget, widget, NULL);
    }

    XmString open_ = XmStringCreateLocalized("Open...");
    XmString quit = XmStringCreateLocalized("Quit");
    XmVaCreateSimplePulldownMenu(menubar, "file_menu", 0, file_cb,
    XmVaPUSHBUTTON, open_, 'N', NULL, NULL,
    XmVaSEPARATOR,
    XmVaPUSHBUTTON, quit, 'Q', NULL, NULL,
    NULL);
    XmStringFree(open_);
    XmStringFree(quit);

    /* Second menu is the Edit menu -- callback is change_color() */
    XmString black = XmStringCreateLocalized(colors[0]);
    XmString red = XmStringCreateLocalized(colors[1]);
    XmString green = XmStringCreateLocalized(colors[2]);
    XmString blue = XmStringCreateLocalized(colors[3]);
    Widget menu = XmVaCreateSimplePulldownMenu(menubar, "edit_menu", 1, edit_cb,
    XmVaRADIOBUTTON, black, 'k', NULL, NULL,
    XmVaRADIOBUTTON, red, 'R', NULL, NULL,
    XmVaRADIOBUTTON, green, 'G', NULL, NULL,
    XmVaRADIOBUTTON, blue, 'B', NULL, NULL,
    XmNradioBehavior, True, /* RowColumn resources to enforce */
    XmNradioAlwaysOne, True, /* radio behavior in Menu */
    NULL);
    XmStringFree(black);
    XmStringFree(red);
    XmStringFree(green);
    XmStringFree(blue);

    if ((widget = XtNameToWidget(menu, "button_0"))) {
        XtVaSetValues(widget, XmNset, True, NULL);
    }

    XmVaCreateSimplePulldownMenu(menubar, "help_menu", 2, help_cb,
            XmVaPUSHBUTTON, help, 'H', NULL, NULL,
            NULL);
    XmStringFree(help); /* we're done with it; now we can free it */
    XtManageChild(menubar);

    /* set the label as the "work area" of the main window */
    Widget label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, main_w, NULL);
    XtVaSetValues(main_w,
            XmNmenuBar, menubar,
            XmNworkWindow, label,
            NULL);
    XtRealizeWidget(top);
}

void load_pixmap(dialog, client_data, call_data)
    Widget dialog;XtPointer client_data;XtPointer call_data; {
    char *file = NULL;
    XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct *) call_data;

    if (cbs) {
        if (!XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &file))
            return; /* internal error */
        (void) strcpy(cur_bitmap, file);
        XtFree(file); /* free allocated data from XmStringGetLtoR() */
    }

    Pixmap pixmap = XmGetPixmap(XtScreen(topLevel), cur_bitmap, cur_color,
            WhitePixelOfScreen(XtScreen(topLevel)));

    if (pixmap == XmUNSPECIFIED_PIXMAP) {
        printf("Can't create pixmap from %s\n", cur_bitmap);
    } else {
        Pixmap old;
        XtVaGetValues(label, XmNlabelPixmap, &old, NULL);
        XmDestroyPixmap(XtScreen(topLevel), old);
        XtVaSetValues(label,
                XmNlabelType, XmPIXMAP,
                XmNlabelPixmap, pixmap,
                NULL);
    }
}

void change_color(widget, client_data, call_data)
    Widget widget; /* menu item that was selected */
    XtPointer client_data; /* the index into the menu */
    XtPointer call_data; /* unused */
{
    XColor xcolor, unused;
    Display *dpy = XtDisplay(label);
    Colormap cmap = DefaultColormapOfScreen(XtScreen(label));
    int item_no = (int) client_data;

    if (XAllocNamedColor(dpy, cmap, colors[item_no], &xcolor, &unused) == 0
            || cur_color == xcolor.pixel)
        return;

    cur_color = xcolor.pixel;
    load_pixmap(widget, NULL, NULL);
}

static void renderImage(World* world, const char* outpath) {
    int h, w;
    NetPBM* pbm;
    float du = 1.0f / world->width, dv = 1.0f / world->height;
    float v = 0.0f + dv * 0.5f; /* emit ray from pixel centers */
    pbm = createNetPBM();

    world->depth = 255;
    if (!pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        printf("Can't write image '%s'\n", outpath);
        return;
    }

    printf("Rendering scene (%dx%d)\n", world->width, world->height);
    for (h = 0; h < world->height; h++, v += dv) {
        float u = 0.0f + du * 0.5f;
        printf("Line %d (%d%%)\n", h, 100 * h / world->height);
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
        }
    }
    pbm->close(pbm);
}

Boolean render_proc(XtPointer client_data)
{
    World* world = (World*) client_data;
    printf("Render\n");
    renderImage(world, "out.ppm"); // TODO: save as same <filename>.ppm
    return 1;
}

void startRender(const char* path) {
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
    XtAppAddWorkProc(app, render_proc, world);
}

int main(int argc, char **argv) {
    XtSetLanguageProc(NULL, NULL, NULL);
    topLevel = XtVaAppInitialize(&app, "MainWindow", NULL, 0, &argc, argv, NULL, NULL);
    createHierarchy(app, topLevel);

    if (argc > 1) {
        startRender(argv[1]);
    }

    #ifdef PROFILE
        printf("%ld intersections\n", intersections);
    #endif /* PROFILE */

    XtAppMainLoop(app);
    return 0;
}

