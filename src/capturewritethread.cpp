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
#include "capturewritethread.h"

Logger CaptureWriteThread::logger = Logger::getInstance("Ve.CaptureWriteThread");


CaptureWriteThread::CaptureWriteThread(CaptureBuffer* buffer, CaptureInfo* info) {
    this->buffer = buffer;
    this->info = info;
    counter = 0;
    running = false;
    info->writeTimer.reset();
}

void CaptureWriteThread::run() {
    running = true;
	info->writeTimer.start();
    while (running) {
		writeImages();
		counter++;
		if ((counter % 100) == 0) {
			LOG4CPLUS_INFO(logger, "Wrote: " << counter << " images.");
		}
		info->writeTimer.count();
    }
	info->writeTimer.stop();
}

void CaptureWriteThread::writeImages() {
	CaptureImagePair* buf = buffer->getQueueFirst();
	// Wait for both buffers to have data
	while (buf == NULL) {
		buf = buffer->getQueueFirst();
	}
	// Names for the image files
	char name1[256], name2[256];
	sprintf(name1,"%s_a_%d.img",info->filePrefix, counter);
	sprintf(name2,"%s_b_%d.img",info->filePrefix, counter);
	CaptureController::writeRAWFiles(name1, name2, buf, info);
	if (info->writeTimestamp) {
		char stampFile[256];
		sprintf(stampFile, "%s_%d.txt", info->timstampPrefix, counter);
		CaptureController::writeMetaStamp(stampFile, buffer->getBufferAt(counter));
	} 

	buffer->removeQueueFirst();
}

void CaptureWriteThread::quit() {
    running = false;
}

CaptureWriteThread::~CaptureWriteThread()
{
}


