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

#include "glextensions.h"

Logger GLExtensions::logger = Logger::getInstance("Ve.GLExtensions");

bool GLExtensions::checkExtension(char* name) {
    char* extension_list = (char*) glGetString(GL_EXTENSIONS);
    char *end = extension_list + strlen(extension_list);
    int extNameLen = strlen(name);
    
    while (extension_list < end) {
	int pos = strcspn(extension_list, " ");
	if ((extNameLen == pos) && (strncmp(name, extension_list, pos) == 0)) {
	    return true;
	}
	extension_list += (pos + 1);
    }
    
    return false;
}

bool GLExtensions::initExtensions() {
    bool retVal = true;
    
    #ifdef WIN32
    // Setup Windows function pointers here
    #ifndef GL_VERSION_1_2
    #ifdef GL_BLEND_EQUATION
    if (GLExtension::checkExtension("ARB_imaging")) {
	glBlendEquation = (PFNGLBLENDEQUATIONPROC) wglGetProcAddress("glBlendEquation");
    } else {
	LOG4CPLUS_ERROR("OpenGL imaging subset not supported by this driver/hardware.");
	retVal = false;
    }
    #endif
    #endif
    // End of GL 1.2 stuff
    #endif
    // End of Win32 init
    
    return retVal;
}