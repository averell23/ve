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
#include "sf1280controller.h"

Logger SF1280Controller::logger = Logger::getInstance("1280Controller");

SF1280Controller::SF1280Controller(int unit)
 : EpixCameraController(unit)
{
    if (! XCLIBController::initCamLinkSerial(unit)) {
	LOG4CPLUS_WARN(logger, "Unable to open Cam Link serial, controller not usable.");
	unit = -1;
    }
}

bool SF1280Controller::initCamera() {
    if (unit == -1) {
	LOG4CPLUS_WARN(logger, "Camera init not done, Cam Link serial not usable.");
	return false;
    } 
    string retVal = XCLIBController::writeCamLinkSerial(unit, "s\r");
    LOG4CPLUS_DEBUG(logger, "Camera status message: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "ly0005\r");
    LOG4CPLUS_DEBUG(logger, "Setting rolling shutter mode, camera response: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "lc35e709\r");
    LOG4CPLUS_DEBUG(logger, "Setting clock, camera response: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "ly804d\r");
    LOG4CPLUS_DEBUG(logger, "Setting gain, camera response: " << retVal);
    
    return true; /// FIXME: No error detection done
}

bool SF1280Controller::setGain(float gain) {
    if (unit == -1) {
	LOG4CPLUS_WARN(logger, "Camera init not done, Cam Link serial not usable.");
	return false;
    } 
    string command;
    if (gain <= 1.00) {
	command = "ly8050\r";
    } else if (gain <= 1.41) {
	command = "ly8040\r";
    } else if (gain <= 1.62) {
	command = "ly8041\r";
    } else if (gain <= 1.88) {
	command = "ly8042\r";
    } else if (gain <= 2.17) {
	command = "ly8043\r";
    } else if (gain <= 2.52) {
	command = "ly8044\r";
    } else if (gain <= 2.92) {
	command = "ly8045\r";
    } else if (gain <= 3.38) {
	command = "ly8046\r";
    } else if (gain <= 3.90) {
	command = "ly8047\r";
    } else if (gain <= 4.50) {
	command = "ly8048\r";
    } else if (gain <= 5.17) {
	command = "ly8049\r";
    } else if (gain <= 5.93) {
	command = "ly804a\r";
    } else if (gain <= 6.76) {
	command = "ly804b\r";
    } else if (gain <= 7.86) {
	command = "ly804c\r";
    } else if (gain <= 8.64) {
	command = "ly804d\r";
    } else if (gain <= 9.61) {
	command = "ly804e\r";
    } else {
	command = "ly804f\r";
    }
    string response = XCLIBController::writeCamLinkSerial(unit, command);
    LOG4CPLUS_DEBUG(logger, "Reset gain, camera response: " << response);
    
    return true;
}

SF1280Controller::~SF1280Controller()
{
}


