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

#include <iostream>
#include "../stubs/positionconnector.hh"

using namespace std;

/*
  Simple utility to test the function of the CORBA server in ve.
*/

int main(int argc, char** argv) {
    try {
	CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
    
	if (argc != 2) {
	    cout << "Usage: " << argv[0] << " <object reference>" << endl;
	    return 1;
	}
	
	CORBA::Object_var obj = orb->string_to_object(argv[1]);
	PositionConnector_var connector = PositionConnector::_narrow(obj);
	
	if (CORBA::is_nil(connector)) {
	    cout << "Can't narrow reference to type PositionConnector (or it was nil)." << endl;
	    return 1;
	}
	
	connector->update(23, 17, 12);
	
	orb->destroy();
    } catch (CORBA::COMM_FAILURE& ex) {
	cout << "Caught CORBA::COMM_FAILURE, unable to contact the object." << endl;
    } catch (CORBA::SystemException&) {
	cout << "Caught CORBA::SystemException" << endl;
    } catch (CORBA::Exception&) {
	cout << "Caught CORBA::Exception" << endl;
    }
    
}