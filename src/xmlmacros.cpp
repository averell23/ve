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
#include "xmlmacros.h"

Logger XMLMacros::logger = Logger::getInstance("Ve.XMLMacros");

xercesc::DOMDocument* XMLMacros::XMLReadFile(string filename) {
    XMLCh tmpStr[256];
    
    xercesc::XercesDOMParser parser;
    xercesc::HandlerBase errHandler; // Dummy handler
    parser.setErrorHandler(&errHandler);

    try {
        parser.parse(filename.c_str());
    } catch (const xercesc::XMLException& e) {
        char* msg = xercesc::XMLString::transcode(e.getMessage());
        LOG4CPLUS_ERROR(logger, "Caught XML exception while loading: " << msg);
        xercesc::XMLString::release(&msg);
        return NULL;
    } catch (const xercesc::DOMException& e) {
        char* msg = xercesc::XMLString::transcode(e.msg);
        LOG4CPLUS_ERROR(logger, "Caught DOM exception while loading: " << msg);
        xercesc::XMLString::release(&msg);
        return NULL;
    } catch (...) {
        LOG4CPLUS_ERROR(logger, "Caught unknown exception while loading.");
        return NULL;
    }

    return parser.getDocument();
}

const XMLCh* XMLMacros::getAttributeByName(xercesc::DOMNode* node, XMLCh* name) {
    xercesc::DOMNamedNodeMap* attributes = node->getAttributes();
    if (attributes) {
        xercesc::DOMNode* attr = attributes->getNamedItem(name);
        if (attr) {
            return attr->getNodeValue();
        } else {
            char* tmpChr = xercesc::XMLString::transcode(name);
            LOG4CPLUS_WARN(logger, "Attribute not found: " << name);
            xercesc::XMLString::release(&tmpChr);
        }
    } else {
        LOG4CPLUS_WARN(logger, "No attributes found when getting by name.");
       
    }
    return NULL;
}

const XMLCh* XMLMacros::getTextChild(xercesc::DOMNode* node) {
    xercesc::DOMNodeList* children = node->getChildNodes();
    for (XMLSize_t i=0 ; i<children->getLength() ; i++) {
        xercesc::DOMNode* curChild = children->item(i);
        if (curChild->getNodeType() == xercesc::DOMNode::TEXT_NODE) {
            return curChild->getNodeValue();
        }
    }
    return NULL;
}

void XMLMacros::XMLSaveFile(string filename, xercesc::DOMElement* root) {
    XMLCh tempStr[256];
    xercesc::XMLString::transcode("Range", tempStr, 255);
    xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
    xercesc::DOMWriter* serializer = impl->createDOMWriter();
    xercesc::LocalFileFormatTarget target(filename.c_str());
    try {
        serializer->writeNode(&target, *root);
    } catch (const xercesc::XMLException& e) {
        char* msg = xercesc::XMLString::transcode(e.getMessage());
        LOG4CPLUS_ERROR(logger, "Caught XML exception while saving: " << msg);
        xercesc::XMLString::release(&msg);
        serializer->release();
        return;
    } catch (const xercesc::DOMException& e) {
        char* msg = xercesc::XMLString::transcode(e.msg);
        LOG4CPLUS_ERROR(logger, "Caught DOM exception while saving: " << msg);
        xercesc::XMLString::release(&msg);
        serializer->release();
        return;
    } catch (...) {
        LOG4CPLUS_ERROR(logger, "Caught unknown exception while saving.");
        serializer->release();
        return;
    }
    
    serializer->release();
}
