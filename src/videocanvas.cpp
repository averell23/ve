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
#include "videocanvas.h"

VideoCanvas::VideoCanvas(VideoSource *left, VideoSource *right)
{
    leftEye = left;
    rightEye = right;
    imageHeight = leftEye->getHeight();
    imageWidth = leftEye->getWidth();
    if ((leftEye->getHeight() != rightEye->getHeight()) || (leftEye->getWidth() != rightEye->getWidth())) {
	std::cerr << "Error: Image formats for left and right eye do not match. This may be fatal." << std::endl;
    } else {
	GLint maxTexSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
	printf("Found max tex size: %d\n", maxTexSize);
	int vidSize = (imageWidth > imageHeight)?imageWidth:imageHeight;
	printf("Video input size: %d\n", vidSize);
	
	bool accomodated = false;
	textureSize = 2;
	while (textureSize <= maxTexSize && (!accomodated)) {
	    textureSize = textureSize * 2;
	    if (textureSize >= vidSize) accomodated = true;
	    printf("Tex size set %d\n", textureSize);
	}

	printf("Assigning Texture of size %d\n", textureSize);
	glGenTextures(2, textures);   /* create the texture names */

	glBindTexture(GL_TEXTURE_2D, textures[0]); /* Bind texture[0] ??? */
	printf("Binding tex image...");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
		     GL_RGB, GL_UNSIGNED_BYTE, NULL);
	printf("done\n");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	printf("Binding Tex image...");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0,
		     GL_RGB, GL_UNSIGNED_BYTE, NULL);
	printf("done\n");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	printf("Texture bind successful\n");
	
	// Calculate the size factors
	widthFactor = 1.0f - ((double) imageWidth / (double) textureSize);
	heightFactor = 1.0f - ((double) imageHeight / (double) textureSize);
	printf("widthFactor = 1.0 - (%d / %d) = %f\n", imageWidth, textureSize, widthFactor);
	printf("heightFactor = 1.0 - (%d / %d) = %f\n", imageHeight, textureSize, heightFactor);
    }
     
    std::cerr << "Video Canvas created." << std::endl;
}


void VideoCanvas::draw() {
    if (leftEye == NULL || rightEye == NULL) { 	// Sanity check
	return;
    }
    
    glColor3f(1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glLoadIdentity();				// ...and load the Identity Matrix instead
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();
    
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);  // FIXME: Flipping not standard

    // Left Quad
    glBindTexture(GL_TEXTURE_2D, textures[0]);
	IplImage* leftImage = leftEye->getImage();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, 
		    GL_RGB, GL_UNSIGNED_BYTE, leftImage->imageData);
	delete leftImage->imageData;
	cvReleaseImageHeader(&leftImage);
    glBegin(GL_QUADS);
	glTexCoord2d(0.0f, 0.0f);	/* Bottom left */
	glVertex3i(-1, -1, 1);
	glTexCoord2d(1.0f - widthFactor, 0.0f);		/* Bottom right */
	glVertex3i(0, -1, 1);
	glTexCoord2d(1.0f - widthFactor, 1.0f - heightFactor);		/* Top right */
	glVertex3i(0, 1, 1);
	glTexCoord2d(0.0f, 1.0f - heightFactor);		/* Top left */
	glVertex3i(-1, 1, 1);
    glEnd();
    
    // Right Quad
    glBindTexture(GL_TEXTURE_2D, textures[1]);
	IplImage* rightImage = rightEye->getImage();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, 
		    GL_RGB, GL_UNSIGNED_BYTE, rightImage->imageData);
	delete rightImage->imageData;
	cvReleaseImageHeader(&rightImage);
    glBegin(GL_QUADS);
	glTexCoord2d(0.0f, 0.0f);
	glVertex3i(0, -1, 1);
	glTexCoord2d(1.0f - widthFactor, 0.0f);
	glVertex3i(1, -1, 1);
	glTexCoord2d(1.0f - widthFactor, 1.0f - heightFactor);
	glVertex3i(1, 1, 1);
	glTexCoord2d(0.0f, 1.0f - heightFactor);
	glVertex3i(0, 1, 1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0); // FIXME: Symbolic name?
    
    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );		
    glPopMatrix();
}

VideoCanvas::~VideoCanvas()
{
    delete rightEye;
    delete leftEye;
}


