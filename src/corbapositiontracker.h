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
#ifndef CORBAPOSITIONTRACKER_H
#define CORBAPOSITIONTRACKER_H

#include <cv.hpp>
#include <log4cplus/logger.h>
#include "veeventlistener.h"
#include "veeventsource.h"
#include "position.h"
#include "vepositionevent.h"
#include "ve.h"

using namespace log4cplus;

/**
Recieves position updates through CORBA and forwards them to the display overlay.
 
@author Daniel Hahn,,,
*/
class CORBAPositionTracker : public VeEventListener, public VeEventSource {
public:

    /**
      Create a new tracker, using the given left and right source id
    */
    CORBAPositionTracker(int leftId = 3, int rightId = 4);

    ~CORBAPositionTracker();

    /**
      Event handler for recieving Updates through CORBA.
    */
    virtual void recieveEvent(VeEvent &e);
    
private:
    /// Source ID for the left eye
    int leftId;
    /// Source ID for the right eye
    int rightId;
    /// Logger for this class
    static Logger logger;
};

#endif
