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
#ifndef CORBACONTROLLER_H
#define CORBACONTROLLER_H

#include <log4cplus/logger.h>
#include <cc++/thread.h>
#include "veeventsource.h"
#include "stubs/positionconnector.hh"
#include "stubs/positionconnector_impl.h"

using namespace log4cplus;
using namespace std;
using namespace ost;

/**
Static class that controls the operations of the CORBA subsystem.
This is a singleton class that will run the CORBA processing in
a separate thread once it is initialized.

@author Daniel Hahn,,,
*/
class CORBAController : public Thread {
public:
    
    /**
      Gets the singleton instance of this class.
    */
    static CORBAController& getInstance() { return myInstance; }
    
    /**
      Initializes the CORBA subsystem
    */
    void init(int argc, char** argv);
    
    /**
      Adds a listener for position events.
    */
    void addPositionEventListener(VeEventListener* listener);
    
    /**
      Indicates if the CORBA controller thread is running;
    */
    bool isRunning() { return running; }
    
    void run();
    
    ~CORBAController();
    
private:
    /// The CORBA ORB
    CORBA::ORB_var orb;
    /// Logger for this class
    static Logger logger;
    /// Internal instance that will run the CORBA worker thread
    static CORBAController myInstance;
    /// Private contructor for singleton class
    CORBAController();
    /// Indicates if the internal thread is running
    bool running;
    /// Event source/CORBA object for position events
    PositionConnector_Impl* positionSource;

};

#endif
