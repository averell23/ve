#include "buffersource.h"

BufferSource::BufferSource(int width, int height, int frameCount, int pixBytes, int planes, void** buffers)
{
	BufferSource::width = width;
	BufferSource::height = height;
	BufferSource::frameCount = frameCount;
	BufferSource::pixBytes = pixBytes;
	BufferSource::planes = planes;
	BufferSource::buffers = buffers;
	curImg = 0;
	// Create image size for internal buffer
	CvSize size;
    size.height = height;
    size.width = width;
	// Determine type of internal buffer
		int type;
	if ((pixBytes == 2) && (planes = 1)) {
		type = IPL_DEPTH_8U;
	} else if (pixBytes == 2) {
		type = IPL_DEPTH_16U;
	} else {
		type = IPL_DEPTH_8U;
	}
	// Create internal buffer
	imgBuffer = cvCreateImageHeader(size, type, planes);
	imgBuffer->imageData = NULL;
	// init frame count
	frameCount = 0;
}

BufferSource::~BufferSource(void)
{
	if ((pixBytes == 2) && (planes = 1) && (imgBuffer->imageData != NULL)) {
		delete imgBuffer->imageData;
	}
	cvReleaseImageHeader(&imgBuffer);
}


void BufferSource::recieveEvent(VeEvent &e) {
	if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 32)) {
		imgMutex.enterMutex();
		curImg = (curImg + 1) % frameCount;
		frameCount++;
		// Set to the new buffer
		char* data = (char*) buffers[curImg];
		if ((pixBytes == 2) && (planes = 1)) {
			if (imgBuffer->imageData != NULL) { delete imgBuffer->imageData; }
			imgBuffer->imageData = transformBuffer((short*) data, height * width);
		} else {
			imgBuffer->imageData = data;
		}
		imgMutex.leaveMutex();
	}
}



char* BufferSource::transformBuffer(short* buffer, int size) {
	char* retBuffer = new char[3 * size];
	for (int i=0 ; i < size ; i++) {
		retBuffer[i*3] = (char) buffer[i];
		retBuffer[(i*3) + 1] = (char) buffer[i];
		retBuffer[(i*3) + 2] = (char) buffer[i];
	}

	return retBuffer;
}

