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
#include "fontmanager.h"

Logger FontManager::logger = Logger::getInstance("Ve.FontManager");
FontManager FontManager::myInstance;

FTGLTextureFont* FontManager::internalGetFont(int size) {
    FTGLTextureFont* retVal = NULL;
    int idx = size - 1;
    if (size <= 0) {
	LOG4CPLUS_WARN(logger, "Cannot get font of negativ or zero size.");
    } else {
	if (size > fontsSize) {
	    growFonts(((size+1)-fontsSize)*2);
	}
	if (fonts[idx] == NULL)
	createFont(size);
    }
    return fonts[idx];
}

void FontManager::createFont(int size) {
    if ((size > fontsSize) || (size <= 0)) {
	LOG4CPLUS_WARN(logger, "Can not create font with illegal size " << size);
	return;
    }
    int idx = size - 1;
    fonts[idx] = new FTGLTextureFont(DEFAULT_FONT);
    if (fonts[idx]->Error()) {
        LOG4CPLUS_ERROR(logger, "Font could not be created.");
        fonts[idx] = NULL;
    } else {
        fonts[idx]->FaceSize(size);
        LOG4CPLUS_DEBUG(logger, "Font successfully created.");
    }
}

void FontManager::growFonts(int increment) {
    LOG4CPLUS_DEBUG(logger, "Resizing fonts array.");
    if (increment <= 0) {
	LOG4CPLUS_ERROR(logger, "Warning: Could not resize font, illegal value. Continuing, crash possible.");
	return;
    }// Sanity check
    
    int newSize = fontsSize + increment;
    FTGLTextureFont** tmpFonts = new FTGLTextureFont*[newSize];
    for (int i=0 ; i<fontsSize ; i++) { // Copy old content
	tmpFonts[i] = fonts[i];
    }
    for (int i=fontsSize ; i<newSize ; i++) {
	tmpFonts[i] = NULL;
    }
    delete fonts;
    fonts = tmpFonts;
}
    
FontManager::FontManager() {
    fontsSize = INTIAL_FONTS_SIZE;
    // Create initial font array.
    fonts = new FTGLTextureFont*[fontsSize]; 
    for (int i=0 ; i<fontsSize ; i++) {
	fonts[i] = NULL;
    }
    LOG4CPLUS_DEBUG(logger, "Font manager created.");
}


FontManager::~FontManager() {
    for (int i=0 ; i<fontsSize ; i++) {
	if (fonts[i] != NULL) delete fonts[i];
    }
    delete fonts;
}


