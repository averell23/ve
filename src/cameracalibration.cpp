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
                                     CvSize patternDimension,
                                     CvSize chessSize) {
    CameraCalibration::input = input;
    setPatternDimension(patternDimension);
    setChessSize(chessSize);
    useIntrisincGuess = false;
    // init calibration values
    distortions = new float[DISTORT_VEC_SIZE];
    calibrationMatrix = new float[CALIB_MATRIX_SIZE*CALIB_MATRIX_SIZE];
    rotationMatrices = NULL;
    translationVects = NULL;
    try { 
		xercesc::XMLPlatformUtils::Initialize(); 
	} catch (const xercesc::XMLException& toCatch) { 
	char* message = xercesc::XMLString::transcode(toCatch.getMessage()); 
	LOG4CPLUS_ERROR(logger, "Could not initialize XMLPlatformUtils:" << message); 
	xercesc::XMLString::release(&message); 
    }
}

void CameraCalibration::setFilename(string filename) {
    CameraCalibration::filename = filename;
}

bool CameraCalibration::takeSnapshot() {
	LOG4CPLUS_DEBUG(logger, "Taking calibration snapshot");
    if (input == NULL) { 		// Basic sanity check
        LOG4CPLUS_ERROR(logger, "Could not take snapshot, video source is NULL.");
        return false;
    }
	LOG4CPLUS_DEBUG(logger, "Trying to get snapshot");

	IplImage* blackOffset = (IplImage*) input->getBlackOffset();
	input->lockImage();
    IplImage* capt = input->getImage();

    if (blackOffset != NULL) {
		CvSize size;
		size.height = capt->height;
		size.width = capt->width;
		IplImage* temp = cvCreateImageHeader(size, capt->depth, capt->nChannels);
		temp->imageData = new char[capt->height * capt->width * capt->depth * capt->nChannels];
		cvSub(capt, blackOffset, temp);
		capt = temp;
		LOG4CPLUS_DEBUG(logger, "Subtracted black offset image.");
    }
    IplImage* grayTmp = cvCreateImage(cvSize( capt->width, capt->height ), IPL_DEPTH_8U, 1);
	LOG4CPLUS_TRACE(logger, "Got image");
    cvCvtColor(capt, grayTmp, CV_BGR2GRAY);

	input->releaseImage();

	LOG4CPLUS_TRACE(logger, "Converted Image");
	if (blackOffset != NULL) {
		delete capt->imageData;
		cvReleaseImageHeader(&capt);
	}

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
    int result = gsl_poly_solve_cubic(coeff_a, coeff_b, coeff_c, x1, x2, x3);
    if (result == 1) {
	retPoint.y = *x1;
    } else if (result == 3) {
	retPoint.y = *x2; // Should be the "middle" according to GSL docu
    }
    retPoint.x = (point.x / point.y) * retPoint.y;
    
    return retPoint;
}

bool CameraCalibration::save(string filename) { 
    if (!filename.compare(""))
	filename = CameraCalibration::filename;
	XMLCh tempStr[256];
	XMLCh tempStr2[256];
	XMLCh tempStr3[256];
    char tempChr[256];
	xercesc::XMLString::transcode("Range", tempStr, 255);
	xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
    xercesc::XMLString::transcode("calibration", tempStr, 255);
	xercesc::DOMDocument* doc = impl->createDocument(0, tempStr, 0);
	xercesc::DOMElement* root = doc->getDocumentElement();
    // Insert Calibration matrix
	xercesc::XMLString::transcode("calibrationMatrix", tempStr, 255);
    xercesc::DOMElement* calibMatrixEl = doc->createElement(tempStr);
    root->appendChild(calibMatrixEl);
    xercesc::XMLString::transcode("value", tempStr, 255);
    for (int i=0 ; i < CALIB_MATRIX_SIZE ; i++) {
	for (int j=0 ; j < CALIB_MATRIX_SIZE ; j++) {
	    xercesc::DOMElement* curElement = doc->createElement(tempStr);
	    sprintf(tempChr, "%u", i);
	    xercesc::XMLString::transcode("x", tempStr2, 255);
	    xercesc::XMLString::transcode(tempChr, tempStr3, 255);
	    curElement->setAttribute(tempStr2, tempStr3);
	    sprintf(tempChr, "%u", j);
	    xercesc::XMLString::transcode("y", tempStr2, 255);
	    xercesc::XMLString::transcode(tempChr, tempStr3, 255);
	    curElement->setAttribute(tempStr2, tempStr3);
	    calibMatrixEl->appendChild(curElement);
	    sprintf(tempChr, "%f", calibrationMatrix[(i*CALIB_MATRIX_SIZE)+j]);
	    xercesc::XMLString::transcode(tempChr, tempStr2, 255);
	    xercesc::DOMText* text = doc->createTextNode(tempStr2);
	    curElement->appendChild(text);
	}
    }
    // Insert distortion coefficients
    xercesc::XMLString::transcode("distortionVector", tempStr, 255);
    xercesc::DOMElement* distortVecEl = doc->createElement(tempStr);
    root->appendChild(distortVecEl);
    xercesc::XMLString::transcode("value", tempStr, 255);
    for (int i=0 ; i<DISTORT_VEC_SIZE ; i++) {
	xercesc::DOMElement* curElement = doc->createElement(tempStr);
	sprintf(tempChr, "%u", i);
	xercesc::XMLString::transcode("pos", tempStr2, 255);
	xercesc::XMLString::transcode(tempChr, tempStr3, 255);
	curElement->setAttribute(tempStr2, tempStr3);
	distortVecEl->appendChild(curElement);
	sprintf(tempChr, "%f", distortions[i]);
	xercesc::XMLString::transcode(tempChr, tempStr2, 255);
	xercesc::DOMText* text = doc->createTextNode(tempStr2);
	curElement->appendChild(text);
    }
    
	xercesc::DOMWriter* serializer = impl->createDOMWriter();
	xercesc::LocalFileFormatTarget target(filename.c_str());
    try {
	serializer->writeNode(&target, *root);
	} catch (const xercesc::XMLException& e) {
	char* msg = xercesc::XMLString::transcode(e.getMessage());
	LOG4CPLUS_ERROR(logger, "Caught XML exception while saving: " << msg);
	xercesc::XMLString::release(&msg);
	doc->release();
	serializer->release();
	return false;
	} catch (const xercesc::DOMException& e) {
	char* msg = xercesc::XMLString::transcode(e.msg);
	LOG4CPLUS_ERROR(logger, "Caught DOM exception while saving: " << msg);
	xercesc::XMLString::release(&msg);
	doc->release();
	serializer->release();
	return false;
    } catch (...) {
	LOG4CPLUS_ERROR(logger, "Caught unknown exception while saving.");
	doc->release();
	serializer->release();
	return false;
    }
    
    doc->release();
    serializer->release();
    return true;
}

bool CameraCalibration::load(string filename) { 
    XMLCh tmpStr[256];
    if (!filename.compare("")) 
	filename = CameraCalibration::filename;
    
	xercesc::XercesDOMParser parser;
	xercesc::HandlerBase errHandler; // Dummy handler
    parser.setErrorHandler(&errHandler);
    
    try {
	parser.parse(filename.c_str());
	} catch (const xercesc::XMLException& e) {
	char* msg = xercesc::XMLString::transcode(e.getMessage());
	LOG4CPLUS_ERROR(logger, "Caught XML exception while loading: " << msg);
	xercesc::XMLString::release(&msg);
	return false;
	} catch (const xercesc::DOMException& e) {
	char* msg = xercesc::XMLString::transcode(e.msg);
	LOG4CPLUS_ERROR(logger, "Caught DOM exception while loading: " << msg);
	xercesc::XMLString::release(&msg);
	return false;
    } catch (...) {
	LOG4CPLUS_ERROR(logger, "Caught unknown exception while loading.");
	return false;
    } 
 
	xercesc::DOMDocument* doc = parser.getDocument();
    xercesc::DOMElement* root = doc->getDocumentElement();
    const XMLCh* nStr = root->getTagName();
    xercesc::XMLString::transcode("calibration", tmpStr, 255);
    if (xercesc::XMLString::compareString(nStr, tmpStr)) {
	char* nChr = xercesc::XMLString::transcode(nStr);
	LOG4CPLUS_ERROR(logger, "Could not load. Unknown root element: " << nChr);
	xercesc::XMLString::release(&nChr);
	return false;
    }
    
    xercesc::DOMNodeList* nList = root->getChildNodes();
    for (XMLSize_t i = 0 ; i < nList->getLength() ; i++) {
	xercesc::DOMNode* curNode = nList->item(i);
	xercesc::XMLString::transcode("calibrationMatrix", tmpStr, 255);
	if (!xercesc::XMLString::compareString(tmpStr, curNode->getNodeName())) {
	    if (!readCalibrationMatrix(curNode->getChildNodes())) {
		LOG4CPLUS_ERROR(logger, "Error when trying to read calibration matrix");
		return false;
	    }
	}
	xercesc::XMLString::transcode("distortionVector", tmpStr, 255);
	if (!xercesc::XMLString::compareString(tmpStr, curNode->getNodeName())) {
	    if (!readDistortionVec(curNode->getChildNodes())) {
		LOG4CPLUS_ERROR(logger, "Error when trying to read distortion vector");
		return false;
	    }
	}
    } // for
} // function

bool CameraCalibration::readCalibrationMatrix(xercesc::DOMNodeList* nodeList) {
    XMLCh tmpStr[256];
    XMLCh tmpStr2[256];
    char* tmpChr;
    bool retVal = true;
    xercesc::XMLString::transcode("value", tmpStr, 255);
    for (XMLSize_t i=0 ; i<nodeList->getLength() ; i++) {
	xercesc::DOMNode* curNode = nodeList->item(i);
	if (!xercesc::XMLString::compareString(tmpStr, curNode->getNodeName())) {
	    int x = -1;
	    int y = -1;
	    double matVal = 0;
	    // try to get attributes
	    xercesc::XMLString::transcode("x", tmpStr2, 255);
	    const XMLCh* nameX = getAttributeByName(curNode, tmpStr2);
	    if (nameX) {
		tmpChr = xercesc::XMLString::transcode(nameX);
		x = atoi(tmpChr);
		xercesc::XMLString::release(&tmpChr);
	    }
	    xercesc::XMLString::transcode("y", tmpStr2, 255);
	    const XMLCh* nameY = getAttributeByName(curNode, tmpStr2);
	    if (nameY) {
		tmpChr = xercesc::XMLString::transcode(nameY);
		y = atoi(tmpChr);
		xercesc::XMLString::release(&tmpChr);
	    }
	    const XMLCh* valCh = getTextChild(curNode);
	    if (valCh) {
		tmpChr = xercesc::XMLString::transcode(valCh);
		matVal = atof(tmpChr);
		xercesc::XMLString::release(&tmpChr);
	    }
	    if ((x>=0) && (x<CALIB_MATRIX_SIZE) && (y>=0) && (y<CALIB_MATRIX_SIZE)) {
		calibrationMatrix[(x*CALIB_MATRIX_SIZE)+y] = matVal;
		LOG4CPLUS_TRACE(logger, "Set calibration matrix element at [" 
				        << x << "," << y << "] to " << matVal);
	    } else {
		LOG4CPLUS_WARN(logger, "Could not set Matrix element");
		retVal = false;
	    }
	} else {
	    tmpChr = xercesc::XMLString::transcode(curNode->getNodeName());
	    LOG4CPLUS_DEBUG(logger, "Encountered unknown node with name: " << tmpChr);
	    xercesc::XMLString::release(&tmpChr);
	} // if
    } // for
    return retVal;
}

bool CameraCalibration::readDistortionVec(xercesc::DOMNodeList* nodeList) {
    XMLCh tmpStr[256];
    XMLCh tmpStr2[256];
    char* tmpChr;
    bool retVal = true;
    xercesc::XMLString::transcode("value", tmpStr, 255);
    for (XMLSize_t i=0 ; i<nodeList->getLength() ; i++) {
	xercesc::DOMNode* curNode = nodeList->item(i);
	if (!xercesc::XMLString::compareString(tmpStr, curNode->getNodeName())) {
	    int pos = -1;
	    double distVal = 0;
	    xercesc::XMLString::transcode("pos", tmpStr2, 255);
	    const XMLCh* namePos = getAttributeByName(curNode, tmpStr2);
	    if (namePos) {
		tmpChr = xercesc::XMLString::transcode(namePos);
		pos = atoi(tmpChr);
		xercesc::XMLString::release(&tmpChr);
	    }
	    const XMLCh* valCh = getTextChild(curNode);
	    if (valCh) {
		tmpChr = xercesc::XMLString::transcode(valCh);
		distVal = atof(tmpChr);
		xercesc::XMLString::release(&tmpChr);
	    }
	    if ((pos>=0) && (pos<DISTORT_VEC_SIZE)) {
		distortions[pos] = distVal;
		LOG4CPLUS_TRACE(logger, "Set distortion value " << pos << " to " << distVal);
	    } else {
		LOG4CPLUS_WARN(logger, "Could not set distortion vector element.");
		retVal = false;
	    }
	} else {
	    char* tmpChr = xercesc::XMLString::transcode(curNode->getNodeName());
	    LOG4CPLUS_DEBUG(logger, "Encountered unknown node with name: " << tmpChr);
	    xercesc::XMLString::release(&tmpChr);
	} // if
    }
    return retVal;
}

const XMLCh* CameraCalibration::getAttributeByName(xercesc::DOMNode* node, XMLCh* name) {
	xercesc::DOMNamedNodeMap* attributes = node->getAttributes();
    if (attributes) {
	xercesc::DOMNode* attr = attributes->getNamedItem(name);
	if (attr) {
	    return attr->getNodeValue();
	} else {
	    char* tmpChr = xercesc::XMLString::transcode(name);
	    LOG4CPLUS_WARN(logger, "Attribute not found: " << name);
	    xercesc::XMLString::release(&tmpChr);
	} 
    } else {
	LOG4CPLUS_WARN(logger, "No attributes found when getting by name.");
    }
}

const XMLCh* CameraCalibration::getTextChild(xercesc::DOMNode* node) {
    xercesc::DOMNodeList* children = node->getChildNodes();
    for (XMLSize_t i=0 ; i<children->getLength() ; i++) {
	xercesc::DOMNode* curChild = children->item(i);
	if (curChild->getNodeType() == xercesc::DOMNode::TEXT_NODE) {
	    return curChild->getNodeValue();
	}
    }
    return NULL;
}
