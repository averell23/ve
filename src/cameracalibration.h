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
#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H


#include <cv.hpp>		// OpenCV headers
#include "videosource.h"
#include <cstdlib>		// STL string class
#include <iostream>

using namespace std;

/**
This contains all calibration data for a camera. The data is acquired either by calibrating the camera using OpenCV, or by loading the data from disk.

@author Daniel Hahn,,,
*/
class CameraCalibration{
public:
    /** Create a new calibration object.
       @param input The video source that will be calibrated
       @param filename Name of a file from which to read the initial calibration
    */
    CameraCalibration(VideoSource *input, string filename = "");
    
    /**
      Take a snapshot from the video source. All snapshots will
      be used for calibration.
      
      @return true if the snapshot is successfully taken.
      */
    bool takeSnapshot();
    
    /**
      Delete the internally stored snapshots.
    */
    void deleteSnapshots();
    
     /**string
       Recalibrate the camera from the internal snapshots.
       <cstdlib>
       @return true if the calibration was successful.
     */
    bool recalibrate();

    /**
      Returns the camera calibration matrix.
      
      @return The calibration matrix in OpenCV format
    */
    CvMatr32f getCalibrationMatrix();
    
    /**
      Returns the distortion coefficients.
      
      @return The coefficients in OpenCV format
    */
    CvVect32f getDistortion(); // 
    
    /**
      Saves the calibration to a file.
      
      @param filename Name of the file to save to. Defaults to the "current" file.
      @return bool true if the calibration was successfully saved.
    */
    bool save(string filename = "");
    
    /** 
      Loads the calibration from a file. 
      
      @param filename Name of the calibration file. Defaults to the "current" file.
      @return bool true if the calibration was successfully loaded.
    */
    bool load(string filename = "");
    
    ~CameraCalibration();
    
private:
    /** The video source to be calibrated. */
    VideoSource *input;
    /** "Current" filename */
    string filename;
    
};

#endif
