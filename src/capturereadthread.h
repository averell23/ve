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
#ifndef CAPTUREREADTHREAD_H
#define CAPTUREREADTHREAD_H

#include <cc++/thread.h>
#include <log4cplus/logger.h>
#ifdef WIN32
#include <windows.h>
#endif
extern "C" {
    #include "xcliball.h"
}
#include "capturebuffer.h"
#include "capturecontroller.h"
#include "captureinfo.h"

using namespace ost;
using namespace log4cplus;

/**
Thread to read captured images from the frame buffer device.

@author Daniel Hahn,,,
*/
class CaptureReadThread : public Thread
{
public:

    /**
      Creates a new reading thread.
      
      @param buffer_a Buffers to store images from card a
      @param buffer_b Buffers to store images from card b
      @param info Meta-Information for capturing
    */
    CaptureReadThread(CaptureBuffer* buffer, CaptureInfo* info);
    
    ~CaptureReadThread();
    
    void run();

    /**
      Reads a pair of images to the buffers.
    */
    void readImages();

   int getCounter() { return counter; }

private:
    /// Meta information for capture
    CaptureInfo* info;
    /// Buffer to store images
    CaptureBuffer *buffer;
    /// Logger for this class
    static Logger logger;
    /// Indicates if the thread is running
    bool running;
    /// Image counter
    int counter;
};

#endif
