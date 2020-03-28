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
    public:
        Vesa();
        virtual ~Vesa();

        // Initializes to closest video mode with given depth {1, 2, 4, 8, 16, 24 or 32}
        void setMode(int xres, int yres, int depth);

        // Draws a dot at the given location
        void dot(int x, int y, int color);

        // Selects the current graphics page (usually 64kB)
        void setPage(int page);
};

#endif /* APPS_VESA_VESA_H_ */
