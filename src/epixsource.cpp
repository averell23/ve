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
#include "epixsource.h"

Logger EpixSource::logger = Logger::getInstance("Ve.EpixSource");

EpixSource::EpixSource(int unit, int cameraModel, string configfile)
 : VideoSource()
{
    int result;
	if (!XCLIBController::isOpen()) {
		result = XCLIBController::openLib(configfile);
	}
    if (XCLIBController::isOpen()) {
	    height = pxd_imageYdim();
	    width = pxd_imageXdim();
    }
	EpixSource::cameraModel = cameraModel;
    EpixSource::unit = unit;
	cameraSetup();
    XCLIBController::goLive(unit);
    readerThread = new EpixReaderThread(unit);
    readerThread->start();
}


EpixSource::~EpixSource()
{
    readerThread->stop();
    XCLIBController::goUnLive(unit);
    #ifdef WIN32
    if (serialRef != NULL) clSerialClose(serialRef);
    #endif
}


IplImage* EpixSource::getImage() {
    IplImage* image;
    CvSize size;
    size.height = height;
    size.width = width;
    
    image = cvCreateImageHeader(size, IPL_DEPTH_16U, 3);
	uchar* buffer = readerThread->getBuffer();
    if (buffer != NULL) {
		image->imageData = (char*) buffer;
		timer->count();
    }

    return image;
}

bool EpixSource::timerSupported() {
	return true;
}

void EpixSource::cameraSetup() {
	{
		switch (cameraModel) {
			case CAMERA_DEFAULT:
				// No setup for unknow camera
			break;
			case CAMERA_1280F:
				// Setup for Silicon Imaging 1280 F
				// FIXME: Does not check board capabilities, clSerial only available for WIN32
				LOG4CPLUS_INFO(logger, "Setting up camera model SI-1280F unit " << unit);
                                #ifdef WIN32
				void* serialRef;
				char* buffer = new char[3];
				int result = clSerialInit(unit, &serialRef);
				if (result != 0) {
					LOG4CPLUS_WARN(logger, "Error while opening camera link serial, code: " 
						<< result);
				} else {
					ulong size = 7;
					clSerialWrite(serialRef, "ly804d\r", &size , 1000); // Gain
					size = 3;
					clSerialRead(serialRef, buffer, &size, 1000);
					if (strcmp(buffer, "104")) {
						LOG4CPLUS_WARN(logger, "Unable to get result code from camera.");
					}
					size = 9;
					clSerialWrite(serialRef, "lc36cb8f\r", &size, 1000); // Clock: 60 Mhz
				}
                                #else
				LOG4CPLUS_WARN(logger, "Warning: Unable to set camera parameters: Camera Link serial only available under Windows.");
                                #endif
			break;
		}
	}
}
