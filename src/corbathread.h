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
#ifndef CORBATHREAD_H
#define CORBATHREAD_H


#include <log4cplus/logger.h>
#include <cc++/thread.h>
#include "stubs/positionconnector.hh"
#include "stubs/positionconnector_impl.h"

using namespace ost;

/**
Simple thread class for the CORBAController, to work around a glitch in the WIN32 Common C++ implementation.

@author Daniel Hahn,,,
*/
class CORBAThread : public Thread {
public:

    CORBAThread(CORBA::ORB_var orb);
    
    void run();
    
    ~CORBAThread();
private:
    /// ORB handled by this thread
    CORBA::ORB_var myORB;
    /// Indicates whether the main loop is currently running
    bool running;
    /// Logger for this class
    static Logger logger;
    
};

#endif
