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
#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <sys/timeb.h>  	// Time buffer header
#include <stdlib.h>
#include <cstdlib>
#include <iostream>


/**
Quick and dirty stopwatch class for fps calculations.
  
@author Daniel Hahn,,,
*/
class Stopwatch{
public:
    
    /** Starts the timer. */
    void start();
    
    /** Stops the timer */
    void stop();
    
    /** Increases Frame count */
    void count(int number = 1);

	/** Resets the frame count */
	void reset();
    
    /** Get the elapsed time in full seconds **/
    long getSeconds();
    
    /** Get the elapsed time in milis 
        (from the last elapsed second) */
    long getMilis();
    
    /** Get the frame count */
    long getCount();
    
    /** Get the framerate */
    float getFramerate();

	/**
		Get a data rate for the time in which the Stopwatch has run.
		@param kbytes Kilobytes of data while the watch ran
		@return Throughput in MByte/s
	*/
	float getDatarate(long kbytes);
    
    Stopwatch();

    ~Stopwatch();
    
private:
    /** Timestamps */
    timeb startStamp, stopStamp;
    
    /** Indicates if the clock is running */
    bool running;
    
    /** Internal (frame) counter */
    long counter;

};

#endif
