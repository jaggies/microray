/*
 * hd63484.c
 *
 *  Created on: Mar 14, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include "os.h"
#include "hd63484.h"
#include "imagraph.h"
#include "io.h"

#define AREA(a) ((a & 0x7) << 5)
#define COL(a) ((a & 0x3) << 3)
#define OPM(a) (a & 0x7)

// AREA modes
enum {
    AREA_NONE=0,
    AREA_EXIT, // drawing stops on exit area, ARD = CED = 1
    AREA_OUTSIDE, // drawing continues, but suppressed outside, ARD not set
    AREA_OUTSIDE_SET, // drawing continues, but suppressed outside, ARD is set
    AREA_NONE1, // same as 0
    AREA_ENTER, // drawing stops on enter area, ARD = CED = 1
    AREA_INSIDE, // drawing continues but suppressed inside, ARD not set
    AREA_INSIDE_SET // drawing continues, but suppressed outside
};

enum {
    STATUS_CER = 0x80, // Command error
    STATUS_ARD = 0x40, // Area detect
    STATUS_CED = 0x20, // Command end (1 = ready, 0 = command in progress
    STATUS_LPD = 0x10, // Light pen strobe detect
    STATUS_RFF = 0x08, // Read FIFO Full
    STATUS_RFR = 0x04, // Read FIFO Ready
    STATUS_WFR = 0x02, // Write FIFO Ready
    STATUS_WFE = 0x01  // Write FIFO Empty
};

// COL modes
enum {COL_REG=0, COL_0_SUP, COL_1_SUP, COL_DIRECT};

// OPM modes
enum {OPM_REPLACE=0, OPM_OR, OPM_AND, OPM_EOR, OPM_EQ, OPM_NE, OPM_LT, OPM_GT};


static const char *const instruction_name[64] =
{
    "undef","ORG  ","WPR  ","RPR  ",    /* 0x */
    "undef","undef","WPTN ","RPTN ",    /* 1x */
    "undef","DRD  ","DWT  ","DMOD ",    /* 2x */
    "undef","undef","undef","undef",    /* 3x */
    "undef","RD   ","WT   ","MOD  ",    /* 4x */
    "undef","undef","CLR  ","SCLR ",    /* 5x */
    "CPY  ","CPY  ","CPY  ","CPY  ",    /* 6x */
    "SCPY ","SCPY ","SCPY ","SCPY ",    /* 7x */
    "AMOVE","RMOVE","ALINE","RLINE",    /* 8x */
    "ARCT ","RRCT ","APLL ","RPLL ",    /* 9x */
    "APLG ","RPLG ","CRCL ","ELPS ",    /* Ax */
    "AARC ","RARC ","AEARC","REARC",    /* Bx */
    "AFRCT","RFRCT","PAINT","DOT  ",    /* Cx */
    "PTN  ","PTN  ","PTN  ","PTN  ",    /* Dx */
    "AGCPY","AGCPY","AGCPY","AGCPY",    /* Ex */
    "RGCPY","RGCPY","RGCPY","RGCPY"     /* Fx */
};

// Checks status register for common problems and prints something
void checkStatus() {
    uint8_t status = in8(ACRTC_STATUS);
    if (status & 0x80) {
        printf("Command error detected in status\n");
    }
    if (status & 0x40) {
        printf("Area detect flag is set\n");
    }
}

// Returns 1 if write succeeds, 0 otherwise
uint16_t fifo_write(uint16_t data) {
    int n = 1000;
    while(n-- && !(in8(ACRTC_STATUS) & STATUS_WFR)) // TODO: check for ABORT
        ;
    if (!n) {
        printf("Timeout writing FIFO\n");
    }
    out16(ACRTC_ADDR, HD_FIFO_ENTRY);
    out16(ACRTC_DATA, data);
    return 1;
}

/* Returns 0 in status if nothing to read. Otherwise, data contains contents */
uint16_t fifo_read(uint16_t* status) {
    uint16_t data = 0;
    if (in8(ACRTC_STATUS) & STATUS_RFR) {
        *status = 0;
    } else {
        out16(ACRTC_ADDR, HD_FIFO_ENTRY);
        data = in16(ACRTC_DATA);
        *status = 1;
    }
    return data;
}

void setParameter(uint8_t param, uint16_t value) {
    fifo_write(WPR | (param & 0x1f));
    fifo_write(value);
}

/* Drawing commands follow */
void origin(int16_t x, int16_t y) {
    fifo_write(ORG);
    fifo_write(x);
    fifo_write(y);
}

void moveto(uint16_t x, uint16_t y) {
    fifo_write(AMOVE);
    fifo_write(x);
    fifo_write(y);
}

void lineto(uint16_t x, uint16_t y) {
    fifo_write(ALINE | AREA(AREA_OUTSIDE) | COL(COL_REG) | OPM(OPM_REPLACE));
    fifo_write(x);
    fifo_write(y);
}

void circle(uint16_t radius) {
    fifo_write(CRCL | AREA(AREA_OUTSIDE) | COL(COL_REG) | OPM(OPM_REPLACE));
    fifo_write(radius);
}

void rectangle(uint16_t dx, uint16_t dy) {
    fifo_write(RFRCT | AREA(AREA_OUTSIDE) | COL(COL_REG) | OPM(OPM_REPLACE));
    fifo_write(dx);
    fifo_write(dy);
    // wait for it to complete
    while(!(in8(ACRTC_STATUS) & STATUS_CED)) // Wait for cmd end
        ;
}

void clear(uint16_t color) {
    fifo_write(CLR);
    fifo_write(color);
    fifo_write(1280);
    fifo_write(1024);
    // wait for it to complete
    while(!(in8(ACRTC_STATUS) & STATUS_CED)) // Wait for cmd end
        ;
}

void dot(uint16_t x, uint16_t y, uint8_t color) {
	uint16_t clr = ((uint16_t)(color) << 8) | color;
	setParameter(CL0, clr); 
	setParameter(CL1, clr);
	moveto(x, y);
	fifo_write(DOT);
}
