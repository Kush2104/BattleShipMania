#include <math.h>
#include "include/battleship.h"
#include "include/utils.h"

// Global variables
Bullet bullets[MAX_BULLETS] = {0};
ShipState shipState = {0};
int cooldown = 0;

void DrawBullet(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Bright orange core
    glColor4f(1.0, 0.6, 0.0, 1.0);
    Sphere(0, 0, 0, 0.08);
    
    // Orange glow
    glColor4f(1.0, 0.3, 0.0, 0.6);
    Sphere(0, 0, 0, 0.12);
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

void FireBullet(void) {
    if (cooldown > 0) return;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            // Calculate bullet spawn position based on ship's rotation
            float angle = shipState.yaw * M_PI / 180.0f;
            float pitch = shipState.pitch * M_PI / 180.0f;
            
            bullets[i].x = shipState.x - 2.1 * cos(angle);
            bullets[i].y = shipState.y - 2.1 * sin(pitch);
            bullets[i].z = shipState.z - 2.1 * sin(angle);
            bullets[i].active = 1;
            
            cooldown = FIRE_RATE;
            break;
        }
    }
}

void UpdateBullets(void) {
    if (cooldown > 0) cooldown--;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            float angle = shipState.yaw * M_PI / 180.0f;
            float pitch = shipState.pitch * M_PI / 180.0f;
            
            // Move bullet in direction ship was facing when fired
            bullets[i].x -= BULLET_SPEED * cos(angle);
            bullets[i].y -= BULLET_SPEED * sin(pitch);
            bullets[i].z -= BULLET_SPEED * sin(angle);
            
            // Deactivate if too far
            if (bullets[i].x < -20.0 || bullets[i].x > 20.0 ||
                bullets[i].y < -20.0 || bullets[i].y > 20.0 ||
                bullets[i].z < -20.0 || bullets[i].z > 20.0) {
                bullets[i].active = 0;
            }
        }
    }
}

void MoveShip(float dx, float dz) {
    float angle = shipState.yaw * M_PI / 180.0f;
    shipState.x += dx * cos(angle) - dz * sin(angle);
    shipState.z += dx * sin(angle) + dz * cos(angle);
}

void RotateShip(float yaw, float pitch) {
    shipState.yaw += yaw;
    
    // Keep yaw between 0 and 360 degrees
    if (shipState.yaw > 360.0f) shipState.yaw -= 360.0f;
    if (shipState.yaw < 0.0f) shipState.yaw += 360.0f;
    
    // Handle pitch with limits
    shipState.pitch = fmax(-MAX_PITCH, fmin(MAX_PITCH, shipState.pitch + pitch));
}

void SetPitchPressed(int isW, int state) {
    if (isW) {
        shipState.wPressed = state;
    } else {
        shipState.sPressed = state;
    }
}

void UpdateShipState(void) {
    // Handle pitch return to neutral when keys released
    if (!shipState.wPressed && !shipState.sPressed) {
        if (shipState.pitch > 0) {
            shipState.pitch = fmax(0, shipState.pitch - PITCH_RETURN_SPEED);
        } else if (shipState.pitch < 0) {
            shipState.pitch = fmin(0, shipState.pitch + PITCH_RETURN_SPEED);
        }
    }
    
    // Update bullets
    UpdateBullets();
}

void drawBattleship(void) {
    glPushMatrix();
    
    // Move to ship's position and apply rotations
    glTranslatef(shipState.x, shipState.y, shipState.z);
    glRotatef(shipState.yaw, 0, 1, 0);    // Yaw (left/right)
    glRotatef(shipState.pitch, 0, 0, 1);   // Pitch (up/down)
    
    // Main fuselage
    glColor3f(0.3, 0.32, 0.35);
    Cube(0, 0, 0, 2.0, 0.2, 0.5, 0, 0, 1, 0);  // Main body
    
    // Nose section
    glColor3f(0.25, 0.27, 0.3);
    Cube(-1.8, 0.1, 0, 0.4, 0.15, 0.3, 15, 0, 1, 0);  // Upper nose
    Cube(-1.8, -0.05, 0, 0.4, 0.1, 0.4, -10, 0, 1, 0);  // Lower nose
    
    // Cockpit
    glColor3f(0.2, 0.6, 0.8);  // Blue tinted glass
    Cube(-1.0, 0.2, 0, 0.6, 0.1, 0.3, 5, 0, 1, 0);  // Canopy
    
    // Main wings
    glColor3f(0.28, 0.3, 0.33);
    // Left wing
    Cube(-0.2, 0, -1.2, 1.2, 0.08, 0.8, 10, 0, 1, 0);
    // Right wing
    Cube(-0.2, 0, 1.2, 1.2, 0.08, 0.8, -10, 0, 1, 0);
    
    // Wing tips
    glColor3f(0.25, 0.27, 0.3);
    // Left tip
    Cube(-0.2, 0.2, -1.6, 0.4, 0.3, 0.08, 60, 0, 0, 1);
    // Right tip
    Cube(-0.2, 0.2, 1.6, 0.4, 0.3, 0.08, -60, 0, 0, 1);
    
    // Tail section
    glColor3f(0.28, 0.3, 0.33);
    Cube(1.5, 0.3, 0, 0.4, 0.4, 0.08, 30, 0, 0, 1);  // Center tail
    // Horizontal stabilizers
    Cube(1.2, 0, -0.8, 0.6, 0.08, 0.4, 15, 1, 0, 0);  // Left
    Cube(1.2, 0, 0.8, 0.6, 0.08, 0.4, -15, 1, 0, 0);  // Right
    
    // Engine nozzles
    glColor3f(0.2, 0.22, 0.25);
    Cylinder(1.8, -0.1, -0.3, 0.15, 0.3);  // Left engine
    Cylinder(1.8, -0.1, 0.3, 0.15, 0.3);   // Right engine
    
    // Engine glow
    glColor3f(0.8, 0.4, 0.2);
    Cylinder(2.0, -0.1, -0.3, 0.1, 0.1);  // Left glow
    Cylinder(2.0, -0.1, 0.3, 0.1, 0.1);   // Right glow
    
    // Front laser cannon
    glColor3f(0.24, 0.26, 0.29);
    Cylinder(-2.0, 0, 0, 0.1, 0.2);  // Cannon housing
    
    glPopMatrix();
    
    // Draw all active bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawBullet(bullets[i].x, bullets[i].y, bullets[i].z);
        }
    }
}

// Function to set up chase camera
void SetupCamera(void) {
    float camDist = 10.0f;    // Distance behind ship
    float camHeight = 2.0f;   // Height above ship
    
    // Calculate camera position based on ship's rotation
    float shipRad = shipState.yaw * M_PI / 180.0f;
    float pitchRad = shipState.pitch * M_PI / 180.0f;
    
    float camX = shipState.x + camDist * sin(shipRad);
    float camY = shipState.y + camHeight + camDist * sin(pitchRad);
    float camZ = shipState.z + camDist * cos(shipRad);
    
    // Set up the camera
    gluLookAt(camX, camY, camZ,                    // Camera position
              shipState.x, shipState.y, shipState.z,// Look at ship
              0, 1, 0);                            // Up vector
}