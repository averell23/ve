#include "benchmark.h"

Logger Benchmark::logger = Logger::getInstance("Ve.Benchmark");

Benchmark::Benchmark(void) {
    numFrames = 300;
    imgType = "Bayer";
    planes = 1;
    pixBytes = 2;
    top_x = 0;
    top_y = 0;
    height = 480; // pxd_imageYdim();
    width = 640; // pxd_imageXdim();
    /*bufsize = pxd_imageXdim() * pxd_imageYdim() * planes;*/
    bufsize = height * width * planes;

    LOG4CPLUS_INFO(logger, "Benchmark init complete. Will use " << numFrames << " images of size "
                   << bufsize << " with " << planes << " color planes.");
    LOG4CPLUS_INFO(logger, "XCLIB reports image size: " << pxd_imageYdim() << "x" << pxd_imageXdim());
    LOG4CPLUS_INFO(logger, "Using image size: " << height << "x" << width);
}

Benchmark::~Benchmark(void) {}

void Benchmark::run() {
    // create buffer for reading test
    buffers_a = new void*[numFrames];
    buffers_b = new void*[numFrames];
    for (int i = 0 ; i < numFrames ; i++) {
        buffers_a[i] = new char[bufsize * pixBytes];
        buffers_b[i] = new char[bufsize * pixBytes];
        /*rob : es ginge auch new ushort[bufsize]*/
    }
    LOG4CPLUS_INFO(logger, "Created " << numFrames << " buffers of " << bufsize * pixBytes << " bytes for each eye.");

    runCapture(); /*write Images in Buffer*/

    // runCapture();  /*Lesen und Schreiben von BMP-Bilddateien*/

    runDisk(); /* reines Schreiben von Dateien aber ohne Daten ?*/

    // runConcurrent(); /*schreiben in verschiedene Dateien*/

    // showImages();

    for (int i = 0 ; i < numFrames ; i++) {
        delete buffers_a[i];
        delete buffers_b[i];
    }
    delete buffers_a;
    delete buffers_b;
    LOG4CPLUS_INFO(logger, "Buffers deallocated.");
}

void Benchmark::runConcurrent() {
    // Names for the image files
    char name1[256], name2[256];
    // File handles
    FILE *file1, *file2;
    timer.reset();

    pxvbtime_t fieldCount1 = pxd_capturedFieldCount(1<<0);
    pxvbtime_t fieldCount2 = pxd_capturedFieldCount(1<<1);
    LOG4CPLUS_INFO(logger, "Field count init to " << fieldCount1 << "/" << fieldCount2);

    pxd_goLive(1<<0, 1);
    pxd_goLive(1<<1, 1);
    HANDLE captEvent = pxd_eventCapturedFieldCreate(1<<0);
    // Now start reading cycle
    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        sprintf(name1,"D:\\tmp\\bild_AX_%d.img",i);
        sprintf(name2,"D:\\tmp\\bild_BX_%d.img",i);
        WaitForSingleObject(captEvent, INFINITE);
        pxd_goSnap(1<<0, 1);
        pxd_goSnap(1<<1, 1);
        file1 = fopen(name1, "wb");
        file2 = fopen(name2, "wb");
        readWriteBuffers(0, file1, file2);
        fclose(file1);
        fclose(file2);
        timer.count();
    }
    timer.stop();
    pxd_goUnLive(1<<0);
    pxd_goUnLive(1<<1);

    LOG4CPLUS_INFO(logger, "Read/Wrote " << numFrames << " images in " << timer.getSeconds()
                   << "s " << timer.getMilis() << "ms. Framerate was " << timer.getFramerate());
    // Calculate data rate
    long kBytes = (numFrames * bufsize * pixBytes * 2) / 1024; /*rob: 2 fuer 2 Bilder*/
    LOG4CPLUS_INFO(logger, "Transferred " << ((double) kBytes / (double) 1024) << " MB of data in each direction. Data rate was " << timer.getDatarate(kBytes) << " MBytes/s");
    LOG4CPLUS_INFO(logger, "Field count was " << (pxd_capturedFieldCount(1<<0) - fieldCount1)
                   << "/" << (pxd_capturedFieldCount(1<<1) - fieldCount2));
}

void Benchmark::readWriteBuffers(int i, FILE* file1, FILE* file2) {
    if (pixBytes == 1) {
        int result = pxd_readuchar(1<<0, 1, top_x, top_y, top_x + width , top_y + height, (uchar*) buffers_a[i], bufsize, imgType);
        if (result != bufsize) {
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << bufsize);
        } else {
            if (fwrite(buffers_a[i], pixBytes, bufsize, file1) != bufsize)
                LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
        }
        result = pxd_readuchar(1<<1, 1, top_x, top_y, top_x + width , top_y + height, (uchar*) buffers_b[i], bufsize, imgType);
        if (result != bufsize) {
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << bufsize);
        } else {
            if (fwrite(buffers_b[i], pixBytes, bufsize, file2) != bufsize)
                LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
        }
    } else if (pixBytes == 2) {
        int result = pxd_readushort(1<<0, 1, top_x, top_y, top_x + width , top_y + height, (ushort*) buffers_a[i], bufsize, imgType);
        if (result != bufsize) {
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << bufsize);
        } else {
            if (fwrite(buffers_a[i], pixBytes, bufsize, file1) != bufsize)
                LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
        }
        result = pxd_readushort(1<<1, 1, top_x, top_y, top_x + width , top_y + height, (ushort*) buffers_b[i], bufsize, imgType);
        if (result != bufsize) {
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << bufsize);
        } else {
            if (fwrite(buffers_b[i], pixBytes, bufsize, file2) != bufsize)
                LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
        }
    } else {
        LOG4CPLUS_ERROR(logger, pixBytes << " pixel bytes not supported.");
    }
}

void Benchmark::runCapture() {
    timer.reset();

    pxvbtime_t fieldCount1 = pxd_capturedFieldCount(1<<0);
    pxvbtime_t fieldCount2 = pxd_capturedFieldCount(1<<1);
    LOG4CPLUS_INFO(logger, "Field count init to " << fieldCount1 << "/" << fieldCount2);

    readWaitImages();

    LOG4CPLUS_INFO(logger, "Read " << numFrames << " images in " << timer.getSeconds()
                   << "s " << timer.getMilis() << "ms. Framerate was " << timer.getFramerate());
    // Calculate data rate
    long kBytes = (numFrames * bufsize * pixBytes * 2) / 1024; /*rob: 2 fuer 2 Bilder*/
    LOG4CPLUS_INFO(logger, "Transferred " << ((double) kBytes / (double) 1024) << " MB of data. Data rate was " << timer.getDatarate(kBytes) << " MBytes/s");
    LOG4CPLUS_INFO(logger, "Field count was " << (pxd_capturedFieldCount(1<<0) - fieldCount1)
                   << "/" << (pxd_capturedFieldCount(1<<1) - fieldCount2));

}

void Benchmark::readSnapImages() {
    // Now start reading cycle
    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        pxd_goSnap(1<<0,1);
        pxd_goSnap(1<<1,1);
        readBuffers(i);
        timer.count();
    }
    timer.stop();
}

void Benchmark::readLiveImages() {
    pxd_goLive(1<<0, 1);
    pxd_goLive(1<<1, 1);
    // Now start reading cycle
    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        readBuffers(i);
        timer.count();
    }
    timer.stop();
    pxd_goUnLive(1<<0);
    pxd_goUnLive(1<<1);
}

void Benchmark::readWaitImages() {
    pxd_goLive(1<<0, 1);
    pxd_goLive(1<<1, 1);

    pxvbtime_t fieldCount1 = pxd_capturedFieldCount(1<<0);
    pxvbtime_t fieldCount2 = pxd_capturedFieldCount(1<<1);

    // Now start reading cycle
    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        while ((fieldCount1 == pxd_capturedFieldCount(1<<0)) && (fieldCount2 == pxd_capturedFieldCount(1<<1))) {
            Thread::yield();
        }
        readBuffers(i);
        fieldCount1 = pxd_capturedFieldCount(1<<0);
        fieldCount2 = pxd_capturedFieldCount(1<<1);
        timer.count();
    }
    timer.stop();
    pxd_goUnLive(1<<0);
    pxd_goUnLive(1<<1);
}

void Benchmark::readEventImages() {
    pxd_goLive(1<<0, 1);
    pxd_goLive(1<<1, 1);
    HANDLE captEvent = pxd_eventCapturedFieldCreate(1<<1);
    // Now start reading cycle
    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        WaitForSingleObject(captEvent, INFINITE);
        readBuffers(i);
        timer.count();
    }
    timer.stop();
    pxd_goUnLive(1<<0);
    pxd_goUnLive(1<<1);
}

void Benchmark::readAndWriteImages() {
    // Names for the image files
    char name1[256], name2[256];
    pxd_goLive(1<<0, 1);
    pxd_goLive(1<<1, 1);
    // Now start reading cycle
    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        sprintf(name1,"D:\\tmp\\bild_a_%d.bmp",i);
        sprintf(name2,"D:\\tmp\\bild_b_%d.bmp",i);
        int result = pxd_saveBmp(1<<0, name1, 1, top_x, top_y, top_x + width , top_y + height, 0, 0);
        if (result != 0)
            LOG4CPLUS_ERROR(logger, "Error code while writing board a buffer: " << result);
        result = pxd_saveBmp(1<<1, name2, 1, top_x, top_y, top_x + width , top_y + height, 0, 0);
        if (result != 0)
            LOG4CPLUS_ERROR(logger, "Error code while writing board b buffer: " << result);
        timer.count();
    }
    timer.stop();
    pxd_goUnLive(1<<0);
    pxd_goUnLive(1<<1);
}

void Benchmark::readBuffers(int i) {
    if (pixBytes == 1) {
        int result = pxd_readuchar(1<<0, 1, top_x, top_y, top_x + width , top_y + height, (uchar*) buffers_a[i], bufsize, imgType);
        if (result != bufsize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << bufsize);
        result = pxd_readuchar(1<<1, 1, top_x, top_y, top_x + width , top_y + height, (uchar*) buffers_b[i], bufsize, imgType);
        if (result != bufsize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << bufsize);
    } else if (pixBytes == 2) {
        int result = pxd_readushort(1<<0, 1, top_x, top_y, top_x + width , top_y + height, (ushort*) buffers_a[i], bufsize, imgType);
        if (result != bufsize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board a, only read " << result << " of " << bufsize);
        result = pxd_readushort(1<<1, 1, top_x, top_y, top_x + width , top_y + height, (ushort*) buffers_b[i], bufsize, imgType);
        if (result != bufsize)
            LOG4CPLUS_ERROR(logger, "Read failure: Not all bytes read for board b, only read " << result << " of " << bufsize);
    } else {
        LOG4CPLUS_ERROR(logger, pixBytes << " pixel bytes not supported.");
    }
}

void Benchmark::runDisk() {
    timer.reset();
    writeMultiFiles();
    LOG4CPLUS_INFO(logger, "Wrote " << numFrames << " images in " << timer.getSeconds()
                   << "s " << timer.getMilis() << "ms. Framerate was " << timer.getFramerate());
    // Calculate data rate
    long kBytes = (numFrames * bufsize * pixBytes * 2) / 1024; /*rob: 2 fuer 2 Bilder*/
    LOG4CPLUS_INFO(logger, "Transferred " << ((double) kBytes / (double) 1024) << " MB of data. Data rate was " << timer.getDatarate(kBytes) << " MBytes/s");
}

void Benchmark::writeSingleFiles() {
    // File handles
    FILE *file1, *file2;
    file1 = fopen("D:/tmp/data1", "wb");
    file2 = fopen("D:/tmp/data2", "wb");

    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        if (fwrite(buffers_a[i], pixBytes, bufsize, file1) != bufsize)
            LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
        if (fwrite(buffers_b[i], pixBytes, bufsize, file2) != bufsize)
            LOG4CPLUS_ERROR(logger, "Error writing file for cam b");

        timer.count();
    }
    timer.stop();
    fclose(file1);
    fclose(file2);
}

void Benchmark::writeMultiFiles() {
    // Names for the image files
    char name1[256], name2[256];
    // File handles
    FILE *file1, *file2;

    timer.start();
    for (int i = 0 ; i < numFrames ; i++) {
        sprintf(name1,"D:\\tmp\\bild_a_%d.img",i);
        sprintf(name2,"D:\\tmp\\bild_b_%d.img",i);

        // write to files
        file1 = fopen(name1, "wb");
        if (fwrite(buffers_a[i], pixBytes, bufsize, file1) != bufsize)
            LOG4CPLUS_ERROR(logger, "Error writing file for cam a");
        fclose(file1);
        file2 = fopen(name2, "wb");
        if (fwrite(buffers_b[i], pixBytes, bufsize, file2) != bufsize)
            LOG4CPLUS_ERROR(logger, "Error writing file for cam b");
        fclose(file2);

        timer.count();
    }
    timer.stop();
}

void Benchmark::writeSingleFilesDirect() {
    HANDLE fileHandle1 = CreateFile("D:\\tmp\\data1.dat", FILE_ALL_ACCESS, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE fileHandle2 = CreateFile("D:\\tmp\\data2.dat", FILE_ALL_ACCESS, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if ((fileHandle1 == INVALID_HANDLE_VALUE) || (fileHandle2 == INVALID_HANDLE_VALUE)) {
        LOG4CPLUS_ERROR(logger, "Could not open file handle, error code:" << GetLastError());
        return;
    }
    LOG4CPLUS_INFO(logger, "File handles created.");

    timer.start();
    LPDWORD writtenBytes = new DWORD;
    for (int i = 0 ; i < numFrames ; i++) {
        WriteFile(fileHandle1, buffers_a[i], (bufsize * pixBytes), writtenBytes, NULL);
        if (*writtenBytes != (bufsize * pixBytes)) {
            LOG4CPLUS_ERROR(logger, "Only wrote " << writtenBytes << " bytes out of " << (bufsize * pixBytes) << " from buffer a.");
        }
        WriteFile(fileHandle2, buffers_b[i], (bufsize * pixBytes), writtenBytes, NULL);
        if (*writtenBytes != (bufsize * pixBytes)) {
            LOG4CPLUS_ERROR(logger, "Only wrote " << writtenBytes << " bytes out of " << (bufsize * pixBytes) << " from buffer b.");
        }
        timer.count();
    }
    timer.stop();
    LOG4CPLUS_INFO(logger, "Writing finished.");
    CloseHandle(fileHandle1);
    CloseHandle(fileHandle2);
}

void Benchmark::showImages() {
    BufferSource *left = new BufferSource(width, height, numFrames, pixBytes, planes, buffers_a);
    BufferSource *right = new BufferSource(width, height, numFrames, pixBytes, planes, buffers_b);
    Ve::addListener(left);
    Ve::addListener(right);
    Ve::init(left, right);
    Ve::start();
}
