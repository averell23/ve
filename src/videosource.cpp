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
#include "videosource.h"

VideoSource::VideoSource() {
    calibrationObject = new CameraCalibration(this);
    registrationObject = new ARRegistration(this);
    timer = new Stopwatch();
	blackOffset = NULL;
    timer->start();
	imgLock = false;
}


VideoSource::~VideoSource() {
    delete timer;
}

int VideoSource::getWidth() {
    return width;
}

int VideoSource::getHeight() {
    return height;
}

Stopwatch* VideoSource::getAndStopTimer() {
    timer->stop();
    return timer;
}

bool VideoSource::timerSupported() {
    return false;
}

void VideoSource::setBrightness(int brightness) {
    if (brightness > 100) {
        brightness = 100;
    } else if (brightness < 0) {
        brightness = 0;
    }
    VideoSource::brightness = brightness;
}

void VideoSource::storeBlackOffset() {
	lockImage();
	IplImage* currentImage = getImage();
	if (blackOffset == NULL) {
		CvSize size;
		size.height = currentImage->height;
		size.width = currentImage->width;
		blackOffset = cvCreateImageHeader(size, currentImage->depth, currentImage->nChannels);
		int bufsize = currentImage->height * currentImage->width 
			* currentImage->nChannels * (currentImage->depth/8);
		char* bufCopy = Ve::bufferCopy(currentImage->imageData, bufsize);
		blackOffset->imageData = bufCopy;
	}
	releaseImage();
	VeEvent e(VeEvent::MISC_EVENT, VeEvent::OFFSET_UPDATE_CODE);
	postEvent(e);
}

void VideoSource::clearBlackOffset() {
	if (blackOffset != NULL) {
		delete blackOffset->imageData;
		cvReleaseImageHeader(&blackOffset);
	}
	blackOffset = NULL;
	VeEvent e(VeEvent::MISC_EVENT, VeEvent::OFFSET_UPDATE_CODE);
	postEvent(e);
}

void VideoSource::lockImage() {
	tmpMutex.enterMutex();
	if (imgLock) { // Someone else already locked this
		tmpMutex.leaveMutex();
		return; 
	}
	imgLock = true;
	tmpMutex.leaveMutex();
	imgMutex.enterMutex();
}

void VideoSource::releaseImage() {
	tmpMutex.enterMutex();
	if (!imgLock) { // The image isn't locked
		tmpMutex.leaveMutex();
		return;
	}
	imgLock = false;
	tmpMutex.leaveMutex();
	imgMutex.leaveMutex();
}