#include "epixreaderthread.h"

Logger EpixReaderThread::logger = Logger::getInstance("Ve.EpixReaderThread");

EpixReaderThread::EpixReaderThread(int unit) : Thread(50) {
    bufsize = 0;
    tMutex = new Mutex();
    running = false;
    EpixReaderThread::unit = unit;
    stale = false;
}

EpixReaderThread::~EpixReaderThread() {
    delete buffer;
    delete tmpBuffer;
}

void EpixReaderThread::run() {
    // Check wether the EPIX lib is open
    if (!XCLIBController::isOpen())
        return;
    int width = pxd_imageXdim();
    int height = pxd_imageYdim();
	bufsize = width * height * 3;
    uchar* swapBuffer;
    pxvbtime_t fieldCounter = 0;
    int readResult = 0;
    if (bufsize <= 0)
        return; // Last sanity check

    buffer = new uchar[bufsize];
    tmpBuffer = new uchar[bufsize];

    LOG4CPLUS_INFO(logger, "Starting reader thread on unit " << unit);

    running = true;
    while (running) {
        readResult = XCLIBController::getBufferCopy(unit, tmpBuffer, bufsize);
        // cout << "Pix value: " << tmpBuffer[0] << " " <<  tmpBuffer[1] << " " << tmpBuffer[2] << endl;
        if (readResult == bufsize) {
            tMutex->enterMutex(); // swap the buffers
            swapBuffer = buffer;
            buffer = tmpBuffer;
            tmpBuffer = swapBuffer;
            tMutex->leaveMutex();
            stale = false;
        } else {
            LOG4CPLUS_WARN(logger, "Buffer read error: " << pxd_mesgErrorCode(readResult));
        }
    }
    LOG4CPLUS_INFO(logger, "Thread shut down.");
}


uchar* EpixReaderThread::getBuffer() {
    if (stale)
        return NULL; // don't return a stale buffer

    uchar* retVal;
    tMutex->enterMutex();
    retVal = buffer;
    buffer = new uchar[bufsize];
    stale = true;
    tMutex->leaveMutex();

    return retVal;
}

void EpixReaderThread::stop() {
    running = false;
}
