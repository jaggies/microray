/*
 * Vesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DOS
#include <conio.h> // clrscr()
#endif
#include "vesa.h"

enum MemoryModel {
    TextMode = 0,
    CGAGraphics = 1,
    HerculesGraphics = 2,
    PlanePlanar4bpp = 3,
    PackedPixel = 4,
    NonChain4or256Color = 5,
    DirectColor = 6,
    YUV = 7
    // 0x08 - 0x0f Reserved, defined by VESA
    // 0x10 - 0xff To be defined by OEM
};

const char* modeToText[] = {
    "TextMode",
    "CGAGraphics",
    "HerculesGraphics",
    "PlanePlanar4bpp",
    "PackedPixel",
    "NonChain4or256Color",
    "DirectColor",
    "YUV"
};

Vesa::Vesa() : _rasterPage(-1), _raster((uint8_t*)(0xa0000000)), _dac8supported(false),
       _saveState(NULL) {
    if (!getVesaInfoBlock(&_vesaInfo)) {
        printf("VESA not supported!\n");
    } else {
        saveState();
    }
}

Vesa::~Vesa() {
    setPage(0);
    restoreState();
    delete [] _saveState;
    system("cls"); // hack since clrscr() doesn't work
}

Vesa::VesaInfoBlock* Vesa::getVesaInfoBlock(VesaInfoBlock *blockInfo) {
    int16_t status = 0;
    bzero(blockInfo, sizeof(VesaInfoBlock));
    #ifdef DOS
    uint16_t segment = uint32_t(blockInfo) >> 16;
    uint16_t address = uint32_t(blockInfo) & 0xffff;
    asm {
        mov ax, segment
        mov es, ax
        mov ax, 0x4f00 // get vesa block _vesaInfo
        mov di, address
        int 0x10
        mov status, ax
    }
    #endif
    return (status == 0x004f
            && strncmp("VESA", &blockInfo->signature[0], 4) == 0) ? blockInfo : nullptr;
}

Vesa::ModeInfoBlock* Vesa::getVesaModeInfo(uint16_t videoMode, ModeInfoBlock* modeInfo) const {
    int16_t status = 0;
    #ifdef DOS
    uint16_t segment = uint32_t(modeInfo) >> 16;
    uint16_t addr = uint32_t(modeInfo) & 0xffff;
    asm {
        mov ax, 0x4f01 // get mode _vesaInfo
        mov bx, segment
        mov es, bx
        mov cx, videoMode
        mov di, addr
        int 0x10
        mov status, ax
    }
    #endif
    // TODO: Check status!
    return modeInfo;
}

uint16_t Vesa::setMode(int xres, int yres, int depth) {
    const uint16_t* modes = _vesaInfo.modes;
    ModeInfoBlock tmp;
    uint16_t bestMode = 0; // undefined
    uint32_t bestScore = 0xffffffff;
    while (*modes != 0xffff) {
        uint16_t mode = *modes;
        if (getVesaModeInfo(mode, &tmp)) {
            if (tmp.bitsPerPixel == depth &&
                    tmp.horizontalResolution >= xres && tmp.verticalResolution >= yres) {
                uint16_t dx = abs(tmp.horizontalResolution - xres);
                uint16_t dy = abs(tmp.verticalResolution - yres);
                uint32_t score = (uint32_t) dx*dx + dy*dy;
                if (score < bestScore) {
                    bestMode = *modes;
                    bestScore = score;
                }
            }
        }
        modes++;
    }
    if (bestMode) {
        setMode(bestMode);
        _dac8supported = setDacWidth(8);
        printf("24-bit DAC is %s\n", _dac8supported ? "supported" : "not supported");
        getVesaModeInfo(bestMode, &_currentMode);
        _raster = (uint8_t*) ((uint32_t) _currentMode.windowAstartSegment << 16);
        printf("Selected mode %04x: ");
        dumpMode(&_currentMode);
    } else {
        printf("Failed to find a mode for %dx%d@%d\n", xres, yres, depth);
    }
    return bestMode;
}

void Vesa::setMode(uint16_t mode) {
    uint16_t status = 0;
    #ifdef DOS
    asm {
        mov ax, 0x4f02 // get mode _vesaInfo
        mov bx, mode
        int 0x10
        mov status, ax
    }
    #endif
}

void Vesa::setPage(uint16_t page) {
    // TODO: use far call instead of IRQ
    uint16_t status = 0;
    #ifdef DOS
    asm {
        mov ax, 0x4f05 // set
        mov bx, 0 // windowA = xx00, windowB = xx01
        mov dx, page
        int 0x10
        mov status, ax
    }
    #endif
}

void Vesa::palette(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
    if (!_dac8supported) {
        red = red >> 2;
        green = green >> 2;
        blue = blue >> 2;
    }
    #ifdef DOS
    asm {
        mov dx, 0x3c8
        mov al, index
        out dx, al
        inc dx
        mov al, red
        out dx, al
        mov al, green
        out dx, al
        mov al, blue
        out dx, al
    }
    #endif
}

bool Vesa::setDacWidth(uint8_t width) const {
    uint16_t status = 0;
    if (width == 8 && (_vesaInfo.capabilities & 0x01)) { // Switchable DAC
        #ifdef DOS
        asm {
           mov ax, 0x4f08 // get mode _vesaInfo
           mov bl, 0
           mov bh, width
           int 0x10
           mov status, ax
        }
        #endif
        return status == 0x004f; // status_ok
    } else {
        return width == 6; // standard VGA
    }
}

const uint16_t flags = 0x000f; // D3:0 = {SVGA, DAC, BIOS, Hardware}

void Vesa::saveState() {
    uint16_t status = 0;
    uint16_t blocks = 0; // number of 64-byte blocks

    delete [] _saveState;
    _saveState = NULL;

    #ifdef DOS
    asm {
        mov ax, 0x4f04 // save/restore state
        mov dl, 0 // get size
        mov cx, flags
        int 0x10
        mov status, ax
        mov blocks, bx
    }
    #endif

    if (blocks > 0 && status == 0x004f) {
        _saveState = new char[blocks*64];
        #ifdef DOS
        const uint16_t hi = (uint32_t) _saveState >> 16;
        const uint16_t lo = (uint32_t) _saveState & 0xffff;
        asm {
            mov ax, 0x4f04 // save/restore state
            mov dl, 0x01 // save state
            mov cx, flags
            mov es, hi
            mov bx, lo
            int 0x10
            mov status, ax
        }
        #endif
    } else {
        printf("Can't save SVGA state!\n");
    }
}

void Vesa::restoreState() {
    uint16_t status = 0;
    if (_saveState) {
        const uint16_t hi = (uint32_t) _saveState >> 16;
        const uint16_t lo = (uint32_t) _saveState & 0xffff;
        #ifdef DOS
        asm {
            mov ax, 0x4f04 // save/restore state
            mov dl, 0x02 // restore state
            mov cx, flags
            mov es, hi
            mov bx, lo
            int 0x10
            mov status, ax
        }
        #endif
    }
}

void Vesa::dump() const {
    uint16_t* modes = _vesaInfo.modes;
    printf("VESA card manufacturer='%s'\nversion=%04x\nmemory=%dkB\ncapabilities=%04x\n",
            _vesaInfo.oemString, _vesaInfo.version, _vesaInfo.totalMemory*64,
            _vesaInfo.capabilities);
    printf("Supported modes:\n");
    while (*modes != 0xffff) {
        dumpMode(*modes);
        modes++;
    }
}

void Vesa::dumpMode(uint16_t mode) const {
    ModeInfoBlock tmp;
    printf("Mode %04x: ", mode);
    dumpMode(getVesaModeInfo(mode, &tmp));
}

void Vesa::dumpMode(ModeInfoBlock* info) const {
    printf("(%s) %dx%d %d BPP, MASK=%d(%d),%d(%d),%d(%d), winA=%04x, winB=%04x\n",
            info->memoryModelType < Number(modeToText) ? modeToText[info->memoryModelType] : "unknown",
            info->horizontalResolution,
            info->verticalResolution,
            info->bitsPerPixel,
            info->sizeOfDirectColorRedMaskInBits,
            info->bitPositionOfLSBofRedMask,
            info->sizeOfDirectColorGreenMaskInBits,
            info->bitPositionOfLSBofGreenMask,
            info->sizeOfDirectColorBlueMaskInBits,
            info->bitPositionOfLSBofBlueMask,
            info->windowAstartSegment,
            info->windowBstartSegment);
    //info->windowFunction
}

