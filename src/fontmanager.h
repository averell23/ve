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
#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <FTGLTextureFont.h>
#include <log4cplus/logger.h>

#define DEFAULT_FONT "../fonts/obliviousfont.ttf"
// #define DEFAULT_FONT "C:/Windows/fonts/Arial.ttf" // Windows fallback

using namespace log4cplus;

/**
Simple Font Manager Class for FTGL fon#include <FTGLTextureFont.h>ts. Currently, only a single
font is supported.
 
@author Daniel Hahn,,,
*/
class FontManager {
public:

    ~FontManager();

    /**
      Returns the single instance of the managed font. 
      
      At the moment the
      font has a default size, and the face size should NOT be changed
      outside this class. This is because FaceSize() is an expensive 
      operation and should not be done for every frame. 
      
      To get around this future implementatios may contain multiple 
      Font objects, one for each font size.
      
      @return The font object, or NULL if it could not be created.
    */
    static FTGLTextureFont* getFont();

private:
    /**
      Private for singleton class.
    */
    FontManager();

    /**
      This actually creates the font object.
    */
    static void createFont();

    /// The font managed by this class
    static FTGLTextureFont* font;

    /// Logger for this class
    static Logger logger;

};

#endif
