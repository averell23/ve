/****************************************************************grunge
***********
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
#include "xclibcontroller.h"


bool XCLIBController::openState = false;
Mutex XCLIBController::cMutex;
Logger XCLIBController::logger = Logger::getInstance("Ve.XCLIBController");
void** XCLIBController::camLinkSerialRef = new void*[1024];

XCLIBController::XCLIBController()
{
}


XCLIBController::~XCLIBController()
{
}

int XCLIBController::openLib(string configFile) {
    LOG4CPLUS_DEBUG(logger, "Now opening epix library");
    if (openState == true) return 0;

    cMutex.enterMutex();
    char* driverParams = NULL;
    char* format = NULL;
	if (!strcmp(configFile.c_str(), "")) {
		LOG4CPLUS_WARN(logger, "No config file given, setting default config.");
		format = "default";
    }
	LOG4CPLUS_INFO(logger, "Opening XCLIB Library with: " << " Config File: " << configFile.c_str());
	int retVal = pxd_PIXCIopen(driverParams, format, (char*) configFile.c_str());
	if (retVal == 0) {
		openState = true;
		LOG4CPLUS_INFO(logger, "XCLIB Library opened successfully.");
	} else {
		LOG4CPLUS_ERROR(logger, "Could not open library, error code: " << pxd_mesgErrorCode(retVal));
	}
	
    cMutex.leaveMutex();
    return retVal;
}

int XCLIBController::closeLib() {
    cMutex.enterMutex();
    if (!openState) return 0;
    int result = pxd_PIXCIclose();
    if (result == 0) openState = false;
    return result;
    cMutex.leaveMutex();
}

bool XCLIBController::isOpen() {
    return openState;
}

int XCLIBController::goLive(int unit) {
    cMutex.enterMutex();
    int result = pxd_goLive(1<<unit, 1);
    if (result == 0) {
		LOG4CPLUS_DEBUG(logger, "Gone live successfully on unit " << unit
			<< ", state is " << pxd_goneLive(1<<unit, 0));
	} else {
		LOG4CPLUS_ERROR(logger, "Not gone live on unit " << unit
			<< ", error code: " << pxd_mesgErrorCode(result));
	}
    cMutex.leaveMutex();
    return result;
}

int XCLIBController::goUnLive(int unit) {
    int result = pxd_goUnLive(1<<unit);
    return result;
}

int XCLIBController::getBufferCopy(int unit, uchar* buffer, int bufsize) {
	cMutex.enterMutex();
	int result = pxd_readuchar(1<<unit, 1, 0, 0, pxd_imageXdim(), pxd_imageYdim(), buffer, bufsize, "RGB");
	cMutex.leaveMutex();
    
    return result;
}

bool XCLIBController::initCamLinkSerial(int unit) {
    #ifdef WIN32
    if ((unit < 0) || (unit >= 1024)) {
	LOG4CPLUS_WARN(logger, "Could not open camera link, illegal unit number: " << unit);
	return false;
    }
    int result = clSerialInit(unit, &camLinkSerialRef[unit]);
    if (result != 0) {
	LOG4CPLUS_WARN(logger, "Error while opening camera link serial, code: " 
		       << result);
	return false;
    } 
    return true;
    #else
    LOG4CPLUS_WARN(logger, "CamLink serial not available/only supported for MS Windows.");
    return false;
    #endif
}

string XCLIBController::writeCamLinkSerial(int unit, string message) {
    #ifdef WIN32
    if ((unit < 0) || (unit >= 1024)) {
		LOG4CPLUS_WARN(logger, "Could not open camera link, illegal unit number: " << unit);
		return false;
    }
    ulong size = message.size();
    ulong origSize = size;
    string retVal = "";
    
    clSerialWrite(camLinkSerialRef[unit], (char*) message.c_str(), &size , serial_timeout);
    if (size != origSize) {
		LOG4CPLUS_WARN(logger, "Not all characters could be written to serial out, written " << size << " of " << origSize);
		return "Internal: Could not write all characters.";
    }
    
    size = 1024;
    char* buffer = new char[1024];
    clSerialRead(camLinkSerialRef[unit], buffer, &size, serial_timeout);
	string tmp = buffer;
	while ((buffer[size - 1] == '\r') || (buffer[size - 1] == '\n')) size--; // Strip endlines from response
	retVal = tmp.substr(0, size);
    delete buffer;
    
    return retVal;
    #else
    LOG4CPLUS_WARN(logger, "CamLink serial not available/only supported for MS Windows.");
    return "";    
    #endif
}

bool XCLIBController::closeCamLinkSerial(int unit) {
    #ifdef WIN32
    if ((unit < 0) || (unit >= 1024)) {
	LOG4CPLUS_WARN(logger, "Could not open camera link, illegal unit number: " << unit);
	return false;
    }
    int result = clSerialClose(camLinkSerialRef[unit]);
    if (result != 0) {
	LOG4CPLUS_WARN(logger, "Error while closing camera link serial, code: " 
		       << result);
	return false;
    } 
    return true;
    #else
    LOG4CPLUS_WARN(logger, "CamLink serial not available/only supported for MS Windows.");
    return false;
    #endif
}
