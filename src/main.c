#include "include/display.h"
#include "include/init.h"
#include "include/input.h"
#include "include/utils.h"
#include "include/battleship.h"

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Adjusted field of view and view distance for better chase camera view
    gluPerspective(45.0, (float)width / height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    // Update ship state and movement
    updateMovement();
    UpdateShipState();
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);  // 60 FPS timing
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Battleship Flight Sim");

    // Initialize OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);  // Smooth star points
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glPointSize(1.0); 

    // Initialize game components
    initStars();
    initializeInput();

    // Set up display callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}