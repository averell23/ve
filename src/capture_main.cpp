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

#include "commandlineparser.h"

using namespace log4cplus;

int main(int argc, char *argv[])
{ 
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

	int colorMode; // Color mode 
	string param = parser.getParamValue("colors");
	if (param == "bayer") {
		colorMode = CaptureController::IMAGE_BAYER;
	} else if (param == "rgb") {
		colorMode = CaptureController::IMAGE_RGB;
	} else if (param == "gray") {
		colorMode = CaptureController::IMAGE_GRAY;
	} else {
		cout << "Illegal value for colors parameter: " << param;
		parser.printUsage();
		exit(0);
	}

	param = parser.getParamValue("height");
	int height = atoi(param.c_str());
	param = parser.getParamValue("width");
	int width = atoi(param.c_str());
	param = parser.getParamValue("size");
	int size = atoi(param.c_str());

	if (size < 1) {
		LOG4CPLUS_FATAL(logger, "Illegal size: " << size);
		exit(0);
	}

	string filePrefix = parser.getParamValue("prefix");

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

	if ((height < 1) || (height > pxd_imageYdim()))  {
		LOG4CPLUS_WARN(logger, "Illegal value for image height " << height << ", setting to " << pxd_imageYdim());
		height = pxd_imageYdim();
	}

	if ((width < 1) || (width > pxd_imageXdim())) {
		LOG4CPLUS_WARN(logger, "Illegal value for image width " << width << ", setting to " << pxd_imageXdim());
		width = pxd_imageXdim();
	}

	LOG4CPLUS_INFO(logger, "Creating controller with prefix: " << filePrefix);
	CaptureController controller(height, width, size, colorMode, (char*) filePrefix.c_str());

	controller.captureToBuffer();
	controller.writeBuffer();

	XCLIBController::closeLib();
	
	return EXIT_SUCCESS;
}
