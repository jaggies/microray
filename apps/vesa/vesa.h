/*
 * Vesa.h
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#ifndef APPS_VESA_VESA_H_
#define APPS_VESA_VESA_H_

#include "os.h"

class Vesa {
    struct VesaInfoBlock;
    struct ModeInfoBlock;

    public:
        Vesa();
        virtual ~Vesa() { }

        // Initializes to closest video mode with given depth {1, 2, 4, 8, 16, 24 or 32}
        uint16_t setMode(int xres, int yres, int depth);

        // Draws a dot at the given location
        void dot(int x, int y, int color);

        // Selects the current graphics page (usually 64kB)
        void setPage(uint16_t page);

        // Dumps (prints) all vesa mode info
        void dump() const;

        // Dumps the given mode
        void dumpMode(uint16_t mode) const;

    private:
        VesaInfoBlock* getVesaInfoBlock(VesaInfoBlock *block);
        ModeInfoBlock* getVesaModeInfo(uint16_t mode, ModeInfoBlock* info) const;
        void setMode(uint16_t mode);
        void saveState();
        void restoreState();
        void dumpMode(ModeInfoBlock* mode) const;

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

        VesaInfoBlock _vesaInfo;
        ModeInfoBlock _currentMode;

};

#endif /* APPS_VESA_VESA_H_ */
