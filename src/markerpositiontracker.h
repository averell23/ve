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
#ifndef MARKERPOSITIONTRACKER_H
#define MARKERPOSITIONTRACKER_H

#include "veeventsource.h"
#include <cc++/thread.h>
#include <log4cplus/logger.h> 	// Log4cplus
#include <AR/ar.h>		// ARToolkit
#include "videosource.h"
#include "position.h"
#include "vepositionevent.h"

using namespace log4cplus;

/**
Tracks the position of ARToolkit markers in the image.
 
@author Daniel Hahn,,,
*/
class MarkerPositionTracker : public VeEventSource, public Thread {
public:

    /**
      Creates a new PositionTracker.
      
      @param source VideoSource in which the markers are detected.
      @param sourceID Source ID code that will be used for the 
                      @see Position objects.
    */
    MarkerPositionTracker(VideoSource* source, int sourceID);

    ~MarkerPositionTracker();

    /**
      Acquire image data in ARToolkit-compliant format. (ABGR, where the 
      RGB order isn't relevant, but the position of the alpha channel is.)
    */
    ARUint8* getImageData();

    /**
      Initialize the ARToolkit.
    */
    void initARToolkit();

    /// Thread runner method.
    void run();

private:
    /// Threshold for marker detection
    int thresh;
    /// Logger for this class
    static Logger logger;
    /// Video source on which markers are being detected
    VideoSource* source;
    /// Mutex to synchronize ARToolkit access. (ARToolkit re-uses data structures!)
    static Mutex mtx;
    /// Indicates wether the thread is running
    bool running;
    /** Source ID that will be supplied with the @see Position objects */
    int sourceID;

};

#endif
