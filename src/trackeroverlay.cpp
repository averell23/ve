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
#include "trackeroverlay.h"

Logger TrackerOverlay::logger = Logger::getInstance("Ve.TrackerOverlay");
char* TrackerOverlay::paramFile = "../config/camera.param";

TrackerOverlay::TrackerOverlay(int leftSourceID, int rightSourceID, bool display) {
    expireT = 500;
    text = new char[256];
    doText = false;
    doCrosshairs = true;
    doHighlight = true;
    TrackerOverlay::leftSourceID = leftSourceID;
    TrackerOverlay::rightSourceID = rightSourceID;
    TrackerOverlay::displayState = display;
    LOG4CPLUS_DEBUG(logger, "Overlay created.");
    width = Ve::getLeftSource()->getWidth();
    height = Ve::getLeftSource()->getHeight();
}

TrackerOverlay::~TrackerOverlay()
{
	delete text;
}


void TrackerOverlay::drawOverlay() {
    // Drawing code
    GLMacros::initVirtualCoords();

    positionListCleanup();
    
    // glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    if (doText) {
		if (leftPositions.size() > 0) {
			sprintf(text, "%f/%f/%f", leftPositions.begin()->second.x, leftPositions.begin()->second.y,
				sqrt(centerDistanceSquared(leftPositions.begin()->second.x, leftPositions.begin()->second.y)));
		} else {
			sprintf(text, "Detected %d/%d", leftPositions.size(), rightPositions.size());
		} 
    } 
	

    // The following draws the left eye
    glTranslatef(-1.0f, 0.0f, 0.0f);
    drawOneEye(leftPositions);
    glLoadIdentity();
    
    // The following draws the right eye
    glTranslatef(0.0f, 0.0f,  1.0f);
    drawOneEye(rightPositions);
    
    GLMacros::revertMatrices();
}


void TrackerOverlay::drawOneEye(map<int,Position>& positions) {
    font = FontManager::getFont();
    int yOff = Ve::getVirtualSize().y;
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
	    drawCrosshairs((int) posIterator->second.x, (int) posIterator->second.y + yOff);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
    }
    if (doHighlight && (positions.size() >= 0)) {
	drawHighlight((int) center->second.x, (int) center->second.y + yOff);
    }
}


void TrackerOverlay::drawCrosshairs(int x, int y) {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // vertical line
    glVertex3i(x, -1000, 0);
    glVertex3i(x, 1000, 0);
    // horizontal line
    glVertex3i(0, y, 0);
    glVertex3i(1000, y, 0);
    //  box
    glVertex3i(x-10, y-10, 0);
    glVertex3i(x-10, y+10, 0);
    glVertex3i(x+10, y-10, 0);
    glVertex3i(x+10, y+10, 0);
    glVertex3i(x-10, y-10, 0);
    glVertex3i(x+10, y-10, 0);
    glVertex3i(x-10, y+10, 0);
    glVertex3i(x+10, y+10, 0);
    glEnd();
}

void TrackerOverlay::drawHighlight(int x, int y) {
	// set color to translucent green
	glColor4f(0.6f, 0.6f, 0.9f, 0.5f);
	// Stretching factor and dimensions
	double yStretch = (double) width / (double) height;
	int boxDimension = width / 6;
	int boxHeight = (int) (boxDimension * yStretch) * 2;
	int boxWidth = boxDimension;
	// Draw box
	glBegin(GL_QUADS);
	glVertex3i(x-boxWidth, y-boxHeight, 0);
	glVertex3i(x+boxWidth, y-boxHeight, 0);
	glVertex3i(x+boxWidth, y+boxHeight, 0);
	glVertex3i(x-boxWidth, y+boxHeight, 0);
	glEnd();
	// reset color
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

map<int,Position>::iterator TrackerOverlay::getCenterMarker(map<int,Position>& positions) {
	float shortestDistance;
	map<int,Position>::iterator posIterator = positions.begin();
	map<int,Position>::iterator centerObj = posIterator;
	
	// Init shortest distance
	int size = positions.size();
	if (size > 0) {
		shortestDistance = centerDistanceSquared(posIterator->second.x, posIterator->second.y);
	}

	for ( posIterator = positions.begin() ; posIterator != positions.end() ; posIterator++) { 
		float distance = centerDistanceSquared(posIterator->second.x, posIterator->second.y);
		if (distance < shortestDistance) {
			shortestDistance = distance;
			centerObj = posIterator;
		}
	}

	return centerObj;
}

void TrackerOverlay::positionListCleanup() {
    cleanupSingleList(leftPositions);
    cleanupSingleList(rightPositions);
}

void TrackerOverlay::cleanupSingleList(map<int,Position>& positions) {
    map<int,Position>::iterator posIter;
	vector<map<int,Position>::iterator> delElements; // Vector for elements to be deleted
    timeb currentT, tmpT;
    ftime(&currentT);
	mtx.enterMutex(); // Protect list operations against position updates
    for (posIter=positions.begin() ; posIter!=positions.end() ; posIter++) {
		LOG4CPLUS_TRACE(logger, "Position cleanup iteration");
		tmpT = posIter->second.timeStamp;
		long age = ((currentT.time - tmpT.time) * 1000) + abs(currentT.millitm - tmpT.millitm);
		if (age > expireT) {
			delElements.push_back(posIter);
		}
    }
	for (int i=0 ; i<delElements.size() ; i++) {
		LOG4CPLUS_TRACE(logger, "Erasing element");
		positions.erase(delElements[i]);
		LOG4CPLUS_TRACE(logger, "Erased element");
	}
	mtx.leaveMutex();
}

double TrackerOverlay::centerDistanceSquared(double x, double y) {
	double xPos = x - ((double) width / 2.0f);
	double yPos = y - ((double) height / 2.0f);
	/* LOG4CPLUS_DEBUG(logger, "x: " << x << ", y: " << y << ", xPos: " << xPos << ", yPos: " 
		<< yPos << ", width: " << width << ", height:" << height); */
	return (xPos*xPos) + (yPos*yPos);
}

void TrackerOverlay::recieveEvent(VeEvent &e) {
	if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 'n')) {
        doCrosshairs = !doCrosshairs;
        LOG4CPLUS_DEBUG(logger, "Recieved keyboard event, toggling crosshairs.");
    }
	if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 'm')) {
        doHighlight = !doHighlight;
        LOG4CPLUS_DEBUG(logger, "Recieved keyboard event, toggling highlights.");
    }
	if (e.getType() == VeEvent::POSITION_EVENT) {
		LOG4CPLUS_TRACE(logger, "Position event handler");
	    Position pos = ((VePositionEvent&) e).getPosition();
	    if (pos.source == leftSourceID) { // Add position events to the list
			leftPositions[pos.index] = pos;
	    } else if (pos.source == rightSourceID) {
			rightPositions[pos.index] = pos;
	    } else {
			LOG4CPLUS_DEBUG(logger, "Caught position event from unknown source.");
	    }
		LOG4CPLUS_TRACE(logger, "Position event handler finished"); 
	}
}
