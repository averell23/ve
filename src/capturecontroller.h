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
#include <fstream>
#include "stopwatch.h"
#include "capturebuffer.h"
#include "captureinfo.h"
#include "capturereadthread.h"
#include "capturewritethread.h"
#include "bmpstruct.h"

// Forward declarations
class CaptureWriteThread;
class CaptureReadThread;

using namespace log4cplus;
using namespace ost;
using namespace std;

class CaptureController
{
public:
	/**
		Creates a new CaptureController with an internal image buffer.
		
		@param info The @see CaptureInfo metadata structure. It will
		            be completely initialized in this constructur.
			    FIXME: Another kludge, not object oriented...
		@param size Size of the internal buffer in images.
	*/
	CaptureController(CaptureInfo* info, int size);
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
	
	/**
	   Writes the image data as RAW files (byte arrays)
	   @param aFile Name of first file
	   @param bFile Name of second file
	   @param imgPair Image pair to be written to disk
	   @param myInfo Meta information structure
         */
	static void writeRAWFiles(char* aFile, char* bFile, CaptureImagePair* imgPair, CaptureInfo* myInfo);
	
	/**
	  Writes the image data as BMP files 
	  @see writeRAWFiles
	*/
	static void writeBMPFiles(char* aFile, char* bFile, CaptureImagePair* imgPair, CaptureInfo* myInfo);
	
	/**
	  Reads the contents of the frame buffer into the given memory
	  locations.
		
	  @param imgPair Pointer to the @see CaptureImagePair that is to be 
	                 written
	  @param myInfo Meta info structure
	*/
	static void readBuffer(CaptureImagePair* imgPair, CaptureInfo* myInfo);
	
	/**
	  Write a seperate timestamp/metadata file
	  
	  @param file Name of the file to write the timestamp to.
	  @param imgPair Pointer to the image pair which's metatdata
	                 is to be written
	*/
	static void writeMetaStamp(char* file, CaptureImagePair* imgPair);
	
private:
	/// Pointer to the data buffer
	CaptureBuffer *buffer;
	/// Info structure for meta information
	CaptureInfo* info;
	/// Size of one buffer in images
	int bufferSize;
	/// Logger for this class
	static Logger logger;
	/// Reader thread
	CaptureReadThread* readThread;
	/// Writer thread
	CaptureWriteThread* writeThread;
	/// Cache for the BMP header (for .bmp writing)
	BMP_HEADER* bmpHeader;
	/// Cache for the BMP info header (for .bmp writing)
	BMP_INFOHEADER* bmpInfoHeader;

};

#endif
