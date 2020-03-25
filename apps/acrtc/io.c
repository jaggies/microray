/*
 * io.cpp
 *
 *  Created on: Mar 14, 2020
 *      Author: jmiller
 */

#include "os.h"
#include "io.h"

void out8(uint16_t addr, uint8_t data) {
    #ifdef DOS
    asm {
        mov dx, addr
        mov al, data
	out dx, al
    }
    #endif
}

uint8_t in8(uint16_t addr) {
    uint8_t result = 0;
    #ifdef DOS
    asm {
        mov dx, addr
        in al, dx
	mov result, al
    }
    #endif
    return result;
}

void out16(uint16_t addr, uint16_t data) {
    //out8(addr, data >> 8);
    //out8(addr+1, data & 0xff);
    #ifdef DOS
    asm {
        mov dx, addr
        mov ax, data
        out dx, ax
    }
    #endif
}

uint16_t in16(uint16_t addr) {
    uint16_t result = 0;
    #ifdef DOS
    asm {
        mov dx, addr
        in ax, dx
	mov result, ax
    }
    #endif
    return result;
}

