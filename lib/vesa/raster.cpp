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
    _rasterOffset = (uint32_t)_rasterY * _currentMode.bytesPerScanLine + _rasterX*_stride;
}

void Vesa::clear() {
    moveTo(0,0);
    rectangle(_currentMode.horizontalResolution, _currentMode.verticalResolution, true);
}

void Vesa::color(uint32_t color) {
    _rasterColor = color; // TODO: Support other bit depths
}

uint32_t Vesa::color(uint8_t red, uint8_t green, uint8_t blue) {
    // TODO: Check if we're actually in direct color mode
    red >>= 8 - _currentMode.sizeOfDirectColorRedMaskInBits;
    green >>= 8 - _currentMode.sizeOfDirectColorGreenMaskInBits;
    blue >>= 8 - _currentMode.sizeOfDirectColorBlueMaskInBits;

    _rasterColor = (uint32_t) red << _currentMode.bitPositionOfLSBofRedMask;
    _rasterColor |= (uint32_t) green << _currentMode.bitPositionOfLSBofGreenMask;
    _rasterColor |= (uint32_t) blue << _currentMode.bitPositionOfLSBofBlueMask;
    return _rasterColor;
}

void Vesa::dot() {
    uint16_t page = _rasterOffset >> _pageShift;
    if (page != _rasterPage) {
        setPage(page);
        _rasterPage = page;
    }
    switch(_currentMode.bitsPerPixel) {
        case 8:
            *(uint8_t*) (_raster + (_rasterOffset & _pageMask)) = _rasterColor;
        break;
        case 15:
        case 16:
            *(uint16_t*) (_raster + (_rasterOffset & _pageMask)) = _rasterColor;
        break;
        case 24:
            *(RGB*) (_raster + (_rasterOffset & _pageMask)) = *(RGB*)&_rasterColor;
        break;
        case 32:
            *(uint32_t*) (_raster + (_rasterOffset & _pageMask)) = _rasterColor;
    }
}

void Vesa::span(int16_t count) {
    if (_rasterY < 0 || _rasterY > _currentMode.verticalResolution) {
        return;
    }
    if (count < 0) {
        _rasterOffset += count;
        count = -count;
    }
    switch(_currentMode.bitsPerPixel) {
        case 8:
            memset8(_rasterOffset, _rasterColor, count);
        break;

        case 15:
        case 16:
            memset16(_rasterOffset, _rasterColor, count);
        break;

        case 24: // TODO
            memset16(_rasterOffset, _rasterColor, count);
        break;

        case 32:
            memset32(_rasterOffset, _rasterColor, count);
        break;
    }
}

void Vesa::span(void* buffer, size_t elt, uint16_t n) {
    if (_rasterY < 0) {
        return;
    }
    memcpy8(_rasterOffset, (uint8_t*) buffer, elt*n);
}

void Vesa::incX() {
    _rasterX++;
    _rasterOffset += _stride;
}

void Vesa::decX() {
    _rasterX--;
    _rasterOffset -= _stride;
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
    _rasterX = x1 - 1; // Line doesn't include the final pixel
    _rasterY = y1 - 1;
    if (y0 > y1) {
        int16_t tmp = x0; x0 = x1; x1 = tmp;
        tmp = y0; y0 = y1; y1 = tmp;
        moveTo(x0, y0); // TODO: inline
    }
    const int16_t stepX = x0 < x1 ? _stride : -_stride;
    const int16_t stepY = _currentMode.bytesPerScanLine; // always incrementing
    const int16_t dx = abs(x1 - x0);
    const int16_t dy = y0 - y1; // dy is always negative

    int count = max(dx, -dy);
    int16_t err = dx + dy;
    do {
        dot();
        int16_t e2 = err << 1;
        if (e2 < dx) {
            err += dx;
            _rasterOffset += stepY; // inline rasterInc/DecY
        }
        if (e2 > dy) {
            err += dy;
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
        moveTo(x0, y0);
        do {
            dx > 1 ? span(dx) : dot();
            incY();
        } while (_rasterY < y1);
    } else { // Draw outline
        moveTo(x0, y0); lineTo(x1, y0);
        moveTo(x0, y1); lineTo(x1, y1);
        moveTo(x0, y0); lineTo(x0, y1);
        moveTo(x1, y0); lineTo(x1, y1);
    }
}

typedef struct _Edge {
    int16_t x; // Current X and Y
    int16_t y;
    int16_t stepX; // Amount to step (1 or -1)
    int16_t stepY;
    int16_t err; // current error
    int16_t dx; // total number of x pixels
    int16_t dy; // total number of y pixels
    uint16_t count;
} Edge;

// Creates an edge starting at v0 and ending at v1.
static void createEdge(const int16_t* v0, const int16_t* v1, Edge* edge) {
    edge->x = v0[X];
    edge->y = v0[Y];
    edge->stepX = v0[X] < v1[X] ? 1 : -1;
    edge->stepY = v0[Y] < v1[Y] ? 1 : -1;
    edge->dx = abs(v1[X] - v0[X]);
    edge->dy = abs(v1[Y] - v0[Y]);
    edge->count = max(edge->dx, edge->dy);
    edge->err = edge->dx - edge->dy;
}

// Walks an edge using Bresenham's algorithm. Returns remaining count when Y changes.
static bool walkEdge(Edge* edge) {
    bool ychanged = false;
    while (edge->count && !ychanged) {
        edge->count--;
        int16_t e2 = edge->err << 1;
        if (e2 < edge->dx) {
            edge->err += edge->dx;
            edge->y += edge->stepY;
            ychanged = true;
        }
        if (e2 > -edge->dy) {
            edge->err -= edge->dy;
            edge->x += edge->stepX;
        }
    }
    return ychanged;
}

void Vesa::triangle(size_t p0, size_t p1, size_t p2, const int16_t* vertices) {
    // Sort by Y coordinates.
    const int16_t* v0 = &vertices[p0<<1];
    const int16_t* v1 = &vertices[p1<<1];
    const int16_t* v2 = &vertices[p2<<1];
    if (v0[Y] > v1[Y]) {
        const int16_t* tmp = v0; v0 = v1; v1 = tmp;
    }
    if (v1[Y] > v2[Y]) {
        const int16_t* tmp = v1; v1 = v2; v2 = tmp;
        if (v0[Y] > v1[Y]) {
            tmp = v0; v0 = v1; v1 = tmp;
        }
    }

    Edge edge1, edge2; // The current two edges we're walking
    if (v0[Y] == v1[Y]) { // flat on the top (v2[Y] >= either, or degenerate)
        createEdge(v0, v2, &edge1);
        createEdge(v1, v2, &edge2);
    } else {
        createEdge(v0, v1, &edge1);
        createEdge(v0, v2, &edge2);
    }

    do {
        moveTo(edge1.x, edge1.y);
        span(edge2.x - edge1.x);
    } while (walkEdge(&edge1) && walkEdge(&edge2));

    // edge2 is longest because we sort vertices by Y, so walk additional segment if not finished
    if (edge2.count) {
        createEdge(v1, v2, &edge1);
        do {
            moveTo(edge1.x, edge1.y);
            span(edge2.x - edge1.x);
        } while (walkEdge(&edge1) && walkEdge(&edge2));
    }
}

// Set maximum of 64k-1 bytes to any 24-bit page
void Vesa::memset8(uint32_t addr, uint8_t value, uint16_t length) {
    uint16_t page = (uint16_t) (addr >> _pageShift);
    uint16_t offset = (uint16_t) (addr & _pageMask); // initial offset in page
    while (length > 0) {
        const uint16_t size = min((1L << _pageShift) - offset, length);
        setPage(page++);
        memset(_raster + (offset & _pageMask), value, size); // TODO: optimize memset with 16-bit STD
        length -= size;
        offset = 0;
    }
}

// Set maximum of 64k-1 bytes to any 24-bit page
void Vesa::memset16(uint32_t addr, uint16_t value, uint16_t length) {
    uint16_t page = (uint16_t) (addr >> _pageShift);
    uint16_t offset = (uint16_t) (addr & _pageMask); // initial offset in page
    while (length > 0) {
        const uint16_t size = min(((1L << _pageShift) - offset) / sizeof(uint16_t), length);
        setPage(page++);
        uint16_t* ptr = (uint16_t*) (_raster + (offset & _pageMask));
#ifdef DOS
        // memset with 16-bit values
        uint16_t hi = ((uint32_t) ptr) >> 16;
        uint16_t lo = ((uint16_t) ptr);
        asm {
            mov ax, value
            mov cx, size
            mov es, hi
            mov di, lo
            rep stosw
        }
#else
        uint16_t count = size;
        while(count--) {
            *ptr++ = value;
        }
#endif
        length -= size;
        offset = 0;
    }
}

// Set maximum of 64k-1 bytes to any 24-bit page
void Vesa::memset32(uint32_t addr, uint32_t value, uint16_t length) {
    uint16_t page = (uint16_t) (addr >> _pageShift);
    uint16_t offset = (uint16_t) (addr & _pageMask); // initial offset in page
    while (length > 0) {
        const uint16_t size = min(((1L << _pageShift) - offset) / sizeof(uint32_t), length);
        setPage(page++);
        uint32_t* ptr = (uint32_t*) (_raster + (offset & _pageMask));
#ifdef DOS32
        // memset with 16-bit values
        uint16_t hi = ((uint32_t) ptr) >> 16;
        uint16_t lo = ((uint16_t) ptr);
        asm {
            mov eax, value
            mov ecx, size
            mov es, hi
            mov di, lo
            rep stosd
        }
#else
        uint16_t count = size;
        while(count--) {
            *ptr++ = value;
        }
#endif
        length -= size;
        offset = 0;
    }
}


// Set maximum of 64k-1 bytes to any 24-bit page
void Vesa::memcpy8(uint32_t addr, uint8_t* mem, uint16_t length) {
    uint16_t page = (uint16_t) (addr >> _pageShift);
    uint16_t offset = (uint16_t) (addr & _pageMask); // initial offset in page
    while (length > 0) {
        const uint16_t size = min((1L << _pageShift) - offset, length);
        setPage(page++);
        memcpy(_raster + (offset & _pageMask), mem, size); // TODO: optimize memset with 16-bit STD
        mem += size;
        length -= size;
        offset = 0;
    }
}


