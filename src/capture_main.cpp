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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#ifdef WIN32
#include <windows.h>
#endif
extern "C" {
    #include "xcliball.h"
}
#include "xclibcontroller.h"
#include "capturecontroller.h"
#include "sf1280controller.h"

#include "commandlineparser.h"

using namespace log4cplus;

// FIXME: Evil things with string.c_str() happen in this code. Check other classes as well.
int main(int argc, char *argv[]) {
    cout << "Ve capture utility. (c) 2003 ISAS, author: Daniel Hahn, Kathrin Roberts" << endl;

    PropertyConfigurator::doConfigure("../config/logger.properties");
    Logger logger = Logger::getInstance("Ve.main");

    CommandLineParser parser("ve");
    parser.setupOption("help", "Show usage information");
    parser.setupParameter("epixconf", false, "Epix config file");
    parser.setupParameter("height", false, "Height of images");
    parser.setupParameter("width", false, "Width of images");
    parser.setupParameter("colors", true, "Color model (bayer|rgb|gray)");
    parser.setupParameter("prefix", false, "File prefix, includeing path");
    parser.setupParameter("size", true, "Buffer size in images (per imaging board)");
    parser.setupParameter("initA", false, "Initialization file for camera A");
    parser.setupParameter("initB", false, "Initialization file for camera B");
    parser.setupOption("direct", "Write directly to disk (continous capture, q quits)");
    parser.setupParameter("timestamp", false, "Create a separate timestamp file for each image with this parameter as prefix");
    parser.setupParameter("format", true, "Sets the file format for output (raw|bmp)");
    bool result = parser.parseCommandLine(argc, argv);
    if (!result) {
        cout << endl;
        parser.printUsage();
        exit(1);
    }
    bool option = parser.getOptionValue("help");
    if (option) {
        cout << endl;
        parser.printUsage();
        exit(0);
    }

    CaptureInfo info;

    string param = parser.getParamValue("colors");
    if (param == "bayer") {
        info.colorType = CaptureInfo::IMAGE_BAYER;
    } else if (param == "rgb") {
        info.colorType = CaptureInfo::IMAGE_RGB;
    } else if (param == "gray") {
        info.colorType = CaptureInfo::IMAGE_GRAY;
    } else {
        cout << "Illegal value for colors parameter: " << param;
        parser.printUsage();
        exit(0);
    }

    param = parser.getParamValue("format");
    if (param == "raw") {
        info.fileFormat = CaptureInfo::FILE_FORMAT_RAW;
    } else if (param == "bmp") {
        info.fileFormat = CaptureInfo::FILE_FORMAT_BMP;
    } else {
        cout << "Unknown file format: " << param;
        parser.printUsage();
        exit(0);
    }

    param = parser.getParamValue("height");
    info.height = atoi(param.c_str());
    param = parser.getParamValue("width");
    info.width = atoi(param.c_str());
    param = parser.getParamValue("size");
    int size = atoi(param.c_str());


    string filePrefix = parser.getParamValue("prefix");
    info.filePrefix = (char*) filePrefix.c_str();

    string timeString = parser.getParamValue("timestamp");
    if (timeString == "") {
        info.writeTimestamp = false;
        info.timstampPrefix = "";
    } else {
        info.writeTimestamp = true;
        info.timstampPrefix = (char*) timeString.c_str();
    }

    if (size < 1) {
        LOG4CPLUS_FATAL(logger, "Illegal size: " << size);
        exit(0);
    }



    char* config; // config file name
    param = parser.getParamValue("epixconf");
    if (param == "") {
        LOG4CPLUS_DEBUG(logger, "No epix config file given, using defaults.");
    } else {
        config = (char*) param.c_str();
    }

    if (!XCLIBController::isOpen()) {
        if (XCLIBController::openLib(config) != 0) {
            LOG4CPLUS_FATAL(logger, "Unable to open XCLIB library.");
            exit(0);
        }
    }


    string param1 = parser.getParamValue("initA");
    string param2 = parser.getParamValue("initB");

    EpixCameraController* cont0 = new SF1280Controller(0,param1);
    LOG4CPLUS_DEBUG(logger, "Right video source created");
    EpixCameraController* cont1 = new SF1280Controller(1,param2);
    LOG4CPLUS_DEBUG(logger, "Left video source created");
    cont0->initCamera();
    cont1->initCamera();

    if ((info.height < 1) || (info.height > pxd_imageYdim()))  {
        LOG4CPLUS_WARN(logger, "Illegal value for image height " << info.height << ", setting to " << pxd_imageYdim());
        info.height = pxd_imageYdim();
    }

    if ((info.width < 1) || (info.width > pxd_imageXdim())) {
        LOG4CPLUS_WARN(logger, "Illegal value for image width " << info.width << ", setting to " << pxd_imageXdim());
        info.width = pxd_imageXdim();
    }

    LOG4CPLUS_INFO(logger, "Creating controller with prefix: " << info.filePrefix);
    CaptureController controller(&info, size);

    if (parser.getOptionValue("direct")) {
        controller.startLiveCapture();
        char key = 0;
        while (key != 'q') {
            scanf("%c", &key);
        }
        controller.stopLiveCapture();
        info.readTimer.stop();
        info.writeTimer.stop();
        LOG4CPLUS_INFO(logger, "Read " << info.readTimer.getCount()
                       << " images in " << info.readTimer.getSeconds()
                       << "s" << info.readTimer.getMilis() << "ms, framerate: "
                       << info.readTimer.getFramerate());
        LOG4CPLUS_INFO(logger, "Wrote " << info.writeTimer.getCount()
                       << " images in " << info.writeTimer.getSeconds()
                       << "s" << info.writeTimer.getMilis() << "ms, framerate: "
                       << info.writeTimer.getFramerate());
    } else {
        controller.captureToBuffers();
        controller.writeBuffers();
    }

    XCLIBController::closeLib();

    return EXIT_SUCCESS;
}
