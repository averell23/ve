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
#include "registrationoverlay.h"

Logger RegistrationOverlay::logger = Logger::getInstance("Ve.RegistrationOverlay");


RegistrationOverlay::~RegistrationOverlay()
{
    delete tmpSensorPoints;
}

RegistrationOverlay::RegistrationOverlay(bool display) : Overlay(display) {
    measureSize = 5;
    measureCount = 0;
    tmpSensorPoints = new CvPoint3D32f[measureSize];
}

void RegistrationOverlay::measurePoint(int x, int y) {
    mutex.enterMutex();
    measureCount = 0;
    mutex.leaveMutex();
    while (measureCount != measureSize) {
	Thread::yield();
    }
    
}

void RegistrationOverlay::drawOverlay() {
    glColor3f(1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glOrtho(-1000, 1000, -1000, 1000, 0, 1);
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(-1.0f, 0.0f, 0.0f);
    drawOneEye(); // left eye
    glLoadIdentity();
    drawOneEye(); // right eye

    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

void RegistrationOverlay::drawOneEye() {
    drawCrosshairs(500, 0);
}

void RegistrationOverlay::drawCrosshairs(int x, int y) {
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

void RegistrationOverlay::recieveEvent(VeEvent &e) {
    mutex.enterMutex();
    if ((e.getType() == VeEvent::POSITION_EVENT) && (measureCount < measureSize)) {
	VePositionEvent& eP = (VePositionEvent&) e;
	Position pos = eP.getPosition();
	tmpSensorPoints[measureCount].x = pos.x;
	tmpSensorPoints[measureCount].y = pos.y;
	tmpSensorPoints[measureCount].z = pos.z;
	LOG4CPLUS_DEBUG(logger, "Recieved position event (" << pos.x << "," << pos.y << "," << pos.z << ")");
	measureCount++;
    }
    mutex.leaveMutex();
}
