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
#include "dummyoverlay.h"

Logger DummyOverlay::logger = Logger::getInstance("Ve.DummyOverlay");

DummyOverlay::DummyOverlay(bool display) : Overlay(display)
{
    rotation = 0;
}


DummyOverlay::~DummyOverlay()
{
}

void DummyOverlay::drawOverlay() {
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glColor3f(1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glLoadIdentity();				// ...and load the Identity Matrix instead
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();
    
	// glTranslatef(0.0f, 0.0f, 0.5f); // In front of offset, maybe behind other overlays...

    glTranslatef(-0.5f, 0.0f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS); 
	glVertex3f(-0.1f, -0.1f, 0.0f);
	glVertex3f(0.1f, -0.1f, 0.0f);
	glVertex3f(0.1f, 0.1f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	glVertex3f(-0.1f, 0.1f, 0.0f);
    glEnd();
    
    glLoadIdentity();
    glTranslatef(-0.5f + (sin(rotation/5.0)*0.3), cos(rotation/5.0)*0.3, 0.0f);
    glRotatef(rotation*4.0, 0.0f, 0.0f, 1.0f);    
    
    glBegin(GL_TRIANGLES); 
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.1f, 0.1f, 0.0f);
	glVertex3f(0.1f, -0.1f, 0.0f);
    glEnd();

    glLoadIdentity();
    glTranslatef(0.5f - (sin(rotation/5.0)*0.3), cos(rotation/5.0)*0.3, 0.0f);
    glRotatef(rotation*4.0, 0.0f, 0.0f, 1.0f);

    glBegin(GL_TRIANGLES); 
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.1f, 0.1f, 0.0f);
	glVertex3f(0.1f, -0.1f, 0.0f);
    glEnd();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );		
    glPopMatrix();
    
    rotation += 0.8f;
    LOG4CPLUS_TRACE(logger, "Rotation now at " << rotation);
}

