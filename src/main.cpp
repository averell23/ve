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
#include "commandlineparser.h"
#include "corbacontroller.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

using namespace log4cplus;



int main(int argc, char *argv[])
{
    cout << "Ve Augmented Reality toolbox. (c) 2003 ISAS, author: Daniel Hahn" << endl;
    
    PropertyConfigurator::doConfigure("../config/logger.properties");
  
    // Initialize all stuff that reads from the command line, before starting our
    // own command line parser
    Ve::initGL(argc, argv);
    CORBAController corba = CORBAController::getInstance();
    corba.init(argc, argv);
	
    CommandLineParser parser("ve");
    parser.setupOption("help", "Show usage information");
    parser.setupParameter("debug", false, "Force debug level: (trace|debug|info|warn|error|fatal)");
    parser.setupParameter("source", true, "Video source (dummy|epix)");
    parser.setupParameter("epixconf", false, "Epix config file (only with -source epix)");
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
    
    string param = parser.getParamValue("debug");
    if (param !=  "") {
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
	    cout << "Unknown log level: " << param << ", could not set." << endl;
	}
    }
    
    Logger logger = Logger::getInstance("Ve.main");
    LOG4CPLUS_DEBUG(logger, "Logging initialized");
    
  VideoSource *left,*right;
  
  param = parser.getParamValue("source");
  if (param == "dummy") {
      LOG4CPLUS_INFO(logger, "Using dummy video source");
      right = new DummySource(); 
      LOG4CPLUS_DEBUG(logger, "Right video source created");
      left = new DummySource();
      LOG4CPLUS_DEBUG(logger, "Left video source created");
  } else if (param == "epix") {
	  param = parser.getParamValue("epixconf");
      LOG4CPLUS_INFO(logger, "Using epix video source");
	  if (param == "") LOG4CPLUS_DEBUG(logger, "No epix config file given");
      right = new EpixSource(1, EpixSource::CAMERA_1280F, param);
      LOG4CPLUS_DEBUG(logger, "Right video source created");
      left = new EpixSource(0, EpixSource::CAMERA_1280F, param);
      LOG4CPLUS_DEBUG(logger, "Left video source created");
  } else {
      cout << "Unknown video source: " << param << endl;
      exit(1);
  }
  
  Ve::init(left, right);
  
  LOG4CPLUS_DEBUG(logger, "Adding overlays");
  // Create overlays
  OffsetOverlay* offset = new OffsetOverlay(false);
  Ve::addOverlay(offset);
  Ve::addListener(offset);
  // Ve::addOverlay(new DummyOverlay(true)); 
  StatusOverlay* status = new StatusOverlay(true);
  Ve::addOverlay(status);
  Ve::addListener(status);
  LOG4CPLUS_DEBUG(logger, "Overlays added.");
  
  Ve::start();

  return EXIT_SUCCESS;
}
