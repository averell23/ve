#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "stopwatch.h"
#include <log4cplus/logger.h>
#ifdef WIN32
#include <windows.h>
#endif
extern "C" {
    #include "xcliball.h"
}
#include "ve.h"
#include "buffersource.h"

using namespace log4cplus;

class Benchmark
{
public:
	
	void run();

	Benchmark(void);
	~Benchmark(void);
private:
	/** Timer for benchmark calculations */
	Stopwatch timer;
	/// Type of image to request from cam
	char *imgType;
	/** Number of frames in benchmark */
	int numFrames;
	/// Logger for this class
	static Logger logger;
	/// Pointers to buffer
	void **buffers_a, **buffers_b;
	// Number of color planes
	int planes;
	// Bytes per plane per pixel
	int pixBytes;
	// Size of one image
	int bufsize;

	int top_x;
    int top_y;
	int height; /*pxd_imageXdim()*/
	int width;
	/**
		Show images in OpenGL
	*/
	void showImages();
	/// Run Image reading sub-benchmark
	void runCapture();
	/// Snap images to buffer
	void readSnapImages();
	/// Read live images to buffer
	void readLiveImages();
	/// Read images using event handler
	void readEventImages();
	/// Read images while waiting for the next field to arrive
	void readWaitImages();
	/// Reads the frame buffer into memory buffer i
	void readBuffers(int i);
	/// Reads the frame buffer into memory buffer i and writes out to disk immediately
	void readWriteBuffers(int i, FILE* file1, FILE* file2);
	/// Reads the byte buffers directly to image files on disk
	void readAndWriteImages();
	/// Disk I/O sub-benchmark
	void runDisk();
	/// Write single files (standard I/O)
	void writeSingleFiles();
	/// Write multiple files (standard I/O)
	void writeMultiFiles();
	/// Write single files (Storage SDK)
	void writeSingleFilesDirect();
	/// Runs the concurrent read-/write sub-benchmark
	void runConcurrent();

};

#endif
