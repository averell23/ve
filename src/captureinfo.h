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
#ifndef CAPTUREINFO_H
#define CAPTUREINFO_H

#include "stopwatch.h"

/**
Contains meta-information that is used by the capture classes.
 
@author Daniel Hahn,,,
*/
class CaptureInfo {
public:
    /// Image height
    int height;
    /// Image width
    int width;
    /// Top offset (for AOI)
    int offset_y;
    /// Left offset (for AOI)
    int offset_x;
    // Number of color planes
    int planes;
    // Bytes per plane per pixel
    int pixBytes;
    // Size of one image in color componets
    int imgSize;
    /// File name prefix for saving files
    char* filePrefix;
    /// Name of the color model (for the XCLIB functions)
    char* colorName;
    /// Timer for reading (in continous mode) FIXME: kludge
    Stopwatch readTimer;
    /// Timer for writing (in continous mode) FIXME: kludge
    Stopwatch writeTimer;
    /// Color info type
    int colorType;
    /// Indicates wether as seperate timestamp should be written
    bool writeTimestamp;
    /// Prefix for the timestamp files (including path)
    char* timstampPrefix;
    /// Indicates the preferred file format
    int fileFormat;

    static const int IMAGE_BAYER = 0;
    static const int IMAGE_RGB = 1;
    static const int IMAGE_GRAY = 2;

    static const int FILE_FORMAT_RAW = 0;
    static const int FILE_FORMAT_BMP = 1;
};

#endif
