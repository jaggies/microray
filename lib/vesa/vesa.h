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

        // Draws a dot at the given location with the current position and color.
        void dot();

        // Draws a circle at the current posit radius r
        void circle(uint16_t radius, bool fill);

        // Clears the sceen with the current color
        void clear();

        // Sets the current color
        void color(uint8_t color);

        // Draws a line from current position to (x1, y1) using the current color.
        void lineTo(int16_t x1, int16_t y1);

        // Sets the current position
        void moveTo(int16_t x, int16_t y);

        // Sets the given palette to given red, green, blue.  If 18-bit DAC is supported,
        // only upper 6 MSBs have an effect for 262k total colors.
        void palette(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);

        // Draws a rectangle from the current position to the given position.
        void rectangle(int16_t x1, int16_t y1, bool fill);

        // Enabled 18 or 24-bit DAC mode. Choices are 6 or 8.
        bool setDacWidth(uint8_t width) const;

        // Fills n horizontal pixels with current color, starting with the current raster position.
        // A negative value for n means draw the other way (towards negative X).
        void span(int16_t n);

        // Fills horizontal line with pixels from buffer, starting with the current raster position.
        void span(uint8_t* buffer, uint16_t n);

        uint16_t width() const { return _currentMode.horizontalResolution; }

        uint16_t height() const { return _currentMode.verticalResolution; }

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
        // Like memset/memcpy, but copies to framebuffer memory with banking support
        void memset24(uint32_t addr, uint8_t value, uint16_t length);
        void memcpy24(uint32_t addr, uint8_t* mem, uint16_t length);

        // Move one pixel to the right
        void incX();

        // Move one pixel to the left
        void decX();

        // Move one pixel up
        void incY();

        // Move one pixel down
        void decY();

        // Selects the current graphics page (usually 64kB)
        void setPage(uint16_t page);

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
            uint16_t horizontalResolution;
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

        int16_t     _rasterX;
        int16_t     _rasterY;
        uint32_t    _rasterColor;
        uint32_t    _rasterOffset; // Offset in bytes from start of frame buffer
        uint16_t    _rasterPage; // Cached copy of upper 16 bits of _rasterOffset
        uint8_t*    _raster; // usually 0xa000:0
        uint8_t     _dac8supported;
};

#endif /* APPS_VESA_VESA_H_ */
