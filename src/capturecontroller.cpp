#include "capturecontroller.h"

Logger CaptureController::logger = Logger::getInstance("Ve.CaptureController");

CaptureController::CaptureController(int height, int width, int size, int type, char* filePrefix)
{
	info.height = height;
	info.width = width;
	info.filePrefix = filePrefix;
	info.offset_x = 0;
	info.offset_y = 0;
	
	if (type == IMAGE_BAYER) {
		info.planes = 1;
		info.pixBytes = 2;
		info.colorName = "Bayer";
		LOG4CPLUS_DEBUG(logger, "Bayer color model selected.");
	} else if (type == IMAGE_RGB) {
		info.planes = 3;
		info.pixBytes = 1;
		info.colorName = "RGB";
		LOG4CPLUS_DEBUG(logger, "RGB color model selected.");
	} else if (type == IMAGE_GRAY) {
		info.planes = 1;
		info.pixBytes = 1;
		info.colorName = "Gray";
		LOG4CPLUS_DEBUG(logger, "Grayscale color model selected.");
	} else {
		info.planes = 1;
		info.pixBytes = 1;
		info.colorName = "Gray";
		LOG4CPLUS_WARN(logger, "Illegal color mode, defaulting to grayscale");
	}

	if (size < 1) {
		LOG4CPLUS_WARN(logger, "0 or negative buffer size given. Defaulting to size 1.");
		bufferSize = 1;
	} else {
		bufferSize = size;
	}

	info.imgSize = info.height * info.width * info.planes;

	buffer_a = new CaptureBuffer(bufferSize, info.imgSize * info.pixBytes);
	buffer_b = new CaptureBuffer(bufferSize, info.imgSize * info.pixBytes);

	LOG4CPLUS_INFO(logger, "Creation complete, size is " << info.width << "x" 
		       << info.height << "x(" << info.planes << "x" << info.pixBytes 
		       << "), buffer size is " << bufferSize << " images.");
	mutex = new Mutex();
	readThread = NULL;
	writeThread = NULL;
}

CaptureController::~CaptureController(void)
{
    delete mutex;
	delete buffer_a;
	delete buffer_b;
	LOG4CPLUS_INFO(logger, "Buffers deallocated.");
}

void CaptureController::captureToBuffers() {
	pxvbtime_t fieldCount1 = pxd_capturedFieldCount(1<<0);
	pxvbtime_t fieldCount2 = pxd_capturedFieldCount(1<<1);
	Stopwatch timer;
	
	LOG4CPLUS_INFO(logger, "Starting image capture");
	timer.start();
	for (int i = 0 ; i < bufferSize ; i++) {
		pxd_goSnap(1<<0, 1);
		pxd_goSnap(2<<0, 1);
		// Wait for images to arrive
		while ((fieldCount1 == pxd_capturedFieldCount(1<<0)) && (fieldCount2 == pxd_capturedFieldCount(1<<1))) {
			Thread::yield();
		}
		readBuffer(i);
		fieldCount1 = pxd_capturedFieldCount(1<<0);
		fieldCount2 = pxd_capturedFieldCount(1<<1);
		timer.count();
	}
	timer.stop();
	LOG4CPLUS_INFO(logger, "Read " << bufferSize << " images in " 
		<< timer.getSeconds() << "s, " << timer.getMilis()  << "ms, framerate was " << timer.getFramerate());
}

void CaptureController::writeBuffers() {
	// Names for the image files
	char name1[256], name2[256];
	// File handles
	FILE *file1, *file2;
	Stopwatch timer;

	LOG4CPLUS_INFO(logger, "Starting to write buffer to disk, with file prefix " << info.filePrefix);
	timer.start();
	for (int i = 0 ; i < bufferSize ; i++) {
		sprintf(name1,"%s_a_%d.img",info.filePrefix, i);
		sprintf(name2,"%s_b_%d.img",info.filePrefix, i);

		// write to files
		file1 = fopen(name1, "wb");
		if (fwrite(buffer_a->getBufferAt(i), info.pixBytes, info.imgSize, file1) != info.imgSize)
			LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
		fclose(file1);
		file2 = fopen(name2, "wb");
		if (fwrite(buffer_b->getBufferAt(i), info.pixBytes, info.imgSize, file2) != info.imgSize)
			LOG4CPLUS_ERROR(logger, "Error writing file for cam b");
		fclose(file2);
		timer.count();
	}
	timer.stop();
	LOG4CPLUS_INFO(logger, "Wrote " << bufferSize << " images to disk in " 
		<< timer.getSeconds() << "s, " << timer.getMilis()  << "ms, framerate was " << timer.getFramerate());
}

void CaptureController::readBuffer(int i) {
	if (info.pixBytes == 1) {
			int result = pxd_readuchar(1<<0, 1, info.offset_x, info.offset_y, info.offset_x + info.width , info.offset_y + info.height, (uchar*) buffer_a->getBufferAt(i), info.imgSize, info.colorName);
			if (result != info.imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << info.imgSize);
			result = pxd_readuchar(1<<1, 1, info.offset_x, info.offset_y, info.offset_x + info.width , info.offset_y + info.height, (uchar*) buffer_b->getBufferAt(i), info.imgSize, info.colorName);
			if (result != info.imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << info.imgSize);
		} else if (info.pixBytes == 2) {
			int result = pxd_readushort(1<<0, 1, info.offset_x, info.offset_y, info.offset_x + info.width , info.offset_y + info.height, (ushort*) buffer_a->getBufferAt(i), info.imgSize, info.colorName);
			if (result != info.imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << info.imgSize);
			result = pxd_readushort(1<<1, 1, info.offset_x, info.offset_y, info.offset_x + info.width , info.offset_y + info.height, (ushort*) buffer_b->getBufferAt(i), info.imgSize, info.colorName);
			if (result != info.imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << info.imgSize);
		} else {
			LOG4CPLUS_ERROR(logger, info.pixBytes << " pixel bytes not supported.");
		}
}

void CaptureController::startLiveCapture() {
    readThread = new CaptureReadThread(buffer_a, buffer_b, &info, mutex);
    writeThread = new CaptureWriteThread(buffer_a, buffer_b, &info, mutex);
    readThread->start();
    writeThread->start();
    LOG4CPLUS_INFO(logger, "Started reader and writer threads.");
}

void CaptureController::stopLiveCapture() {
    if (writeThread != NULL) {
	writeThread->quit();
	writeThread->join();
	info.writeTimer.stop();
	delete writeThread;
    }
    if (readThread != NULL) {
		info.readTimer.stop();
		delete readThread;
    }
    LOG4CPLUS_INFO(logger, "Reader and writer thread shut down.");
	LOG4CPLUS_INFO(logger, "Read thread read " << info.readTimer.getCount()
		<< " images in " << info.readTimer.getSeconds() << "s" 
		<< info.readTimer.getMilis() << "ms. Frame rate was " 
		<< info.readTimer.getFramerate());
	LOG4CPLUS_INFO(logger, "Write thread wrote " << info.writeTimer.getCount()
		<< " images in " << info.writeTimer.getSeconds() << "s" 
		<< info.writeTimer.getMilis() << "ms. Frame rate was " 
		<< info.writeTimer.getFramerate());
}
