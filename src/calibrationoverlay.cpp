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
    calibrationMode = LEFT_EYE;
    cCalibrationObject = Ve::getLeftSource()->getCalibration();
    
    // Init textur
    textureSize = GLMacros::checkTextureSize();
    if (textureSize == 0) {
        LOG4CPLUS_FATAL(logger, "Cannot accomodate texture, shutting down.");
        exit(1);
    }

    LOG4CPLUS_DEBUG( logger, "Assigning Texture of size " << textureSize);
    glGenTextures(1, textures);   /* create the texture names */

    glBindTexture(GL_TEXTURE_2D, textures[0]); /* Bind texture[0] ??? */
    LOG4CPLUS_DEBUG(logger, "Binding NULL texture image...");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
                    GL_RGB, GL_UNSIGNED_BYTE, NULL);
    LOG4CPLUS_DEBUG(logger, "Texture image bound.");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Calculate the size factors
    imageHeight = Ve::getLeftSource()->getHeight();
    imageWidth = Ve::getLeftSource()->getWidth();
    widthFactor = 1.0f - ((double) imageWidth / (double) textureSize);
    heightFactor = 1.0f - ((double) imageHeight / (double) textureSize);
}

CalibrationOverlay::~CalibrationOverlay() {}

void CalibrationOverlay::drawOverlay() {
    GLMacros::initVirtualCoords();

    // glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    if (calibrationMode == LEFT_EYE) {
        sprintf(text[0], "Calibrating left eye (%i snaps)", cCalibrationObject->getSnapshotCount());
    } else {
        sprintf(text[0], "Calibrating right eye (%i snaps)", cCalibrationObject->getSnapshotCount());
    }

    glTranslatef(-1.0f, 0.0f, 0.0f);
    drawOneEye(); // left eye
    /// Current Calibration object
    CameraCalibration* cCalibrationObject;
    glLoadIdentity();
    drawOneEye(); // right eye

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );

    drawOtherEye();

    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

void CalibrationOverlay::drawOtherEye() {
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);
    IplImage* img = cCalibrationObject->lastSnapshot;
    if (img) {
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight,
            GL_RGB, GL_UNSIGNED_BYTE, img->imageData);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    if (Ve::mainVideo->xRot) {
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    }
    if (Ve::mainVideo->yRot) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }
    if (Ve::mainVideo->zRot) {
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    }
    if (calibrationMode == RIGHT_EYE) {
        drawLeftQuad();
    } else {
        drawRightQuad();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Now draw the found corners
    GLMacros::initVirtualCoords();
    CvPoint vCoords = Ve::getVirtualSize();
    float xFac = vCoords.x / (float) imageWidth;
    float yFac = - vCoords.y / (float) imageHeight;
    int yOff = Ve::getVirtualSize().y;
    int count = cCalibrationObject->lastCornerCount;
    if (calibrationMode == RIGHT_EYE) {
        glTranslatef(-1.0f, 0.0f, 0.0f);
    }
    for (int i=0 ; i<cCalibrationObject->lastCornerCount ; i++) {
        int x = (int) (cCalibrationObject->lastCorners[i].x * xFac);
        int y = (int) ((cCalibrationObject->lastCorners[i].y * yFac * 2) + yOff);
        GLMacros::drawMarker(x, y);
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
    case 'v':
    case 'V':
        if (calibrationMode == LEFT_EYE) {
            calibrationMode = RIGHT_EYE;
            cCalibrationObject = Ve::getRightSource()->getCalibration();
        } else {
            calibrationMode = LEFT_EYE;
            cCalibrationObject = Ve::getLeftSource()->getCalibration();
        }
        break;
    case 'x':
    case 'X':
        cCalibrationObject->takeSnapshot();
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
    }

}

void CalibrationOverlay::drawLeftQuad() {
    glBegin(GL_QUADS);
    glTexCoord2d(0.0f, 0.0f);
    glVertex3i(-1, -1, 0);
    glTexCoord2d(1.0f - widthFactor, 0.0f);
    glVertex3i(0, -1, 0);
    glTexCoord2d(1.0f - widthFactor, 1.0f - heightFactor);
    glVertex3i(0, 1, 0);
    glTexCoord2d(0.0f, 1.0f - heightFactor);
    glVertex3i(-1, 1, 0);
    glEnd();
}

void CalibrationOverlay::drawRightQuad() {
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
