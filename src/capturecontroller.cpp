#include "capturecontroller.h"

Logger CaptureController::logger = Logger::getInstance("Ve.CaptureController");

CaptureController::CaptureController(int height, int width, int size, int type, char* filePrefix)
{
	this->height = height;
	this->width = width;
	this->filePrefix = filePrefix;
	offset_x = 0;
	offset_y = 0;
	
	if (type == IMAGE_BAYER) {
		planes = 1;
		pixBytes = 2;
		colorName = "Bayer";
		LOG4CPLUS_DEBUG(logger, "Bayer color model selected.");
	} else if (type == IMAGE_RGB) {
		planes = 3;
		pixBytes = 1;
		colorName = "RGB";
		LOG4CPLUS_DEBUG(logger, "RGB color model selected.");
	} else if (type == IMAGE_GRAY) {
		planes = 1;
		pixBytes = 1;
		colorName = "Gray";
		LOG4CPLUS_DEBUG(logger, "Grayscale color model selected.");
	} else {
		planes = 1;
		pixBytes = 1;
		colorName = "Gray";
		LOG4CPLUS_WARN(logger, "Illegal color mode, defaulting to grayscale");
	}

	if (size < 1) {
		LOG4CPLUS_WARN(logger, "0 or negative buffer size given. Defaulting to size 1.");
		bufferSize = 1;
	} else {
		bufferSize = size;
	}

	imgSize = height * width * planes;

	buffers_a = new char*[bufferSize];
	buffers_b = new char*[bufferSize];
	for (int i = 0 ; i < bufferSize ; i++) {
		buffers_a[i] = new char[imgSize * pixBytes];
		buffers_b[i] = new char[imgSize * pixBytes];
	}
	LOG4CPLUS_INFO(logger, "Creation complete, size is " << width << "x" << height << "x(" 
		<< planes << "x" << pixBytes << "), buffer size is " << bufferSize << " images.");
}

CaptureController::~CaptureController(void)
{
	for (int i = 0 ; i < bufferSize ; i++) {
		delete buffers_a[i];
		delete buffers_b[i];
	}
	delete buffers_a;
	delete buffers_b;
	LOG4CPLUS_INFO(logger, "Buffers deallocated.");
}

void CaptureController::captureToBuffer() {
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
		readBuffers(i);
		fieldCount1 = pxd_capturedFieldCount(1<<0);
		fieldCount2 = pxd_capturedFieldCount(1<<1);
		timer.count();
	}
	timer.stop();
	LOG4CPLUS_INFO(logger, "Read " << bufferSize << " images in " 
		<< timer.getSeconds() << "s, " << timer.getMilis()  << "ms, framerate was " << timer.getFramerate());
}

void CaptureController::writeBuffer() {
	// Names for the image files
	char name1[256], name2[256];
	// File handles
	FILE *file1, *file2;
	Stopwatch timer;

	LOG4CPLUS_INFO(logger, "Starting to write buffer to disk, with file prefix " << filePrefix);
	timer.start();
	for (int i = 0 ; i < bufferSize ; i++) {
		sprintf(name1,"%s_a_%d.img",filePrefix, i);
		sprintf(name2,"%s_b_%d.img",filePrefix, i);

		// write to files
		file1 = fopen(name1, "wb");
		if (fwrite(buffers_a[i], pixBytes, imgSize, file1) != imgSize)
			LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
		fclose(file1);
		file2 = fopen(name2, "wb");
		if (fwrite(buffers_b[i], pixBytes, imgSize, file2) != imgSize)
			LOG4CPLUS_ERROR(logger, "Error writing file for cam b");
		fclose(file2);
		timer.count();
	}
	timer.stop();
	LOG4CPLUS_INFO(logger, "Wrote " << bufferSize << " images to disk in " 
		<< timer.getSeconds() << "s, " << timer.getMilis()  << "ms, framerate was " << timer.getFramerate());
}

void CaptureController::readBuffers(int i) {
	if (pixBytes == 1) {
			int result = pxd_readuchar(1<<0, 1, offset_x, offset_y, offset_x + width , offset_y + height, (uchar*) buffers_a[i], imgSize, colorName);
			if (result != imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << imgSize);
			result = pxd_readuchar(1<<1, 1, offset_x, offset_y, offset_x + width , offset_y + height, (uchar*) buffers_b[i], imgSize, colorName);
			if (result != imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << imgSize);
		} else if (pixBytes == 2) {
			int result = pxd_readushort(1<<0, 1, offset_x, offset_y, offset_x + width , offset_y + height, (ushort*) buffers_a[i], imgSize, colorName);
			if (result != imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << imgSize);
			result = pxd_readushort(1<<1, 1, offset_x, offset_y, offset_x + width , offset_y + height, (ushort*) buffers_b[i], imgSize, colorName);
			if (result != imgSize) 
				LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << imgSize);
		} else {
			LOG4CPLUS_ERROR(logger, pixBytes << " pixel bytes not supported.");
		}
}