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
#include "cameracalibration.h"

CameraCalibration::CameraCalibration(VideoSource *input, 
				     string filename, 
				     CvSize patternDimension,
				     CvSize chessSize)
{
    CameraCalibration::input = input;
    CameraCalibration::filename = filename;
	if (strcmp(filename.c_str(), "") == true) {
		load(filename);
    }
    setPatternDimension(patternDimension);
    setChessSize(chessSize);
    useIntrisincGuess = false;
}

bool CameraCalibration::takeSnapshot() {
    if (input == NULL) { 		// Basic sanity check
		return false;
    }
    IplImage* capt = input->getImage();
    IplImage* grayTmp = cvCreateImage(cvSize( capt->width, capt->height ), IPL_DEPTH_8U, 1);
    cvCvtColor(capt, grayTmp, CV_BGR2GRAY);
    images.push_back(grayTmp);
    return true;
}

bool CameraCalibration::recalibrate() {
    CvPoint2D32f* imagePoints = guessCorners();
    CvPoint3D32f* objectPoints = generatePattern();
    int* numPoints = new int[images.size()];
    CvSize imageSize = cvSize(images[0]->width, images[0]->height);
    if (&translationVects != NULL) {
		delete &translationVects;
    }
    if (&rotationMatrices != NULL) {
		delete &rotationMatrices;
    }
    translationVects = new float[3 * images.size()];
    rotationMatrices = new float[3 * 3 * images.size()];
    
    cvCalibrateCamera(images.size(), numPoints, imageSize,
		      imagePoints, objectPoints,
		      distortions, calibrationMatrix,
		      translationVects, rotationMatrices,
		      useIntrisincGuess);
		      
    
    delete imagePoints;
    delete objectPoints;
    delete numPoints;

	return true; // FIXME (later, since calibration errors are not reported by lib)
}

bool CameraCalibration::load(string filename) {
	return false;
}

void CameraCalibration::deleteSnapshots() {
    for ( int i = 0 ; i < images.size() ; i++) {
	cvReleaseImage(&images[i]);
    }
    images.clear();
}

void CameraCalibration::setPatternDimension(CvSize dimension) {
    if (dimension.height < dimension.width) {
		int temp = dimension.height;
		dimension.height = dimension.width;
		dimension.width = temp;
    }
    patternDimension = dimension;
}

void CameraCalibration::setChessSize(CvSize size) {
    chessSize = size;
}

CvPoint3D32f* CameraCalibration::generatePattern() {
    CvPoint3D32f* objPoints = new CvPoint3D32f[images.size() * patternDimension.width * patternDimension.height];
    int pointPosition = 0; // Position in the point list
    for (int imgNum = 0 ; imgNum < images.size() ; imgNum++) { // count through images
		for (int heightPos = patternDimension.height - 1 ; heightPos >= 0 ; heightPos--) { // count through height
			for (int widthPos = patternDimension.width - 1 ; widthPos >= 0 ; widthPos--) { // count through width
			objPoints[pointPosition].x = heightPos * chessSize.height;
			objPoints[pointPosition].y = widthPos * chessSize.width;
			objPoints[pointPosition].z = 0;
			pointPosition++; // absolute count
			} // for width
		} // for height 
    } // for images
    
    return objPoints;
}

CvPoint2D32f* CameraCalibration::guessCorners() { // FIXME: Could also remove "bad" images. Don't forget docu update if fixed
    if (images.size() == 0) // Sanity check
		return NULL; 
    int pointCount = patternDimension.width * patternDimension.height;
    int pointAbsPos = 0; // Position of current point in all images
    CvPoint2D32f* cornerPoints = new CvPoint2D32f[images.size() * pointCount];
    CvPoint2D32f* tempPoints = new CvPoint2D32f[pointCount + 1];
    IplImage* threshTmp; // Temp Image for corner detection
    int cornerNum; // Number of corners found
    threshTmp = cvCreateImage(cvSize(images[0]->width, images[0]->height), IPL_DEPTH_8U, 1);
    CvSize searchWindow = cvSize(chessSize.width * 2, chessSize.height * 2); // Size of the search window for SubPix 
                                                                             // FIXME: value's unit not understood
    for (int imgPos = 0 ; imgPos < images.size() ; imgPos++) { // through images
		int found = cvFindChessBoardCornerGuesses(images[imgPos], threshTmp,
							0, patternDimension,
							tempPoints,
							&cornerNum);
		if (found != 0) {
			cvFindCornerSubPix(images[imgPos], tempPoints, cornerNum,			// FIXME: Last parameter not understood
							searchWindow, cvSize(-1, -1),
					cvTermCriteria(CV_TERMCRIT_EPS|CV_TERMCRIT_ITER,30,0.1));
					      
		}
		for (int pointPos = 0 ; pointPos < pointCount ; pointPos++) {
			cornerPoints[pointAbsPos].x = tempPoints[pointPos].x;
			cornerPoints[pointAbsPos].y = tempPoints[pointPos].y;
			pointAbsPos++;
		}
    } // for images
  
    cvReleaseImage(&threshTmp);
    delete tempPoints;
    return cornerPoints;
}

CameraCalibration::~CameraCalibration()
{
    deleteSnapshots();
}


