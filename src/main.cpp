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

#include "ve.h"

#include "epixsource.h"
#include "dummysource.h"
#include "dummyoverlay.h"
#include "statusoverlay.h"
#include "offsetoverlay.h"
#include "calibrationoverlay.h"
#include "registrationoverlay.h"
#include "commandlineparser.h"
#include "corbacontroller.h"
#include "arregistration.h"
#include "trackertestoverlay.h"
#include "texttrackeroverlay.h"
#include "markerpositiontracker.h"
#include "corbapositiontracker.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

using namespace log4cplus;



int main(int argc, char *argv[]) {
    cout <<
    "Ve Augmented Reality toolbox. (c) 2003 ISAS, author: Daniel Hahn"
    << endl;

    PropertyConfigurator::doConfigure("../config/logger.properties");

    // Initialize all stuff that reads from the command line, before starting our
    // own command line parser
    Ve::initGL(argc, argv);

    CommandLineParser parser("ve", true, true);
    parser.setupOption("help", "Show usage information");
    parser.setupParameter("debug", false,
                          "Force debug level: (trace|debug|info|warn|error|fatal)");
    parser.setupParameter("source", true, "Video source (dummy|epix)");
    parser.setupParameter("epixconf", false,
                          "Epix config file (only with -source epix)");
    parser.setupParameter("initA", false,
                          "Initialization file for camera a (EPIX only)");
    parser.setupParameter("initB", false,
                          "Initialization file for camera b (EPIX only)");
    parser.setupParameter("calibrationL", false,
                          "Calibration parameter file for left camera");
    parser.setupParameter("calibrationR", false,
                          "Calibration parameter file for right camera");
    parser.setupParameter("artthreshold", false, 
                          "Threshold for AR Toolkit detection");
    parser.setupParameter("tracking", false,
                          "Use tracking system: (audio|artoolkit)");
    parser.setupParameter("leftBright", false, 
                          "Brightness of left video channel");
    parser.setupParameter("rightBright", false, 
                          "Brightness of right video channel");
    parser.setupOption("swap", "Swap the video sources");
    parser.setupOption("rotX", "Rotate video images around x axis");
    parser.setupOption("rotY", "Rotate video images around y axis");
    parser.setupOption("rotZ", "Rotate video images around z axis");
    parser.setupOption("statusoverlay", "Use the status overlay");
    parser.setupOption("textoverlay", "User text augmentation overlay");
    parser.setupOption("trackingoverlay", "Use Tracker test overlay");
    parser.setupOption("dummyoverlay", "Use the dummy (demo) overlay");
    parser.setupOption("nocorba", "Disable CORBA system");

    bool result = parser.parseCommandLine(argc, argv);
    if (!result) {
        cout << endl;
        parser.printUsage();
        exit(1);
    }

    if (!parser.getOptionValue("nocorba")) {
        CORBAController corba = CORBAController::getInstance();
        corba.init(argc, argv);
    }

    if (parser.getOptionValue("help")) {
        cout << endl;
        parser.printUsage();
        exit(0);
    }

    string param = parser.getParamValue("debug");
    if (param != "") {
        Logger rootLogger = Logger::getInstance("Ve");
        if (param == "trace") {
            rootLogger.setLogLevel(TRACE_LOG_LEVEL);
            cout << "Log level set to TRACE" << endl;
        } else if (param == "debug") {
            rootLogger.setLogLevel(DEBUG_LOG_LEVEL);
            cout << "Log level set to DEBUG" << endl;
        } else if (param == "info") {
            rootLogger.setLogLevel(INFO_LOG_LEVEL);
            cout << "Log level set to INFO" << endl;
        } else if (param == "warn") {
            rootLogger.setLogLevel(WARN_LOG_LEVEL);
            cout << "Log level set to WARN" << endl;
        } else if (param == "error") {
            rootLogger.setLogLevel(ERROR_LOG_LEVEL);
            cout << "Log level set to ERROR" << endl;
        } else if (param == "fatal") {
            rootLogger.setLogLevel(FATAL_LOG_LEVEL);
            cout << "Log level set to FATAL" << endl;
        } else {
            cout << "Unknown log level: " << param << ", could not set." <<
            endl;
        }
    }

    Logger logger = Logger::getInstance("Ve.main");
    LOG4CPLUS_DEBUG(logger, "Logging initialized");

    VideoSource *left, *right;

    param = parser.getParamValue("source");


    if (param == "dummy") {
        LOG4CPLUS_INFO(logger, "Using dummy video source");
        DummySource dumRight;
        right = &dumRight;
        LOG4CPLUS_DEBUG(logger, "Right video source created");
        DummySource dumLeft;
        left = &dumLeft;
        LOG4CPLUS_DEBUG(logger, "Left video source created");
    } else if (param == "epix") {
        param = parser.getParamValue("epixconf");
        string init1 = parser.getParamValue("initA");
        string init2 = parser.getParamValue("initB");
        LOG4CPLUS_INFO(logger, "Using epix video source");
        if (param == "")
            LOG4CPLUS_DEBUG(logger, "No epix config file given");
        // EpixSource epixLeft(1, EpixSource::CAMERA_1280F, param, init2);
        left = new EpixSource(1, EpixSource::CAMERA_1280F, param, init2);	// FIXME: Try to create on stack.
        LOG4CPLUS_DEBUG(logger, "Left video source created");
        // EpixSource epixRight(0, EpixSource::CAMERA_1280F, param, init1);
        right = new EpixSource(0, EpixSource::CAMERA_1280F, param, init1);
        LOG4CPLUS_DEBUG(logger, "Right video source created");
    } else {
        cout << "Unknown video source: " << param << endl;
        exit(1);
    }

    param = parser.getParamValue("calibrationL");
    if (param != "") {
        left->getCalibration()->setFilename(param);
        LOG4CPLUS_DEBUG(logger, "Left camera parameter file: " << param);
    } else {
        left->getCalibration()->setFilename("default_left_cal.xml");
        LOG4CPLUS_DEBUG(logger,
                        "Left camera parameter file set to default");
    }
    left->getCalibration()->load();
    

    param = parser.getParamValue("calibrationR");
    if (param != "") {
        right->getCalibration()->setFilename(param);
        LOG4CPLUS_DEBUG(logger, "Right camera parameter file: " << param);
    } else {
        right->getCalibration()->setFilename("default_right_cal.xml");
        LOG4CPLUS_DEBUG(logger,
                        "Right camera parameter file set to default.");
    }
    right->getCalibration()->load();

    int leftBrightness = 50;
    int rightBrightness = 50;
    bool xRot = false;
    bool yRot = false; 
    bool zRot = false;
    bool swap;

    param = parser.getParamValue("leftBright");
    if (param != "") {
        leftBrightness = atoi(param.c_str());
        if (leftBrightness < 0) leftBrightness = 0;
        if (leftBrightness > 100) leftBrightness = 100;
        LOG4CPLUS_DEBUG(logger, "Video brightness for left eye set to " << leftBrightness);
    }
    param = parser.getParamValue("rightBright");
    if (param != "") {
        rightBrightness = atoi(param.c_str());
        if (rightBrightness < 0) rightBrightness = 0;
        if (rightBrightness > 100) rightBrightness = 100;
        LOG4CPLUS_DEBUG(logger, "Video brightness for right eye set to " << rightBrightness);
    }
    if (parser.getOptionValue("swap")) {
        swap = true;
        LOG4CPLUS_DEBUG(logger, "Swapping left and right eye.");
    }
    if (parser.getOptionValue("rotX")) {
        xRot = true;
        LOG4CPLUS_DEBUG(logger, "Rotating image around x axis");
    }
    if (parser.getOptionValue("rotY")) {
        yRot = true;
        LOG4CPLUS_DEBUG(logger, "Rotating image around y axis");
    }
    if (parser.getOptionValue("rotZ")) {
        zRot = true;
        LOG4CPLUS_DEBUG(logger, "Rotating image around z axis");
    }

    Ve::init(left, right, swap, xRot, yRot, zRot, leftBrightness, rightBrightness);
    CameraCalibration *calTmp = Ve::getLeftSource()->getCalibration();
    calTmp->setPatternDimension(cvSize(6, 8));
    calTmp->setChessSize(cvSize(28, 28));
    calTmp = Ve::getRightSource()->getCalibration();
    calTmp->setPatternDimension(cvSize(6, 8));
    calTmp->setChessSize(cvSize(28, 28));

    LOG4CPLUS_DEBUG(logger, "Adding overlays");
    // Create overlays
    OffsetOverlay offset(false);
    Ve::addOverlay(&offset);
    Ve::addListener(&offset);
    if (parser.getOptionValue("dummyoverlay")) {
        LOG4CPLUS_DEBUG(logger, "Using dummy overlay");
        Ve::addOverlay(new DummyOverlay(true));
    }
    string trackParam = parser.getParamValue("tracking");

    VeEventSource* leftTrack;
    VeEventSource* rightTrack;
    bool trackingActive = false;
    if (trackParam == "artoolkit") {
        param = parser.getParamValue("artthreshold");
        int artthresh = 100;
        if (param != "") {
            artthresh = atoi(param.c_str());
            if (artthresh < 0) artthresh = 0;
            if (artthresh > 100) artthresh = 100;
            LOG4CPLUS_DEBUG(logger, "ARToolkit threshold set to " << artthresh);
        }
        LOG4CPLUS_INFO(logger, "Using AR Toolkit tracking functions");
        leftTrack = new MarkerPositionTracker(left,1, artthresh);
        rightTrack = new MarkerPositionTracker(right, 2, artthresh);
        trackingActive = true;
    } else if (trackParam == "audio") {
        CORBAPositionTracker* cTrack = new CORBAPositionTracker(1,2);
	    leftTrack = (VeEventSource*) cTrack;
	    rightTrack = leftTrack;
        CORBAController corba = CORBAController::getInstance();
        corba.addPositionEventListener(cTrack);
        LOG4CPLUS_INFO(logger, "Using audio tracking through CORBA");
        trackingActive = true;
    } else {
        LOG4CPLUS_WARN(logger, "No tracking option given, no tracker was initialized.");
    }
    TrackerTestOverlay testTracker(1,2);
    TextTrackerOverlay tracker(1,2);
    if (trackingActive) {
        if (parser.getOptionValue("trackingoverlay")) {
            LOG4CPLUS_DEBUG(logger, "Using test overlay for tracker.");
            leftTrack->addListener(&testTracker); 
            if (leftTrack != rightTrack) {
                rightTrack->addListener(&testTracker);
            }
            Ve::addOverlay(&testTracker);
            Ve::addListener(&testTracker); 
        }
        if (parser.getOptionValue("textoverlay")) {
            LOG4CPLUS_DEBUG(logger, "Using text augmentation");
            leftTrack->addListener(&tracker);
            if (leftTrack != rightTrack) {
                rightTrack->addListener(&tracker);
            }
            Ve::addOverlay(&tracker);
            Ve::addListener(&tracker); 
        }
    } else {
        LOG4CPLUS_INFO(logger, "Tracker dependent overlays skipped: No tracker active");
    }
    StatusOverlay status(true);	
    if (parser.getOptionValue("statusoverlay")) {
        LOG4CPLUS_DEBUG(logger, "Initializing adding Status overlay.");
        Ve::addOverlay(&status);
    } else {
        LOG4CPLUS_DEBUG(logger, "Status not requested, skipping.");
    }
    Ve::addListener(&status);
    CalibrationOverlay calibration(false);
    Ve::addOverlay(&calibration);
    Ve::addListener(&calibration);
    RegistrationOverlay registration(false);
    if (trackParam == "audio") {
        LOG4CPLUS_DEBUG(logger, "Tracking method " << trackParam
                        << " requires registration, setting up overlay.");
        CORBAController corba = CORBAController::getInstance();
        corba.addPositionEventListener(&registration);
        Ve::addListener(&registration);
        Ve::addOverlay(&registration);
    } else {
        LOG4CPLUS_DEBUG(logger, "Registration overlay skipped: Tracking method"
                        << trackParam << " does not require registration");
    }
    LOG4CPLUS_DEBUG(logger, "Overlays added.");

    Ve::start();

    LOG4CPLUS_INFO(logger, "Shutting down ve.");

    return EXIT_SUCCESS;
}
