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

Logger SF1280Controller::logger = Logger::getInstance("Ve.1280Controller");

SF1280Controller::SF1280Controller(int unit, string initFile)
: EpixCameraController(unit) {
    if (! XCLIBController::initCamLinkSerial(unit)) {
        LOG4CPLUS_WARN(logger, "Unable to open Cam Link serial, controller not usable.");
        unit = -1;
    }

	printf("Fkt sf1280 Controller vorbeigekommen\n\n");

	if(initFile!="")
	{specific = 1;
	initFile_cam = initFile;  
	}
	else
	 specific = 0;
}

bool SF1280Controller::initCamera() {
    if (unit == -1) {
        LOG4CPLUS_WARN(logger, "Camera init not done, Cam Link serial not usable.");
        return false;
    }
    string retVal;

	if (specific==0){
    // retVal = XCLIBController::writeCamLinkSerial(unit, "s\r");
    // LOG4CPLUS_DEBUG(logger, "Camera status message: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "c\r");
    LOG4CPLUS_DEBUG(logger, "Setting continuous/command mode, camera response: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "ly0005\r");
    LOG4CPLUS_DEBUG(logger, "Setting rolling shutter mode, camera response: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "lc37cb8f\r");
    LOG4CPLUS_DEBUG(logger, "Setting clock, camera response: " << retVal);
    // retVal = XCLIBController::writeCamLinkSerial(unit, "ly3000\r");
    // LOG4CPLUS_DEBUG(logger, "Setting exposure time: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "ly804c\r");
    LOG4CPLUS_DEBUG(logger, "Setting gain, camera response: " << retVal);
    retVal = XCLIBController::writeCamLinkSerial(unit, "ly9041\r");
    LOG4CPLUS_DEBUG(logger, "Setting Black offset, camera response: " << retVal);
	// Setting full frame AOI FIXME: All this settings should be configurable
	/*
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly127f\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI width, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly23fd\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI height offset, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly4000\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI begin column, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly5000\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI begin row, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly6000\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI begin row, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "c\r");
    LOG4CPLUS_DEBUG(logger, "Setting Continuous mode, camera response: " << retVal);
	*/
	// Sets the AOI
	/*
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly113f\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI width, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly21df\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI height offset, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly40a0\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI begin column, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly5100\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI begin row, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly6100\r");
    LOG4CPLUS_DEBUG(logger, "Setting AOI begin row, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "c\r");
    LOG4CPLUS_DEBUG(logger, "Setting Continuous mode, camera response: " << retVal);
	*/
	// Sets XCAP-like AOI
	
	retVal = XCLIBController::writeCamLinkSerial(unit, "lc306886\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly7000\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly804e\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly9041\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "lya055\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly113f\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly21df\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly4000\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly5000\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly6000\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "c\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly0005\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	retVal = XCLIBController::writeCamLinkSerial(unit, "ly3000\r");
    LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
	}
	else
	{  char s[30];
	   char order[30];
	   int j;
	   char *p_on_c;
	   char *p_on_fn;
	   FILE *f1;

	   p_on_fn = &initFile_cam[0];	  

	   f1 = fopen(p_on_fn,"r");
	   
	   if(f1==NULL)
       {printf("Datei %s nicht existent \n",p_on_fn);}
      
	   while(fgets(s,30,f1)!=NULL){
		   j=0;
	       p_on_c=&s[0];  /*zeigt auf ersten Buchstaben des dateinamens*/
           while(*p_on_c!='\n')
           {order[j]=*p_on_c;
            p_on_c++;
            j++;
           }
		   order[j]='\0';
       printf("gelesener Befehl %s\n",order);
	   retVal = XCLIBController::writeCamLinkSerial(unit,order);
       LOG4CPLUS_DEBUG(logger, "Sent camera command, camera response: " << retVal);
    }/*end while*/
	      
		  
	   fclose(f1);
	}/*end else*/
    
    return true; /// FIXME: No error detection done
}

bool SF1280Controller::setGain(int gain) {
    if (unit == -1) {
        LOG4CPLUS_WARN(logger, "Camera init not done, Cam Link serial not usable.");
        return false;
    }
    string command;
    if (gain <= 6) {
        command = "ly8050\r";
    } else if (gain <= 12) {
        command = "ly8040\r";
    } else if (gain <= 18) {
        command = "ly8041\r";
    } else if (gain <= 24) {
        command = "ly8042\r";
    } else if (gain <= 30) {
        command = "ly8043\r";
    } else if (gain <= 36) {
        command = "ly8044\r";
    } else if (gain <= 42) {
        command = "ly8045\r";
    } else if (gain <= 48) {
        command = "ly8046\r";
    } else if (gain <= 54) {
        command = "ly8047\r";
    } else if (gain <= 60) {
        command = "ly8048\r";
    } else if (gain <= 66) {
        command = "ly8049\r";
    } else if (gain <= 72) {
        command = "ly804a\r";
    } else if (gain <= 78) {
        command = "ly804b\r";
    } else if (gain <= 84) {
        command = "ly804c\r";
    } else if (gain <= 90) {
        command = "ly804d\r";
    } else if (gain <= 95) {
        command = "ly804e\r";
    } else {
        command = "ly804f\r";
    }
    string response = XCLIBController::writeCamLinkSerial(unit, command);
    LOG4CPLUS_DEBUG(logger, "Reset gain to " << gain << " camera response: " << response);

    return true;
}

SF1280Controller::~SF1280Controller() {}


