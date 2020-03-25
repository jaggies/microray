/*
 * imagraph.cpp
 *
 *  Created on: Mar 14, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include "os.h"
#include "io.h"
#include "imagraph.h"
#include "hd63484.h"

// For initial palette
#define RBITS 3
#define GBITS 3
#define BBITS 2

static const uint8_t overlay_init[] = {
       0xff, 0x00, 0x00, // overlay color 0
       0x00, 0xff, 0x00, // overlay color 1
       0x00, 0x00, 0xff, // overlay color 2
       0x00, 0x00, 0x00  // overlay color 3
};

static const uint8_t cntrl_init[] = {
       0xff, 0x00, 0x40, 0x00 // read mask, blink mask, command, test
};

// Drawing parameter registers. DN = {upper(0), base(1), lower(2), window(3)}
static const uint16_t drawing_param_init[] = {
        0x0000, // Pr00 CL0
        0x0000, // Pr01 CL1
        0x0000, // Pr02 CCMP
        0x0000, // Pr03 EDG
        0xffff, // Pr04 MASK
        0x0000, // Pr05 {PPY, PZCY, PPX PZCX } - 1 nibble each
        0x0000, // Pr06 {PSY, NA, PSX, NA } - 1 nibble each
        0x0000, // Pr07 {PSE, PZY, PEX, PZX } - 1 nibble each
        0x0000, // Pr08 (XMIN)
        0x0000, // Pr09 (YMIN)
        0x0500, // Pr0a (XMAX) // TODO: update these with screen resolution
        0x0400, // Pr0b (YMAX) // 4k x 4k
        0x4000, // Pr0c read/write pointer { DN[15:14], NA[13:8], RWPH[7:0]}, 4-> base
        0x0000, // Pr0d read/write pointer { RWPL[15:4], NA }
        0x0000, // Pr0e <not used>
        0x0000, // Pr0f <not used>
        0x0000, // Pr10 Drawing pointer (RO) { DN[15:14], NA[13:8], DPAH }
        0x0000, // Pr11 Drawing pointer (RO) { DPAL[15:4], DPD[3:0] }
        0x0000, // Pr12 Current pointer (RO) X
        0x0000, // Pr13 Current pointer (RO) Y
        0x0000, // Pr14 <not used>
        0x0000, // Pr15 <not used>
};

// These should match number of items below
static const char* regNames[] = {
    "ccr",  "dmr",  "dcr",  "rcr",
    "hsr",  "hdr",  "vsr",  "vdr",
    "ssw0",  "ssw1", "ssw2",  "bcr",
    "hwr", "vwr0", "vwr1", "gcr0", "gcr1", "gcr2",
    "rar0",  "mwr0",  "sar0", "sar1",
    "rar1",  "mwr1",  "sar1a", "sar1b",
    "rar2",  "mwr2",  "sar2a", "sar2b",
    "rar3",  "mwr3",  "sar3a", "sar3b",
    "bcur1a", "bcur1b",  "bcur2a", "bcur2b", "cdr",  "zfr",
    "lpar0", "lpar1"
};

static const uint8_t regs[] = {
        0x2, 0x4, 0x6, 0x80,
        0x82, 0x84, 0x86, 0x88,
        0x8a, 0x8c, 0x8e, 0x90,
        0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c,
        0xc0, 0xc2, 0xc4, 0xc6,
        0xc8, 0xca, 0xcc, 0xce,
        0xd0, 0xd2, 0xd4, 0xd6,
        0xd8, 0xda, 0xdc, 0xde,
        0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xe,
        0xec, 0xee
};

static const uint16_t init1280[] = {
        0x0300, 0xc00c, 0xc300, 0x0000,
        0x3304, 0x0201, 0x0426, 0x2203,
        0x0400, 0x0000, 0x0000, 0x0000,
        0x0201, 0x0022, 0x0400, 0x320b, 0x0022, 0x0421,
        0x0000, 0x0400, 0x0000, 0x0000,
        0x0000, 0x0400, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0100, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0xa000, 0x0000,
        0x0000, 0x0000
};

static const uint16_t init1024[] = {
        0x0300, 0xc00c, 0xc300, 0x0000,
        0x2703, 0x0201, 0x0341, 0x2606,
        0x0300, 0x0000, 0x0000, 0x0000,
        0x0201, 0x0026, 0x0300, 0x2506, 0x0026, 0x0325,
        0x0000, 0x0400, 0x0000, 0x0000,
        0x0000, 0x0400, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0100, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0xa000, 0x0000,
        0x0000, 0x0000
};

void palette(uint8_t entry, uint8_t r, uint8_t g, uint8_t b) {
    out8(DAC_ADDR, entry);
    out8(DAC_PALLETTE, r);
    out8(DAC_PALLETTE, g);
    out8(DAC_PALLETTE, b);
}

void hd_init(const uint16_t *mode, int n) {
	uint16_t idx, data;
    // Abort current command and reset the HD63484.
    out16(ACRTC_ADDR, HD_COMMMAND_CTRL);
    out16(ACRTC_DATA, 0x8000);

    // Set up video mode
    for (idx = 0; idx < n; idx++) {
        // printf("%s = %04x\n", regNames[idx], mode[idx]);
        out16(ACRTC_ADDR, regs[idx]);
        out16(ACRTC_DATA, mode[idx]);

        // read it back for verification
        out16(ACRTC_ADDR, regs[idx]);
        data = in16(ACRTC_DATA);
        if (data != mode[idx]) {
            printf("Failed to initialize register %s, %04x != %04x\n", regNames[idx], mode[idx],
                    data);
        }
    }
}

void dac_init() {
	size_t i, j;
    const int16_t rlevels = 1 << RBITS;
    const int16_t glevels = 1 << GBITS;
    const int16_t blevels = 1 << BBITS;
    int16_t r, red, g, green, b, blue;
    uint8_t n = 0;

    out8(DAC_ADDR, 0x00); // auto increments for palette entries
    for (i = 0; i < Number(overlay_init); i++) {
        out8(DAC_CONTROL, overlay_init[i]);
    }

    for (j = 0; j < Number(cntrl_init); j++) {
        out8(DAC_ADDR, j+4); // these don't auto increment
        out8(DAC_CONTROL, cntrl_init[j]);
    }

	n = 0;
    for (r = 0; r < rlevels; r++) {
       red = 255 * r / (rlevels - 1);
       for (g = 0; g < glevels; g++) {
           green = 255 * g / (glevels - 1);
           for (b = 0; b < blevels; b++) {
               uint16_t blue = 255 * b / (blevels - 1);
               //printf("Palette %02x (%02x %02x %02x)\n", n, red, green, blue);
               palette(n++, red, green, blue);
           }
       }
    }
}

void pr_init() {
    uint16_t i;
    // Initialize graphics drawing parameter registers
    for (i = 0; i < Number(drawing_param_init); i++) {
        setParameter(i, drawing_param_init[i]);
    }
}

void pattern_init() {
	int i;
    fifo_write(WPTN | 0);
    fifo_write(16);
    for (i = 0; i < 16; i++) {
        fifo_write((i&1) ? 0xaaaa : 0x5555); // checkerboard pattern
    }
}

void board_init() {
    out8(MEMORY_CTL, 0); /* clear page select, overlay disabled */
    out8(BOARD_CTL, 0x81); /* enable host memory and display output */
    out8(WRITE_MASK, 0xff); /* all bit writes enabled */
    dac_init();
    hd_init(init1280, Number(init1280));
    pr_init();
    origin(0,0);
    pattern_init();
    clear(0);
}
