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
    _rasterX = x1 - 1; // Line doesn't include the final pixel
    _rasterY = y1 - 1;
    if (y0 > y1) {
        int16_t tmp = x0; x0 = x1; x1 = tmp;
        tmp = y0; y0 = y1; y1 = tmp;
        moveTo(x0, y0); // TODO: inline
    }
    const int16_t stepX = x0 < x1 ? 1 : -1;
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

