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
#include "offsetoverlay.h"

Logger OffsetOverlay::logger = Logger::getInstance("Ve.OffsetOverlay");

OffsetOverlay::OffsetOverlay(bool display)
: Overlay(display), VeEventListener() {
    // Check for imaging subset first
    if (!GLEW_ARB_imaging) {
        LOG4CPLUS_ERROR(logger, "Can not use overlay, imaging extension not supported.");
        return;
    }
    leftEye = Ve::getLeftSource();
    rightEye = Ve::getRightSource();
    if (leftEye == NULL || rightEye == NULL) {
        LOG4CPLUS_ERROR(logger, "Missing video source: Could not create overlay.");
        return;
    }
    imageHeight = leftEye->getHeight();
    imageWidth = leftEye->getWidth();
    if ((leftEye->getHeight() != rightEye->getHeight()) || (leftEye->getWidth() != rightEye->getWidth())) {
        LOG4CPLUS_ERROR(logger, "Error: Image formats for left and right eye do not match. This may be fatal.");
    } else {
        GLint maxTexSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
        LOG4CPLUS_INFO(logger, "Found max tex size: " << maxTexSize);
        int vidSize = (imageWidth > imageHeight)?imageWidth:imageHeight;
        LOG4CPLUS_INFO(logger, "Video input size: " << vidSize);

        bool accomodated = false;
        textureSize = 2;
        while (textureSize <= maxTexSize && (!accomodated)) {
            textureSize = textureSize * 2;
            if (textureSize >= vidSize)
                accomodated = true;
            LOG4CPLUS_TRACE(logger, "Tex size set to " << textureSize);
        }

        LOG4CPLUS_INFO( logger, "Assigning Texture of size " << textureSize);
        glGenTextures(2, textures);   /* create the texture names */

        glBindTexture(GL_TEXTURE_2D, textures[0]); /* Bind texture[0] ??? */
        LOG4CPLUS_DEBUG(logger, "Binding NULL texture image...");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, NULL);
        LOG4CPLUS_DEBUG(logger, "Texture image bound.");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, textures[1]);
        LOG4CPLUS_DEBUG(logger, "Binding NULL texture image...");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, NULL);
        LOG4CPLUS_DEBUG(logger, "Texture image bound");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        LOG4CPLUS_DEBUG(logger, "Texture bind successful");

        // Calculate the size factors
        widthFactor = 1.0f - ((double) imageWidth / (double) textureSize);
        heightFactor = 1.0f - ((double) imageHeight / (double) textureSize);
        LOG4CPLUS_DEBUG(logger, "widthFactor = 1.0 - (" << imageWidth << " / "
                        << textureSize << " ) = " << widthFactor);
        LOG4CPLUS_DEBUG(logger, "heightFactor = 1.0 - (" << imageHeight << " / "
                        << textureSize << ") = " << heightFactor);
    }

    rightEye->addListener(this);
    leftEye->addListener(this);

    LOG4CPLUS_INFO(logger, "Offset Overlay created.");
}

void OffsetOverlay::drawOverlay() { // FIXME: Could use Multitexturing if supported
    LOG4CPLUS_TRACE(logger, "Entering drawOverlay()");
    if (!GLEW_ARB_imaging) { // FIXME: Should rather show on-screen message
        LOG4CPLUS_TRACE(logger, "Offset unusable: Imaging subset not available");
        return;
    }

    glColor3f(1.0f, 1.0f, 1.0f);		// Set normal color
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glLoadIdentity();				// ...and load the Identity Matrix instead
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, 1.0f);	  // go to zFar

    if (Ve::mainVideo->xRot) {
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    }
    if (Ve::mainVideo->yRot) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }
    if (Ve::mainVideo->zRot) {
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    }

    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    glBlendFunc(GL_ONE, GL_ONE);
    // Left Quad
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    drawLeftQuad();
    // Right Quad
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    drawRightQuad();

    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore standard blending FIXME: Do in a central location?
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

bool OffsetOverlay::createOffsetTextures() {
    LOG4CPLUS_DEBUG(logger, "Trying to assign new black offset textures.");
    const IplImage* rightOffset = Ve::getRightSource()->getBlackOffset();
    const IplImage* leftOffset = Ve::getLeftSource()->getBlackOffset();


    // Right texture
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    if (rightOffset != NULL) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight,
                        GL_RGB, GL_UNSIGNED_BYTE, rightOffset->imageData);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    // Left texture
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    if (leftOffset != NULL) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight,
                        GL_RGB, GL_UNSIGNED_BYTE, leftOffset->imageData);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    LOG4CPLUS_INFO(logger, "Texture creation complete");

    return true;
}

void OffsetOverlay::recieveEvent(VeEvent &e) {
    if ((e.getType() == VeEvent::KEYBOARD_EVENT) && ((e.getCode() == 79) || (e.getCode() == 111))) {
        toggleDisplay();
        LOG4CPLUS_DEBUG(logger, "Toggling offset correction");
    }
    if ((e.getType() == VeEvent::MISC_EVENT) && (e.getCode() == VeEvent::OFFSET_UPDATE_CODE)) {
        LOG4CPLUS_DEBUG(logger, "Trying to assign offset correction textures.");
        createOffsetTextures();
    }
}

OffsetOverlay::~OffsetOverlay() {}

void OffsetOverlay::drawLeftQuad() {
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

void OffsetOverlay::drawRightQuad() {
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
