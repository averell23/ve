#include "capturecontroller.h"

Logger CaptureController::logger = Logger::getInstance("Ve.CaptureController");

CaptureController::CaptureController(CaptureInfo* info, int size) {
    info->offset_x = 0;
    info->offset_y = 0;

    if (info->colorType == CaptureInfo::IMAGE_BAYER) {
        info->planes = 1;
        info->pixBytes = 2;
        info->colorName = "Bayer";
        LOG4CPLUS_DEBUG(logger, "Bayer color model selected.");
    } else if (info->colorType == CaptureInfo::IMAGE_RGB) {
        info->planes = 3;
        info->pixBytes = 1;
        info->colorName = "RGB";
        LOG4CPLUS_DEBUG(logger, "RGB color model selected.");
    } else if (info->colorType == CaptureInfo::IMAGE_GRAY) {
        info->planes = 1;
        info->pixBytes = 1;
        info->colorName = "Gray";
        LOG4CPLUS_DEBUG(logger, "Grayscale color model selected.");
    } else {
        info->planes = 1;
        info->pixBytes = 1;
        info->colorName = "Gray";
        LOG4CPLUS_WARN(logger, "Illegal color mode, defaulting to grayscale");
    }

    if (size < 1) {
        LOG4CPLUS_WARN(logger, "0 or negative buffer size given. Defaulting to size 1.");
        bufferSize = 1;
    } else {
        bufferSize = size;
    }

    info->imgSize = info->height * info->width * info->planes;

    buffer = new CaptureBuffer(bufferSize, info->imgSize * info->pixBytes);

    LOG4CPLUS_INFO(logger, "Creation complete, size is " << info->width << "x"
                   << info->height << "x(" << info->planes << "x" << info->pixBytes
                   << "), buffer size is " << bufferSize << " images.");
    readThread = NULL;
    writeThread = NULL;
    bmpHeader = NULL;
    bmpInfoHeader = NULL;
}

CaptureController::~CaptureController(void) {
    delete buffer;
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
        pxd_goSnap(1<<1, 1);
        // Wait for images to arrive
        while ((fieldCount1 == pxd_capturedFieldCount(1<<0)) && (fieldCount2 == pxd_capturedFieldCount(1<<1))) {
            Thread::yield();
        }
        readBuffer(buffer->getBufferAt(i), info);
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

    LOG4CPLUS_INFO(logger, "Starting to write buffer to disk, with file prefix " << info->filePrefix);
    timer.start();
    for (int i = 0 ; i < bufferSize ; i++) {
        sprintf(name1,"%s_a_%d.img",info->filePrefix, i);
        sprintf(name2,"%s_b_%d.img",info->filePrefix, i);
        writeRAWFiles(name1, name2,
                      buffer->getBufferAt(i),
                      info);
	if (info->writeTimestamp) {
	    char stampFile[256];
	    sprintf(stampFile, "%s_%d.txt", info->timstampPrefix, i);
	    writeMetaStamp(stampFile, buffer->getBufferAt(i));
	}
        timer.count();
    }
    timer.stop();
    LOG4CPLUS_INFO(logger, "Wrote " << bufferSize << " images to disk in "
                   << timer.getSeconds() << "s, " << timer.getMilis()  << "ms, framerate was " << timer.getFramerate());
}

void CaptureController::writeRAWFiles(char* aFile, char* bFile, CaptureImagePair* imgPair, CaptureInfo* myInfo) {
    FILE* file1 = fopen(aFile, "wb");
    FILE* file2 = fopen(bFile, "wb");
    if ((file1 == NULL) || (file2 == NULL)) {
        LOG4CPLUS_ERROR(logger, "Could not open file for writing.");
        return;
    }
    if (fwrite(imgPair->buffer_a, myInfo->pixBytes,
	       myInfo->imgSize, file1) != myInfo->imgSize)
    {
        LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
    }
    if (fwrite(imgPair->buffer_b, myInfo->pixBytes, 
	       myInfo->imgSize, file2) != myInfo->imgSize)
    {
        LOG4CPLUS_ERROR(logger, "Error writing file for cam b");
    }
    fclose(file1);
    fclose(file2);
}

void CaptureController::writeBMPFiles(char* aFile, char* bFile, CaptureImagePair* imgPair, CaptureInfo* myInfo) {
    // If no cached headers exist, initialize them
    if (bmpHeader == NULL) {
	bmpHeader = new BMP_HEADER;
    }
    FILE* file1 = fopen(aFile, "wb");
    FILE* file2 = fopen(bFile, "wb");
    if ((file1 == NULL) || (file2 == NULL)) {
        LOG4CPLUS_ERROR(logger, "Could not open file for writing.");
        return;
    }
    if (fwrite(imgPair->buffer_a, myInfo->pixBytes,
	       myInfo->imgSize, file1) != myInfo->imgSize)
    {
        LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
    }
    if (fwrite(imgPair->buffer_b, myInfo->pixBytes, 
	       myInfo->imgSize, file2) != myInfo->imgSize)
    {
        LOG4CPLUS_ERROR(logger, "Error writing file for cam b");
    }
    fclose(file1);
    fclose(file2);
}


void CaptureController::readBuffer(CaptureImagePair* imgPair, CaptureInfo* myInfo) {
    if (myInfo->pixBytes == 1) {
        int result = pxd_readuchar(1<<0, 1, myInfo->offset_x, myInfo->offset_y,
                                   myInfo->offset_x + myInfo->width,
                                   myInfo->offset_y + myInfo->height,
                                   (uchar*) imgPair->buffer_a,
                                   myInfo->imgSize, myInfo->colorName);
        if (result != myInfo->imgSize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << myInfo->imgSize);
        
	result = pxd_readuchar(1<<1, 1, myInfo->offset_x, myInfo->offset_y, 
			       myInfo->offset_x + myInfo->width, 
			       myInfo->offset_y + myInfo->height, 
			       (uchar*) imgPair->buffer_b, 
			       myInfo->imgSize, myInfo->colorName);
        if (result != myInfo->imgSize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << myInfo->imgSize);
    } else if (myInfo->pixBytes == 2) {
        int result = pxd_readushort(1<<0, 1, myInfo->offset_x, myInfo->offset_y, 
				    myInfo->offset_x + myInfo->width, 
				    myInfo->offset_y + myInfo->height, 
				    (ushort*) imgPair->buffer_a, 
				    myInfo->imgSize, myInfo->colorName);
        if (result != myInfo->imgSize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << myInfo->imgSize);
        result = pxd_readushort(1<<1, 1, myInfo->offset_x, myInfo->offset_y, 
				myInfo->offset_x + myInfo->width, 
				myInfo->offset_y + myInfo->height, 
				(ushort*) imgPair->buffer_b, 
				myInfo->imgSize, myInfo->colorName);
        if (result != myInfo->imgSize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << myInfo->imgSize);
    } else {
        LOG4CPLUS_ERROR(logger, myInfo->pixBytes << " pixel bytes not supported.");
    }
    // Sys ticks and field count are taken from the second board
    imgPair->timestamp = pxd_capturedSysTicks(1<<1);
    imgPair->serial = pxd_capturedFieldCount(1<<1);
}

void CaptureController::writeMetaStamp(char* file, CaptureImagePair* imgPair) {
    ofstream tStream(file);
    if (!tStream) {
        LOG4CPLUS_ERROR(logger, "Could not open timestamp file: " << file);
        return;
    }
    tStream << imgPair->timestamp << endl
	<< imgPair->serial << endl;
    tStream.close();
}

void CaptureController::startLiveCapture() {
    readThread = new CaptureReadThread(buffer, info);
    writeThread = new CaptureWriteThread(buffer, info);
    readThread->start();
    writeThread->start();
    LOG4CPLUS_INFO(logger, "Started reader and writer threads.");
}

void CaptureController::stopLiveCapture() {
    if (writeThread != NULL) {
        writeThread->quit();
        writeThread->join();
        delete writeThread;
    }
    if (readThread != NULL) {
        delete readThread;
    }
    LOG4CPLUS_INFO(logger, "Reader and writer thread shut down.");
}
