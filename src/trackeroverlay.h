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
#ifndef TRACKEROVERLAY_H
#define TRACKEROVERLAY_H

#include "overlay.h"
#include "ve.h"
#include <log4cplus/logger.h> 	// Log4cplus
#include <cv.hpp> 		// OpenCv
// Include ARToolkit stuff
#include <AR/ar.h>

using namespace log4cplus;

/**
This allows to track patterns in the video image by means of the ARToolkit.

@author Daniel Hahn,,,
*/
class TrackerOverlay : public Overlay
{
public:

    static const int LEFT = 0;
    
    static const int RIGHT = 1;
    
    TrackerOverlay();
    
    ~TrackerOverlay();

    void drawOverlay();
    
    static char* paramFile; 
    
private:
    /**
	Acquire image data in ARToolkit-compliant format. (ABGR, where the RGB order
	isn't relevant, but the position of the alpha channel is.)
	
	@param leftOrRight Indicates wether the left or the right image should be
	                   captured. Possible values are RIGHT or LEFT
    */
    ARUint8* getImageData(int leftOrRight);
    /// List of markers found in one image
    ARMarkerInfo *markerInfo;
    /// Threshold for marker detection
    int thresh;
    /// Logger for this class
    static Logger logger;
    
};

#endif
