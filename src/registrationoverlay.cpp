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
#include "registrationoverlay.h"

Logger RegistrationOverlay::logger = Logger::getInstance("Ve.RegistrationOverlay");


RegistrationOverlay::~RegistrationOverlay()
{
    delete tmpSensorPoints;
    delete calibPoints;
    delete text[0];
}

RegistrationOverlay::RegistrationOverlay(bool display) : Overlay(display) {
    measureSize = 5;
    measureCount = 0;
    calibPointNum = 5;
    calibPointPos = 0;
    calibPoints = new CvPoint[calibPointNum];
    calibPoints[0].x = 100;
    calibPoints[0].y = 800;
    calibPoints[1].x = 100;
    calibPoints[1].y = -800;
    calibPoints[2].x = 900;
    calibPoints[2].y = 800;
    calibPoints[3].x = 900;
    calibPoints[3].y = -800;
    calibPoints[4].x = 500;
    calibPoints[4].y = 0;
    tmpSensorPoints = new CvPoint3D32f[measureSize];
    mode = LEFT_EYE;
    text[0] = new char[256];
    sprintf(text[0], "No position updates recieved.");
    this->registration = Ve::getLeftSource()->getRegistration();
}

void RegistrationOverlay::measurePoint(int x, int y) {
    LOG4CPLUS_INFO(logger, "Trying to acquire data for point at " << x << "," << y);
    mutex.enterMutex();
    measureCount = 0;
    mutex.leaveMutex();
    while (measureCount != measureSize) {
	Thread::yield();
    }
    mutex.enterMutex();
    // Create observation vector
    gsl_vector* observations = gsl_vector_alloc(measureCount * 3);
    for (int i=0 ; i < measureCount ; i++) {
	gsl_vector_set(observations, i*3, tmpSensorPoints[i].x);
	gsl_vector_set(observations, (i*3)+1, tmpSensorPoints[i].y);
	gsl_vector_set(observations, (i*3)+2, tmpSensorPoints[i].z);
    }
    // Create parameter matrix
    gsl_matrix* params = gsl_matrix_calloc(measureCount*3, 3);
    for (int i=0 ; i < measureCount ; i++) {
	gsl_matrix_set(params, i*3, 0, 1);
	gsl_matrix_set(params, (i*3)+1, 1, 1);
	gsl_matrix_set(params, (i*3)+2, 2, 1);
    }
    // result vector
    gsl_vector* result = gsl_vector_alloc(3);
    // Least squares
    double* chisq;
    gsl_matrix* cov;
    gsl_multifit_linear_workspace* work = gsl_multifit_linear_alloc(measureCount*3, 3);
    gsl_multifit_linear(params, observations, result, cov, chisq, work);
    LOG4CPLUS_DEBUG(logger, "Matched measured points, chi squared was " << *chisq);
    CvPoint2D32f imagePoint;
    imagePoint.x = x;
    imagePoint.y = y;
    CvPoint3D32f sensorPoint;
    sensorPoint.x = gsl_vector_get(result, 0);
    sensorPoint.y = gsl_vector_get(result, 1);
    sensorPoint.z = gsl_vector_get(result, 2);
    registration->addCalibrationPair(imagePoint, sensorPoint);
    // Cleanup
    gsl_vector_free(observations);
    gsl_vector_free(result);
    gsl_matrix_free(params);
    mutex.leaveMutex();
}

void RegistrationOverlay::drawOverlay() {
    GLMacros::initVirtualCoords();

    // Draws left eye
    glTranslatef(-1.0f, 0.0f, 0.0f);
    if (mode == LEFT_EYE) {
	GLMacros::drawCrosshairs(calibPoints[calibPointPos].x, calibPoints[calibPointPos].y);
    }
    
    // Draws right eye
    glTranslatef(1.0f, 0.0f, 0.0f);
    if (mode == RIGHT_EYE) {
	GLMacros::drawCrosshairs(calibPoints[calibPointPos].x, calibPoints[calibPointPos].y);
    }
    
    
    if (mode == LEFT_EYE) {
	GLMacros::blankRightEye();
    } else {
	GLMacros::blankLeftEye();
    }

    GLMacros::revertMatrices();
}



void RegistrationOverlay::recieveEvent(VeEvent &e) {
    mutex.enterMutex();
    if (e.getType() == VeEvent::POSITION_EVENT) {
	VePositionEvent& eP = (VePositionEvent&) e;
	Position pos = eP.getPosition();
	if (measureCount < measureSize) {
	    tmpSensorPoints[measureCount].x = pos.x;
	    tmpSensorPoints[measureCount].y = pos.y;
	    tmpSensorPoints[measureCount].z = pos.z;
	    measureCount++;
	}
	sprintf(text[0], "Last sensor position recieved from CORBA: x:%i y:%i z%i", pos.x, pos.y, pos.z);
	LOG4CPLUS_DEBUG(logger, "Recieved position event (" << pos.x << "," << pos.y << "," << pos.z << ")");
    }
    if (e.getType() == VeEvent::KEYBOARD_EVENT) {
	switch (e.getCode()) {
	    case 'r':
	    case 'R':
		toggleDisplay();
	    break;
	    case 't':
	    case 'T': 
		measurePoint(calibPoints[calibPointPos].x, calibPoints[calibPointPos].y);
		calibPointPos = (calibPointPos + 1) % calibPointNum;
	    break;
	    case 'e':
	    case 'E': 
		if (mode == LEFT_EYE) {
		    mode = RIGHT_EYE;
		    registration = Ve::getRightSource()->getRegistration();
		} else {
		    mode = LEFT_EYE;
		    registration = Ve::getLeftSource()->getRegistration();
		}
	    break;
	    case 'w':
	    case 'W':
		registration->reRegister();
	    break;
	}
    }
    mutex.leaveMutex();
}
