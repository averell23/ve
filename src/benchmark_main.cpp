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

#include "xclibcontroller.h"
#include "sf1280controller.h"
#include "benchmark.h"
#include "commandlineparser.h"

using namespace log4cplus;

int main(int argc, char *argv[]) {
    cout << "Ve benchmark utility. (c) 2003 ISAS, author: Daniel Hahn, Kathrin Roberts" << endl;
    char* config = "";

    PropertyConfigurator::doConfigure("../config/logger.properties");
    Logger logger = Logger::getInstance("Ve.main");

    CommandLineParser parser("ve");
    parser.setupOption("help", "Show usage information");
    parser.setupParameter("epixconf", false, "Epix config file");
    bool result = parser.parseCommandLine(argc, argv);
    if (!result) {
        cout << endl;
        parser.printUsage();
        exit(1);
    }
    int option = parser.getOptionValue("help");
    if (option == CommandLineParser::OPTION_SET) {
        cout << endl;
        parser.printUsage();
        exit(0);
    }

    string param = parser.getParamValue("epixconf");
    if (param == "") {
        LOG4CPLUS_DEBUG(logger, "No epix config file given, using defaults.");
    } else {
        config = (char*) param.c_str();
    }

    if (!XCLIBController::isOpen()) {
        XCLIBController::openLib(config);
    }


    SF1280Controller controller1(0);
    SF1280Controller controller2(1);
    controller1.initCamera();
    controller2.initCamera();


    Ve::initGL(argc, argv);

    Benchmark bench;
    LOG4CPLUS_INFO(logger, "Now running benchmarks.");
    bench.run();

    /*Destruktoren aufrufen*/

    if (XCLIBController::isOpen())
        XCLIBController::closeLib();

    return EXIT_SUCCESS;
}
