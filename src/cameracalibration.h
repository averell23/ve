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
#include <vector>
#include <iostream>
#include <log4cplus/logger.h>

using namespace std;
using namespace log4cplus;

// Forward declaration
class VideoSource;

/**
This contains all calibration data for a camera. The camera can be calibrated 
by using pictures of a simple chessboard pattern. 
The data is acquired either by calibrating the camera using OpenCV, 
or by loading the data from disk. 
 
@author Daniel Hahn,,,
@see OpenCV documentation for more infos
*/
class CameraCalibration {
public:
    /** Create a new calibration object.
       @param input The video source that will be calibrated. 
       @param filename Name of a file from which to read the initial calibration
                       Defaults to empty string.
       @param patternDimension Number of the inner corners in the chessboard field.
                       Defaults to height=8, width=6.
       @param chessSize Size of on chessboard field. Defaults to (2,2).
       @see setPatternDimension
       @see setChessSize
    */
    CameraCalibration(VideoSource *input,
                      string filename = "",
                      CvSize patternDimension = cvSize(5,7),
                      CvSize chessSize = cvSize(2,2));

    /**
      Take a snapshot from the video source. The snapshots are
      stored internally as greyscale images and are used for 
      the calibration.
      
      @return true if the snapshot is successfully taken.
      */
    bool takeSnapshot();

    /**
      Delete the internally stored snapshots.
    */
    void deleteSnapshots();

    /**string
      Recalibrate the camera from the internal snapshots.
      
    */
    void recalibrate();

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

    /**
      Sets the number of inner corners of the chessboard pattern. The larger number
      of the dimension is automatically assumed to be the height (number of rows).
      
      @param dimesion Size of the the chessboard in rows/columns.
    */
    void setPatternDimension(CvSize dimension);

    /**
      Sets the size of one chessboard field. The unit used for the size
      will also be the unit for the calibration parameters (e.g. focal length).
    */
    void setChessSize(CvSize size);

    /**
      Gets the number of snapshots.
    */
    int getSnapshotCount() {
        return images.size();
    }

    ~CameraCalibration();

private:
    /** The video source to be calibrated. */
    VideoSource *input;
    /** "Current" filename */
    string filename;
    /** Vector with pointers to the stored calibration images. */
    vector<IplImage*> images;
    /** Vector with pointers to the points found in each image. */
    vector<CvPoint2D32f*> guessedCorners;
    /** Calibration Matrix */
    CvMatr32f calibrationMatrix;
    /** Distortion Coefficients */
    CvVect32f distortions;
    /** Translation vectors for pattern positons. @see OpenCV documentation */
    CvMatr32f translationVects;
    /** Rotation matrices for pattern positions. @see OpenCV documentation */
    CvMatr32f rotationMatrices;
    /** Size of the chessboard pattern */
    CvSize patternDimension;
    /** Size of one chessboard field */
    CvSize chessSize;
    /** If an intrisinc guess is used for the calibration. @see OpenCV documentation */
    bool useIntrisincGuess;

    /**
      Creates a pattern that describes the (3D) positions of the chessboard
      corners. This will use the internal size for the chessboard fields, 
      and creates a set of points for each images that has already been 
      captured. (This means the pattern has to be re-generated if the number
      of pictures changes for some reason).
      
      @return Pointer to the pattern point positions. 
    */
    CvPoint3D32f* generatePattern();

    /**
      Tries to guess the positions of the chessboard corners in the image. 
      
      @return The detected corner positions, or NULL if not all corners were 
              successfully created.
    */
    CvPoint2D32f* guessCorners(IplImage* image);
    /// Logger for this class
    static Logger logger;
};

#endif
