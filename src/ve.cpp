#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <iostream>
#include <cstdlib>

#include "glutCallbacks.h"
#include "epixsource.h"
#include "dummysource.h"
#include "videocanvas.h"
#include "stopwatch.h"
#include "dummyoverlay.h"


using namespace std;


Stopwatch *timer;
VideoCanvas *mainVideo;
DummyOverlay *overlay;
EpixSource *rightEye;
EpixSource *leftEye;

int main(int argc, char *argv[])
{
  initGL(argc, argv);
  cout << "Init complete." << endl;
  rightEye = new EpixSource(1, "cam1.fmt");
  // DummySource* right = new DummySource();
  cout << "Right source created" << endl;
  leftEye = new EpixSource(0);
  // DummySource* left = new DummySource();
  cout << "Left source created" << endl;
  mainVideo = new VideoCanvas(leftEye, rightEye);
  overlay = new DummyOverlay();
  timer = new Stopwatch();
  
  glutDisplayFunc     ( glDraw );
  glutIdleFunc        ( glDraw );
	
  glutReshapeFunc     ( reshape );
  glutKeyboardFunc    ( keyboard );
  glutSpecialFunc     ( arrow_keys );
  
  timer->start();
  glutMainLoop        ( );          // Initialize The Main Loop
  
  return EXIT_SUCCESS;
}

void shutdown() {
    timer->stop();
    delete mainVideo;
    delete overlay;
    cout << "Elapsed time was " << timer->getSeconds() 
	<< " seconds and " << timer->getMilis() 
	<< " millis with " << timer->getCount() << " frames. "
	<< " Framerate: " << timer->getFramerate() 
	<< endl;
	Stopwatch* rightTimer = rightEye->getReaderTimer();
	Stopwatch* leftTimer = leftEye->getReaderTimer();
	cout << "Right Eye: Elapsed time was " << rightTimer->getSeconds() 
	<< " seconds and " << rightTimer->getMilis() 
	<< " millis with " << rightTimer->getCount() << " frames. "
	<< " Framerate: " << rightTimer->getFramerate() 
	<< endl;
	cout << "Left Eye: Elapsed time was " << leftTimer->getSeconds() 
	<< " seconds and " << leftTimer->getMilis() 
	<< " millis with " << leftTimer->getCount() << " frames. "
	<< " Framerate: " << leftTimer->getFramerate() 
	<< endl;
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) { 
	cout << "Some openGL error has occured." << endl;
    }
}

void glDraw(void) {
    mainVideo->draw();
    overlay->draw();
    timer->count();
    glutSwapBuffers();
}

