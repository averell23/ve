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
#ifndef VIDEOCANVAS_H
#define VIDEOCANVAS_H

#include "videosource.h"
#include "xclibcontroller.h" // FIXME: only for ushort

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glew.h>		// OpenGL headers
#include <GL/glut.h>		// GLUT headers
#include <stdio.h>		// STD input/output headers
#include <iostream>
#include <log4cplus/logger.h>
#include <cv.hpp>
#include "veeventlistener.h"
#include "captureinfo.h"
#include "captureimagepair.h"
#include "capturecontroller.h"
#include "ve.h"

using namespace std;
using namespace log4cplus;

// Forward declarations
class Ve;

/**
A canvas for displaying a stereo video feed. This retrieves a picture for the
left and the right eye and displays them in their respective half of the 
screen.
 
@author Daniel Hahn,,,
*/
class VideoCanvas : public VeEventListener {
public:
    /** Creates a new video canvas with the given sources for the
        right and left eye. **/
    VideoCanvas(VideoSource *left, VideoSource *right, bool xRot = false, bool yRot = false, bool zRot = false);

    /** Draws the Video Canvas by uploading the texture images */
    void draw();

    ~VideoCanvas();

    void recieveEvent(VeEvent &e);

    /// To rotate 180 degrees around the x Axis
    bool xRot;
    /// To rotate 180 degrees around the y Axis
    bool yRot;
    /// To rotate 180 degrees around the z Axis
    bool zRot;

protected:
    /** The width and height of the image to be displayed */
    int imageWidth, imageHeight;
    /** The video sources for this canvas */
    VideoSource *leftEye, *rightEye;
    /** The texture size used by this canvas. This may be large
        than the image size */
    int textureSize;
    /** Textures for OpenGL */
    GLuint textures[2];
    /** Size Factors to scale the image to screen */
    double widthFactor, heightFactor;
    /// Logger for this class
    static Logger logger;
    /// Brightness setting for the sources
    int leftBrightness, rightBrightness;
	/// FIXME: Counter for quick image save hack
	int imagesSaved;

};

#endif
