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
#include "texttrackeroverlay.h"

Logger TextTrackerOverlay::logger = Logger::getInstance("Ve.TextTrackerOverlay");

TextTrackerOverlay::TextTrackerOverlay(int leftSourceID, int rightSourceID, bool display)
 : TrackerOverlay(leftSourceID, rightSourceID, display)
{
    textPos = 0;
    /// Init text stuff
    vector<string> participant_a;
    vector<string> participant_b;
    vector<string> participant_c;
    participant_a.push_back("Hello.");
    participant_b.push_back("Hi Mike!");
    participant_c.push_back("Hi Guys!");
    participant_a.push_back("How are you doing?");
    participant_b.push_back("Fine.");
    participant_c.push_back("Yeah. And you?");
    texts.push_back(participant_a);
    texts.push_back(participant_b);
    texts.push_back(participant_c);
    elPos.push_back(0);
    elPos.push_back(0);
    elPos.push_back(0);
    distanceFactor=1.2;
    maxFontSize = 100;
}


TextTrackerOverlay::~TextTrackerOverlay()
{
}


void TextTrackerOverlay::handleEvent(VeEvent &e) {
    if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 13)) {
        textPos = (textPos + 1) % texts.size();
	    elPos[textPos] = (elPos[textPos] + 1) % texts[textPos].size();
        LOG4CPLUS_DEBUG(logger, "Recieved keyboard event, advancing text.");
    }
}

void TextTrackerOverlay::drawOneEye(map<int,Position>& positions) {
    glColor3f(1.0f, 0.2f, 0.2f);
    int size = positions.size();
    map<int,Position>::iterator posIterator;
    int i=0;
    double max = (height*height) + (width*width);
    for (posIterator=positions.begin() ; posIterator!=positions.end() ; posIterator++) {
	    if (i<texts.size()) {
	        int tpos = elPos[i];
	        double dist = centerDistanceSquared(posIterator->second.x, posIterator->second.y);
	        int tsize = (int) ((((max/distanceFactor) - dist) / max) * maxFontSize);
	        if (tsize > 0) {
		        GLMacros::drawText((int) posIterator->second.x, (int) posIterator->second.y, (char*) texts[i][tpos].c_str(), tsize);
	        }
            i++;
	    }
    }
}
