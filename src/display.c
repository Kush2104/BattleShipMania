#include <GL/glut.h>
#include <math.h>
#include "include/display.h"
#include "include/init.h"
#include "include/battleship.h"

// Global camera angle for rotation
float cameraAngle = 0.0f;

void drawAxes() {
    glBegin(GL_LINES);
    
    // X-axis in Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    
    // Y-axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    
    // Z-axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    
    glEnd();

    // Labels for the axes (optional, using GLUT bitmap text)
    glRasterPos3f(1.1f, 0.0f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'X');
    glRasterPos3f(0.0f, 1.1f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Y');
    glRasterPos3f(0.0f, 0.0f, 1.1f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Z');
}

void updateStarBrightness() {
    for (int i = 0; i < 1000; i++) {
        // Oscillate brightness using a sine wave for a smooth twinkle effect
        stars[i].brightness += stars[i].twinkleSpeed;
        
        if (stars[i].brightness > 1.0f || stars[i].brightness < 0.3f) {
            stars[i].twinkleSpeed = -stars[i].twinkleSpeed;  // Reverse direction when reaching limits
        }
    }
}

void displayStars() {
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        glColor3f(stars[i].brightness, stars[i].brightness, stars[i].brightness);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set the camera's orbit around the battleship
    float camX = 5.0f * sinf(cameraAngle);
    float camZ = 5.0f * cosf(cameraAngle);
    gluLookAt(camX, 2.0, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // // Draw XYZ Axes
    // glPushMatrix();
    // drawAxes();
    // glPopMatrix();

    // Draw Stars
    glPushMatrix();
    displayStars();  // Display background stars
    glPopMatrix();

    // Draw Battleship
    glPushMatrix();
    drawBattleship(0, 0, 0, 0.5, 1);
    glPopMatrix();

    glutSwapBuffers();
}

void idle() {
    updateStarBrightness();  // Keep stars twinkling
    glutPostRedisplay();
}