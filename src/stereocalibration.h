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
#ifndef STEREOCALIBRATION_H
#define STEREOCALIBRATION_H

#include "ve.h"
#include "cameracalibration.h"

// Forward declarations
class CameraCalibration;

/**
Allows stereo and intrisinc calibration of a stereo camera pair.

@author Daniel Hahn,,,
*/
class StereoCalibration{
public:
    
    /**
      Creates a new StereoCalibration Object. Any stored snapshots in the
      two calibration objects will be deleted.
    */
    StereoCalibration(CameraCalibration* left, CameraCalibration* right);

    /// Get the left calibration object. 
    CameraCalibration* getLeftCalibration() { return left; }
    
    /// Get the right calibration object.
    CameraCalibration* getRightCalibration() { return right; }
    
    /**
      Takes a stereo snapshot for both calibration objects. 
      
      @return True if both Calibration objects successfully acquired
                   a snapshot.
    */
    bool takeSnapshot();
    
    /**
      Deletes all snapshots from both calibration objects.
    */
    void deleteSnapshots();
    
    /**
      Recalibrates the intrinsic camera parameters for both
      cameras and the extrinsic transformation from the left
      camera's world coordinates to the right's.
    */
    void recalibrate();
    
    /**
      Get the extrinsic transformation from left camera to
      right camera world coordinates.
    */
    gsl_matrix* getCameraTransformation() { return transMatrix; }
    
    /**
      Gets the internal snapshot count.
    */
    int getSnapshotCount();
    
    ~StereoCalibration();


private:
    /// Calibration object for left camera.
    CameraCalibration* left;
    /// Calibration Object for right camera.
    CameraCalibration* right;
    /// Left to right extrinsic transforamtion
    gsl_matrix* transMatrix;
    /// Logger for this class
    static Logger logger;
    /**
       Create the "observation vector" which contains the extrinsic 
       matrices of the left camera.
       
       @param trans Array of translation vectors
       @param rot Array of rotation matrices
       @int size Number of translation vectors/rotation matrices
    */
    gsl_vector* getExtVector(CvMatr32f trans, CvMatr32f rot, int size);
    
    /**
       Create the "parameter matrix" which contains the extrinsic 
       matrices of the right camera.
       
       @param trans Array of translation vectors
       @param rot Array of rotation matrices
       @int size Number of translation vectors/rotation matrices
    */
    gsl_matrix* getExtMatrix(CvMatr32f trans, CvMatr32f rot, int size);
};

#endif
