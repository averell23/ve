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
#ifndef XMLMACROS_H
#define XMLMACROS_H

#include <cstdlib>		
#include <log4cplus/logger.h>
#include <xercesc/parsers/XercesDOMParser.hpp>  // Xerces related files
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

using namespace log4cplus;
using namespace std;
#ifndef XERCES_HAS_CPP_NAMESPACE
#error "Xerces must have cpp namespace support for ve to compile."
#endif


/**
Collection of XML utility functions.

@author Daniel Hahn,,,
*/
class XMLMacros{
public:
    XMLMacros();

    ~XMLMacros();
    
    /**
      Attempts to open the XML file for reading. The file is
      opened and parsed into a DOM. If this isn't successful,
      for whatever reason, the error messages will be logged. A dummy error
      handler is used for the parser.
      
      @param filename Name of the XML file to open.
      
      @return Pointer to the DOMDocument or NULL, if errors occured.
    */
    static xercesc::DOMDocument* XMLReadFile(string filename);
    
    /**
      Attempts to save the given DOM document as an XML file. Error messages
      are logged.
      
      @param filename Name of the file to save to.
      @param root Root element of the DOM document to save.
    */
    static void XMLSaveFile(string filename, xercesc::DOMElement* root);
    
    /**
       Helper method to read text child from a node.
       
       @return The contents of the first text node child, or null if
               node has no text children. Does not combine multiple
	       text children.
    */
    static const XMLCh* getTextChild(xercesc::DOMNode* node);

    /**
      Helper method to get an attribute by name from a node.
      
      @return The contents of the attribute, or null if the attribute
              does not exist.
    */
    static const XMLCh* getAttributeByName(xercesc::DOMNode* node, XMLCh* name);
	
private:
    /// Logger for this class.
    static Logger logger;
	
};

#endif
