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
#ifndef OFFSETOVERLAY_H
#define OFFSETOVERLAY_H

#include <cv.hpp>
#include "overlay.h"
#include "veeventlistener.h"
#include "videosource.h"
#include "ve.h"
#include "glmacros.h"


/**
	Tries to correct camera artefacts by applying an offset correction 
	texture.
 
	@author Daniel Hahn,,,
*/
class OffsetOverlay : public Overlay, public VeEventListener {
public:

    /**
    	Creates the overlay and registers with right eye video source
    	for event handling.
    */
    OffsetOverlay(bool display);

    ~OffsetOverlay();

    void drawOverlay();

    /**
    	Recieve Events. This overlay recieves events from the keyboard,
    	and registeres itself with the @see VideoSource to get 
    	black offset updates. 
    */
    void recieveEvent(VeEvent &e);


private:
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
    /**
    	Indicates wether the offset/gain texture should
    	be displayed. This is independent of the general
    	"display" variable.
    */
    bool displayOffset, displayGain;

    /**
      Tries to create the offset textures from the pictures in the video
      source. The textures are immediately applied if successful.
      
      @return true if the creation of the textures was successful
    */
    bool createOffsetTextures();

    /// Draws the right quad of the display
    void drawRightQuad();
    /// Draws the left quad of the display
    void drawLeftQuad();

};

#endif
