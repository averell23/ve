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

CaptureWriteThread::CaptureWriteThread(CaptureBuffer* buffer_a, CaptureBuffer* buffer_b, CaptureInfo info, Mutex* mutex) {
    this->buffer_a = buffer_a;
    this->buffer_b = buffer_b;
    this->info = info;
    this->mutex = mutex;
    counter = 0;
    running = false;
}

void CaptureWriteThread::run() {
    running = true;
    while (running) {
	writeImages();
    }
}

void CaptureWriteThread::writeImages() {
    mutex->enterMutex();
    char* buf_a = buffer_a->getQueueFirst();
    char* buf_b = buffer_b->getQueueFirst();
    mutex->leaveMutex();
    // Wait for both buffers to have data
    while ((buf_a == NULL) || (buf_b == NULL)) {
	mutex->enterMutex(); // FIXME: FRAGILE kludge
	if (buf_a == NULL) buf_a = buffer_a->getQueueFirst();
	if (buf_b == NULL) buf_b = buffer_b->getQueueFirst();
	mutex->leaveMutex();
    }
    
    // Names for the image files
    char name1[256], name2[256];
    // File handles
    FILE *file1, *file2;
    sprintf(name1,"%s_a_%d.img",info.filePrefix, counter);
    sprintf(name2,"%s_b_%d.img",info.filePrefix, counter);
    // write to files
    file1 = fopen(name1, "wb");
    if (fwrite(buf_a, info.pixBytes, info.imgSize, file1) != info.imgSize)
	LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
    fclose(file1);
    file2 = fopen(name2, "wb");
    if (fwrite(buf_b, info.pixBytes, info.imgSize, file2) != info.imgSize)
	LOG4CPLUS_ERROR(logger, "Error writing file for cam b");
    fclose(file2);
    
    // Remove buffers
    mutex->enterMutex();
    buffer_a->removeQueueFirst();
    buffer_b->removeQueueFirst();
    mutex->leaveMutex();
    counter++;
}

void CaptureWriteThread::quit() {
    running = false;
}

CaptureWriteThread::~CaptureWriteThread()
{
}


