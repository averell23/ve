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
#ifndef REGISTRATIONOVERLAY_H
#define REGISTRATIONOVERLAY_H

#include "overlay.h"
#include "veeventlistener.h"
#include "vepositionevent.h"
#include "arregistration.h"
#include "ve.h"
#include "fontmanager.h"
#include "glmacros.h"
#include <log4cplus/logger.h>
#include <cv.hpp>
#include <cc++/thread.h>

using namespace ost;
using namespace log4cplus;

/**
Allows the registration of screen coordinates to sensor coordinates recieved through CORBA.

@author Daniel Hahn,,,
*/
class RegistrationOverlay : public Overlay, public VeEventListener
{
public:
    /// Indicates calibration for the left eye is in progress
    static const int LEFT_EYE = 0;
    /// Indicates calibration for the right eye is in progress
    static const int RIGHT_EYE = 1;
    
    /**
      Creates a new registration overlay.
      @param registration The registration object used by this overlay
    */
    RegistrationOverlay(bool display);
    
    ~RegistrationOverlay();
    
    void drawOverlay();
    
    /// Recieves events
    void recieveEvent(VeEvent &e);
    
    /**
      Takes a full set of sensor updates (a set of size @see measureSize) and combines
      them into a sensor measurement. The result is added to the current eye's registration
      object.
      
      param x,y The coordinates of the image plane point that corresponds to the measurement.
    */
    void measurePoint(int x, int y);
    
private:
    /// Indicates if measure points are being collected
    bool measuring;
    /// Logger for this class
    static Logger logger;
    /// Temporary sensor points for smoothing out inaccuracies
    CvPoint3D32f* tmpSensorPoints;
    /// Number of sensor updates in a full set
    int measureSize;
    /// Number of sensor updates recieved since the measurement has started
    int measureCount;
    /// Indicates calibration mode (LEFT_EYE or RIGHT_EYE)
    int mode;
    /// Mutex to protect measurent updates
    Mutex mutex;
    /// The registration object used by this overlay
    ARRegistration* registration;
    /// Calibration points
    CvPoint* calibPoints;
    /// Number of calibration points
    int calibPointNum;
    /// Position in the calibration point list
    int calibPointPos;
    /// Buffers for text rendering
    char* text[1];
};

#endif
