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
#ifndef STATUSOVERLAY_H
#define STATUSOVERLAY_H

#include "overlay.h"
#include "stopwatch.h"
#include "fontmanager.h"
#include "ve.h"
#include "veeventlistener.h"
#include "videosource.h"


/**
Overlay to display system status information.

@author Daniel Hahn,,,
*/
class StatusOverlay : public Overlay, public VeEventListener
{
public:
    StatusOverlay(bool display);

    ~StatusOverlay();
    
    void drawOverlay();
    
private:
    /// Various timers
    Stopwatch *rightTimer, *leftTimer, *videoTimer;
	/// Pointers to the video sources
	VideoSource *rightSource, *leftSource;
    /// Logger for this class
    static Logger logger;
    /// The font used for status display
    FTGLTextureFont* font;
    /// Text buffer
    char *text, *text2;
    /// Contains the drawing code for a single eye.
    void drawOneEye();
    
    /// Recieves events 
    void recieveEvent(VeEvent &e);

};

#endif
