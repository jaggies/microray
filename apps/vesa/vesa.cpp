/*
 * Vesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <string.h>
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
    if (getVesaInfoBlock(&info)) {
        uint16_t* modes = info.modes;
        printf("VESA version=%04x, capabilities=%08x\n",
                info.signature, info.version, info.capabilities);
        while (*modes != 0xffff) {
            ModeInfoBlock modeInfo;
            getVesaModeInfo(*modes, &modeInfo);
            printf("Mode %04x: %dx%d %d BPP\n", *modes,
                    modeInfo.horizontalRsolution,
                    modeInfo.verticalResolution,
                    modeInfo.bitsPerPixel);
            modes++;
        }
    } else {
        printf("VESA not supported!\n");
    }
}

Vesa::~Vesa() {

}

VesaInfoBlock* Vesa::getVesaInfoBlock(VesaInfoBlock *blockInfo) {
    int16_t status = 0;
    bzero(blockInfo, sizeof(VesaInfoBlock));
    #ifdef DOS
    uint16_t segment = uint32_t(blockInfo) >> 16;
    uint16_t address = uint32_t(blockInfo) & 0xffff;
    asm {
        mov ax, segment
        mov es, ax
        mov ax, 0x4f00 // get vesa block info
        mov di, address
        int 0x10
        mov status, ax
    }
    #endif
    return (status == 0x004f
            && strncmp("VESA", &blockInfo->signature[0], 4) == 0) ? blockInfo : nullptr;
}

VesaInfoBlock* Vesa::getVesaModeInfo(uint16_t videoMode, ModeInfoBlock* modeInfo) {
    int16_t status = 0;
    #ifdef DOS
    uint16_t segment = uint32_t(modeInfo) >> 16;
    uint16_t addr = uint32_t(modeInfo) & 0xffff;
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
    return nullptr;
}

void Vesa::setMode(int xres, int yres, int depth) {

}

void Vesa::dot(int x, int y, int color) {

};

void Vesa::setPage(int page) {

}

