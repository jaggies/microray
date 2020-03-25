/*
 * imagraph.h
 *
 *  Created on: Mar 14, 2020
 *      Author: jmiller
 */

#ifndef IMAGRAPH_H_
#define IMAGRAPH_H_

#define IO_BASE 0x2b0
#define MEM_BASE 0xe000 // segment register address. Real address is 0xe000:0

#define IO(offset) (IO_BASE+offset)

#define ACRTC_ADDR IO(0)
#define ACRTC_STATUS IO(0) /* { [7:0] = {err, areadet, cmdend, vsync, rfull, rrdy, wrdy, wempty } */
#define ACRTC_DATA IO(2)
#define MEMORY_CTL IO(4) /* [7:0] = {rsvd, rsvd, overlay_select, pagesel4:0} */
#define BOARD_CTL IO(6) /* write [7:0] = {HMAEN, rsvd6:1, vga_pass_through(0=enabled) }
                           read [7:0] = {HMAEN, *VERT, OVS, PS4:0} */
#define WRITE_MASK IO(8) /* 1 = result = data, 0 = result = previous contents */

#define DAC_ADDR IO(0xc)
#define DAC_PALLETTE IO(0xd)
#define DAC_CONTROL IO(0xe) /* [7:0] = {mpx(0=4:1, 1=5:1), ov_en, blink[2], blink_en, ol_en[4]} */
#define DAC_OVERLAY IO(0xf)

extern void palette(uint8_t entry, uint8_t r, uint8_t g, uint8_t b);
extern void hd_init();
extern void dac_init();
extern void board_init();

struct InitStruct {
    uint16_t ccr;
    uint16_t dmr;
    uint16_t dcr;
    uint16_t rcr;

    uint16_t hsr;
    uint16_t hdr;
    uint16_t vsr;
    uint16_t vdr;

    uint16_t ssw[3];
    uint16_t bcr;

    uint16_t hwr;
    uint16_t vwr[2];
    uint16_t gcr[3];

    uint16_t rar0;
    uint16_t mwr0;
    uint16_t sar0[2];

    uint16_t rar1;
    uint16_t mwr1;
    uint16_t sar1[2];

    uint16_t rar2;
    uint16_t mwr2;
    uint16_t sar2[2];

    uint16_t rar3;
    uint16_t mwr3;
    uint16_t sar3[2];

    uint16_t bcur1[2];
    uint16_t bcur2[2];
    uint16_t cdr;
    uint16_t zfr;

    uint16_t lpar[2];
};

#endif /* IMAGRAPH_H_ */
