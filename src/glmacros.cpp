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
#include "glmacros.h"

Logger GLMacros::logger = Logger::getInstance("Ve.GLMacros");

void GLMacros::initVirtualCoords() {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glOrtho(-Ve::getVirtualSize().x, Ve::getVirtualSize().x, -Ve::getVirtualSize().y, Ve::getVirtualSize().y, 0, 1);
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();
}

void GLMacros::revertMatrices() {
    // Remove text textures
    glBindTexture(GL_TEXTURE_2D, 0);
    // Restore Matrices
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
}


void GLMacros::drawCrosshairs(int x, int y, float width) {
    int xsize = Ve::getVirtualSize().x;
    int ysize = Ve::getVirtualSize().y;
    glLineWidth(width);
    glBegin(GL_LINES);
    // vertical line
    glVertex3i(x, -ysize, 0);
    glVertex3i(x, ysize, 0);
    // horizontal line
    glVertex3i(0, y, 0);
    glVertex3i(xsize, y, 0);
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
void GLMacros::drawMarker(int x, int y, float width) {
    int xsize = Ve::getVirtualSize().x;
    int ysize = Ve::getVirtualSize().y;
    glLineWidth(width);
    glBegin(GL_LINES);
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

void GLMacros::drawLine(int x1, int y1, int x2, int y2, float width) {
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex3i(x1,y1,0);
    glVertex3i(x2,y2,0);
    glEnd();
}

void GLMacros::drawText(int x, int y, char* text, int fontSize) {
    if (fontSize < 2) return; // Sanity check
    FTGLTextureFont* font = FontManager::getFont(fontSize);
    float xTrans = (float) x / Ve::getVirtualSize().x;
    float yTrans = (float) y / Ve::getVirtualSize().y;
    glTranslatef(xTrans, yTrans, 0.0f);
    font->Render(text);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(-xTrans, -yTrans, 0.0f); // Translate back
}

void GLMacros::blankLeftEye() {
    glColor3f(0.0f, 0.0f, 0.0f);
    int xsize = Ve::getVirtualSize().x;
    int ysize = Ve::getVirtualSize().y;
    glBegin(GL_QUADS);
    glVertex3i(-xsize, -ysize, 0);
    glVertex3i(0, -ysize, 0);
    glVertex3i(0, ysize, 0);
    glVertex3i(-xsize, ysize, 0);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
}

void GLMacros::blankRightEye() {
    glColor3f(0.0f, 0.0f, 0.0f);
    int xsize = Ve::getVirtualSize().x;
    int ysize = Ve::getVirtualSize().y;
    glBegin(GL_QUADS);
    glVertex3i(0, -ysize, 0);
    glVertex3i(xsize, -ysize, 0);
    glVertex3i(xsize, ysize, 0);
    glVertex3i(0, ysize, 0);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
}

int GLMacros::checkTextureSize() {
    int imageWidth = Ve::getLeftSource()->getWidth();
    int imageHeight = Ve::getLeftSource()->getHeight();
    GLint maxTexSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    LOG4CPLUS_INFO(logger, "Found max tex size: " << maxTexSize);
    int vidSize = (imageWidth > imageHeight)?imageWidth:imageHeight;
    LOG4CPLUS_INFO(logger, "Video input size: " << vidSize);

    bool accomodated = false;
    int textureSize = 2;
    while (textureSize <= maxTexSize && (!accomodated)) {
        textureSize = textureSize * 2;
        if (textureSize >= vidSize)
            accomodated = true;
        LOG4CPLUS_TRACE(logger, "Tex size set to " << textureSize);
    }

    if (textureSize > maxTexSize) {
        LOG4CPLUS_WARN(logger, "Image larger than maximum texture size.");
        return 0;
    }

    return textureSize;
}

void GLMacros::rotateImage() {
    if (Ve::mainVideo->xRot) {
	    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    }
    if (Ve::mainVideo->yRot) {
	    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }
    if (Ve::mainVideo->zRot) {
	    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	}
}