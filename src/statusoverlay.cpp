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
#include "statusoverlay.h"

Logger StatusOverlay::logger = Logger::getInstance("Ve.StatusOverlay");

StatusOverlay::StatusOverlay(bool display)
: Overlay(display) {
    rightSource = Ve::getRightSource();
    leftSource = Ve::getLeftSource();
    rightTimer = Ve::getRightSource()->getTimer();
    leftTimer = Ve::getLeftSource()->getTimer();
    videoTimer = Ve::getTimer();
    text = new char[256];
    text2 = new char[256];
    LOG4CPLUS_INFO(logger, "Status overlay created");
}

void StatusOverlay::drawOverlay() {
    glColor3f(1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glOrtho(-1000, 1000, -1000, 1000, 0, 1);	// ...and load the Identity Matrix instead
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();

    // glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    sprintf(text, "%f/%f/%f", leftTimer->getFramerate(), rightTimer->getFramerate(), videoTimer->getFramerate());
    sprintf(text2, "Video Brightness: %d%%/%d%%", leftSource->getBrightness(), rightSource->getBrightness());

    glTranslatef(-1.0f, 0.0f, 0.0f);
    drawOneEye();
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f,  1.0f);
    drawOneEye();

    // Remove text textures
    glBindTexture(GL_TEXTURE_2D, 0);
    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

void StatusOverlay::drawOneEye() {
    font = FontManager::getFont();
    if (font == NULL)
        return; // Sanity check

    glTranslatef(0.05f, 0.8f, 0.0f);
    font->Render(text);
    glTranslatef(0.0f, -1.4f, 0.0f);
    font->Render(text2);
    glTranslatef(0.0f, -0.1f, 0.0f);
    font->Render("Status Display active");
}

void StatusOverlay::recieveEvent(VeEvent &e) {
    if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 32)) {
        toggleDisplay();
        LOG4CPLUS_DEBUG(logger, "Recieved keyboard event, toggling display");
    }
}

StatusOverlay::~StatusOverlay() {
    delete text;
    delete text2;
}


