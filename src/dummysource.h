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
#ifndef DUMMYSOURCE_H
#define DUMMYSOURCE_H

#include "videosource.h"
#include <stdlib.h>
#include <iostream>
#include <cv.hpp>

#define NUM_IMAGES 50

using namespace std;

/* Texture Image */
typedef struct {
    int width;
    int height;
    unsigned char *data;
} textureImage;

/**
Dummy video source that loads images from disk and retrieves them
in a circular fashion.
  
@author Daniel Hahn,,,
*/
class DummySource : public VideoSource
{
public:
    DummySource();

    ~DummySource();
    
    int loadBMP(char *filename, IplImage **image);
    
    IplImage* getImage();
    
    char* copyBuffer(char* buffer, int size);

private:
    IplImage* images[NUM_IMAGES];
    int position;
};

#endif
