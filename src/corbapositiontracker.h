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
#include "veeventlistener.h"
#include "veeventsource.h"
#include "position.h"
#include "vepositionevent.h"
#include "ve.h"

/**
Recieves position updates through CORBA and forwards them to the display overlay.
 
@author Daniel Hahn,,,
*/
class CORBAPositionTracker : public VeEventListener, VeEventSource {
public:
    /// Source ID for the left eye
    static const int LEFT_EYE_SOURCE = 3;
    /// Source ID for the right eye
    static const int RIGHT_EYE_SOURCE = 4;

    CORBAPositionTracker();

    ~CORBAPositionTracker();

    /**
      Event handler for recieving Updates through CORBA.
    */
    void recieveEvent(VeEvent &e);

};

#endif
