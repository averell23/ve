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
#include "commandlineparser.h"

Logger CommandLineParser::logger = Logger::getInstance("Ve.CommandLineParser");

CommandLineParser::CommandLineParser(string programName, bool ignoreUnknown, bool useConfigFile) {
    CommandLineParser::ignoreUnknown = ignoreUnknown;
    CommandLineParser::programName = programName;
    if (useConfigFile) {
	setupParameter("config", false, "Read configuration from XML file");
    }
}


int CommandLineParser::getOptionValue(string optionName) {
    if (legalOptions.find(optionName) == legalOptions.end())
        return OPTION_ILLEGAL;
    if (options.find(optionName) == options.end())
        return OPTION_UNSET;
    else
        return OPTION_SET;
}

string CommandLineParser::getParamValue(string paramName) {
    return parameters[paramName];
}


void CommandLineParser::setupParameter(string name, bool required, string description) {
    if (legalParameters.find(name) != legalParameters.end())
        return; // Sanity check
    if (required) {
        requiredParameters[name] = false;
    }
    legalParameters[name] = description;
}

void CommandLineParser::setupOption(string name, string description) {
    if (legalOptions.find(name) != legalOptions.end())
        return; // Sanity check
    legalOptions[name] = description;
}

void CommandLineParser::printUsage() {
    cout << "Usage: " << programName << " [options/parameters]" << endl << endl;
    cout << "Options: " << endl;
    for (map<string,string>::iterator pos = legalOptions.begin() ; pos != legalOptions.end() ; pos++) {
        cout << "\t -" << pos->first << "\t" << pos->second << endl;
    }
    cout << endl << "Parameters:" << endl;
    for (map<string,string>::iterator pos = legalParameters.begin() ; pos != legalParameters.end() ; pos++) {
        cout << "\t -" << pos->first << "\t" << pos->second << endl;
    }
    cout << endl;
}

bool CommandLineParser::parseCommandLine(int argc, char *argv[]) {
    programName = argv[0];
    resetRequiredParameters();

    for (int i = 1 ; i < argc ; i++) {
        if (argv[i][0] != '-') {
            if (!ignoreUnknown) {
                cout << "Illegal option: " << argv[i] << endl;
                return false;
            }
        } // Sanity check

        char* current = &argv[i][1];

        if (legalOptions.find(current) != legalOptions.end()) {
            options.insert(current);
            LOG4CPLUS_DEBUG(logger, "Set option " << current);
        } else if (legalParameters.find(current) != legalParameters.end()) {
            if (requiredParameters.find(current) != requiredParameters.end()) {
                requiredParameters[current] = true;
                LOG4CPLUS_DEBUG(logger, "Found required parameter " << current << endl);
            }
            if (++i >= argc) {
                cout << "Must have value for parameter " << current << endl;
                return false;
            }
            parameters[current] = argv[i];
            LOG4CPLUS_DEBUG(logger, "Set parameter " << current << " to " << argv[i]);
        } else {
            if (!ignoreUnknown) {
                cout << "Unknown Option/Parameter: " << current << endl;
                return false;
            }
        }
    } // for

    if (getParamValue("config") != "") readConfigFile(getParamValue("config"));

    for (map<string,bool>::iterator pos = requiredParameters.begin() ; pos != requiredParameters.end() ; pos++) {
        if (!(pos->second)) {
            cout << "Required parameter " << pos->first << " not set." << endl;
            return false;
        }
    }

    return true;
}

void CommandLineParser::resetRequiredParameters() {
    for (map<string,bool>::iterator pos = requiredParameters.begin() ; pos != requiredParameters.end() ; pos++) {
        pos->second = false;
    }
    LOG4CPLUS_DEBUG(logger, "Required Parameters reset");
}

void CommandLineParser::readConfigFile(string filename) {
    XMLCh tmpStr[256];

    xercesc::DOMDocument* doc = XMLMacros::getInstance().XMLReadFile(filename);
    
    if (!doc) {
	LOG4CPLUS_ERROR(logger, "Unable to load configuration from: " << filename);
	return;
    }
    
    xercesc::DOMElement* root = doc->getDocumentElement();
    const XMLCh* nStr = root->getTagName();
    xercesc::XMLString::transcode("config", tmpStr, 255);
    if (xercesc::XMLString::compareString(nStr, tmpStr)) {
        char* nChr = xercesc::XMLString::transcode(nStr);
        LOG4CPLUS_ERROR(logger, "Could not load. Unknown root element: " << nChr);
        xercesc::XMLString::release(&nChr);
        return;
    }
    
    xercesc::XMLString::transcode("option", tmpStr, 255);
    xercesc::DOMNodeList* nodeList = root->getChildNodes();
    XMLCh optionStr[7];
    xercesc::XMLString::transcode("option", optionStr, 6);
    XMLCh parameterStr[10];
    xercesc::XMLString::transcode("parameter", parameterStr, 9);
    XMLCh nameStr[5];
    xercesc::XMLString::transcode("name", nameStr, 4);
    XMLCh valueStr[6];
    xercesc::XMLString::transcode("value", valueStr, 5);
    XMLCh trueStr[5];
    xercesc::XMLString::transcode("true", trueStr, 4);
    for (XMLSize_t i = 0 ; i < nodeList->getLength() ; i++) {
        xercesc::DOMNode* curNode = nodeList->item(i);
	if (!xercesc::XMLString::compareString(curNode->getNodeName(), optionStr)) {
	    const XMLCh* optionNameCh = XMLMacros::getInstance().getAttributeByName(curNode, nameStr);
	    char* optionName = xercesc::XMLString::transcode(optionNameCh);
	    const XMLCh* optionValue = XMLMacros::getInstance().getAttributeByName(curNode, valueStr);
	    if ((!xercesc::XMLString::compareString(optionValue, trueStr)) 
		&& (legalOptions.find(optionName) != legalOptions.end()) 
		&& (options.find(optionName) == options.end()))
	    {
		    options.insert(optionName);
		    LOG4CPLUS_DEBUG(logger, "Set option from config file: " << optionName);
	    }
	    xercesc::XMLString::release(&optionName);
	}
	if (!xercesc::XMLString::compareString(curNode->getNodeName(), parameterStr)) {
	    const XMLCh* paramNameCh = XMLMacros::getInstance().getAttributeByName(curNode, nameStr);
	    char* paramName = xercesc::XMLString::transcode(paramNameCh);
	    if ((legalParameters.find(paramName) != legalParameters.end()) &&
		(parameters.find(paramName) == parameters.end()))
	    {
		    if (requiredParameters.find(paramName) != requiredParameters.end()) {
		        requiredParameters[paramName] = true;
		        LOG4CPLUS_DEBUG(logger, "Found required parameter in config file: " << paramName << endl);
		    }
		    const XMLCh* content = XMLMacros::getInstance().getTextChild(curNode);
		    char* paramVal = xercesc::XMLString::transcode(content);
		    parameters[paramName] = paramVal;
		    LOG4CPLUS_DEBUG(logger, "Set parameter from config file: " << paramName << " to " << paramVal);
		    xercesc::XMLString::release(&paramVal);
	    }
	    xercesc::XMLString::release(&paramName);
	}
    }
}
CommandLineParser::~CommandLineParser() {}



