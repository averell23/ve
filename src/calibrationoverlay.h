/***************************************************************************
 *   Copyright (C) 2003 by Daniel Hahn,,,                                  *
 *   daniel@81pc04                                                         *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/
#ifndef CALIBRATIONOVERLAY_H
#define CALIBRATIONOVERLAY_H

#include "overlay.h"
#include "veevent.h"
#include "veeventlistener.h"
#include "fontmanager.h"
#include "cameracalibration.h"
#include "ve.h"
#include "glmacros.h"
#include <log4cplus/logger.h>

using namespace log4cplus;

/**
Provides an interface to the camera calibration routines.
 
@author Daniel Hahn,,,
*/
class CalibrationOverlay : public Overlay, public VeEventListener {
public:

    /// Creates a new overlay
    CalibrationOverlay(bool display);

    /// Destructor
    ~CalibrationOverlay();

    /// Recieves events
    void recieveEvent(VeEvent &e);

    void drawOverlay();

    static const int LEFT_EYE = 0;
    static const int RIGHT_EYE = 1;

private:
    /// Pointer to the font object
    FTGLTextureFont* font;
    /**
      Draws the picture for one eye.
    */
    void drawOneEye();
    /// Buffers for text rendering
    char* text[1];
    /// Calbibration mode: LEFT_EYE or RIGHT_EYE
    int calibrationMode;
    /// Logger for this class
    static Logger logger;
    /**
        Draws the image of the last snapshot to the
        other eye.
    */
    void drawOtherEye();
    /// Current calibration object
    CameraCalibration* cCalibrationObject;
    /** Texture for OpenGL */
    GLuint textures[1];
    /** The texture size used by this overlay. */
    int textureSize;    
    /// Draws the right quad of the display
    void drawRightQuad();
    /// Draws the left quad of the display
    void drawLeftQuad();
    /** Size Factors to scale the image to screen */
    double widthFactor, heightFactor;
    /** The width and height of the image to be displayed */
    int imageWidth, imageHeight;
    /// Sets picture-in-picture coordinates
    void setPiPCoordinates();


};

#endif
