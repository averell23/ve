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
#ifndef ARREGISTRATION_H
#define ARREGISTRATION_H

#include <cv.hpp>
#include <cstdlib>
#include <vector>

using namespace std;

/**
Contains the routines and data to register a sensor coordinate system to the user's view. 

@author Daniel Hahn,,,
*/
class ARRegistration{
public:


    ~ARRegistration();
    
    /**
      Adds a image plane/sensor coordinate pair of points to the
      internal calibration data.
    */
    void addCalibrationPair(CvPoint2D32f imagePlane, CvPoint3D32f sensor);
    
    /**
      Recalibrates the internal registration matrix with the calibration pairs.
      If not at least 4 calibration pairs are present, the registration matrix
      will be set to the identity matrix.
    */
    void reRegister();
    
    /**
      Clears the internal calibration data.
    */
    void clearCalibrationPoints();
    
    /**
      Transforms a point from sensor coordinates to (ideal) image plane coordinates.
    */
    CvPoint2D32f transformSensorToImage(CvPoint3D32f sensor);
    
    /**
      Transforms a point from (ideal) image plane coordinates to sensor coordinates.
    */
    CvPoint3D32f transformImageToSensor(CvPoint2D32f image);

private:
    /// Vector for the image plane calibration points
    vector<CvPoint2D32f> imagePoints;
    /// Vector for the sensor coordinate calibration points
    vector<CvPoint3D32f> sensorPoints;
};

#endif