#ifndef EPIXREADERTHREAD_H
#define EPIXREADERTHREAD_H

#include <iostream>
#include "xclibcontroller.h"
#ifdef WIN32
#include <windows.h>
extern "C" {
    #include "xcliball.h"
}
#else
#include "xcliball.h"
#endif
#include <cc++/thread.h>
#include <log4cplus/logger.h>
#include "stopwatch.h"

using namespace std;
using namespace ost;
using namespace log4cplus;

/**
Does the actual reading/copy operations for the EPIX card, to keep them out of the main update cycle.

@author Daniel Hahn,,,
*/
class EpixReaderThread : public Thread
{
public:
    
    /**
      Creates a new EpixReaderThread.
      
      @param unit Number of the unit from which to read.
    */
    EpixReaderThread(int unit);
    ~EpixReaderThread();
    
    void run();
    
    /**
      Gets the image which is currently in the buffer.
    */
    uchar* getBuffer();
    
    /**
      Requests the reader to stop after the current iteration.
    */
    void stop();

private:
    /// Picture buffer
    uchar* buffer;
    /// Temporary buffer
    uchar* tmpBuffer;
    /// Unit which is read from
    int unit;
    /// Mutex for buffer access
    Mutex* tMutex;
    /// buffer size for the image buffer
    int bufsize;
    /// indicates whether the thread is inside the main loop
    bool running;
    /// indicates wether the current buffer is stale
    bool stale;
    /// Logger for this class
    static Logger logger;
};

#endif
