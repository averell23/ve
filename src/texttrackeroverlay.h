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
#ifndef TEXTTRACKEROVERLAY_H
#define TEXTTRACKEROVERLAY_H

#include "trackeroverlay.h"

/**
Places dummy text items at the positions of the tracked objects. The text size is 
adjusted according to the distance of the object from the center of the screen.

@author Daniel Hahn,,,
*/
class TextTrackerOverlay : public TrackerOverlay
{
public:
    /**
      Creates a new TrackerTestOverlay see @see TrackerOverlay for the meaning
      of the parameters.
    */
    TextTrackerOverlay(int leftSourceID, int rightSourceID, bool display=true);

    ~TextTrackerOverlay();
    
    virtual void handleEvent(VeEvent &e);

protected:
    virtual void drawOneEye(map<int,Position>& positions);
    /// Logger for this class
    static Logger logger;
    /// Text strings to display
    vector<vector<string> > texts;
    /// Indicates which text vector is currently active
    int textPos;
    /// Indicates the position in each text vector
    vector<int> elPos;
    /** 
        Indicates the distance from the center where the text
        size will be zero. This is a factor where 1 = full distance
        2 = half distance, and so on.
    */
    double distanceFactor;
    /// Maximum font size (at the center of the screen)
    int maxFontSize;
};

#endif
