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
#include "stopwatch.h"

Stopwatch::Stopwatch()
{
    running = false;
    ftime(&startStamp);
    ftime(&stopStamp);
}


Stopwatch::~Stopwatch()
{
}

void Stopwatch::start() {
    ftime(&startStamp);
    running = true;
}

void Stopwatch::stop() {
    ftime(&stopStamp);
    running = false;
}

void Stopwatch::count(int number) {
   counter += number; 
}

long Stopwatch::getSeconds() {
    if (running) {
	ftime(&stopStamp);
    }
    return stopStamp.time - startStamp.time;
}

long Stopwatch::getMilis() {
    if (running) {
	ftime(&stopStamp);
    }
    return abs(stopStamp.millitm - startStamp.millitm);
}

long Stopwatch::getCount() {
    return counter;
}

float Stopwatch::getFramerate() {
    if (running) {
	ftime(&stopStamp);
    }
    
    long seconds = stopStamp.time - startStamp.time;
    long milis = abs(stopStamp.millitm - startStamp.millitm);
		
    return counter / ((double) ((seconds * 1000) + milis) / 1000.0f);   
}
