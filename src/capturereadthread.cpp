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
#include "capturereadthread.h"

Logger CaptureReadThread::logger = Logger::getInstance("Ve.CaptureReadThread");

CaptureReadThread::CaptureReadThread(CaptureBuffer* buffer_a, CaptureBuffer* buffer_b, CaptureInfo* info, Mutex* mutex) {
    this->buffer_a = buffer_a;
    this->buffer_b = buffer_b;
    this->mutex = mutex;
	this->info = info;
	info->readTimer.reset();
    running = false;
	counter = 0;
}

CaptureReadThread::~CaptureReadThread()
{
}


void CaptureReadThread::run() {
    running = true;
	info->readTimer.start();
    while (running) {
		readImages();
		counter++;
		if ((counter % 100) == 0) {
			LOG4CPLUS_INFO(logger, "Read " << counter << " images.");
		}
		info->readTimer.count();
    }
	info->readTimer.stop();
}

void CaptureReadThread::readImages() {
	mutex->enterMutex();
	char* newBuf_a = buffer_a->addQueueElement();
	char* newBuf_b = buffer_b->addQueueElement();
	mutex->leaveMutex();
	// Wait for free space in both buffers
	while ((newBuf_a == NULL) || (newBuf_b == NULL)) {
		mutex->enterMutex(); // FIXME: FRAGILE kludge
		if (newBuf_a == NULL) newBuf_a = buffer_a->addQueueElement();
		if (newBuf_b == NULL) newBuf_b = buffer_b->addQueueElement();
		mutex->leaveMutex();
		Thread::yield();
	}

	pxvbtime_t fieldCount1 = pxd_capturedFieldCount(1<<0);
	pxvbtime_t fieldCount2 = pxd_capturedFieldCount(1<<1);
	pxd_goSnap(1<<0, 1);
	pxd_goSnap(2<<0, 1);
	// Wait for imaging boards to be ready
	while ((fieldCount1 == pxd_capturedFieldCount(1<<0)) && (fieldCount2 == pxd_capturedFieldCount(1<<1))) {
		Thread::yield();
	}

	readBuffer(newBuf_a, newBuf_b);

}

void CaptureReadThread::readBuffer(char* buf_a, char* buf_b) {
	if (info->pixBytes == 1) {
			int result = pxd_readuchar(1<<0, 1, info->offset_x, info->offset_y, info->offset_x + info->width , info->offset_y + info->height, (uchar*) buf_a, info->imgSize, info->colorName);
			if (result != info->imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << info->imgSize);
			result = pxd_readuchar(1<<1, 1, info->offset_x, info->offset_y, info->offset_x + info->width , info->offset_y + info->height, (uchar*) buf_b, info->imgSize, info->colorName);
			if (result != info->imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << info->imgSize);
		} else if (info->pixBytes == 2) {
			int result = pxd_readushort(1<<0, 1, info->offset_x, info->offset_y, info->offset_x + info->width , info->offset_y + info->height, (ushort*) buf_a, info->imgSize, info->colorName);
			if (result != info->imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << info->imgSize);
			result = pxd_readushort(1<<1, 1, info->offset_x, info->offset_y, info->offset_x + info->width , info->offset_y + info->height, (ushort*) buf_b, info->imgSize, info->colorName);
			if (result != info->imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << info->imgSize);
		} else {
			LOG4CPLUS_ERROR(logger, info->pixBytes << " pixel bytes not supported.");
		}
}
