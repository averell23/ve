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
#ifndef EPIXCAMERACONTROLLER_H
#define EPIXCAMERACONTROLLER_H

#include <log4cplus/logger.h>
#include "xclibcontroller.h"

using namespace log4cplus;

/**
Interface to control a camera that is connected to an EPIX framegrabber board. 
The behaviour of the methods will be determined by the camara model, they
have no effect in the default implementation.

@author Daniel Hahn,,,
*/
class EpixCameraController{
public:
    /**
      Creates a new camera controller. 
      
      @param unit The unit number of the EPIX board
                  to which the camera is connected.
    */
    EpixCameraController(int unit);

    ~EpixCameraController();
    
    /**
      Initializes the camera for use.
      
      @return true, if the camera was initialized successfully.
    */
    virtual bool initCamera();
    
    /**
      Sets the gain value for the camera.
      
      @param gain The gain value, in multiples of the normal gain.
      @return true if the gain was set successfully.
    */
    virtual bool setGain(float gain);

protected:
    /// Unit number of the board to which the camera is connected
    int unit;
    
private:
    /// Logger for this class
    static Logger logger;
};

#endif
