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
#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <stdio.h>
#include <cv.hpp>
#include "stopwatch.h"

/**
Wrapper class for a source of video images.

@author Daniel Hahn,,,
*/
class VideoSource{
public:
    /** 
        Retrieve the next image for this video source. The class should
		expect the IplImage to be consumed (read: deleted) by the 
		subsequent processing, so it should not be expected to exist
		after getImage() was called.
		
		This should return NULL if no new image was created/acquired 
		since the last getImage() call, in order to avoid unecessary
		display updates.
		
		Child methods should update the internal timer whenever a 
		none-NULL image is returned.

		@see waitAndGetImage();
    */
    virtual IplImage *getImage() = 0;

	/**
		Works like getImage(), except that this function will block
		until a new image is available from the source. It's not 
		recommended to use this function for the regular display
		updates.

		Until this function is finished, getImage() should not block,
		but should always return  NULL.

		@see getImage()
	*/
	virtual IplImage *waitAndGetImage() = 0;


    /** Gets the image width for this video source */
    int getWidth();
    /** Gets the image height for this video source */
    int getHeight();
    /** Indicates whether the child class correctly updates the
        internal timer. */
    virtual bool timerSupported();
    
    /**
      Gets an handle to the internal timer. The timer will be stopped,
      so measurements taken after this called are no longer accurate.
      
      It's up to the child classes to actually start and update 
      the internal timer
    */
    Stopwatch* getAndStopTimer();

	/**
		Gets an handle to the internal timer.
	 */
	Stopwatch* getTimer() { return timer; }
    
    VideoSource();
    
    /**
      Set the source's brightness. The behaviour of this method will
      depend on the underlying video source: Brightness may be changed
      through image manipulations, gain control, exposure time changes
      etc. 
      
      It is also possible that this method has no effect at all for some
      video sources, or has a different granularity for different sources.
      
      @param brightness The brightness in percent of the brightness range.
    */
    virtual void setBrightness(int brightness) { }

    virtual ~VideoSource();

protected:
    /** Width and height of this video source */
    int width, height;
    /// Internal timer for performance measures.
    Stopwatch* timer;
};

#endif
