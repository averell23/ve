#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>

#include "glutCallbacks.h"
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
  DummySource *right = new DummySource();
  DummySource *left = new DummySource();
  std::cout << "Drawing cycle" << std::endl;
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
    free(mainVideo);
    free(overlay);
    cout << "Elapsed time was " << timer->getSeconds() 
	<< " seconds and " << timer->getMilis() 
	<< " millis with " << timer->getCount() << " frames. "
	<< " Framerate: " << timer->getFramerate() 
	<< endl;
}

void glDraw(void) {
    mainVideo->draw();
    overlay->draw();
    timer->count();
    glutSwapBuffers();
}

