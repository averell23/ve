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

Logger CameraCalibration::logger = Logger::getInstance("Ve.CameraCalibration");

CameraCalibration::CameraCalibration(VideoSource *input,
                                     string filename,
                                     CvSize patternDimension,
                                     CvSize chessSize) {
    CameraCalibration::input = input;
    CameraCalibration::filename = filename;
    if (strcmp(filename.c_str(), "") == true) {
        load(filename);
    }
    setPatternDimension(patternDimension);
    setChessSize(chessSize);
    useIntrisincGuess = false;
    // init calibration values
    distortions = new float[4];
    calibrationMatrix = new float[3*3];
    rotationMatrices = NULL;
    translationVects = NULL;
}

bool CameraCalibration::takeSnapshot() {
    if (input == NULL) { 		// Basic sanity check
        LOG4CPLUS_ERROR(logger, "Could not take snapshot, video source is NULL.");
        return false;
    }
	LOG4CPLUS_DEBUG(logger, "Trying to get snapshot");
    IplImage* capt = input->waitAndGetImage();
    IplImage* grayTmp = cvCreateImage(cvSize( capt->width, capt->height ), IPL_DEPTH_8U, 1);
	LOG4CPLUS_TRACE(logger, "Got image");
    cvCvtColor(capt, grayTmp, CV_BGR2GRAY);
	LOG4CPLUS_TRACE(logger, "Convertedimage");
    delete capt->imageData;
    cvReleaseImageHeader(&capt);
    CvPoint2D32f* corners = guessCorners(grayTmp);
    if (corners != NULL) {
	images.push_back(grayTmp);
	guessedCorners.push_back(corners);
	LOG4CPLUS_DEBUG(logger, "Snapshot taken.");
    } else {
	LOG4CPLUS_DEBUG(logger, "Snapshot not taken, not all corners found.");
    }
    return true;
}

void CameraCalibration::recalibrate() {
    if (images.size() == 0) {
        LOG4CPLUS_DEBUG(logger, "No snapshots available, cannot calibrate.");
        return;
    }
    LOG4CPLUS_DEBUG(logger, "Trying to calibrate camera.");
    int pointCount = patternDimension.width * patternDimension.height;
    // Assemble the array of all guessed corners
    CvPoint2D32f* imagePoints = new CvPoint2D32f[guessedCorners.size() * pointCount];
    int absPointPos = 0;
    for (int imgPos = 0 ; imgPos < guessedCorners.size() ; imgPos++) {
	for (int pointPos = 0 ; pointPos < pointCount ; pointPos++) {
	    imagePoints[absPointPos].x = guessedCorners[imgPos][pointPos].x;
	    imagePoints[absPointPos].y = guessedCorners[imgPos][pointPos].y;
	    absPointPos++;
	}
    }
    // Get the chessboard pattern description
    CvPoint3D32f* objectPoints = generatePattern();
    // Initialize the number of point counter for each image
    int* numPoints = new int[images.size()];
    for (int i = 0 ; i < images.size() ; i++) {
        numPoints[i] = pointCount;
    }
    // The image size
    CvSize imageSize = cvSize(images[0]->width, images[0]->height);
    // Initialize the result 
    if (translationVects != NULL) {
        delete translationVects;
    }
    if (rotationMatrices != NULL) {
        delete rotationMatrices;
    }
    translationVects = new float[3 * images.size()];
    rotationMatrices = new float[3 * 3 * images.size()];

    LOG4CPLUS_DEBUG(logger, "About to call calibration routine.");
    cvCalibrateCamera(images.size(), numPoints, imageSize,
                      imagePoints, objectPoints,
                      distortions, calibrationMatrix,
                      translationVects, rotationMatrices,
                      useIntrisincGuess);
    LOG4CPLUS_DEBUG(logger, "Calibration routine complete");


    delete imagePoints;
    delete objectPoints;
    delete numPoints;

    LOG4CPLUS_INFO(logger, "Camera calibrated.");
    LOG4CPLUS_INFO(logger, "Focal length: " 
		   << calibrationMatrix[0] << " " 
		   << calibrationMatrix[4]);
    LOG4CPLUS_INFO(logger, "Principal Point: "
		   << calibrationMatrix[2] << " "
		   << calibrationMatrix[5]);
    LOG4CPLUS_INFO(logger, "Distortion: " 
		   << distortions[0] << " "
		   << distortions[1] << " "
		   << distortions[2] << " "
		   << distortions[3]);
}

bool CameraCalibration::load(string filename) {
    return false;
}

void CameraCalibration::deleteSnapshots() {
    for ( int i = 0 ; i < images.size() ; i++) {
        cvReleaseImage(&images[i]);
    }
    images.clear();
    for ( int i = 0 ; i < guessedCorners.size() ; i++) {
	delete guessedCorners[i];
    }
    guessedCorners.clear();
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

    LOG4CPLUS_DEBUG(logger, "Calibration pattern created.");

    return objPoints;
}

CvPoint2D32f* CameraCalibration::guessCorners(IplImage* image) {
	LOG4CPLUS_DEBUG(logger, "Guessing corners");
    int pointCount = patternDimension.width * patternDimension.height;
    CvPoint2D32f* tempPoints = new CvPoint2D32f[pointCount + 1];
    IplImage* threshTmp; // Temp Image for corner detection
    int cornerNum; // Number of corners found
    threshTmp = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
    CvSize searchWindow = cvSize(chessSize.width * 2, chessSize.height * 2); // Size of the search window for SubPix
    // FIXME: value's unit not understood
    cornerNum = patternDimension.height * patternDimension.width;
    int found = cvFindChessBoardCornerGuesses(image, threshTmp,
                0, patternDimension,
                tempPoints,
                &cornerNum);

    if (found != 0) {
        cvFindCornerSubPix(image, tempPoints, cornerNum,			// FIXME: Last parameter not understood
                           searchWindow, cvSize(-1, -1),
                           cvTermCriteria(CV_TERMCRIT_EPS|CV_TERMCRIT_ITER,30,0.1));
        LOG4CPLUS_DEBUG(logger, cornerNum << " corners found in calibration image, result was " << found);
    } else {
        LOG4CPLUS_WARN(logger, "Corner find unsuccessful, corner count: " << cornerNum);
        tempPoints = NULL;
    }

    cvReleaseImage(&threshTmp);
    return tempPoints;
}

CameraCalibration::~CameraCalibration() {
    deleteSnapshots();
}

CvPoint2D32f CameraCalibration::unDistortPoint(CvPoint2D32f point) {
    CvPoint2D32f retPoint;
    float r_square = pow(point.x,2) + pow(point.y,2);
    float distortion_factor = distortions[0] * r_square;
    retPoint.x = point.x + (point.x * distortion_factor);
    retPoint.y = point.y + (point.y * distortion_factor);
    
    return retPoint;
}



CvPoint2D32f CameraCalibration::distortPoint(CvPoint2D32f point) {
    CvPoint2D32f retPoint;
    double a = (((double) pow(point.x,2) / (double) pow(point.y,2)) + 1.0f) * distortions[0];
    double coeff_a = 0;
    double coeff_b = 1.0f / a;
    double coeff_c = - (point.y / a);
    double *x1, *x2, *x3;
    int result = 0; // FIXME gsl_poly_solve_cubic(coeff_a, coeff_b, coeff_c, x1, x2, x3);
    if (result == 1) {
	retPoint.y = *x1;
    } else if (result == 3) {
	retPoint.y = *x2; // Should be the "middle" according to GSL docu
    }
    retPoint.x = (point.x / point.y) * retPoint.y;
    
    return retPoint;
}
