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

#ifndef BMPSTRUCT_H
#define BMPSTRUCT_H

/**
  Collection of header to allow writing of the .bmp file format without
  depending on any platform library.
  WARNING: This relies on short/int/long having 2/4/8 bytes, respectively
*/

/**
  BMP file format header structure.
*/
typedef struct {
   /// Magic identifier           
   unsigned short int type;
   /// File size in bytes         
   unsigned int size;                      
   /// Reserved fields
   unsigned short int reserved1, reserved2;
   /// Offset to image data, bytes 
   unsigned int offset;                     
} BMP_HEADER;

/**
  BMP file format info header structure.
*/
typedef struct {
   /// Header size in bytes
   unsigned int size;
   /// Width and height of image
   int width,height;                
   /// Number of colour planes  
   unsigned short int planes;
   /// Bits per pixel          
   unsigned short int bits;
   /// Compression type
   unsigned int compression;
   /// Image size in bytes
   unsigned int imagesize;
   /// Pixels per meter
   int xresolution,yresolution;
   /// Number of colours
   unsigned int ncolours;
   /// Important colours
   unsigned int importantcolours;   
} BMP_INFOHEADER;

#endif
