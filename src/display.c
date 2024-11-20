#include <GL/glut.h>
#include <math.h>
#include "include/display.h"
#include "include/init.h"
#include "include/battleship.h"
#include "include/input.h"

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
    for (int i = 0; i < NUM_STARS; i++) {
        // Oscillate brightness using a sine wave for a smooth twinkle effect
        stars[i].brightness += stars[i].twinkleSpeed;
        
        // Reduced brightness range
        if (stars[i].brightness > 0.7f || stars[i].brightness < 0.3f) {
            stars[i].twinkleSpeed = -stars[i].twinkleSpeed;
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

    // Set up chase camera
    SetupCamera();

    // Regenerate stars based on ship position
    regenerateStarField(shipState.x, shipState.y, shipState.z);

    // Draw Stars
    glPushMatrix();
    glPointSize(1.0);  // Keep stars small
    glEnable(GL_POINT_SMOOTH);  // Enable point smoothing
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        float distToShip = sqrt(
            pow(stars[i].x - shipState.x, 2) +
            pow(stars[i].y - shipState.y, 2) +
            pow(stars[i].z - shipState.z, 2)
        );
        
        // Fade stars based on distance
        float fadeScale = 1.0f - (distToShip / STAR_VIEW_DISTANCE);
        if (fadeScale > 0) {
            glColor3f(stars[i].brightness * fadeScale, 
                     stars[i].brightness * fadeScale, 
                     stars[i].brightness * fadeScale);
            glVertex3f(stars[i].x, stars[i].y, stars[i].z);
        }
    }
    glEnd();
    
    glDisable(GL_POINT_SMOOTH);
    glPopMatrix();

    // Draw Battleship
    glPushMatrix();
    drawBattleship();
    glPopMatrix();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawBullet(bullets[i].x, bullets[i].y, bullets[i].z);
        }
    }

    glutSwapBuffers();
}

void idle() {
    // Update star twinkling
    updateStarBrightness();

    // Update ship state (handles automatic pitch/roll returns)
    UpdateShipState();

    // Request display update
    glutPostRedisplay();
}