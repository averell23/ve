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
#include "arregistration.h"

Logger ARRegistration::logger = Logger::getInstance("Ve.ARRegistration");

ARRegistration::ARRegistration(VideoSource* source) {
    this->source = source;
    Trans = gsl_matrix_alloc(3,4);
    gsl_matrix_set_identity(Trans);
    T_orig = gsl_matrix_alloc(4,4);
    gsl_matrix_set_identity(T_orig);
}

ARRegistration::~ARRegistration() {
    if (Trans != NULL) {
        gsl_matrix_free(Trans);
    }
    if (T_orig != NULL) {
	    gsl_matrix_free(T_orig);
    }
}

void ARRegistration::addCalibrationPair(CvPoint2D32f imagePlane, CvPoint3D32f sensor) {
    imagePoints.push_back(imagePlane);
    sensorPoints.push_back(sensor);
}

void ARRegistration::clearCalibrationPoints() {
    imagePoints.clear();
    sensorPoints.clear();
}

void ARRegistration::reRegister() {
    LOG4CPLUS_DEBUG(logger, "Re-Registering sensor coordinates.");
    if (imagePoints.size() < 4) {
        LOG4CPLUS_WARN(logger, "Could not proceed with registration, too few registration points.");
        return;
    }
    // Create the observation vector
    gsl_vector* y = gsl_vector_alloc(imagePoints.size() * 3);
    for (int i=0 ; i < imagePoints.size() ; i++) {
        gsl_vector_set(y, i*3, imagePoints[i].x);
        gsl_vector_set(y, (i*3)+1, imagePoints[i].y);
        gsl_vector_set(y, (i*3)+2, 1);
    }
    // calibration matrix
    gsl_matrix* c = getCalibrationMatrix();
    gsl_matrix* c_1 = MatrixUtils::submatrixCopy(c, 0, 0, 2, 3); // Top part
    gsl_matrix* c_3 = MatrixUtils::submatrixCopy(c, 2, 0, 1, 3); // Bottom part
    if (logger.isEnabledFor(TRACE_LOG_LEVEL)) {
        cout << "Calibration matrix: " << endl;
        MatrixUtils::printMatrix(c);
        cout << "c_1 part: " << endl;
        MatrixUtils::printMatrix(c_1);
        cout << "c_3 part: " << endl;
        MatrixUtils::printMatrix(c_3);
    }
    // Create the "master" parameter matrix
    gsl_matrix* params = gsl_matrix_alloc(sensorPoints.size()*3, 12);
    // Fill the parameter matrix
    for (int i=0 ; i < sensorPoints.size() ; i++) {
        gsl_matrix* x_1 = gsl_matrix_calloc(3, 12);
        for (int j=0 ; j < 3 ; j++) { 
	        gsl_matrix_set(x_1, j, j*4, sensorPoints[i].x);
	        gsl_matrix_set(x_1, j, (j*4)+1, sensorPoints[i].y);
	        gsl_matrix_set(x_1, j, (j*4)+2, sensorPoints[i].z);
	        gsl_matrix_set(x_1, j, (j*4)+3, 1);
        } // for rows
        LOG4CPLUS_TRACE(logger, "Sensor matrix initialized.");
        if (logger.isEnabledFor(TRACE_LOG_LEVEL)) {
            cout << "Sensor matrix: " << endl;
            MatrixUtils::printMatrix(x_1);
        }
        // Select the region of the parameter matrix to write into
        gsl_matrix_view sub_top = gsl_matrix_submatrix(params, i*3, 0, 2, 12);
        gsl_matrix_view sub_bottom = gsl_matrix_submatrix(params, (i*3)+2, 0, 1, 12);
        LOG4CPLUS_TRACE(logger, "Submatrices selected.");
	    if (logger.isEnabledFor(TRACE_LOG_LEVEL)) {
	        cout << "Top submatrix:" << endl;
	        MatrixUtils::printMatrix(&sub_top.matrix);
	        cout << "Bottom submatrix:" << endl;
	        MatrixUtils::printMatrix(&sub_bottom.matrix);
	    }
        // Compute the parameter values: c_1 * x_1, c_3 * x_1
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c_1, x_1, 0.0, &sub_top.matrix);
        LOG4CPLUS_TRACE(logger, "First submatrix multiplication.");
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c_3, x_1, 0.0, &sub_bottom.matrix);
        LOG4CPLUS_TRACE(logger, "Submatrix multiplication complete.");
        gsl_matrix_free(x_1);
    } // for sensorPoints
    LOG4CPLUS_DEBUG(logger, "Temp data structures initialized, now trying to do least-squares fit.");
    gsl_vector* T_vec = gsl_vector_alloc(12);
    // Least-square
    gsl_matrix* cov = gsl_matrix_alloc(12,12);
    double chisq;
    gsl_multifit_linear_workspace* work = gsl_multifit_linear_alloc(sensorPoints.size()*3, 12);
    if (logger.isEnabledFor(TRACE_LOG_LEVEL)) {
        cout << "Observation vector is:" << endl;
        MatrixUtils::printVector(y);
        cout << "Parameter matrix is:" << endl;
        MatrixUtils::printMatrix(params);
    }
    gsl_multifit_linear(params, y, T_vec, cov, &chisq, work);
    LOG4CPLUS_DEBUG(logger, "Least squares returned, chi squared is " << chisq);
    if (logger.isEnabledFor(TRACE_LOG_LEVEL)) {
        cout << "Covariance is:" << endl;
        MatrixUtils::printMatrix(cov);
	cout << "Result vector is:" << endl;
	MatrixUtils::printVector(T_vec);
    }
    gsl_multifit_linear_free(work);
    // Recreate the transformation matrix
    gsl_matrix_set(T_orig, 3, 3, 1);
    gsl_matrix_view T_sub_vw = gsl_matrix_submatrix(T_orig, 0, 0, 3, 4);
    gsl_matrix_view T_vec_vw = gsl_matrix_view_vector(T_vec, 3, 4);
    gsl_matrix_memcpy(&T_sub_vw.matrix, &T_vec_vw.matrix);
    if (logger.isEnabledFor(TRACE_LOG_LEVEL)) {
        cout << "Original registration matrix is: " << endl;
        MatrixUtils::printMatrix(T_orig);
    }
    // calculate the final transformation matrix
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c, T_orig, 0.0, Trans);
    /* FIXME: Numerical Stability kludge
    gsl_matrix_set(Trans, 2, 0, 0);
    gsl_matrix_set(Trans, 2, 1, 0);
    gsl_matrix_set(Trans, 2, 2, 0);
     FIXME: Kludge Ends */
    if (logger.isEnabledFor(TRACE_LOG_LEVEL)) {
	    cout << "Registration matrix is now: " << endl;
	    MatrixUtils::printMatrix(Trans);
    }
    // Clean up
    gsl_matrix_free(c);
    gsl_matrix_free(c_1);
    gsl_matrix_free(c_3);
    gsl_vector_free(y);
    gsl_vector_free(T_vec);
    gsl_matrix_free(params);
    gsl_matrix_free(cov);
    LOG4CPLUS_INFO(logger, "Re-Registered Sensors.");
}

void ARRegistration::resetCalibration() {
    gsl_matrix* c = getCalibrationMatrix();
    gsl_matrix_set_identity(T_orig);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c, T_orig, 0.0, Trans);
    gsl_matrix_free(c);
    LOG4CPLUS_INFO(logger, "Registration reset.");
}

gsl_matrix* ARRegistration::getCalibrationMatrix() {
    // Initialize the calibration matrix from the calibration object
    CameraCalibration* calib = source->getCalibration();
    CvMatr32f calibMatrix = calib->getCalibrationMatrix();
    gsl_matrix* c = gsl_matrix_calloc(3,4);
    gsl_matrix_set(c, 0, 0, calibMatrix[0]); // Focus length x
    gsl_matrix_set(c, 0, 1, calibMatrix[1]);
    gsl_matrix_set(c, 0, 2, calibMatrix[2]); // Principal point x
    gsl_matrix_set(c, 1, 0, calibMatrix[3]);
    gsl_matrix_set(c, 1, 1, calibMatrix[4]); // Focus length y
    gsl_matrix_set(c, 1, 2, calibMatrix[5]); // Principal point y
    // Last row of the calibration matrix
    gsl_matrix_set(c, 2, 2, 1);

    return c;
}

CvPoint2D32f ARRegistration::transformSensorToImage(CvPoint3D32f sensor) {
    gsl_matrix* point = gsl_matrix_alloc(4,1);
    CvPoint2D32f retVal;
    gsl_matrix_set(point, 0, 0, sensor.x);
    gsl_matrix_set(point, 1, 0, sensor.y);
    gsl_matrix_set(point, 2, 0, sensor.z);
    gsl_matrix_set(point, 3, 0, 1);
    gsl_matrix* result = gsl_matrix_alloc(3,1);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, Trans, point, 0.0, result);
    retVal.x = gsl_matrix_get(result, 0, 0) / gsl_matrix_get(result, 2, 0);
    retVal.y = gsl_matrix_get(result, 1, 0) / gsl_matrix_get(result, 2, 0);
    gsl_matrix_free(result);
    gsl_matrix_free(point);

    return retVal;
}


void ARRegistration::insertTransformation(gsl_matrix* newTrans) {
    if ((newTrans->size1 != T_orig->size1) || (newTrans->size2 != T_orig->size2)) {
	LOG4CPLUS_ERROR(logger, "Could not insert transformation matrix, new matrix has wrong size.");
	return;
    }
    gsl_matrix* c = getCalibrationMatrix();
    gsl_matrix_memcpy(T_orig, newTrans);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c, T_orig, 0.0, Trans);
    gsl_matrix_free(c);
}
