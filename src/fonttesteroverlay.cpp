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
#include "fonttesteroverlay.h"

FontTesterOverlay::FontTesterOverlay()
{
    cout << "Creating font" << endl;
    font = new FTGLPolygonFont( "/usr/share/fonts/truetype/ttf-bitstream-vera/VeraIt.ttf");
    cout << "Yup." << font << endl;
    if (font->Error()) {
	cout << "Duh!" << endl;
	delete font;
	font = NULL;
    } else {
	font->FaceSize(100);
    }
    cout << "Font tester created" << endl;
}


FontTesterOverlay::~FontTesterOverlay()
{
}

void FontTesterOverlay::draw() {
    glColor3f(1.0f, 1.0f, 1.0f);		/* Set normal color */
    glMatrixMode( GL_MODELVIEW );		// Select the ModelView Matrix...
    glPushMatrix();				// ...push the Matrix for backup...
    glOrtho(-1000, 1000, -1000, 1000, 0, 1);	// ...and load the Identity Matrix instead
    glMatrixMode( GL_PROJECTION );		// ditto for the Projection Matrix
    glPushMatrix();
    glLoadIdentity();
    
    glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    glTranslatef(-0.5f, 0.0f, 0.0f);
    
    if (font != NULL) {
	font->Render("Hello World");
    }
    
    // Restore Matrices
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );		
    glPopMatrix();
}