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

EpixSource::EpixSource(int unit, string configfile)
 : VideoSource()
{
    int result;
    if (!XCLIBController::isOpen()) 
	result = XCLIBController::openLib(configfile);
    if (XCLIBController::isOpen()) {
	    height = pxd_imageYdim();
	    width = pxd_imageXdim();
    }
    EpixSource::unit = unit;
    XCLIBController::goLive(unit);
    readerThread = new EpixReaderThread(unit);
    readerThread->start();
}


EpixSource::~EpixSource()
{
    readerThread->stop();
    XCLIBController::goUnLive(unit);
}


IplImage* EpixSource::getImage() {
    IplImage* image;
    CvSize size;
    size.height = height;
    size.width = width;
    
    image = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
    uchar* buffer = readerThread->getBuffer();
    if (buffer != NULL) {
	image->imageData = (char*) buffer;
	timer->count();
    }

    return image;
}