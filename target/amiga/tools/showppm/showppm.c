#include <stdio.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <intuition/screens.h>

struct IntuitionBase *IntuitionBase = 0;

int main() {
    struct NewScreen Screen1 = {
        0,0,320,200,6,             /* Screen of 640 x 480 of depth 8 (2^8 = 256 colours)    */
        DETAILPEN, BLOCKPEN,
        HAM,                     /* see graphics/view.h for view modes */
        CUSTOMSCREEN,              /* Screen types */
        NULL,                      /* Text attributes (use defaults) */
        "Pixel Screen", 
        NULL,
        NULL
    };

    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);

    ShowTitle((struct Screen*) &Screen1, FALSE);

    if (!IntuitionBase) {
        printf("Can't open Intuition\n");
    }

    struct Screen *myScreen = OpenScreen(&Screen1); 
    Delay(500);
    if (myScreen) {
        CloseScreen((struct Screen*)&Screen1);
    }

    if (IntuitionBase) {
        CloseLibrary((struct Library*) IntuitionBase);
    }
} 
