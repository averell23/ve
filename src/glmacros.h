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
#ifndef GLMACROS_H
#define GLMACROS_H

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include "ve.h"
#include "fontmanager.h"

/**
A collection of OpenGL macros for common tasks

@author Daniel Hahn,,,
*/
class GLMacros{
public:
    /**
      Initialize the GL matrices with an ortho matrix that allows to
      use virtual coordinates in GL space. Color is set to white. 
      
      See also @see Ve::getVirtualSize
    */
    static void initVirtualCoords();
    
    /**
      Reverts the projection and model view matrix to the pre-stored 
      values.
    */
    static void revertMatrices();
    
    /**
      Draws crosshairs at the given virtual coordinates.
    */
    static void drawCrosshairs(int x, int y);
    
    /**
      Draws the text at the given virtual coordinates.
      
      @param size FaceSize of the text font.
    */
    static void drawText(int x, int y, char* text, int fontSize);
    
    /**
      Blanks the left eye by drawing a black square in front of 
      all existing elements. 
    */
    static void blankLeftEye();

    /**
      Blanks the left eye by drawing a black square in front of 
      all existing elements. 
    */
    static void blankRightEye();
};

#endif
