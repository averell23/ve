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
#ifndef CAPTUREWRITETHREAD_H
#define CAPTUREWRITETHREAD_H

#include <cc++/thread.h>
#include <log4cplus/logger.h>
#ifdef WIN32
#include <windows.h>
#endif
extern "C" {
    #include "xcliball.h"
}
#include "capturebuffer.h"
#include "captureinfo.h"

using namespace ost;
using namespace log4cplus;

/**
Thread that writes images from the capture buffer to disk

@author Daniel Hahn,,,
*/
class CaptureWriteThread : public Thread
{
public:
    /**
      Creates a new writing thread.
      
      @param buffer_a Buffers to read card a images from
      @param buffer_b Buffers to read card b images from
      @param info Meta-Information for capturing
    */
    CaptureWriteThread(CaptureBuffer* buffer_a, CaptureBuffer* buffer_b, CaptureInfo* info, Mutex *mutex);
    
    
    /**
      Writes a pair of images to disk.
    */
    void writeImages();
    
    /**
      Quit writing after the next pair of images and exit thread.
    */
    void quit();
    
    ~CaptureWriteThread();
    
    void run();

	int getCounter() { return counter; }
    
private:
    /// Meta information for capture
    CaptureInfo* info;
    /// Buffers to store images
    CaptureBuffer *buffer_a, *buffer_b;
    /// Logger for this class
    static Logger logger;
    /// Pointer to global Mutex FIXME: kludge
    Mutex* mutex;
    /// Counts the images writen until now
    int counter;
    /// Indicates if the thread is running
    bool running;

};

#endif
