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
#include "capturebuffer.h"

CaptureBuffer::CaptureBuffer(int images, int size) {
    this->images = images;
    queueBegin = 0;
    queueEnd = 0;
    buffer = new CaptureImagePair*[images];
    for (int i=0 ; i < images ; i++) {
	buffer[i] = new CaptureImagePair(size);
    }
}

CaptureBuffer::~CaptureBuffer()
{
    for (int i=0 ; i < images ; i++) {
	delete buffer[i];
    }
    delete buffer;
}

CaptureImagePair* CaptureBuffer::addQueueElement() {
    CaptureImagePair* retVal;
    mutex.enterMutex();
    int newPos = (queueEnd + 1) % images;
    if (newPos == queueBegin) {
	retVal = NULL; // No more free buffers
    } else {
	queueEnd = newPos;
	retVal = buffer[queueEnd];
    }
    mutex.leaveMutex();
    return retVal;
}

CaptureImagePair* CaptureBuffer::getQueueFirst() {
    CaptureImagePair* retVal;
    mutex.enterMutex();
    if (queueEnd == queueBegin) {
	// Queue is empty
	retVal = NULL;
    } else {
	retVal = buffer[queueBegin];
    }
    mutex.leaveMutex();
    return retVal;
}

void CaptureBuffer::removeQueueFirst() {
    mutex.enterMutex();
    if (queueEnd != queueBegin) {
	int newPos = (queueBegin + 1) % images;
	queueBegin = newPos;
    }
    mutex.leaveMutex();
}
