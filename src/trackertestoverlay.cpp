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
#include "trackertestoverlay.h"

Logger TrackerTestOverlay::logger = Logger::getInstance("Ve.TrackerTestOverlay");

TrackerTestOverlay::TrackerTestOverlay(int leftSourceID, int rightSourceID, bool display)
 : TrackerOverlay(leftSourceID, rightSourceID, display)
{
    doCrosshairs = true;
    doHighlight = true;
}


TrackerTestOverlay::~TrackerTestOverlay()
{
}

void TrackerTestOverlay::handleEvent(VeEvent &e) {
    if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 'j')) {
        doCrosshairs = !doCrosshairs;
        LOG4CPLUS_DEBUG(logger, "Recieved keyboard event, toggling crosshairs.");
    }
    if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 'k')) {
        doHighlight = !doHighlight;
        LOG4CPLUS_DEBUG(logger, "Recieved keyboard event, toggling highlights.");
    }
}

void TrackerTestOverlay::drawOneEye(map<int,Position>& positions) {
    font = FontManager::getFont();
    int size = positions.size();
    map<int,Position>::iterator posIterator;
    // Write static text
    if (doText && (font != NULL)) { // Sanity check
        glTranslatef(0.05f, 0.2f, 0.0f);
        font->Render(text);
        glBindTexture(GL_TEXTURE_2D, 0);
        glTranslatef(-0.05f, -0.2f, 0.0f); // Translate back
    }
    map<int,Position>::iterator center = getCenterMarker(positions);
    // Draw crosshairs on objects
    if (doCrosshairs) {
        for (posIterator=positions.begin() ; posIterator!=positions.end() ; posIterator++) {
            if (posIterator == center) {
                glColor3f(1.0f, 0.2f, 0.2f);
            } else {
                glColor3f(1.0f, 1.0f, 1.0f);
            }
	    CvPoint pos = GLMacros::getPixelCoords((int) posIterator->second.x, (int) posIterator->second.y);
            GLMacros::drawCrosshairs(pos.x, pos.y);
        }
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    if (doHighlight && (positions.size() > 0)) {
	CvPoint pos = GLMacros::getPixelCoords((int) center->second.x, (int) center->second.y);
        drawHighlight(pos.x, pos.y);
    }
}
