#include <math.h>
#include "include/display.h"
#include "include/init.h"
#include "include/battleship.h"
#include "include/input.h"
#include "include/celestial.h"

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
    glPushMatrix();
    
    // Enable blending for stars
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glPointSize(2.0);  // Smaller points for distant stars
    glEnable(GL_POINT_SMOOTH);
    
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        // Calculate distance to star
        float dx = stars[i].x - shipState.x;
        float dy = stars[i].y - shipState.y;
        float dz = stars[i].z - shipState.z;
        
        // Calculate apparent motion (extremely subtle parallax)
        float parallaxFactor = 0.0001f;  // Very small factor for subtle movement
        float apparentX = stars[i].x - shipState.x * parallaxFactor;
        float apparentY = stars[i].y - shipState.y * parallaxFactor;
        float apparentZ = stars[i].z - shipState.z * parallaxFactor;
        
        // Calculate relative velocity effect (very subtle)
        float speedEffect = 1.0f;
        if (keyStates.upPressed) {
            // Extremely subtle stretching effect
            float dotProduct = (dx * cos(shipState.yaw * M_PI / 180.0f) +
                              dy * sin(shipState.pitch * M_PI / 180.0f) +
                              dz * sin(shipState.yaw * M_PI / 180.0f));
            speedEffect = 1.0f + fabs(dotProduct) / (STAR_VIEW_DISTANCE * 10.0f);
        }
        
        // Very subtle brightness adjustment
        float adjustedBrightness = stars[i].brightness * speedEffect;
        
        // Additional fade at edges of view for better depth perception
        float angleFromCenter = atan2(sqrt(dx*dx + dz*dz), dy);
        float viewFade = cos(angleFromCenter * 0.5f);
        adjustedBrightness *= viewFade;
        
        // Set star color
        glColor4f(1.0f, 1.0f, 1.0f, adjustedBrightness);
        glVertex3f(apparentX, apparentY, apparentZ);
    }
    glEnd();
    
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    
    glPopMatrix();
}

void renderGameOverText(void) {
    if (!gameOver) return;
    
    // Switch to orthographic projection for 2D text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable lighting for text
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Set text color to red
    glColor3f(1.0f, 0.0f, 0.0f);
    
    // Position text in center of screen
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    // Draw "GAME OVER"
    glRasterPos2i(windowWidth/2 - 40, windowHeight/2 + 10);
    const char* gameOverText = "GAME OVER";
    for (const char* c = gameOverText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Draw "Press R to Restart"
    glRasterPos2i(windowWidth/2 - 70, windowHeight/2 - 20);
    const char* restartText = "Press R to Restart";
    for (const char* c = restartText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Restore states
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    
    // Restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void display() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up chase camera
    SetupCamera();

    // Regenerate stars based on ship position
    regenerateStarField(shipState.x, shipState.y, shipState.z);

    // Draw stars
    displayStars();

    // Draw solar system
    drawSolarSystem();

    // Draw Battleship and/or explosion
    if (!isShipDestroyed()) {
        drawBattleship();
    }
    drawShipExplosion();
    glPopMatrix();

    // Draw active bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawBullet(bullets[i].x, bullets[i].y, bullets[i].z);
        }
    }

    drawHealthBar();
    
    // Render game over text if needed
    renderGameOverText();

    glutSwapBuffers();
}

void idle(void) {
    // Update star twinkling
    updateStarBrightness();

    // Update ship state (handles automatic pitch/roll returns)
    UpdateShipState();
    
    // Check for collisions
    checkBulletAsteroidCollisions();

    // Request display update
    glutPostRedisplay();
}