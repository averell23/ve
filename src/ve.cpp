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

int main(int argc, char *argv[])
{
  initGL(argc, argv);
  cout << "Init complete." << endl;
  EpixSource *right = new EpixSource();
  // DummySource* right = new DummySource();
  right->selectUnit(1);
  cout << "Right source created" << endl;
  EpixSource *left = new EpixSource();
  // DummySource* left = new DummySource();
  left->selectUnit(0);
  cout << "Left source created" << endl;
  mainVideo = new VideoCanvas(left, right);
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

