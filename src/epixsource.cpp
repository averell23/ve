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

EpixSource::EpixSource(int unit, int cameraModel, string configfile, string adjust)
: VideoSource() {
    int result;
    if (!XCLIBController::isOpen()) {
        result = XCLIBController::openLib(configfile);
    }
    if (XCLIBController::isOpen()) {
        height = pxd_imageYdim();
        width = pxd_imageXdim();
    }
    EpixSource::cameraModel = cameraModel;
    switch (cameraModel) {
    case CAMERA_1280F:
        LOG4CPLUS_INFO(logger, "Trying to set up controller for SF1280 camera.");
        controller = new SF1280Controller(unit,adjust);
        break;
    case CAMERA_DEFAULT:
        LOG4CPLUS_INFO(logger, "Trying to set up default (dummy) camera controller.");
        controller = new EpixCameraController(unit);
        break;
    default:
        LOG4CPLUS_WARN(logger, "Unknown camera model. Trying default (dummy) camera controller");
        controller = new EpixCameraController(unit);
        break;
    }

	// Create the image buffer
    CvSize size;
    size.height = height;
    size.width = width;
    imgBuffer = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
	imgBuffer->imageData = NULL;

    controller->initCamera();
    EpixSource::unit = unit;
    XCLIBController::goLive(unit);
    readerThread = new EpixReaderThread(unit, this);
    timer->start();
    readerThread->start();
}


EpixSource::~EpixSource() {
    readerThread->stop();
    XCLIBController::goUnLive(unit);
	if (imgBuffer->imageData != NULL) delete imgBuffer->imageData;
	cvReleaseImageHeader(&imgBuffer);
    delete controller;
}


bool EpixSource::timerSupported() {
    return true;
}

void EpixSource::setBrightness(int brightness) {
    VideoSource::setBrightness(brightness);
    if (cameraModel == CAMERA_1280F) {
        controller->setGain(VideoSource::brightness);
    }
}

void EpixSource::bufferUpdate(char* newBuffer) {
	if (imgMutex.tryEnterMutex()) {
		if (imgBuffer->imageData != NULL) delete imgBuffer->imageData;
		imgBuffer->imageData = newBuffer;
		timer->count();
		frameCount++;
		imgMutex.leaveMutex();
	} else {
		delete newBuffer;
	}
}
