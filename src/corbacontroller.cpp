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
#include "corbacontroller.h"

Logger CORBAController::logger = Logger::getInstance("Ve.CORBAController");
CORBAController CORBAController::myInstance;

CORBAController::CORBAController() : Thread() {
    positionSource = new PositionConnector_Impl();
}

void CORBAController::init(int argc, char** argv) {
    try {
        orb = CORBA::ORB_init(argc, argv);
        LOG4CPLUS_DEBUG(logger, "ORB init complete " << x);
        // Get root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
        // Print EOR FIXME: Using naming service instead
        obj = positionSource->_this();
        CORBA::String_var sior(orb->object_to_string(obj));
        cout << "IOR of position updater object: " << sior << endl;

        // Activate the POA
        PortableServer::POAManager_var pman = poa->the_POAManager();
        pman->activate();
    } catch (CORBA::SystemException& e) {
        LOG4CPLUS_ERROR(logger, "Caught CORBA::SystemException. CORBA init failed.");
        return;
    } catch (CORBA::Exception& e) {
        LOG4CPLUS_ERROR(logger, "Caught CORBA::Exception. CORBA init failed.");
        return;
	} 
		x = 17;
    LOG4CPLUS_INFO(logger, "CORBA initialized, starting CORBA handler.");
    theThread = new CORBAThread(orb);
    theThread->start();
	// CORBAController::start();
}

void CORBAController::addPositionEventListener(VeEventListener* listener) {
    positionSource->addListener(listener);
}

CORBAController::~CORBAController() {
    positionSource->_remove_ref();
    orb->destroy();
}

