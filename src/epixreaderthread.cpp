#include "epixreaderthread.h"

Logger EpixReaderThread::logger = Logger::getInstance("Ve.EpixReaderThread");

EpixReaderThread::EpixReaderThread(int unit, EpixSource* source) : Thread(50) {
    bufsize = 0;
    running = false;
    EpixReaderThread::unit = unit;
	EpixReaderThread::source = source;
}

EpixReaderThread::~EpixReaderThread() {
    delete buffer;
}

void EpixReaderThread::run() {
    // Check wether the EPIX lib is open
    if (!XCLIBController::isOpen())
        return;
    int width = pxd_imageXdim();
    int height = pxd_imageYdim();
	bufsize = width * height * 3;
    pxvbtime_t fieldCounter = 0;
    int readResult = 0;
    if (bufsize <= 0)
        return; // Last sanity check

    buffer = new uchar[bufsize];

    LOG4CPLUS_INFO(logger, "Starting reader thread on unit " << unit);

    running = true;
    while (running) {
        readResult = XCLIBController::getBufferCopy(unit, buffer, bufsize);
        // cout << "Pix value: " << tmpBuffer[0] << " " <<  tmpBuffer[1] << " " << tmpBuffer[2] << endl;
        if (readResult == bufsize) {
			source->bufferUpdate((char*) buffer);
			buffer = new uchar[bufsize];
        } else {
            LOG4CPLUS_WARN(logger, "Buffer read error: " << pxd_mesgErrorCode(readResult));
        }
    }
    LOG4CPLUS_INFO(logger, "Thread shut down.");
}

void EpixReaderThread::stop() {
    running = false;
}
