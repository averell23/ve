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
#include "stereocalibration.h"

Logger StereoCalibration::logger = Logger::getInstance("Ve.StereoCalibration");

StereoCalibration::StereoCalibration(CameraCalibration* left, CameraCalibration* right)
{
    left->deleteSnapshots();
    right->deleteSnapshots();
    StereoCalibration::left = left;
    StereoCalibration::right = right;
    transMatrix = gsl_matrix_alloc(4,4);
}


StereoCalibration::~StereoCalibration()
{
    gsl_matrix_free(transMatrix);
}

bool StereoCalibration::takeSnapshot() {
    bool leftOK = left->takeSnapshot();
    bool rightOK = right->takeSnapshot();
    
    if (!leftOK) {
	if (rightOK) right->popSnapshot();
    } else {
	if (!rightOK) left->popSnapshot();
    }
    
    return leftOK && rightOK;
}

void StereoCalibration::deleteSnapshots() {
    left->deleteSnapshots();
    right->deleteSnapshots();
}

int StereoCalibration::getSnapshotCount() {
    return left->getSnapshotCount();
}

void StereoCalibration::recalibrate() {
    int size = left->getSnapshotCount();
    if (size != right->getSnapshotCount()) {
	LOG4CPLUS_ERROR(logger, "Could not recalibration: Different number of snapshots for left and right");
	return;
    }
    left->recalibrate();
    right->recalibrate();
    CvMatr32f leftTranslate = left->getTranslations();
    CvMatr32f rightTranslate = right->getTranslations();
    CvMatr32f leftRotate = left->getRotations();
    CvMatr32f rightRotate = right->getRotations();
    LOG4CPLUS_DEBUG(logger, "Intrinsic calibration complete, starting stereo calibration.");
    gsl_vector* observ = getExtVector(leftTranslate, leftRotate, size);
    gsl_matrix* params = getExtMatrix(rightTranslate, rightRotate, size);
    gsl_vector* result = gsl_vector_alloc(16);
    gsl_matrix* cov = gsl_matrix_alloc(16,16);
    gsl_multifit_linear_workspace* work = gsl_multifit_linear_alloc(size*4, 16);
    double chisq;
    LOG4CPLUS_DEBUG(logger, "Data structures initialized, now trying least-square fit");
    gsl_multifit_linear(params, observ, result, cov, &chisq, work);
    LOG4CPLUS_DEBUG(logger, "Least squares fit done, chi squared is " << chisq);
    gsl_matrix_view result_mat = gsl_matrix_view_vector(result, 4, 4);
    gsl_matrix_memcpy(transMatrix, &result_mat.matrix);
    gsl_multifit_linear_free(work);
    gsl_matrix_free(cov);
    gsl_vector_free(result);
    gsl_matrix_free(params);
    gsl_vector_free(observ);
    LOG4CPLUS_DEBUG(logger, "Stereo calibration complete");
}

gsl_vector* StereoCalibration::getExtVector(CvMatr32f trans, CvMatr32f rot, int size) {
    gsl_vector* retVal = gsl_vector_alloc(size*16);
    for (int i = 0 ; i<size ; i++) {
	for (int j=0 ; j<3 ; j++) {
	    gsl_vector_set(retVal, (i*16) + (j*3), rot[(i*9) + j]);
	    gsl_vector_set(retVal, (i*16) + (j*3) + 1, rot[(i*9) + (j+3)]);
	    gsl_vector_set(retVal, (i*16) + (j*3) + 2, rot[(i*9) + (j+6)]);
	    gsl_vector_set(retVal, (i*16) + (j*3) + 3, 0);
	}
	for (int j=0 ; j<3 ; j++) {
	    gsl_vector_set(retVal, (i*16) + 12 + j, trans[(i*3) + j]);
	}
	gsl_vector_set(retVal, 15, 1);
    }
    LOG4CPLUS_DEBUG(logger, "External matrix created as vector.");
    return retVal;
}

gsl_matrix* StereoCalibration::getExtMatrix(CvMatr32f trans, CvMatr32f rot, int size) {
    gsl_matrix* retVal = gsl_matrix_calloc(16*size, 16);
    gsl_matrix* extMat = gsl_matrix_calloc(4, 4);
    gsl_matrix_set(extMat, 4, 4, 1);
    for (int i=0 ; i<size ; i++) {
	for (int n=0 ; n<3 ; n++) {
	    for (int j=0 ; j<3 ; j++) {
		gsl_matrix_set(extMat, n, j, rot[(i*9) + (n*3) + j]);
	    }
	    gsl_matrix_set(extMat, n, 4, trans[(i*3) + n]);
	}
	for (int n=0 ; n<4 ; n++) {
	    gsl_matrix_view submat = gsl_matrix_submatrix(retVal, (16*i) + (4*n), 4*n, 4, 4);
	    gsl_matrix_memcpy(&submat.matrix, extMat);
	}
    }
    gsl_matrix_free(extMat);
    LOG4CPLUS_DEBUG(logger, "External matrix created as parameter matrix");
    return retVal;
}
