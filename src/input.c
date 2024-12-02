#include "include/input.h"
#include "include/battleship.h"
#include <GL/freeglut.h>
#include <math.h>

KeyStates keyStates = {0, 0, 0, 0};

void updateMovement(void) {
    float dx = 0, dz = 0;
    
    if (keyStates.upPressed) {
        float pitchFactor = cos(shipState.pitch * M_PI / 180.0f);
        dx += MOVE_SPEED * pitchFactor;
    }
    if (keyStates.downPressed) {
        float pitchFactor = cos(shipState.pitch * M_PI / 180.0f);
        dx -= MOVE_SPEED * pitchFactor;
    }
    if (keyStates.leftPressed) dz -= MOVE_SPEED;
    if (keyStates.rightPressed) dz += MOVE_SPEED;
    
    if (dx != 0 || dz != 0) {
        MoveShip(dx, dz);
    }
    
    UpdateShipState();
}

void key(unsigned char ch, int x, int y) {
    if (gameOver && (ch == 'r' || ch == 'R')) {
        resetGame();
        return;
    }

    if (isShipDestroyed() && ch != 27) return;
    
    switch(ch) {
        case 'w':
        case 'W':
            SetPitchPressed(1, 1);
            RotateShip(0, ROTATION_SPEED);
            break;
            
        case 's':
        case 'S':
            SetPitchPressed(0, 1);
            RotateShip(0, -ROTATION_SPEED);
            break;
            
        case 'a':
        case 'A':
            SetRollPressed(1, 1);
            RotateShip(ROTATION_SPEED, 0);
            break;
            
        case 'd':
        case 'D':
            SetRollPressed(0, 1);
            RotateShip(-ROTATION_SPEED, 0);
            break;
            
        case 'f':
        case 'F':
            FireBullet();
            break;
            
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void keyUp(unsigned char ch, int x, int y) {
    switch(ch) {
        case 'w':
        case 'W':
            SetPitchPressed(1, 0);
            break;
            
        case 's':
        case 'S':
            SetPitchPressed(0, 0);
            break;
            
        case 'a':
        case 'A':
            SetRollPressed(1, 0);
            break;
            
        case 'd':
        case 'D':
            SetRollPressed(0, 0);
            break;
    }
    glutPostRedisplay();
}

void specialKey(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            keyStates.upPressed = 1;
            break;
        case GLUT_KEY_DOWN:
            keyStates.downPressed = 1;
            break;
        case GLUT_KEY_LEFT:
            keyStates.leftPressed = 1;
            break;
        case GLUT_KEY_RIGHT:
            keyStates.rightPressed = 1;
            break;
    }
    glutPostRedisplay();
}

void specialKeyUp(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            keyStates.upPressed = 0;
            break;
        case GLUT_KEY_DOWN:
            keyStates.downPressed = 0;
            break;
        case GLUT_KEY_LEFT:
            keyStates.leftPressed = 0;
            break;
        case GLUT_KEY_RIGHT:
            keyStates.rightPressed = 0;
            break;
    }
    glutPostRedisplay();
}

void initializeInput(void) {
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
}