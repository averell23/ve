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
#ifndef POSITION_H
#define POSITION_H

#include <sys/timeb.h>  	// Time buffer header

/**
    This describes an object position. For a 2D position, 
    z may be set to zero. This is a multi-purpose storage class,
    so no assumptions are made with regard to the unit or meaning of the parameters.

    @author Daniel Hahn,,,
*/
class Position{
public:
    Position(int index, int source=0, double=0, double y=0, double z=0);

    ~Position();

    /// Object index
    int index;
    /**
        ID number of the source from which this position was created.
	This may be used to differntiate between left and right channels etc.
	
	A source number of 0 means that a source has not been specified.
    */
    int source;
    /// X coordinate
    double x;
    /// Y coordinate
    double y;
    /// Z coordinate
    double z;
    /// Time stamp (object creation time)
    timeb timeStamp;
    
    
};

#endif
