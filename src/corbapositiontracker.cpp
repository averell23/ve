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
#include "corbapositiontracker.h"

Logger CORBAPositionTracker::logger = Logger::getInstance("Ve.CORBAPositionTracker");

CORBAPositionTracker::CORBAPositionTracker(int leftId, int rightId) {
    CORBAPositionTracker::leftId = leftId;
    CORBAPositionTracker::rightId = rightId;
    LOG4CPLUS_TRACE(logger, "Tracker created");
}


CORBAPositionTracker::~CORBAPositionTracker() {}


void CORBAPositionTracker::recieveEvent(VeEvent &e) {
    if (e.getType() == VeEvent::POSITION_EVENT) {
        VePositionEvent& posE = (VePositionEvent&) e;
        CvPoint3D32f origPoint;
        origPoint.x = posE.getPosition().x;
        origPoint.y = posE.getPosition().y;
        origPoint.z = posE.getPosition().z;
        CvPoint2D32f leftPoint = Ve::getLeftSource()->getRegistration()->transformSensorToImage(origPoint);
        CvPoint2D32f rightPoint = Ve::getRightSource()->getRegistration()->transformSensorToImage(origPoint);
        Position leftPos(posE.getPosition().index, leftId, leftPoint.x, leftPoint.y, 0);
        Position rightPos(posE.getPosition().index, rightId, rightPoint.x, rightPoint.y, 0);
        VePositionEvent leftE(leftPos);
        VePositionEvent rightE(rightPos);
        postEvent(leftE);
        postEvent(rightE);
        LOG4CPLUS_TRACE(logger, "Created position updates from CORBA event.");
    } else {
        LOG4CPLUS_DEBUG(logger, "Caught non-position event in CORBA tracker.");
    }
}
