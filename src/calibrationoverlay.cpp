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
#include "calibrationoverlay.h"

Logger CalibrationOverlay::logger = Logger::getInstance("Ve.CalibrationOverlay");

CalibrationOverlay::CalibrationOverlay(bool display) : Overlay(display) {
    text[0] = new char[256];
    cCalibrationObject = Ve::getStereoCalibration();;
    
    // Init textur
    textureSize = GLMacros::checkTextureSize();
    if (textureSize == 0) {
        LOG4CPLUS_FATAL(logger, "Cannot accomodate texture, shutting down.");
        exit(1);
    }

    LOG4CPLUS_DEBUG( logger, "Assigning Texture of size " << textureSize);
    glGenTextures(2, textures);   /* create the texture names */

    for (int i=0 ; i<2 ; i++) {
	    glBindTexture(GL_TEXTURE_2D, textures[i]); /* Bind texture[0] ??? */
	    LOG4CPLUS_DEBUG(logger, "Binding NULL texture image...");
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
		        GL_RGB, GL_UNSIGNED_BYTE, NULL);
	    LOG4CPLUS_DEBUG(logger, "Texture image bound.");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   
    }
    // Calculate the size factors
    imageHeight = Ve::getLeftSource()->getHeight();
    imageWidth = Ve::getLeftSource()->getWidth();
    widthFactor = 1.0f - ((double) imageWidth / (double) textureSize);
    heightFactor = 1.0f - ((double) imageHeight / (double) textureSize);
    foundCorners = false;
}

CalibrationOverlay::~CalibrationOverlay() {}

void CalibrationOverlay::drawOverlay() {
    GLMacros::initVirtualCoords();

    // glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    sprintf(text[0], "Calibrating (%i (%i/%i) snaps)", 
            cCalibrationObject->getSnapshotCount(),
            cCalibrationObject->getLeftCalibration()->getSnapshotCount(),
            cCalibrationObject->getRightCalibration()->getSnapshotCount()
    );

    glTranslatef(-1.0f, 0.0f, 0.0f);
    drawOneEye(); // left eye
    glLoadIdentity();
    drawOneEye(); // right eye

    GLMacros::revertMatrices();

    drawPiP();

    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

void CalibrationOverlay::drawPiP() {
    for (int i=0 ; i<2 ; i++) {
        glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
        glLoadIdentity();
        if (0 == i) {
	       glTranslatef(-1.0f, 0.0f, 0.0f);
	    }
        setPiPCoordinates();
        GLMacros::rotateImage();
	    CameraCalibration* calObj;
	    if (0 == i) {
	        calObj = cCalibrationObject->getLeftCalibration();
	    } else {
	        calObj = cCalibrationObject->getRightCalibration();
	    }
	    IplImage* img = calObj->lastSnapshot;
	    if (img) {
	        glBindTexture(GL_TEXTURE_2D, textures[i]);
	        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight,
			        GL_RGB, GL_UNSIGNED_BYTE, img->imageData);
	    } else {
	        glBindTexture(GL_TEXTURE_2D, 0);
	    }
	    drawQuad();
	    glBindTexture(GL_TEXTURE_2D, 0);
	    GLMacros::initVirtualCoords();
        if (0 == i) {
	        glTranslatef(-1.0f, 0.0f, 0.0f);
	    }
        drawMarkers(calObj);
    } // Both eyes for loop
}

void CalibrationOverlay::drawMarkers(CameraCalibration* calObj) {
	// Back to Pip coordinates, set marker color
	setPiPCoordinates();
    GLMacros::rotateImage();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    float redDefault = 0.0f;
    float greenDefault = 0.0f;
    float blueDefault = 0.0f;
    if (foundCorners) {
        greenDefault = 1.0f;
    } else {
        redDefault = 1.0f;
    }

	CvPoint vCoords = Ve::getVirtualSize();
	float xFac = vCoords.x / (float) imageWidth;
	float yFac = - vCoords.y / (float) imageHeight;
	int yOff = Ve::getVirtualSize().y;
	int count = calObj->lastCornerCount;
    int xOld, yOld;
    double colorInc = 1.0f / (double) calObj->lastCornerCount;
	for (int k=0 ; k<calObj->lastCornerCount ; k++) {
        glColor4f(redDefault, greenDefault, 1.0f - (colorInc * (float) k), 1.0f);
	    int x = (int) (calObj->lastCorners[k].x * xFac);
	    int y = (int) ((calObj->lastCorners[k].y * yFac * 2) + yOff);
        if (k>0) {
            GLMacros::drawLine(xOld, yOld, x, y, 1.0f);
        } else {
            glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        }
	    GLMacros::drawMarker(x, y, 1.0f);
        xOld = x;
        yOld = y;
	} 
    GLMacros::revertMatrices();
}

void CalibrationOverlay::drawOneEye() {
    font = FontManager::getFont();
    if (font == NULL)
        return; // Sanity check

    glTranslatef(0.05f, 0.6f, 0.0f);
    font->Render(text[0]);

}

void CalibrationOverlay::setPiPCoordinates() {
    glTranslatef(0.6f, 0.6f, 0.0f);
    glScalef(0.35f, 0.35f, 1.0f);
}

void CalibrationOverlay::recieveEvent(VeEvent &e) {
    LOG4CPLUS_TRACE(logger, "Entering event handler");

    if (e.getType() != VeEvent::KEYBOARD_EVENT)
        return;

    if ((e.getCode() == 'c') || (e.getCode() == 'C')) {
        toggleDisplay();
    }

    if (!displayState)
        return;

    switch (e.getCode()) {
    case 'x':
    case 'X':
        foundCorners = cCalibrationObject->takeSnapshot();
        break;
    case 'n':
    case 'N':
        cCalibrationObject->deleteSnapshots();
        break;
    case 'm':
    case 'M':
        cCalibrationObject->recalibrate();
        break;
    case 'a':
    case 'A':
        Ve::getLeftSource()->getCalibration()->load();
        Ve::getRightSource()->getCalibration()->save();
        break;
    case 's':
    case 'S':
        Ve::getLeftSource()->getCalibration()->save();
        Ve::getRightSource()->getCalibration()->save();
        break;
    case 'b':
    case 'B':
        Ve::getLeftSource()->getCalibration()->popSnapshot();
        Ve::getRightSource()->getCalibration()->popSnapshot();
    }

}

void CalibrationOverlay::drawQuad() {
    glBegin(GL_QUADS);
    glTexCoord2d(0.0f, 0.0f);
    glVertex3i(0, -1, 0);
    glTexCoord2d(1.0f - widthFactor, 0.0f);
    glVertex3i(1, -1, 0);
    glTexCoord2d(1.0f - widthFactor, 1.0f - heightFactor);
    glVertex3i(1, 1, 0);
    glTexCoord2d(0.0f, 1.0f - heightFactor);
    glVertex3i(0, 1, 0);
    glEnd();
}
