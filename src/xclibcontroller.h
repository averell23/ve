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
#ifndef XCLIBCONTROLLER_H
#define XCLIBCONTROLLER_H
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif
extern "C" {
    #include "xcliball.h"
}


using namespace std;

/**
  Controls the behavior of the XCLIB library. This encapsulates the global
  operations for the XCLIB. This has only static methods, no instance can#else
  be created of this class.
  
  @author Daniel Hahn,,,
*/
class XCLIBController{
public:
    ~XCLIBController();
    
    /**
      Opens the XCLIB library for use.
      
      @param configFile Name of the video configuration File.
             Defaults to NULL.
      @return A value of 0 if the library was successfully opened (or is
              already open), if the library could not be opened the
	      XCLIB error code is returned.
    */
    static int openLib(string configFile = "");
    
    /**
      Closes the XCLIB library.
      
      @return A value of 0 if the library was successfully closed (or was
              not open), in case of problems the XCLIB error code is returned.
    */
    static int closeLib();
    
    /**
      Check if the library is currently open.
      
      @return True if, and only if, the library is currently open for use
    */
    static bool isOpen();
    
    /**
      Puts the selected unit into live capture mode.
      
      @param unit Unit to select (0 == first unit)
      @return The pxd_goLive error code from the XCLIB
    */
    static int goLive(int unit);
    
    /**
      Puts the selected unit out of the live capture mode.
      
      @param unit Unit to select (0 == first unit)
      @return The pxd_goUnLive error code from the XCLIB
    */
    static int goUnLive(int unit);
    
    /**
      Gets a copy of the unit's frame buffer. 
      
      @param unit Unit to select (0 == first unit)
      @param buffer &buffer will point to an array containg a
                    copy of the current frame buffer.
      @return The pxd_readushort return code. (0 if successful).
    */
    static uchar* getBufferCopy(int unit, int* result);

    
private:
    /// Constructor is private for singleton class.
    XCLIBController();
    static bool openState; // FIXME: Init?

};

#endif
