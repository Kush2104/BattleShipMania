#include "include/input.h"
#include "include/battleship.h"
#include <GL/freeglut.h>
#include <math.h>

KeyStates keyStates = {0, 0, 0, 0};

void updateMovement(void) {
    float dx = 0, dz = 0;
    
    // Calculate movement based on pressed keys
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
    
    // Apply movement if any keys are pressed
    if (dx != 0 || dz != 0) {
        MoveShip(dx, dz);
    }
    
    // Always update ship state (including bullets) regardless of movement
    UpdateShipState();
}

void key(unsigned char ch, int x, int y) {
    switch(ch) {
        case 'w':
        case 'W':
            SetPitchPressed(1, 1);  // W pressed
            RotateShip(0, ROTATION_SPEED);  // Pitch down (positive for nose down)
            break;
            
        case 's':
        case 'S':
            SetPitchPressed(0, 1);  // S pressed
            RotateShip(0, -ROTATION_SPEED); // Pitch up (negative for nose up)
            break;
            
        case 'a':
        case 'A':
            SetRollPressed(1, 1);  // A pressed - will handle roll in update
            RotateShip(-ROTATION_SPEED, 0);  // Turn left
            break;
            
        case 'd':
        case 'D':
            SetRollPressed(0, 1);  // D pressed - will handle roll in update
            RotateShip(ROTATION_SPEED, 0);   // Turn right
            break;
            
        case 'f':
        case 'F':
            FireBullet();  // Fire laser
            break;
            
        case 27:  // ESC key
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void keyUp(unsigned char ch, int x, int y) {
    switch(ch) {
        case 'w':
        case 'W':
            SetPitchPressed(1, 0);  // W released
            break;
            
        case 's':
        case 'S':
            SetPitchPressed(0, 0);  // S released
            break;
            
        case 'a':
        case 'A':
            SetRollPressed(1, 0);  // A released
            break;
            
        case 'd':
        case 'D':
            SetRollPressed(0, 0);  // D released
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