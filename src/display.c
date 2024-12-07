#include "include/display.h"

void drawAxes() {
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);

    glEnd();

    glRasterPos3f(1.1f, 0.0f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'X');
    glRasterPos3f(0.0f, 1.1f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Y');
    glRasterPos3f(0.0f, 0.0f, 1.1f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Z');
}

void updateStarBrightness() {
    for (int i = 0; i < NUM_STARS; i++) {

        stars[i].brightness += stars[i].twinkleSpeed;

        if (stars[i].brightness > 0.7f || stars[i].brightness < 0.3f) {
            stars[i].twinkleSpeed = -stars[i].twinkleSpeed;
        }
    }
}

void displayStars() {
    glPushMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPointSize(2.0);  
    glEnable(GL_POINT_SMOOTH);

    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {

        float dx = stars[i].x - shipState.x;
        float dy = stars[i].y - shipState.y;
        float dz = stars[i].z - shipState.z;

        float parallaxFactor = 0.0001f;  
        float apparentX = stars[i].x - shipState.x * parallaxFactor;
        float apparentY = stars[i].y - shipState.y * parallaxFactor;
        float apparentZ = stars[i].z - shipState.z * parallaxFactor;

        float speedEffect = 1.0f;
        if (keyStates.upPressed) {

            float dotProduct = (dx * cos(shipState.yaw * M_PI / 180.0f) +
                              dy * sin(shipState.pitch * M_PI / 180.0f) +
                              dz * sin(shipState.yaw * M_PI / 180.0f));
            speedEffect = 1.0f + fabs(dotProduct) / (STAR_VIEW_DISTANCE * 10.0f);
        }

        float adjustedBrightness = stars[i].brightness * speedEffect;

        float angleFromCenter = atan2(sqrt(dx*dx + dz*dz), dy);
        float viewFade = cos(angleFromCenter * 0.5f);
        adjustedBrightness *= viewFade;

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

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 0.0f, 0.0f);

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glRasterPos2i(windowWidth/2 - 40, windowHeight/2 + 10);
    const char* gameOverText = "GAME OVER";
    for (const char* c = gameOverText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glRasterPos2i(windowWidth/2 - 70, windowHeight/2 - 20);
    const char* restartText = "Press R to Restart";
    for (const char* c = restartText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

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

    SetupCamera();

    regenerateStarField(shipState.x, shipState.y, shipState.z);

    displayStars();

    drawSolarSystem();

    if (!isShipDestroyed()) {
        drawBattleship();
    }
    drawShipExplosion();
    glPopMatrix();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawBullet(bullets[i].x, bullets[i].y, bullets[i].z);
        }
    }

    drawHealthBar();

    renderGameOverText();

    glutSwapBuffers();
}

void idle(void) {

    updateStarBrightness();

    UpdateShipState();

    checkBulletAsteroidCollisions();

    glutPostRedisplay();
}