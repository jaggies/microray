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
        enum MemoryModel {
            TextMode = 0,
            CGAGraphics = 1,
            HerculesGraphics = 2,
            PlanePlanar4bpp = 3,
            PackedPixel = 4,
            NonChain4or256Color = 5,
            DirectColor = 6,
            YUV = 7,
            // 0x08 - 0x0f Reserved, defined by VESA
            // 0x10 - 0xff To be defined by OEM
            Any = 0xff // Don't care, give me anything - use for enumerating all modes
        };

        Vesa();
        virtual ~Vesa();

        // Initializes to closest video mode with given depth {1, 2, 4, 8, 16, 24 or 32}
        uint16_t setMode(int xres, int yres, int depth = 8, MemoryModel model = Any);

        // Draws a dot at the given location with the current position and color.
        void dot();

        // Draws a circle at the current posit radius r
        void circle(uint16_t radius, bool fill);

        // Clears the sceen with the current color
        void clear();

        // Sets the current color in mode-dependent way (i.e. color indexed or RGB)
        void color(uint32_t color);

        // Chooses nearest color based on the memory model
        uint32_t color(uint8_t red, uint8_t green, uint8_t blue);

        // Draws a line from current position to (x1, y1) using the current color.
        void lineTo(int16_t x1, int16_t y1);

        // Getters!
        int getWidth() const { return _currentMode.horizontalResolution; }
        int getHeigth() const { return _currentMode.verticalResolution; }
        int getDepth() const { return _currentMode.bitsPerPixel; }
        MemoryModel getMemoryModel() const { return _currentMode.memoryModelType; }

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
        void span(void* buffer, size_t elt, uint16_t n);

        // Draws a triangle given indices into a buffer
        void triangle(size_t p0, size_t p1, size_t p2, const int16_t *vertices);

        uint16_t width() const { return _currentMode.horizontalResolution; }

        uint16_t height() const { return _currentMode.verticalResolution; }

        // Dumps (prints) all vesa mode info
        void dump() const;

        // Dumps the given mode
        void dumpMode(uint16_t mode) const;

    private:
        VesaInfoBlock* getVesaInfoBlock(VesaInfoBlock *block);
        ModeInfoBlock* getVesaModeInfo(uint16_t mode, ModeInfoBlock* info) const;
        uint16_t getVesaMode() const; // Get current VESA mode
        void setVesaMode(uint16_t mode);
        void saveState();
        void restoreState();
        //
        void memset8(uint32_t addr, uint8_t value, uint16_t length);
        void memset16(uint32_t addr, uint16_t value, uint16_t length);
        void memset32(uint32_t addr, uint32_t value, uint16_t length);
        void memcpy8(uint32_t addr, uint8_t* mem, uint16_t length);

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

        struct VgaSaveState {
            uint8_t mode;
            uint8_t page;
            uint8_t* _vesaState;
            VgaSaveState() : mode(0), page(0), _vesaState(NULL) { }
        };

        VesaInfoBlock _vesaInfo;
        ModeInfoBlock _currentMode;
        struct RGB {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };

        int16_t     _rasterX;
        int16_t     _rasterY;
        uint8_t     _stride; // 1, 2, 3 or 4 for depths 8, 16, 24 and 32
        uint32_t    _rasterColor;
        uint32_t    _rasterOffset; // Offset in bytes from start of frame buffer
        uint16_t    _rasterPage; // Cached copy of upper 16 bits of _rasterOffset
        uint8_t*     _raster; // usually 0xa000:0
        uint8_t     _dac8supported;
        uint8_t     _pageShift;
        uint16_t    _pageMask;
        VgaSaveState _save;
};

#endif /* APPS_VESA_VESA_H_ */
