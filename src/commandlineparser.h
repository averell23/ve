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
#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <log4cplus/logger.h>
#include <iostream>
#include <cstdlib>
#include <set>

using namespace log4cplus;
using namespace std;

/**
A parser for Command line options. 

@author Daniel Hahn,,,
*/
class CommandLineParser{
public:
    /** 
       Creates a new parser.
       @value programName This should be set to the program name, that
                          is the value of argv[0]. (This is only used for
			  printing the usage information, and will be
			  automatically re-set when parsing a command line).
    */
    CommandLineParser(string programName);
    ~CommandLineParser();
    
    static const int OPTION_UNSET = 0;
    static const int OPTION_SET = 1;
    static const int OPTION_ILLEGAL = 2;
    
    /**
      Retrieves the value of a parameter that 
      was given in the form of 
      -paramName parameter
      
      @return The value of the parameter, or an empty
               string if paramName was not the name
	       of a parameter option.
    */
    string getParamValue(string paramName);
    
    /**
      Retrieves the value of an command line option
      flag. 
      
      @return A value of OPTION_SET or OPTION_UNSET indicating
              if the option was set. If the option is illegal
	      or an other problem occured, OPTION_ILLEGAL is
	      returned.
    **/
    int getOptionValue(string optionName);
    
    /** 
      Sets up a possible parameter. 
      @param name The parameter's name.
      @param required Indicates if the parameter is required.
      @param description A description of the parameter
    */
    void setupParameter(string name, bool required = true, string description = "");
    
    /**
      Set up a possible option.
      @param name The option's name.
      @param description A description of the parameter
    */
    void setupOption(string name, string description = "");

    /** 
      Parses the command line. (This also re-sets the internal
      programName to the value of argv[0])
      
      @return true If the command line was successfully parsed
                   and syntactically correct.
    */
    bool parseCommandLine(int argc, char *argv[]);
    
    /** Prints usage information */
    void printUsage();

private:
    /// The program name
    string programName;
    /// Contains the list of legal options and their descriptions
    map<string, string> legalOptions;
    /// Contains the list of legal parameters, and their descriptions
    map<string, string> legalParameters;
    /// Contains all required parameters
    map<string, bool> requiredParameters;
    /// Contains the options that have been set
    set<string> options;
    /// Contains the parameter/value pairs
    map<string, string> parameters;
    /// Logger for this class
    static Logger logger;
    
    /**
      Resets the list of required Parameters.
    */
    void resetRequiredParameters();
    
};

#endif
