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

/*
  Header file to handle OpenGL stuff that is only available from extension. 
  Since extensions are not very portable this section includes a lot of
  preprocessor-stuff.
  
  Additionally, one should still check for extensions during run-time, before
  using any of the functions defined here. 
*/

// Include GL extension headers for MS Windows
#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "GL/gl.h"
#include <string.h>
#include <log4cplus/logger.h>

#ifdef WIN32
#include "GL/glext.h"
#endif

// These extension are part of OpenGL 1.2 and above
#ifndef GL_VERSION_1_2
// imaging subset functions
PFNGLBLENDEQUATIONPROC glBlendEquation = NULL;
#endif

using namespace log4cplus;

/**
  Convenience class for handling OpenGL extension.
*/
class GLExtensions {
public:
    /**
      Checks wether a given OpenGL extension is supported
      by the current rendering context.
    */
    static bool checkExtension(char* name);
    
    /** 
      Initializes the functions pointers for the extension
    */
    static bool initExtensions();
    
private:
    /// Static class
    GLExtensions();
    /// Logger for this class
    static Logger logger;
};

#endif
