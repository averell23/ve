#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ve.h"

using namespace std;

Stopwatch* Ve::timer;
VideoCanvas* Ve::mainVideo;
DummyOverlay* Ve::overlay;
VideoSource* Ve::rightEye;
VideoSource* Ve::leftEye;
/// The overlays that will be displayed
vector<Overlay*> Ve::overlays;

int main(int argc, char *argv[])
{
  /* Ve::initGL(argc, argv);  
  
  VideoSource* left = new DummySource();// new EpixSource(1, "cam1.fmt");
  cout << "Left source created" << endl;
  VideoSource* right = new DummySource(); // new EpixSource(0);
  cout << "Right source created" << endl;
  Ve::init(left, right);
  
  // Create overlays
  Overlay* dummy = new DummyOverlay();
  Ve::addOverlay(new DummyOverlay()); 
  
  Ve::start();
  */
  return EXIT_SUCCESS;
}


void init(VideoSource* left, VideoSource* right) {
    cout << "OpenGL Init complete." << endl;
    Ve::rightEye = right;
    Ve::leftEye = left;
    Ve::mainVideo = new VideoCanvas(left, right);
    cout << "Main video canvas created." << endl;
    Ve::timer = new Stopwatch();
  
    glutDisplayFunc     ( Ve::glDraw );
    glutIdleFunc        ( Ve::glDraw );
	
    glutReshapeFunc     ( Ve::reshape );
    glutKeyboardFunc    ( Ve::keyboard );
    glutSpecialFunc     ( Ve::arrow_keys );
}

void start() {
    Ve::timer->start();
    glutMainLoop        ( );          // Initialize The Main Loop
}

void addOverlay(Overlay* ol) {
    Ve::overlays.push_back(ol);
}

void Ve::shutdown() {
    timer->stop();
    delete mainVideo;
    delete overlay;
    cout << "Elapsed time was " << timer->getSeconds() 
	<< " seconds and " << timer->getMilis() 
	<< " millis with " << timer->getCount() << " frames. "
	<< " Framerate: " << timer->getFramerate() 
	<< endl;
	Stopwatch* rightTimer = rightEye->getAndStopTimer();
	Stopwatch* leftTimer = leftEye->getAndStopTimer();
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

void Ve::glDraw(void) {
    mainVideo->draw();
    for (int i=0 ; i < Ve::overlays.size() ; i++) {
	Ve::overlays[i]->draw();
    }
    timer->count();
    glutSwapBuffers();
}

void Ve::reshape ( int w, int h )   // Create The Reshape Function (the viewport)
{
	glViewport     ( 0, 0, w, h );
	glMatrixMode   ( GL_PROJECTION );  // Select The Projection Matrix
	glLoadIdentity ( );                // Reset The Projection Matrix
	if ( h==0 )  // Calculate The Aspect Ratio Of The Window
		gluPerspective ( 80, ( float ) w, 1.0, 5000.0 );
	else
		gluPerspective ( 80, ( float ) w / ( float ) h, 1.0, 5000.0 );
	glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
	glLoadIdentity ( );    // Reset The Model View Matrix
}

void Ve::keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
    
	switch ( key ) {
	case 27:        // When Escape Is Pressed...
		shutdown();
		exit ( EXIT_SUCCESS );   // Exit The Program
		break;        // Ready For Next Case
	default:        // Now Wrap It Up
		break;
	}
}

void Ve::arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
	switch ( a_keys ) {
	case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
		glutFullScreen ( ); // Go Into Full Screen Mode
		break;
	case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
		glutReshapeWindow ( 500, 500 ); // Go Into A 500 By 500 Window
		break;
	default:
		break;
	}
}

void Ve::initGL( int argc, char** argv ) {
    glutInit            ( &argc, argv ); // Erm Just Write It =)
    glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
    glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
    glutCreateWindow    ( "NeHe's OpenGL Framework" ); // Window Title (argv[0] for current directory as title)
    glutFullScreen      ( );          // Put Into Full Screen
    
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glFlush();
}
