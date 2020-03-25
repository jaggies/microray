/*
 * hd63484.h
 *
 *  Created on: Mar 14, 2020
 *      Author: jmiller
 */

#ifndef HD63484_H_
#define HD63484_H_

// Drawing parameter registers, page 113
enum { CL0= 0, CL1, CCMP, EDG, MASK, RPC, ADR, RWP, DP, CP };

// From page 135, HD63484 manual
#define ORG 0x0400 // set origin
#define WPR 0x0800 // wrfte parameter register
#define RPR 0x0c00 // read parameter register
#define WPTN 0x1800
#define RPTN 0x1c00
#define DRD 0x2400
#define DWT 0x2800
#define DMOD 0x2c00
#define RD 0x4400
#define WT 04800
#define MOD 0x4c00
#define CLR 0x5800
#define SCLR 0x5c00
#define CPY 0x6000
#define SCPY 0x7000
#define AMOVE 0x8000
#define RMOVE 0x8400
#define ALINE 0x8800
#define RLINE 0x8c00
#define ARCT 0x9000
#define RRCT 0x9400
#define APLL 0x9800
#define RPLL 0x9c00
#define APLG 0xa000
#define RPLG 0xa400
#define CRCL 0xa800
#define ELPS 0xac00
#define AARC 0xb000
#define RARC 0xb400
#define AEARC 0xb800
#define REARC 0xbc00
#define AFRCT 0xc000
#define RFRCT 0xc400
#define PAINT 0xc800
#define DOT 0xcc00
#define PTN 0xd000
#define AGCPY 0xe000
#define RGCPY 0xf000

// These represent 16-bit registers
#define HD_FIFO_ENTRY       0x00
#define HD_COMMMAND_CTRL    0x02
#define HD_OPERATION_MODE   0x04
#define HD_DISPLAY_CONTROL  0x06
#define HD_RASTER_COUNT     0x80
#define HD_HSYNC            0x82
#define HD_VSYNC            0x84
#define HD_V_DISPLAY        0x86
#define HD_SPLIT_SCREEN     0x88
#define HD_BLINK            0x90
#define HD_H_WINDOW            0x92
#define HD_V_WINDOW0           0x94
#define HD_V_WINDOW1           0x96
#define HD_GRAPHIC_CURSOR_X    0x98
#define HD_CURSOR_Y_START      0x9a
#define HD_CURSOR_Y_END        0x9c
#define HD_MEMORY_WIDTH_BASE   0xca
#define HD_MEMORY_WIDTH_WINDOW 0xda
#define HD_CURSOR_DEFINITION   0xe8
#define HD_ZOOM_FACTOR         0xea

// Returns 1 if write succeeds, 0 otherwise
extern uint16_t fifo_write(uint16_t data);

/* Returns 0 in status if nothing to read. Otherwise, data contains contents */
extern uint16_t fifo_read(uint16_t* status);

extern void origin(int16_t x, int16_t y);
extern void moveto(uint16_t x, uint16_t y);
extern void lineto(uint16_t x, uint16_t y);
extern void setParameter(uint8_t param, uint16_t color);
extern void clear(uint16_t color);
extern void circle(uint16_t radius);
extern void rectangle(uint16_t abx, uint16_t aby);
extern void dot(uint16_t x, uint16_t y, uint8_t color);

#endif /* HD63484_H_ */
