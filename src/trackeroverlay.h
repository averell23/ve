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
#ifndef TRACKEROVERLAY_H
#define TRACKEROVERLAY_H

#include "overlay.h"
#include "ve.h"
#include "fontmanager.h"
#include "veeventlistener.h"
#include "position.h"
#include "glmacros.h"
#include "vepositionevent.h"
#include <log4cplus/logger.h> 	// Log4cplus
#include <cv.hpp> 		// OpenCv
#include <math.h>
// Include ARToolkit stuff
#include <AR/ar.h>

using namespace log4cplus;

/**
This allows to track patterns in the video image by means of the ARToolkit.

@author Daniel Hahn,,,
*/
class TrackerOverlay : public Overlay, public VeEventListener
{
public:

    /**
      Creates a new tracker overlay.
      
      @param leftSourceID ID code of the left channel, used in @see VePositionEvent objects
      @param rightSourceID ID code of the right channel, used in @see VePositionEven objects
      @param display Initial display state
    */
    TrackerOverlay(int leftSourceID, int rightSourceID, bool display=true);
    
    ~TrackerOverlay();

    void drawOverlay();
    
    static char* paramFile; 

	void recieveEvent(VeEvent &e);
    
private:
    /**
      Contains the on-screen positions of objects in the left eye in 
      virtual coordinates.
    */
    map<int, Position> leftPositions;
    /**
      Contains the on-screen positions of objects in the left eye in 
      virtual coordinates.
    */
    map<int, Position> rightPositions;
    /// Expiry time of positions in milis
    int expireT;
    
    /**
      Clean up the lists of positions. This removes expired position 
      entries from the list. Note: For the sake of cleaner code, this
      is not done during the main list traversal. 
    */
    void positionListCleanup();
    
    /**
      Helper function for @see positionListCleanup
    */
    void cleanupSingleList(map<int,Position>& positions);
    
    /// Source ID codes for left and right eye
    int leftSourceID, rightSourceID;
    
    /// Logger for this class
    static Logger logger;
    /**
       Contains the drawing code for a single eye.
       
       @param positions Pointer to the map that contains the object
                        positions for the current eyes.
    */
    void drawOneEye(map<int,Position>& positions);
    /// The font used for status display
    FTGLTextureFont* font;
    /// Text for display
    char* text;
    /// Draws crosshairs at the given position
    void drawCrosshairs(int x, int y);
    /// Draw a highlight indicator at the given position
    void drawHighlight(int x, int y);
    
    /**
        From the given markers, select the one that is closest to the center of 
        the screen.
		
	@param markers Pointer to an array of markers.
	@param markerNum Number of markers in @see markers
	@return Index of the centermost marker. This is -1 if no markers are available.
    */
    map<int,Position>::iterator getCenterMarker(map<int,Position>& positions);
    
    /*
        Calculate distance from sreen center.
	@return Square of the distance of the given point from the center of the screen.
    */
    double TrackerOverlay::centerDistanceSquared(double x, double y);
	
    /// Indicates if the text portion of the overlay should be drawn
    bool doText;
    /// Indicates if a crosshair should be drawn on every marker
    bool doCrosshairs;
    /// Indicates if a highlight indicator should be drawn for the center marker
    bool doHighlight;
    /// Width and height of video picture
    int width, height;
    
};

#endif
