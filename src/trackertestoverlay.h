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
#ifndef TRACKERTESTOVERLAY_H
#define TRACKERTESTOVERLAY_H

#include "trackeroverlay.h"

/**
Overley to test the tracker implementation.

@author Daniel Hahn,,,
*/
class TrackerTestOverlay : public TrackerOverlay
{
public:
    
    /**
      Creates a new TrackerTestOverlay see @see TrackerOverlay for the meaning
      of the parameters.
    */
    TrackerTestOverlay(int leftSourceID, int rightSourceID, bool display=true);

    ~TrackerTestOverlay();
    
    virtual void handleEvent(VeEvent &e);
    
protected:
    /// Indicates if a crosshair should be drawn on every marker
    bool doCrosshairs;
    /// Indicates if a highlight indicator should be drawn for the center marker
    bool doHighlight;
    
    virtual void drawOneEye(map<int,Position>& positions);
    /// Logger for this class
    static Logger logger;
};

#endif