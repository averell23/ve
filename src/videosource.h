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
#include <cc++/thread.h>
#include "stopwatch.h"
#include "cameracalibration.h"
#include "arregistration.h"
#include "ve.h"

// Forward declaration
class CameraCalibration;
class ARRegistration;
class Ve;

using namespace ost;

/**
	Wrapper class for a source of video images. A video source is expected
	to have an internal video buffer. The user may obtain a pointer to the
	buffer, but not change the contents. If a new image is acquired, the
	child implementation <b>must</b> increase the frameCount. 
	Callers must be able to use the frame count in order to check wether
	a new image has been acquired. 
	
	Timer support is only for performance measures, though. Child classes
	may ignore the timer, or update it differently from the internal frame
	count.

	<b>WARNING:</b> Do read the documentation carefully before attempting
	to create a child implementation!
	
	@author Daniel Hahn,,,
*/
class VideoSource {
public:
    /**
        Retrieve the next image for this video source. This returns
		a pointer to the current image buffer. The contents of the
		buffer must not be changed by the caller. Note that the 
		contents of the buffer are prone to being overwritten if 
		the internal buffer is not locked.
    */
	virtual IplImage *getImage() { return imgBuffer; }

    /*
		Locks the internal mutex. Child implementations are expected
		protect the internal buffer with the mutex, so that it is
		never changed when the lock is set. Callers are expected
		to release the image ASAP. If the mutex is already locked,
		calling this function has no effect.
	*/
	void lockImage();

	/**
		Releases the internal mutex. This signals to the class that the
		internal image buffer may now be overwritten.
	*/
	void releaseImage();


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
    Stopwatch* getTimer() {
        return timer;
    }

    VideoSource();

    /**
      Set the source's brightness. The behaviour of this method will
      depend on the underlying video source: Brightness may be changed
      through image manipulations, gain control, exposure time changes
      etc. 
      
      It is also possible that this method has no effect at all for some
      video sources, or has a different granularity for different sources.
      
      @param brightness The brightness in percent of the brightness range.
                        The internal brightness value should always be
    clamped to [0,100]. Subclasses can do this
    by calling VideoSource::setBrightness() first
    in their own setBrightness() function, and then
    use VideoSource::brightness as the brightness 
    value.
    */
    virtual void setBrightness(int brightness);

    /**
      Returns the camera calibration object connected to this source.
    */
    CameraCalibration* getCalibration() {
        return calibrationObject;
    }
    
	/**
		Gets the internal frame count. Child implementations <b>must</b> increase
		the frame count every time a new image is acquired.
	*/
	unsigned long getFrameCount() { return frameCount; }


	/** 
		Stores a "black offset" image that can be subtracted from the video image to correct 
		some cameras (like the Silicon Imaging 1280).
	*/
	void storeBlackOffset();

	/**
		Clears the internal "black offset" image. This may either result in the black offset image
		being cleared, or the image itself being set to NULL.
	*/
	void clearBlackOffset();

	/**
		Returns a pointer to the internal "black offset" image. 

		@return The black offset image, or NULL if the image has not been 
		        initialized.
	*/
	const IplImage* getBlackOffset() { return blackOffset; }

    /**
      Returns the sensor registration object connected to this source.
    */
    ARRegistration* getRegistration() { return registrationObject; }
    
    /**
    	Returns the current brightness setting.

    	@see setBrightness
    */
    virtual int getBrightness() {
        return brightness;
    }

    virtual ~VideoSource();

protected:
    /** Width and height of this video source */
    int width, height;
    /// Internal timer for performance measures.
    Stopwatch* timer;
    /// Current brightness setting
    int brightness;
    /// Internal calibration Object
    CameraCalibration* calibrationObject;
    /// Internal registration Object
    ARRegistration* registrationObject;
	/// Black offset image
	IplImage* blackOffset;
	/// Internal frame counter
	unsigned long frameCount;
	/// Internal image buffer
	IplImage* imgBuffer;
	/// Mutex for locking the buffer
    Mutex imgMutex;
	/// Mutex for temporary locking while updating @see imgLock
	Mutex tmpMutex;
	/// Indicates whether the image was locked by the user
	bool imgLock;
};

#endif
