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
#ifndef CAPTUREBUFFER_H
#define CAPTUREBUFFER_H
#include <cc++/thread.h>
#include "captureimagepair.h"

using namespace ost;

/**
Buffer for use by @see CaptureController. This can either be used as direct storage or as a queue/ring buffer for concurrent I/O
 
@author Daniel Hahn,,,
*/
class CaptureBuffer {
public:

    /**
      Creates a new buffer.
      
      @param images Size of the new buffer in images.
      @param size Size of each image in bytes.
    */
    CaptureBuffer(int images, int size);

    /**
      Returns a pointer to the buffer at the index.
      No range checking is done, this can easily break
      your code.
    */
    CaptureImagePair* getBufferAt(int index) {
        return buffer[index];
    }

    /**
      Returns the capacity of this buffer in images.
    */
    int capacity() {
        return images;
    }

    ~CaptureBuffer();

    /**
      Add a buffer to the beginning of the queue and return a
      pointer to it.
      
      @return Pointer to the new buffer, or NULL if no free buffer
      is available.
    */
    CaptureImagePair* addQueueElement();

    /**
      Get a pointer to the first element in the queue.
      
      @return Pointer to the first buffer in the queue or NULL,
      if the queue is currently empty
    */
    CaptureImagePair* getQueueFirst();

    /**
      Remove the first element of the queue. This has no effect if
      the queue is currently empty.
    */
    void removeQueueFirst();

private:
    /// Buffers that contain the data
    CaptureImagePair** buffer;
    /// Size of the buffer in images
    int images;
    /// Points to the beginning of the queue
    int queueBegin;
    /// Points to the end of the queue
    int queueEnd;
    /// Mutex for thread-safe operation
    Mutex mutex;

};

#endif
