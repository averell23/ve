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
}

BufferSource::~BufferSource(void)
{
}

IplImage* BufferSource::getImage() {
    char* data = (char*) buffers[curImg];
    CvSize size;
    size.height = height;
    size.width = width;
	int type;
	if ((pixBytes == 2) && (planes = 1)) {
		type = IPL_DEPTH_8U;
	} else if (pixBytes == 2) {
		type = IPL_DEPTH_16U;
	} else {
		type = IPL_DEPTH_8U;
	}
    IplImage *image = cvCreateImageHeader(size, type, planes);
	if ((pixBytes == 2) && (planes = 1)) {
		image->imageData = transformBuffer((short*) data, height * width);
	} else {
		image->imageData = copyBuffer(data, height * width * planes * pixBytes);
	}
    
    return image;
}

void BufferSource::recieveEvent(VeEvent &e) {
	if ((e.getType() == VeEvent::KEYBOARD_EVENT) && (e.getCode() == 32)) {
		curImg = (curImg + 1) % frameCount;
	}
}

IplImage* BufferSource::waitAndGetImage() {
	return getImage();
}

char* BufferSource::copyBuffer(char* buffer, int size) {
    char* retVal = new char[size];
    for (int i=0 ; i < size ; i++) {
	retVal[i] = buffer[i];
    }
    return retVal;
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

