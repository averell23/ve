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
#ifndef EPIXSOURCE_H
#define EPIXSOURCE_H

#include "videosource.h"
#include "xclibcontroller.h"
#include "epixcameracontroller.h"
#include "sf1280controller.h"
#include "epixreaderthread.h"
#include "stopwatch.h"
#include <iostream>
#include <cc++/thread.h>
#include <log4cplus/logger.h>

using namespace std;
using namespace log4cplus;

/**
@author Daniel Hahn,,,
*/
class EpixSource : public VideoSource {
public:

    /**
      Creates a new EpixSource. FIXME: Error handling
      
      @param unit Number of the unit from which to read.
      @param configfile Name of an XCAP Video configuration file.
      @param cameraModel Model code of the camera connected to this source
    */
    EpixSource(int unit = 0, int cameraMode = CAMERA_DEFAULT, string configfile = "",
		       string adjust = "") ; //FIXME: Config file handling is stoopid

    virtual IplImage *getImage();

    virtual IplImage *waitAndGetImage();

    ~EpixSource();

    bool timerSupported();

    void setBrightness(int brightness);

    const static int CAMERA_DEFAULT = 0;
    const static int CAMERA_1280F = 1;

private:
    /// Unit number from which this source reads
    int unit;
    // The thread that reads from the frame grabber
    EpixReaderThread* readerThread;
    // Camera model
    int cameraModel;
    // Serial port handle for Camera Link connections
    void* serialRef;
    /// Logger for this class
    static Logger logger;
    /// Mutex for waitAndGetImage()
    Mutex tMutex;
    /// Camera controller for this source
    EpixCameraController* controller;


};

#endif
