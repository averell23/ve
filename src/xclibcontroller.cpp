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
#include "xclibcontroller.h"


bool XCLIBController::openState = false;

XCLIBController::XCLIBController()
{
}


XCLIBController::~XCLIBController()
{
}

int XCLIBController::openLib(string configFile) {
    if (openState == true) return 0;
    
    char* driverParams = NULL;
    char* format = NULL;
	if (strcmp(configFile.c_str(), "")) {
		format = "default";
    }
    int retVal = pxd_PIXCIopen(driverParams, format, (char*) configFile.c_str());
	if (retVal == 0) {
		openState = true;
		cout << "Library opened successfully." << endl;
	} else {
		cout << "Could not open library, error code: " << pxd_mesgErrorCode(retVal)  << endl;
	}

    return retVal;
}

int XCLIBController::closeLib() {
    if (!openState) return 0;
    int result = pxd_PIXCIclose();
    if (result == 0) openState = false;
    return result;
}

bool XCLIBController::isOpen() {
    return openState;
}

int XCLIBController::goLive(int unit) {
    int result = pxd_goLive(1<<unit, 1);
		if (result == 0) {
		cout << "Gone live successfully, state is " << pxd_goneLive(1<<unit, 0) << endl;
	} else {
		cout << "Not gone live, error code: " << pxd_mesgErrorCode(result) << endl;
	}
    return result;
}

int XCLIBController::goUnLive(int unit) {
    int result = pxd_goUnLive(1<<unit);
    return result;
}

uchar* XCLIBController::getBufferCopy(int unit, int* result) {
	pxd_goSnap(1<<unit, 1); // FIXME: Go live!
    int bufsize = pxd_imageXdim() * pxd_imageYdim() * 3;

    uchar* buffer = new uchar[bufsize];
    *result = -1024;
    
    if (buffer) {
		*result = pxd_readuchar(1<<unit, 1, 0, 0, pxd_imageXdim(), pxd_imageYdim(), buffer, bufsize, "RGB");
    }
    
    return buffer;
}
