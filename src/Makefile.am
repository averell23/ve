bin_PROGRAMS = ve capture
ve_SOURCES = ve.cpp videocanvas.h videocanvas.cpp videosource.h videosource.cpp dummysource.h dummysource.cpp stopwatch.h stopwatch.cpp dummyoverlay.h dummyoverlay.cpp cameracalibration.h cameracalibration.cpp epixsource.h epixsource.cpp xclibcontroller.h xclibcontroller.cpp epixreaderthread.h epixreaderthread.cpp fonttesteroverlay.h fonttesteroverlay.cpp overlay.h overlay.cpp ve.h main.cpp fontmanager.h fontmanager.cpp statusoverlay.h statusoverlay.cpp commandlineparser.h commandlineparser.cpp veevent.h veevent.cpp veeventlistener.h veeventsource.h veeventsource.cpp offsetoverlay.h offsetoverlay.cpp epixcameracontroller.h epixcameracontroller.cpp sf1280controller.h sf1280controller.cpp vepositionevent.cpp corbacontroller.cpp calibrationoverlay.cpp corbathread.cpp corbathread.h registrationoverlay.cpp arregistration.cpp buffersource.cpp buffersource.h

# set the include path found by configure
INCLUDES = -I/usr/local/xclib $(all_includes) -I/usr/include/cc++2 -I/usr/local/include/opencv -I/usr/include/freetype2

# the library search path.
ve_LDFLAGS = $(all_libraries) 

SUBDIRS = stubs corbatest
ve_LDADD = $(top_builddir)/src/stubs/libstubs.a -lgslcblas -lgsl /usr/local/xclib/xclibi86.a -lomniORB4 -lftgl -lGLEW -lfreetype -lm -lopencv -llog4cplus -lglut -lGL -lccgnu2
noinst_HEADERS = vepositionevent.h position.h corbacontroller.h calibrationoverlay.h registrationoverlay.h arregistration.h capturebuffer.h capturereadthread.h capturewritethread.h captureinfo.h captureimagepair.h bmpstruct.h
capture_SOURCES = cameracalibration.cpp cameracalibration.h capture_main.cpp capturebuffer.cpp capturebuffer.h capturecontroller.cpp capturecontroller.h commandlineparser.cpp commandlineparser.h epixcameracontroller.cpp epixcameracontroller.h epixsource.cpp epixsource.h overlay.cpp overlay.h position.h sf1280controller.cpp sf1280controller.h ve.cpp ve.h veevent.cpp veevent.h veeventlistener.h veeventsource.cpp veeventsource.h vepositionevent.cpp vepositionevent.h xclibcontroller.cpp xclibcontroller.h epixreaderthread.cpp epixreaderthread.h stopwatch.cpp stopwatch.h videocanvas.cpp videocanvas.h videosource.cpp videosource.h capturereadthread.cpp capturewritethread.cpp captureinfo.h captureimagepair.cpp captureimagepair.h
capture_LDADD = -lglut -lopencv /usr/local/xclib/xclibi86.a -lccgnu2 -lm -lGL -lGLEW -llog4cplus
