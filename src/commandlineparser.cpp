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

CommandLineParser::CommandLineParser(string programName)
{
    CommandLineParser::programName = programName;
}


string CommandLineParser::getParamValue(string paramName) {
    return parameters[paramName];
}

int CommandLineParser::getOptionValue(string optionName) {
    if (legalOptions.find(optionName) == legalOptions.end()) return OPTION_ILLEGAL;
    if (options.find(optionName) == options.end()) 
	return OPTION_UNSET;
    else
	return OPTION_SET;
}


void CommandLineParser::setupParameter(string name, bool required, string description) {
    if (legalParameters.find(name) != legalParameters.end()) return; // Sanity check
    if (required) {
	requiredParameters[name] = false;
    }
    legalParameters[name] = description;
}

void CommandLineParser::setupOption(string name, string description) {
    if (legalOptions.find(name) != legalOptions.end()) return; // Sanity check
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
	    cout << "Illegal option: " << argv[i] << endl;
	    return false;
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
	    cout << "Unknown Option/Parameter: " << current << endl;
	    return false;
	}
    } // for
    
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

CommandLineParser::~CommandLineParser()
{
}


