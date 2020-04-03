/*
 * raster.c
 *
 *  Created on: Apr 1, 2019
 *      Author: jmiller
 */

#include <stdlib.h> // abs()
#include <string.h>
#include "os.h"
#include "util.h" // min/max
#include "vesa.h"

void Vesa::moveTo(int16_t x, int16_t y) {
    _rasterX = x;
    _rasterY = y;
    _rasterOffset = (uint32_t)_rasterY * _currentMode.bytesPerScanLine + _rasterX;
}

void Vesa::clear() {
    moveTo(0,0);
    rectangle(_currentMode.horizontalResolution-1, _currentMode.verticalResolution-1, true);
}

void Vesa::color(uint8_t color) {
    _rasterColor = color; // TODO: Support other bit depths
}

void Vesa::dot() {
    uint16_t page = _rasterOffset >> 16;
    if (page != _rasterPage) {
        setPage(page);
        _rasterPage = page;
    }
    _raster[_rasterOffset & 0xffff] = _rasterColor; // TODO: handle other color depths
}

void Vesa::span(int16_t count) {
    if (_rasterY < 0) {
        return;
    }
    if (count < 0) {
        _rasterOffset += count;
        count = -count;
    }
    memset24(_rasterOffset, _rasterColor, count);
}

void Vesa::span(uint8_t* buffer, uint16_t n) {
    if (_rasterY < 0) {
        return;
    }
    memcpy24(_rasterOffset, buffer, n);
}

void Vesa::incX() {
    _rasterX++;
    _rasterOffset++;
}

void Vesa::decX() {
    _rasterX--;
    _rasterOffset--;
}

void Vesa::incY() {
    _rasterY++;
    _rasterOffset += _currentMode.bytesPerScanLine;
}

void Vesa::decY() {
    _rasterY--;
    _rasterOffset -= _currentMode.bytesPerScanLine;
}

void Vesa::lineTo(int16_t x1, int16_t y1) {
    // Always render in positive Y direction
    uint16_t x0 = _rasterX;
    uint16_t y0 = _rasterY;
    if (_rasterY > y1) {
        int16_t tmp = _rasterX; _rasterX = x1; x1 = tmp;
        tmp = _rasterY; _rasterY = y1; y1 = tmp;
    }
    const int16_t stepX = _rasterX < x1 ? 1 : -1;
    const int16_t stepY = _currentMode.bytesPerScanLine; // always incrementing
    const int16_t dx = abs(x1 - _rasterX);
    const int16_t dy = _rasterY - y1; // dy is always negative

    moveTo(_rasterX, _rasterY); // TODO: inline
    int count = max(dx, -dy);
    int16_t err = dx + dy;
    do {
        dot();
        int16_t e2 = err << 1;
        if (e2 < dx) {
            err += dx;
            _rasterY++;
            _rasterOffset += stepY; // inline rasterInc/DecY
        }
        if (e2 > dy) {
            err += dy;
            _rasterX += stepX;
            _rasterOffset += stepX;
        }
    } while (count--);
}

// Draw circle using the midpoint algorithm.
void Vesa::circle(uint16_t radius, bool fill) {
    if (radius <= 0) {
        moveTo(_rasterX, _rasterY);
        dot();
        return;
    }

    int16_t x = radius;
    int16_t y = 0;
    int16_t midpoint = 1 - radius;
    uint16_t x0 = _rasterX;
    uint16_t y0 = _rasterY;
    do {
        int16_t x2 = x0 - x;
        int16_t x4 = x0 - y;
        int16_t y1 = y + y0;
        int16_t y2 = y0 - y;
        int16_t y3 = x + y0;
        int16_t y4 = y0 - x;
        if (fill) {
            moveTo(x2, y1); span(x<<1);
            moveTo(x2, y2); span(x<<1);
            moveTo(x4, y3); span(y<<1);
            moveTo(x4, y4); span(y<<1);
        } else {
            int16_t x1 = x + x0;
            int16_t x3 = y + x0;
            moveTo(x1, y1); dot();
            moveTo(x2, y1); dot();
            moveTo(x1, y2); dot();
            moveTo(x2, y2); dot();
            moveTo(x3, y3); dot();
            moveTo(x4, y3); dot();
            moveTo(x3, y4); dot();
            moveTo(x4, y4); dot();
        }
        y++;
        if (midpoint <= 0) { // midpoint inside or on the perimeter
            midpoint = midpoint + (y << 1) + 1;
        } else { // midpoint outside the perimeter
            x--;
            midpoint = midpoint + ((y - x) << 1) + 1;
        }
    } while (x >= y);
}

void Vesa::rectangle(int16_t x1, int16_t y1, bool fill)
{
    uint16_t x0 = _rasterX;
    uint16_t y0 = _rasterY;
    if (fill) {
        // Draw left to right
        if (x0 > x1) {
            int16_t tmp = x0; x0 = x1; x1 = tmp;
        }
        // Draw top to bottom
        if (y0 > y1) {
            int16_t tmp = y0; y0 = y1; y1 = tmp;
        }
        int16_t dx = x1 - x0; // always positive
        do {
            moveTo(x0, y0);
            span(dx);
            y0++;
        } while (y0 < y1);
    } else { // Draw outline
        moveTo(x0, y0); lineTo(x1, y0);
        moveTo(x0, y1); lineTo(x1, y1);
        moveTo(x0, y0); lineTo(x0, y1);
        moveTo(x1, y0); lineTo(x1, y1);
    }
}

// Set maximum of 64k-1 bytes to any 24-bit page
void Vesa::memset24(uint32_t addr, uint8_t value, uint16_t length) {
    uint16_t page = (uint16_t) (addr >> 16);
    uint16_t offset = (uint16_t) (addr & 0xffff); // initial offset in page
    while (length > 0) {
        const uint16_t size = min(0x10000L - offset, length);
        setPage(page++);
        memset(_raster + offset, value, size); // TODO: optimize memset with 16-bit STD
        length -= size;
        offset = 0;
    }
}

// Set maximum of 64k-1 bytes to any 24-bit page
void Vesa::memcpy24(uint32_t addr, uint8_t* mem, uint16_t length) {
    uint16_t page = (uint16_t) (addr >> 16);
    uint16_t offset = (uint16_t) (addr & 0xffff); // initial offset in page
    while (length > 0) {
        const uint16_t size = min(0x10000L - offset, length);
        setPage(page++);
        memcpy(_raster + offset, mem, size); // TODO: optimize memset with 16-bit STD
        mem += size;
        length -= size;
        offset = 0;
    }
}

