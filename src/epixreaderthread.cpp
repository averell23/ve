#include "epixreaderthread.h"

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
    if (!XCLIBController::isOpen()) return;
    bufsize = pxd_imageXdim() * pxd_imageYdim() * 3;
    int width = pxd_imageXdim();
    int height = pxd_imageYdim();
    uchar* swapBuffer;
	pxvbtime_t fieldCounter = 0;
    int readResult = 0;
    if (bufsize <= 0) return; // Last sanity check

    buffer = new uchar[bufsize];
    tmpBuffer = new uchar[bufsize];
    
    cout << "Starting reader thread on unit " << unit << endl;
    
    running = true;
    while (running) { 
		XCLIBController::cMutex->enterMutex();
		readResult = pxd_readuchar(1<<unit, 1, 0, 0, width, height, tmpBuffer, bufsize, "RGB");
		XCLIBController::cMutex->leaveMutex();
		// cout << "Pix value: " << tmpBuffer[0] << " " <<  tmpBuffer[1] << " " << tmpBuffer[2] << endl;
		if (readResult == bufsize) {
			tMutex->enterMutex(); // swap the buffers
			swapBuffer = buffer;
			buffer = tmpBuffer;
			tmpBuffer = swapBuffer;
			tMutex->leaveMutex();
			stale = false;
		} else {
			cout << "Buffer read error: " << pxd_mesgErrorCode(readResult) << endl;
		} 
    }
}


uchar* EpixReaderThread::getBuffer() {
	if (stale) return NULL; // don't return a stale buffer

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
