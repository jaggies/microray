#include <stdio.h>
#include "netpbm.h"

static void callback(void* clientData, int x, int y, unsigned char pixel[3]) {
    int* pixels = (int*) clientData;
    (*pixels)++;
}

int main(int argc, char** argv) {
    int width, height, depth, pixels;
    NetPBM* pbm = createNetPBM("out.ppm");
    if (argc < 2) {
        printf("Usage: %s <image.ppm>\n", argv[0]);
        return 0;
    }
    width = height = depth = 0;
    if (!pbm->open(pbm, argv[1], &width, &height, &depth, NETPBM_READ)) {
        printf("Can't open %s\n", argv[1]);
        return 0;
    }
    printf("NetPBM is %d x %d x %d\n", pbm->width, pbm->height, pbm->depth);
    pixels = 0;
    pbm->read(pbm, callback, &pixels);
    printf("Image contains %d pixels\n", pixels);
    return 1;
}
