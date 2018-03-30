#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <graphics/gfxbase.h>
#include <clib/graphics_protos.h>
#include <intuition/screens.h>
#include <stdio.h>
#include <stdlib.h>

/* Modifier bits */
#define HOLD(a) (a | 0x00)
#define MBLU(a) (a | 0x10)
#define MRED(a) (a | 0x20)
#define MGRN(a) (a | 0x30)

#define WIDTH 320
#define HEIGHT 200
#define DEPTH 6
#define COLORS 16

struct IntuitionBase * IntuitionBase;
struct GfxBase * GfxBase;

struct View view;
struct View* oldView;
struct ViewPort viewPort;
struct RasInfo rasInfo;
struct BitMap bitMap;
struct RastPort rastPort;

/* extern void SetAPen(struct RastPort*, int color); */

int colorTable[] = {
    0x000, 0x111, 0x222, 0x333,
    0x444, 0x555, 0x666, 0x777,
    0x888, 0x999, 0xaaa, 0xbbb,
    0xccc, 0xddd, 0xeee, 0xfff
};

void cleanUp(const char* msg);

int main(int argc, char** argv)
{
    int *colorPtr;;
    int i;

    IntuitionBase = (struct IntuitionBase*) OpenLibrary("intuition.library", 0);
    if (!IntuitionBase) {
        cleanUp("Could not open intuition library");
    }

    GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 0);
    if (!GfxBase) {
        cleanUp("Could not open the Graphics library");
    }

    oldView = GfxBase->ActiView;
    InitView(&view);
    view.ViewPort = &viewPort;

    InitVPort(&viewPort);
    viewPort.DWidth = WIDTH;
    viewPort.DHeight = HEIGHT;
    viewPort.RasInfo = &rasInfo; 
    viewPort.Modes = HAM; 

    viewPort.ColorMap = (struct ColorMap *) GetColorMap(COLORS);
    if(viewPort.ColorMap == NULL) {
        cleanUp( "Could not get a ColorMap" );
    }

    colorPtr = (int *) viewPort.ColorMap->ColorTable;
    for (i = 0; i < COLORS; i++) {
        *colorPtr++ = colorTable[i];
    }

    InitBitMap(&bitMap, DEPTH, WIDTH, HEIGHT);

    for (i = 0; i < DEPTH; i++) {
        bitMap.Planes[i] = (PLANEPTR) AllocRaster(WIDTH, HEIGHT);
        if (bitMap.Planes[i] == NULL) {
            cleanUp("Could not allocate raster");
        }
        /* BitClear(bitMap.Planes[i], RASSIZE(WIDTH, HEIGHT), 0); TODO */
    }

    rasInfo.BitMap = &bitMap;
    rasInfo.RxOffset = 0;
    rasInfo.RyOffset = 0;
    rasInfo.Next = NULL; /* single play field */

    MakeVPort(&view, &viewPort);
    MrgCop(&view);

    InitRastPort(&rastPort);
    rastPort.BitMap = &bitMap;

    LoadView(&view);

    SetDrMd(&rastPort, JAM1);

    /* Base color 0 */
    SetAPen(&rastPort, HOLD(0));
    RectFill(&rastPort, 10, 10, 30, 130);

    /* Red gradients */
    SetAPen(&rastPort, MRED(0x03));
    RectFill(&rastPort, 30, 10, 50, 50);
    
    SetAPen(&rastPort, MRED(0x07));
    RectFill(&rastPort, 50, 10, 70, 50);

    SetAPen(&rastPort, MRED(0x0b));
    RectFill(&rastPort, 70, 10, 90, 50);

    SetAPen(&rastPort, MRED(0x0d));
    RectFill(&rastPort, 90, 10, 110, 50);

    SetAPen(&rastPort, MRED(0x0f));
    RectFill(&rastPort, 110, 10, 130, 50);


    /* Blue gradients */
    SetAPen(&rastPort, MBLU(0x03));
    RectFill(&rastPort, 30, 50, 50, 90);
    
    SetAPen(&rastPort, MBLU(0x07));
    RectFill(&rastPort, 50, 50, 70, 90);

    SetAPen(&rastPort, MBLU(0x0b));
    RectFill(&rastPort, 70, 50, 90, 90);

    SetAPen(&rastPort, MBLU(0x0d));
    RectFill(&rastPort, 90, 50, 110, 90);

    SetAPen(&rastPort, MBLU(0x0f));
    RectFill(&rastPort, 110, 50, 130, 90);


    /* Green gradients */
    SetAPen(&rastPort, MGRN(0x03));
    RectFill(&rastPort, 30, 90, 50, 130);
    
    SetAPen(&rastPort, MGRN(0x07));
    RectFill(&rastPort, 50, 90, 70, 130);

    SetAPen(&rastPort, MGRN(0x0b));
    RectFill(&rastPort, 70, 90, 90, 130);

    SetAPen(&rastPort, MGRN(0x0d));
    RectFill(&rastPort, 90, 90, 110, 130);

    SetAPen(&rastPort, MGRN(0x0f));
    RectFill(&rastPort,110, 90, 130, 130);

    for (i = 0; i < COLORS; i++) {
        Delay(50);
        SetAPen(&rastPort, i);
        RectFill(&rastPort, 10, 10, 30, 130);
    }
    Delay(50);
}

void cleanUp(const char* msg) 
{
    int i;
    FreeVPortCopLists(&viewPort);
    FreeCprList(view.LOFCprList);
    for (i = 0; i < DEPTH; i++) {
        if (bitMap.Planes[i]) {
            FreeRaster(bitMap.Planes[i], WIDTH, HEIGHT);
        }
    }

    if (viewPort.ColorMap) {
        FreeColorMap(viewPort.ColorMap);
    }

    if (GfxBase) CloseLibrary((struct Library*) GfxBase);

    if (IntuitionBase) CloseLibrary((struct Library*) IntuitionBase);

    printf("%s\n", msg);
    exit(0);
}
