/*
 * Vesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include "vesa.h"

struct VesaInfoBlock {
        char signature[4]; // == "VESA"
        uint16_t version;
        const char* oemString;
        uint32_t capabilities;
        uint16_t* modes;
        uint16_t totalMemory; // number of 64kB pages
        uint8_t reserved[236];
};

// Returned for mode specified from list of modes returned by function 0
struct ModeInfoBlock {
    // Mandatory information
    uint16_t modeAttributes;
    uint8_t windowAattributes;
    uint8_t windowBattributes;
    uint16_t windowGranularity;
    uint16_t windowSize;
    uint16_t windowAstartSegment;
    uint16_t windowBstartSegment;
    void (*windowFunction)(uint16_t page);
    uint16_t bytesPerScanLine;

    // Formerly optional information (now mandatory)
    uint16_t horizontalRsolution;
    uint16_t verticalResolution;
    uint8_t characterCellWidth;
    uint8_t characterCellHeight;
    uint8_t numberOfMemoryPlanes;
    uint8_t bitsPerPixel;
    uint8_t numberOfBanks;
    uint8_t memoryModelType;
    uint8_t bankSizeInKb;
    uint8_t numberOfImages;
    uint8_t reservedForPageFunction;

    // new Direct Color fields
    uint8_t sizeOfDirectColorRedMaskInBits;
    uint8_t bitPositionOfLSBofRedMask;
    uint8_t sizeOfDirectColorGreenMaskInBits;
    uint8_t bitPositionOfLSBofGreenMask;
    uint8_t sizeOfDirectColorBlueMaskInBits;
    uint8_t bitPositionOfLSBofBlueMask;
    uint8_t sizeOfDirectColorReservedMaskInBits;
    uint8_t directColorModeAttributes;

    uint8_t reserved[216];
};

Vesa::Vesa() {
    VesaInfoBlock info;
    int16_t status;
    #ifdef DOS
    uint16_t segment = uint32_t(&info) >> 16;
    uint16_t addr = uint32_t(&info) & 0xffff;
    asm {
        mov ax, segment
        mov es, ax
        mov ax, 0x4f00 // get info
        mov di, addr
        int 0x10
        mov status, ax
    }
    #endif
    if (status == 0x004f) {
        ModeInfoBlock modeInfo;
        printf("VESA = '%s', version=%04x, status = %d, capabilities=%08x\n",
                info.signature, info.version, status, info.capabilities);
        uint16_t* ptr = info.modes;
        while (*ptr != 0xffff) {
            #ifdef DOS
            uint16_t segment = uint32_t(&modeInfo) >> 16;
            uint16_t addr = uint32_t(&modeInfo) & 0xffff;
            uint16_t videoMode = *ptr;
            asm {
                mov ax, 0x4f01 // get mode info
                mov bx, segment
                mov es, bx
                mov cx, videoMode
                mov di, addr
                int 0x10
                mov status, ax
            }
            #endif
            printf("Mode %04x: %dx%d %dbpp\n", *ptr,
                    modeInfo.horizontalRsolution,
                    modeInfo.verticalResolution,
                    modeInfo.bitsPerPixel);
            ptr++;
        }
    } else {
        printf("VESA not supported, status=%04x\n", status);
    }
}

Vesa::~Vesa() {

}

void Vesa::setMode(int xres, int yres, int depth) {

}

void Vesa::dot(int x, int y, int color) {

};

void Vesa::setPage(int page) {

}

