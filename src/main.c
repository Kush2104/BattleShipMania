#include "include/display.h"
#include "include/init.h"
#include "include/input.h"
#include "include/utils.h"
#include "include/battleship.h"

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)width / height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    UpdateBullets();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);  // 60 FPS timing
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Battleship");

    glEnable(GL_DEPTH_TEST);
    initStars();  // Initialize star positions

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    initializeInput();
    glutIdleFunc(idle);     // Keep stars twinkling
    glutTimerFunc(0, update, 0);  // Start update loop
    glutMainLoop();
    return 0;
}