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
  EpixSource *right = new EpixSource(1);
  // DummySource* right = new DummySource();
  cout << "Right source created" << endl;
  EpixSource *left = new EpixSource(0);
  // DummySource* left = new DummySource();
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

