#include <stdio.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <intuition/screens.h>

struct IntuitionBase *IntuitionBase = 0;

int main() {
    struct NewScreen Screen1 = {
        0,0,640,200,4,             /* Screen of 640 x 480 of depth 8 (2^8 = 256 colours)    */
        DETAILPEN, BLOCKPEN,
        HIRES,                     /* see graphics/view.h for view modes */
        PUBLICSCREEN,              /* Screen types */
        NULL,                      /* Text attributes (use defaults) */
        "Pixel Screen", 
        NULL,
        NULL
    };

	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);

	if (!IntuitionBase) {
		printf("Can't open Intuition\n");
	}

	printf("Opening screen...\n");
    struct Screen *myScreen = OpenScreen(&Screen1); 
    Delay(500);
    if (myScreen) {
		printf("Success!!!  Closing...\n");
		CloseScreen((struct Screen*)&Screen1);
	}
} 
