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
#ifndef OVERLAY_H
#define OVERLAY_H

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <log4cplus/logger.h>

using namespace log4cplus;

/**
This is the base overlay class. Each overlay is a layer upon the main VideoCanvas that displays part of the UI. The overlay is implemented by overwriting the draw() class and calling OpenGL functions from it.
 
@author Daniel Hahn,,,
*/
class Overlay {
public:
    /**
      Constructs a new overlay with the given display state.
    */
    Overlay(bool display = true);

    ~Overlay();

    /**
      Displays this overlay.
    */
    virtual void display() {
        displayState = true;
    }

    /*
      Un-Displays this overlay
    */
    virtual void unDisplay() {
        displayState = false;
    }

    /**
      Toggles this overlay's display status.
    */
    virtual void toggleDisplay() {
        displayState = ! displayState;
    }

    /*
      Wrapper for the main drawing routine.
      
      @see drawOverlay
    */
    void draw();

protected:
    /// Indicates wether this overlay is to be displayed
    bool displayState;
    /*
      Main drawing routine, to be overwritten by the child
      classes.
    */
    virtual void drawOverlay() = 0;

};

#endif
