#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ve.h"

using namespace std;

Stopwatch* Ve::timer;
VideoCanvas* Ve::mainVideo;
VideoSource* Ve::rightEye;
VideoSource* Ve::leftEye;
StereoCalibration* Ve::stereoCal;
/// The overlays that will be displayed
vector<Overlay*> Ve::overlays;

Logger Ve::logger = Logger::getInstance("Ve.Ve");
VeEventSource Ve::eventSource;

void Ve::init(VideoSource* left, VideoSource* right) {
    Ve::rightEye = right;
    Ve::leftEye = left;
    Ve::stereoCal = new StereoCalibration(left->getCalibration(), right->getCalibration());
    Ve::mainVideo = new VideoCanvas(left, right, true, false, false); // FIXME: Should be configurable
    Ve::addListener(Ve::mainVideo);
    LOG4CPLUS_INFO(logger, "Main video canvas created.");
    Ve::timer = new Stopwatch();

    glutDisplayFunc     ( Ve::glDraw );
    glutIdleFunc        ( Ve::glDraw );

    glutReshapeFunc     ( Ve::reshape );
    glutKeyboardFunc    ( Ve::keyboard );
    glutSpecialFunc     ( Ve::arrow_keys );
}

void Ve::start() {
    Ve::timer->start();
    glutMainLoop        ( );          // Initialize The Main Loop
}

void Ve::addOverlay(Overlay* ol) {
    Ve::overlays.push_back(ol);
}

void Ve::shutdown() {
    overlays.clear();
    cout << "Elapsed time was " << timer->getSeconds()
    << " seconds and " << timer->getMilis()
    << " millis with " << timer->getCount() << " frames. "
    << " Framerate: " << timer->getFramerate()
    << endl;
    if (rightEye->timerSupported()) {
        Stopwatch* rightTimer = rightEye->getAndStopTimer();
        cout << "Right Eye: Elapsed time was " << rightTimer->getSeconds()
        << " seconds and " << rightTimer->getMilis()
        << " millis with " << rightTimer->getCount() << " frames. "
        << " Framerate: " << rightTimer->getFramerate()
        << endl;
    }
    if (leftEye->timerSupported()) {
        Stopwatch* leftTimer = leftEye->getAndStopTimer();
        cout << "Left Eye: Elapsed time was " << leftTimer->getSeconds()
        << " seconds and " << leftTimer->getMilis()
        << " millis with " << leftTimer->getCount() << " frames. "
        << " Framerate: " << leftTimer->getFramerate()
        << endl;
    }
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOG4CPLUS_WARN(logger, "Open GL Error encountered on during runtime." << getGLError(error));
    }
    timer->stop();
    /* for (int i=0 ; i < overlays.size() ; i++) {
        delete overlays[i];
    } */
    // FIXME: Proper handling required delete mainVideo;
}

const char* Ve::getGLError(GLenum error) {
    char* retVal;
    if (error == GL_INVALID_ENUM) {
        retVal = "OpenGL Error was: GL_INVALID_ENUM (Unacceptable value for enumerated argument)";
    } else if (error == GL_INVALID_VALUE) {
        retVal = "OpenGL Error was: GL_INVALID_VALUE (Numeric value out of range)";
    } else if (error == GL_INVALID_OPERATION) {
        retVal = "OpenGL Error was: GL_INVALID_OPERATION (Operation called in an illegal state)";
    } else if (error == GL_STACK_OVERFLOW) {
        retVal = "OpenGL Error was: GL_STACK_OVERFLOW (Stack overflow)";
    } else if (error == GL_STACK_UNDERFLOW) {
        retVal = "OpenGL Error was: GL_STACK_UNDERFLOW (Stack underflow)";
    } else if (error == GL_OUT_OF_MEMORY) {
        retVal = "OpenGL Error was: GL_OUT_OF_MEMORY";
    } else if (error == GL_TABLE_TOO_LARGE) {
        retVal = "OpenGL Error was: GL_TABLE_TOO_LARGE (Table size exceeded)";
    } else if (error == GL_NO_ERROR) {
        retVal = "OpenGL reported GL_NO_ERROR";
    } else {
        LOG4CPLUS_WARN(logger, "OpenGL Error code is unknown.");
    }
    return retVal;
}

void Ve::addListener(VeEventListener* listener) {
    eventSource.addListener(listener);
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
    default:        // Default: Post a keyboard event to all listeners
        VeEvent e(VeEvent::KEYBOARD_EVENT, key);
        eventSource.postEvent(e);
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
        glutReshapeWindow ( 500, 500 ); // Go InAs a side note, my doctor and I were discussing Suzi, the woman who ownsto A 500 By 500 Window
        break;
    default:
        break;
    }
}

void Ve::initGL( int argc, char** argv ) {
    glutInit            ( &argc, argv ); // Erm Just Write It =)
    glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
    glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
    glutCreateWindow    ( "VE Augmented Reality" ); // Window Title (argv[0] for current directory as title)
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
    if (glewInit() != GLEW_OK) {
        LOG4CPLUS_ERROR(logger, "Could not initialize the OpenGL extension handler. Problems may occur.");
    } else {
        LOG4CPLUS_INFO(logger, "OpenGL extension handler initialized.");
    }
    LOG4CPLUS_INFO(logger, "GLUT/GLEW/OpenGL initialization complete");
}

char* Ve::bufferCopy(char* buffer, int size) {
    char* newBuffer = new char[size];
    for (int i=0 ; i<size ; i++) {
        newBuffer[i] = buffer[i];
    }
    return newBuffer;
}

CvPoint Ve::getVirtualSize() {
    CvPoint size;
    size.x = 1000;
    size.y = 1000;

    return size;
}

