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
#include "markerpositiontracker.h"

Mutex MarkerPositionTracker::mtx;
Logger MarkerPositionTracker::logger = Logger::getInstance("Ve.MarkderPositionTracker");

MarkerPositionTracker::MarkerPositionTracker(VideoSource* source, int sourceID)
 : VeEventSource()
{
    MarkerPositionTracker::sourceID = sourceID;
    MarkerPositionTracker::source = source;
    initARToolkit();
    thresh = 100;
	LOG4CPLUS_INFO(logger, "Created Tracker with source ID " << sourceID);
	start();
	LOG4CPLUS_DEBUG(logger, "Tracker started.");
}


MarkerPositionTracker::~MarkerPositionTracker()
{
}

void MarkerPositionTracker::run() {
	LOG4CPLUS_DEBUG(logger, "Running tracker with source ID " << sourceID);
    ARUint8* imageData;
    ARMarkerInfo* markerInfo;
    int markerNum;
    // Factors to virtual coordinates
    CvPoint vCoords = Ve::getVirtualSize();
    float xFac = vCoords.x / (float) source->getWidth();
    float yFac = - vCoords.y / (float) source->getHeight();
	int yOff = Ve::getVirtualSize().y;
    
    running = true;
    while (running) {
		LOG4CPLUS_TRACE(logger, "Entering");
		imageData = getImageData();
		mtx.enterMutex();
		arDetectMarker(imageData, thresh, &markerInfo, &markerNum);
		LOG4CPLUS_TRACE(logger, "Detected " << markerNum << " markers in image.");
		for (int i=0 ; i<markerNum ; i++) {
			int x = (int) (markerInfo[i].pos[0] * xFac);
			int y = (int) ((markerInfo[i].pos[1] * yFac * 2) + yOff);
			Position pos(i, sourceID, x, y, 0);
			VePositionEvent e(pos);
			postEvent(e);
		} 
		mtx.leaveMutex();
		delete imageData;
    }
}

void MarkerPositionTracker::initARToolkit() {
	mtx.enterMutex();
    // Hardcoded pattern file positions
    char* paramFile = "../config/camera.param";
    int pattNum = 1;
    char* pattFiles[] = { "../config/patterns/hiro.pattern" };
    
    thresh = 100;
    // Screen dimensions
    int height = source->getHeight();
    int width = source->getWidth();
    // Init ARToolkit camera parameters
    ARParam cparam, wparam;
    if (arParamLoad(paramFile, 1, &wparam) < 0) {
		LOG4CPLUS_WARN(logger, "Could not load parameter file: " << paramFile);
    } else {
		LOG4CPLUS_DEBUG(logger, "ARToolkit parameters loaded.");
		arParamChangeSize(&wparam, width, height, &cparam);
		arInitCparam(&cparam);
		LOG4CPLUS_DEBUG(logger, "Camera parameters initialized.");
    }
    // Load patterns
    for (int i = 0 ; i < pattNum ; i++) {
	int pattId = arLoadPatt(pattFiles[i]); // FIXME: Global variables instead of hardcode
	if (pattId < 0) {
	    LOG4CPLUS_WARN(logger, "Pattern could not be loaded: " << pattFiles[i]);
	} else {
	    LOG4CPLUS_DEBUG(logger, "Pattern loaded: " << pattFiles[i]);
	}
    }
	mtx.leaveMutex();
}

ARUint8* MarkerPositionTracker::getImageData() {
    IplImage *origImage = NULL;
    const IplImage *offset = NULL;

    source->lockImage();
    origImage = source->getImage();
    offset = source->getBlackOffset();
	if (offset == NULL) LOG4CPLUS_TRACE(logger, "Got NULL offset");
    int imageDimension = origImage->width * origImage->height;
    ARUint8* retImage = new ARUint8[imageDimension * 4];
    
    if ((origImage->nChannels == 3) && (origImage->depth == 8)) {
		for (int i=0 ; i < imageDimension ; i++) {
			// FIXME: Original image is always assumed to have RGB order
			retImage[i*4] = 255; // Alpha channel
			if (offset != NULL) {
				retImage[(i*4)+1] = origImage->imageData[(i*3)+2] - offset->imageData[(i*3)+2];
				retImage[(i*4)+2] = origImage->imageData[(i*3)+1] - offset->imageData[(i*3)+1];
				retImage[(i*4)+3] = origImage->imageData[i*3] - offset->imageData[i*3];
			} else{
				retImage[(i*4)+1] = origImage->imageData[(i*3)+2];
				retImage[(i*4)+2] = origImage->imageData[(i*3)+1];
				retImage[(i*4)+3] = origImage->imageData[i*3];
			}
	    }
    } else {
		LOG4CPLUS_WARN(logger, "Wront image format for conversion to ARToolkit AGBR.");
    }
    source->releaseImage();
	LOG4CPLUS_TRACE(logger, "Released image");

    return retImage;
}

