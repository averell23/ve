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
#include "trackeroverlay.h"

Logger TrackerOverlay::logger = Logger::getInstance("Ve.TrackerOverlay");
char* TrackerOverlay::paramFile = "../config/camera.param";

TrackerOverlay::TrackerOverlay() {
    thresh = 100;
    // Init ARToolkit camera parameters
    ARParam cparam, wparam;
    if (arParamLoad(paramFile, 1, &wparam) < 0) {
		LOG4CPLUS_WARN(logger, "Could not load parameter file: " << paramFile);
    } else {
		LOG4CPLUS_DEBUG(logger, "ARToolkit parameters loaded.");
		int height = Ve::getLeftSource()->getHeight();
		int width = Ve::getLeftSource()->getWidth();
		arParamChangeSize(&wparam, width, height, &cparam);
		arInitCparam(&cparam);
		LOG4CPLUS_DEBUG(logger, "Camera parameters initialized.");
    }
    // Load patterns
    int pattId = arLoadPatt("../config/patterns/hiro.pattern"); // FIXME: Global variables instead of hardcode
    if (pattId < 0) {
		LOG4CPLUS_WARN(logger, "Pattern could not be loaded.");
	} else {
		LOG4CPLUS_DEBUG(logger, "Pattern loaded.");
	}
}

TrackerOverlay::~TrackerOverlay()
{
}


void TrackerOverlay::drawOverlay() {
    int markerNum;
    ARUint8* imageData = getImageData(LEFT);
    arDetectMarker(imageData, thresh, &markerInfo, &markerNum);
    LOG4CPLUS_TRACE(logger, "Detected " << markerNum << " markers in image.");
    delete imageData;
}

ARUint8* TrackerOverlay::getImageData(int leftOrRight) {
    IplImage* origImage = NULL;
    if (leftOrRight == LEFT) {
	origImage = Ve::getLeftSource()->waitAndGetImage();
    } else {
	origImage = Ve::getRightSource()->waitAndGetImage();
    }
    int imageDimension = origImage->width * origImage->height;
    ARUint8* retImage = new ARUint8[imageDimension * 4];
    for (int i=0 ; i < imageDimension ; i++) {
	// FIXME: Original image is always assumed to have RGB order
	retImage[i*4] = 0; // Alpha channel
	retImage[(i*4)+1] = origImage->imageData[(i*3)+2];
	retImage[(i*4)+2] = origImage->imageData[(i*3)+1];
	retImage[(i*4)+3] = origImage->imageData[i*3];
    }
    
    return retImage;
}