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
#include "epixsource.h"

using namespace std;
using namespace ost;
using namespace log4cplus;

// Forward declarations
class EpixSource;

/**
Does the actual reading/copy operations for the EPIX card, to keep them out of the main update cycle.
 
@author Daniel Hahn,,,
*/
class EpixReaderThread : public Thread {
public:

    /**
      Creates a new EpixReaderThread.
      
      @param unit Number of the unit from which to read.
    @param source Pointer to the source to which this thread is connected.
    */
    EpixReaderThread(int unit, EpixSource* source);
    ~EpixReaderThread();

    void run();

    /**
      Requests the reader to stop after the current iteration.
    */
    void stop();

private:
    /// Picture buffer
    uchar* buffer;
    /// Unit which is read from
    int unit;
    /// buffer size for the image buffer
    int bufsize;
    /// indicates whether the thread is inside the main loop
    bool running;
    /// Logger for this class
    static Logger logger;
    /// Source to which this thread is connected
    EpixSource* source;
};

#endif
