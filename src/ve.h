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
#ifndef VE_H
#define VE_H

#include <iostream>
#include <cstdlib>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <vector>
#include <log4cplus/logger.h>
#include "stopwatch.h"
#include "videocanvas.h"
#include "videosource.h"
#include "overlay.h"
#include "veeventsource.h"
#include "veeventlistener.h"

using namespace log4cplus;

/** 
  This is the program's main class that includes the basic application framework.
*/
class Ve {
public:
    /**
      Adds an overlay to the internal overlay list. All overlays will
      be called in the order in which they were added.
     */
    static void addOverlay(Overlay* ol);
    
    /**
      Intializes the Ve application and data structures. This
      should be called after initGL().
      
      @param right/left The sources for the two video 
                              images.
    */
    static void init(VideoSource* left, VideoSource* right);

    /*
      Start video display and processing.
    */
    static void start();

	/**
		Get the right video source (e.g. for fps calculations)
	*/
	static VideoSource* getRightSource() { return rightEye; }

	/**
		Get the left video source (e.g. for fps calculations);
	*/
	static VideoSource* getLeftSource()  { return leftEye; }

	/**
		Get the main timer.
	 */
	static Stopwatch* getTimer() { return timer; }
	
    /**
      Registers an event listner with the main event source.
    */
    static void addListener(VeEventListener* listener);

    /** OpenGL initilization */
    static void initGL( int argc, char** argv );
    
    /** Reshape (viewport) callback */
    static void reshape ( int w, int h );

    /** Keyboard callback */
    static void keyboard ( unsigned char key, int x, int y );

    /** Arrow key callback */
    static void arrow_keys ( int a_keys, int x, int y );

    /** Shutdown function for the program  */
    static void shutdown();

    /** Drawing function */
    static void glDraw();
    
    static Stopwatch *timer;
    static VideoCanvas *mainVideo;
    static VideoSource *rightEye;
    static VideoSource *leftEye;
    /// The overlays that will be displayed
    static vector<Overlay*> overlays;
    
    /// Event source used for events from the main framework
    static VeEventSource eventSource;
    
    // Logger for this class
    static Logger logger;
};

#endif
