bin_PROGRAMS = ve
ve_SOURCES = ve.cpp videocanvas.h videocanvas.cpp videosource.h videosource.cpp dummysource.h dummysource.cpp stopwatch.h stopwatch.cpp dummyoverlay.h dummyoverlay.cpp cameracalibration.h cameracalibration.cpp epixsource.h epixsource.cpp xclibcontroller.h xclibcontroller.cpp epixreaderthread.h epixreaderthread.cpp fonttesteroverlay.h fonttesteroverlay.cpp overlay.h overlay.cpp ve.h main.cpp fontmanager.h fontmanager.cpp statusoverlay.h statusoverlay.cpp commandlineparser.h commandlineparser.cpp veevent.h veevent.cpp veeventlistener.h veeventsource.h veeventsource.cpp offsetoverlay.h offsetoverlay.cpp epixcameracontroller.h epixcameracontroller.cpp sf1280controller.h sf1280controller.cpp vepositionevent.cpp corbacontroller.cpp calibrationoverlay.cpp corbathread.cpp corbathread.h registrationoverlay.cpp arregistration.cpp

# set the include path found by configure
INCLUDES = -I/usr/local/xclib $(all_includes) -I/usr/include/cc++2 -I/usr/local/include/opencv -I/usr/include/freetype2

# the library search path.
ve_LDFLAGS = $(all_libraries) 

SUBDIRS = stubs corbatest
ve_LDADD = $(top_builddir)/src/stubs/libstubs.a -lgslcblas -lgsl /usr/local/xclib/xclibi86.a -lomniORB4 -lftgl -lGLEW -lfreetype -lm -lopencv -llog4cplus -lglut -lGL -lccgnu2
noinst_HEADERS = vepositionevent.h position.h corbacontroller.h calibrationoverlay.h registrationoverlay.h arregistration.h
