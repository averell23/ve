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
#include "capturebuffer.h"
#include "captureinfo.h"
#include "capturereadthread.h"
#include "capturewritethread.h"

using namespace log4cplus;
using namespace ost;

class CaptureController
{
public:
	static const int IMAGE_BAYER = 0;
	static const int IMAGE_RGB = 1;
	static const int IMAGE_GRAY = 2;

	/**
		Creates a new CaptureController with an internal image buffer.
		
		@param height Height of images
		@param width Width of imagesfer_b->getBufferAt(i)
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
	void captureToBuffers();

	/**
		Writes the memory buffers out to disk. Each image will
		be a file in RAW format (for now).
	*/
	void writeBuffers();
	
	/**
	  Starts to capture live data directly to disk. The actuall I/O
	  is started in seperate threads, the function returns immediately.
	  
	  @see stopLiveCapture
	*/
	void startLiveCapture();
	
	/**
	  Stops a live capture started with @see startLiveCapture
	*/
	void stopLiveCapture();
private:
	/// Pointer to the byte arrays that will contain the image data
	CaptureBuffer *buffer_a, *buffer_b;
	/// Info structure for meta information
	CaptureInfo info;
	/// Size of one buffer in images
	int bufferSize;
	/**
		Reads the contents of the frame buffer into the given memory
		buffer location.
	*/
	void readBuffer(int i);
	/// Logger for this class
	static Logger logger;
	/// Mutex for global thread synchronization FIXME: kludge
	Mutex *mutex;
	/// Reader thread
	CaptureReadThread* readThread;
	/// Writer thread
	CaptureWriteThread* writeThread;
};

#endif
