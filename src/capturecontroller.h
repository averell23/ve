#ifndef CAPTURECONTROLLER_H
#define CAPTURECONTROLLER_H

#include <log4cplus/logger.h>
#ifdef WIN32
#include <windows.h>
#endif
extern "C" {
    #include "xcliball.h"
}
#include <cc++/thread.h>
#include "stopwatch.h"

using namespace log4cplus;
using namespace ost;

class CaptureController
{
public:
	static const IMAGE_BAYER = 0;
	static const IMAGE_RGB = 1;
	static const IMAGE_GRAY = 2;

	/**
		Creates a new CaptureController with an internal image buffer.
		
		@param height Height of images
		@param width Width of images
		@param size Number of images in buffer
		@param type The color model of the images. Valid types are the IMAGE_*
		            constants, for the respective color model.
		@param filePrefix The filename prefix (including path informtion) when 
			saving files.
	*/
	CaptureController(int height, int width, int size, int type, char* filePrefix = "image_");
	~CaptureController(void);

	/**
		Captures images to the memory buffer until it is filled.
	*/
	void captureToBuffer();

	/**
		Writes the memory buffer out to disk. Each image will
		be a file in RAW format (for now).
	*/
	void writeBuffer();
private:
	/// Pointer to the byte arrays that will contain the image data
	char **buffers_a, **buffers_b;
	/// Image height
	int height;
	/// Image width
	int width;
	/// Top offset (for AOI)
	int offset_y;
	/// Left offset (for AOI)
	int offset_x;
	// Number of color planes
	int planes;
	// Bytes per plane per pixel
	int pixBytes;
	// Size of one image in color componets
	int imgSize;
	/// Size of one buffer in images
	int bufferSize;
	/// Name of the color model (for the XCLIB functions)
	char* colorName;
	/**
		Reads the contents of the frame buffer into the given memory
		buffer location.
	*/
	void readBuffers(int i);
	/// File name prefix for saving files
	char* filePrefix;
	/// Logger for this class
	static Logger logger;
};

#endif
