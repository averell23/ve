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
    blankEye = false;
    calibrationMode = LEFT_EYE;
    cCalibrationObject = Ve::getLeftSource()->getCalibration();
}

CalibrationOverlay::~CalibrationOverlay() {}

void CalibrationOverlay::drawOverlay() {
    glColor3f(1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glOrtho(-1000, 1000, -1000, 1000, 0, 1);
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();


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


    if (blankEye)
        blankOtherEye();

    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

void CalibrationOverlay::blankOtherEye() {
    glLoadIdentity();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (calibrationMode == LEFT_EYE) {
        glBegin(GL_QUADS);
        glVertex3i(-1000, -1000, 0);
        glVertex3i(0, -1000, 0);
        glVertex3i(0, 1000, 0);
        glVertex3i(-1000, 1000, 0);
        glEnd();
    } else {
        glBegin(GL_QUADS);
        glVertex3i(0, -1000, 0);
        glVertex3i(1000, -1000, 0);
        glVertex3i(1000, 1000, 0);
        glVertex3i(0, 1000, 0);
        glEnd();
    }
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
    case 'b':
    case 'B':
        blankEye = ! blankEye;
        if (blankEye) {
            LOG4CPLUS_DEBUG(logger, "Blanking other eye");
        } else {
            LOG4CPLUS_DEBUG(logger, "Unblanking other eye");
        }
        break;
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
