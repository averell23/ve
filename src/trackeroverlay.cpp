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

TrackerOverlay::TrackerOverlay() {
    thresh = 100;
	// Screen dimensions
	height = Ve::getLeftSource()->getHeight();
	width = Ve::getLeftSource()->getWidth();
    // Init ARToolkit camera parameters
    ARParam cparam, wparam;
    if (arParamLoad(paramFile, 1, &wparam) < 0) {
		LOG4CPLUS_WARN(logger, "Could not load parameter file: " << paramFile);
    } else {
		LOG4CPLUS_DEBUG(logger, "ARToolkit parameters loaded.");
		arParamChangeSize(&wparam, width, height, &cparam);
		arInitCparam(&cparam);
		LOG4CPLUS_DEBUG(logger, "Camera parameters initialized.");
    }
    // Load patterns
    int pattId = arLoadPatt("../config/patterns/hiro.pattern"); // FIXME: Global variables instead of hardcode
    if (pattId < 0) {
		LOG4CPLUS_WARN(logger, "Pattern could not be loaded.");
	} else {
		LOG4CPLUS_DEBUG(logger, "Pattern loaded.");
	}
	text = new char[256];
	xOff = 0;
	yOff = 1000;
	xFac = 1000.0f/((float) Ve::getLeftSource()->getWidth());
	yFac = -4;
	doText = false;
	doCrosshairs = true;
	doHighlight = true;
	LOG4CPLUS_DEBUG(logger, "Overlay created.");
}

TrackerOverlay::~TrackerOverlay()
{
	delete text;
}


void TrackerOverlay::drawOverlay() {
	// Marker detection
    int markerNumL, markerNumR;
	// Marker Info structures
	ARMarkerInfo *markerInfo, *markerInfoL, *markerInfoR;
    ARUint8* imageData = getImageData(LEFT);
    arDetectMarker(imageData, thresh, &markerInfo, &markerNumL);
    LOG4CPLUS_TRACE(logger, "Detected " << markerNumL << " markers in left image.");
	markerInfoL = new ARMarkerInfo[markerNumL]; // FIXME: Take care, markerInfoL is not completely initialized
	for (int i=0 ; i < markerNumL ; i++) {
		markerInfoL[i].pos[0] = markerInfo[i].pos[0]; 
		 markerInfoL[i].pos[1] = markerInfo[i].pos[1];
	}
	delete imageData;
	imageData = getImageData(RIGHT);
	arDetectMarker(imageData, thresh, &markerInfoR, &markerNumR);
	LOG4CPLUS_TRACE(logger, "Detected " << markerNumR << " markers in right image.");
	delete imageData;
	
	// Drawing code
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glOrtho(-1000, 1000, -1000, 1000, 0, 1);
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();

    // glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
	if (doText) {
		if (markerNumL > 0) {
			sprintf(text, "%f/%f/%f", markerInfoL[0].pos[0], markerInfoL[0].pos[1],
					sqrt(centerDistanceSquared(markerInfoL[0].pos[0], markerInfoL[0].pos[1])));
		} else {
			sprintf(text, "Detected %d/%d", markerNumL, markerNumR);
		} 
	}
	

    glTranslatef(-1.0f, 0.0f, 0.0f);
	if (doText) {
		drawOneEye();
		glTranslatef(-1.0f, 0.0f, 0.0f);
	}
	int center = getCenterMarker(markerInfoL, markerNumL);
	if (doCrosshairs) {
		for (int i=0 ; i < markerNumL ; i++) {
			if (i == center) {
				glColor3f(1.0f, 0.2f, 0.2f);
			}
			drawCrosshairs((markerInfoL[i].pos[0] * xFac)+ xOff, (markerInfoL[i].pos[1] * yFac) + yOff);
			if (i == center) {
				glColor3f(1.0f, 1.0f, 1.0f);
			}
		}
	}
	if (doHighlight && (center >= 0)) {
		drawHighlight((markerInfoL[center].pos[0] * xFac)+ xOff, (markerInfoL[center].pos[1] * yFac) + yOff);
	}
	delete markerInfoL;
	// drawCrosshairs(500,1000);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f,  1.0f);
	if (doText) { 
		drawOneEye();
		glTranslatef(0.0f, 0.0f,  1.0f);
	}
	center = getCenterMarker(markerInfoR, markerNumR);
	if (doCrosshairs) {
		for (int i=0 ; i < markerNumR ; i++) {
			if (i == center) {
				glColor3f(1.0f, 0.2f, 0.2f);
			}
			drawCrosshairs((markerInfoR[i].pos[0] * xFac)+ xOff, (markerInfoR[i].pos[1] * yFac) + yOff); 
			if (i == center) {
				glColor3f(1.0f, 1.0f, 1.0f);
			}
		}
	}
	if (doHighlight && (center >= 0)) {
		drawHighlight((markerInfoR[center].pos[0] * xFac)+ xOff, (markerInfoR[center].pos[1] * yFac) + yOff);
	}
	// drawCrosshairs(0,0);
    
    // Remove text textures
    glBindTexture(GL_TEXTURE_2D, 0);
    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

ARUint8* TrackerOverlay::getImageData(int leftOrRight) {
    IplImage *origImage = NULL;
	IplImage *offset = NULL;
	VideoSource* currentSource = NULL;
    if (leftOrRight == LEFT) {
		currentSource = Ve::getLeftSource();
    } else {
		currentSource = Ve::getRightSource();
    }
	currentSource->lockImage();
	origImage = currentSource->getImage();
	offset = (IplImage*) currentSource->getBlackOffset();

    int imageDimension = origImage->width * origImage->height;
    ARUint8* retImage = new ARUint8[imageDimension * 4];
    for (int i=0 ; i < imageDimension ; i++) {
		// FIXME: Original image is always assumed to have RGB order
		retImage[i*4] = 255; // Alpha channel
		if (offset != NULL) {
			retImage[(i*4)+1] = origImage->imageData[(i*3)+2] - offset->imageData[(i*3)+2];
			retImage[(i*4)+2] = origImage->imageData[(i*3)+1] - offset->imageData[(i*3)+1];
			retImage[(i*4)+3] = origImage->imageData[i*3] - offset->imageData[i*3];
		} else{
			retImage[(i*4)+1] = origImage->imageData[(i*3)+2];
			retImage[(i*4)+2] = origImage->imageData[(i*3)+1];
			retImage[(i*4)+3] = origImage->imageData[i*3];
		}
    }

	currentSource->releaseImage();

    return retImage;
}

void TrackerOverlay::drawOneEye() {
    font = FontManager::getFont();
    if (font == NULL)
        return; // Sanity check

    glTranslatef(0.05f, 0.2f, 0.0f);
    font->Render(text);
	glBindTexture(GL_TEXTURE_2D, 0);
	glLoadIdentity();
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

int TrackerOverlay::getCenterMarker(ARMarkerInfo* markers, int markerNum) {
	float shortestDistance;
	int centerIndex = -1;
	// Init shortest distance
	if (markerNum > 0) {
		shortestDistance = centerDistanceSquared(markers[0].pos[0], markers[0].pos[1]);
		centerIndex = 0;
	}

	for (int i=0 ; i<markerNum ; i++) { 
		float distance = centerDistanceSquared(markers[i].pos[0], markers[i].pos[1]);
		if (distance < shortestDistance) {
			shortestDistance = distance;
			centerIndex = i;
		}
	}

	return centerIndex;
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
}
