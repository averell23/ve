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
#ifndef VEEVENTSOURCE_H
#define VEEVENTSOURCE_H

#include <cstdlib>
#include <log4cplus/logger.h>
#include "veeventlistener.h"

using namespace std;
using namespace log4cplus;

/**
Convenience class that can be used to post VeEvents to listeners.
 
@author Daniel Hahn,,,
*/
class VeEventSource {
public:
    /**
      Creates a new VeEventSource.
    */
    VeEventSource();

    /**
      Registers an event listener with this source. Each listener will 
      be notified of events from this source.
    */
    void addListener(VeEventListener *listener);

    /**
      Posts an event to all registerd listeners.
    */
    void postEvent(VeEvent &e);

private:
    /// List of registered listeners.
    vector<VeEventListener*> listeners;
    /// Logger for this class
    static Logger logger;

};

#endif
