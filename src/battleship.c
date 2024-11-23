#include <math.h>
#include "include/battleship.h"
#include "include/utils.h"
#include "include/init.h"

// Global variables
Bullet bullets[MAX_BULLETS] = {0};
ShipState shipState = {
    .x = SHIP_START_X,  // Start at a safe distance from the sun
    .y = 0,            // On the orbital plane
    .z = SHIP_START_Z, // Offset to see both sun and Earth
    .yaw = 45,         // Face towards the sun
    .pitch = 0,
    .roll = 0,
    .wPressed = 0,
    .sPressed = 0,
    .aPressed = 0,
    .dPressed = 0
};

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
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            // Convert angles to radians
            float yawRad = shipState.yaw * M_PI / 180.0f;
            float pitchRad = shipState.pitch * M_PI / 180.0f;
            
            // Calculate forward vector from the ship's nose
            float forwardX = cosf(yawRad);
            float forwardY = -sinf(pitchRad);
            float forwardZ = -sinf(yawRad);
            
            // Set initial bullet position at ship's nose
            bullets[i].x = shipState.x + (2.1f * forwardX);
            bullets[i].y = shipState.y + (2.1f * forwardY);
            bullets[i].z = shipState.z + (2.1f * forwardZ);
            
            // Store initial direction for consistent movement
            bullets[i].dirX = forwardX;
            bullets[i].dirY = forwardY;
            bullets[i].dirZ = forwardZ;
            
            bullets[i].active = 1;
            break;
        }
    }
}

void UpdateBullets(void) {
    static float bulletTimers[MAX_BULLETS] = {0};  // Track lifetime of each bullet
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Move bullets using their stored direction
            bullets[i].x += BULLET_SPEED * bullets[i].dirX;
            bullets[i].y += BULLET_SPEED * bullets[i].dirY;
            bullets[i].z += BULLET_SPEED * bullets[i].dirZ;
            
            // Increment bullet lifetime
            bulletTimers[i]++;
            
            // Calculate distance from ship to bullet
            float dx = bullets[i].x - shipState.x;
            float dy = bullets[i].y - shipState.y;
            float dz = bullets[i].z - shipState.z;
            float distanceFromShip = sqrt(dx*dx + dy*dy + dz*dz);
            
            // Deactivate if too far or too old
            if (distanceFromShip > BULLET_VIEW_DISTANCE || bulletTimers[i] > BULLET_LIFETIME) {
                bullets[i].active = 0;
                bulletTimers[i] = 0;
            }
        }
    }
}

void MoveShip(float dx, float dz) {
    float angle = -shipState.yaw * M_PI / 180.0f;  // Negative yaw for correct direction
    float pitchRad = shipState.pitch * M_PI / 180.0f;
    
    // Calculate movement vector based on ship's orientation
    float forwardX = cos(angle);
    float forwardY = -sin(pitchRad);  // Negative for correct pitch direction
    float forwardZ = sin(angle);
    
    // Update position based on velocity and direction
    shipState.x += (dx * forwardX - dz * sin(angle));
    shipState.y += (dx * forwardY);
    shipState.z += (dx * forwardZ + dz * cos(angle));
}

void RotateShip(float yaw, float pitch) {
    shipState.yaw += yaw;
    
    // Keep yaw between 0 and 360 degrees
    if (shipState.yaw > 360.0f) shipState.yaw -= 360.0f;
    if (shipState.yaw < 0.0f) shipState.yaw += 360.0f;
    
    // Handle pitch with limits
    shipState.pitch = fmax(-MAX_PITCH, fmin(MAX_PITCH, shipState.pitch + pitch));
}

void UpdateRoll(void) {
    // Handle roll based on A/D keys
    if (shipState.aPressed) {
        shipState.roll = fmin(MAX_ROLL, shipState.roll + ROTATION_SPEED);
    } else if (shipState.dPressed) {
        shipState.roll = fmax(-MAX_ROLL, shipState.roll - ROTATION_SPEED);
    } else {
        // Return roll to neutral when no keys pressed
        if (shipState.roll > 0) {
            shipState.roll = fmax(0, shipState.roll - ROLL_RETURN_SPEED);
        } else if (shipState.roll < 0) {
            shipState.roll = fmin(0, shipState.roll + ROLL_RETURN_SPEED);
        }
    }
}

void SetPitchPressed(int isW, int state) {
    if (isW) {
        shipState.wPressed = state;
    } else {
        shipState.sPressed = state;
    }
}

void SetRollPressed(int isA, int state) {
    if (isA) {
        shipState.aPressed = state;
    } else {
        shipState.dPressed = state;
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
    
    // Update roll state
    UpdateRoll();
    
    // Update bullets - always update bullets regardless of movement
    UpdateBullets();
}

void drawBattleship(void) {
    glPushMatrix();
    
    // Move to ship's position and apply rotations in correct order
    glTranslatef(shipState.x, shipState.y, shipState.z);
    glRotatef(180, 0, 1, 0);     // Initial -90 degree Y rotation
    glRotatef(shipState.yaw, 0, 1, 0);    // Yaw (left/right)
    glRotatef(shipState.pitch, 0, 0, 1);   // Pitch (up/down)
    glRotatef(shipState.roll, 1, 0, 0);    // Roll (banking)
    
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

void SetupCamera(void) {
    float camDist = 10.0f;    // Distance behind ship
    float camHeight = 2.0f;   // Height above ship
    
    // Calculate camera position based on ship's orientation
    float shipRad = -shipState.yaw * M_PI / 180.0f;  // Negative yaw for correct camera placement
    float pitchRad = shipState.pitch * M_PI / 180.0f;
    float rollRad = shipState.roll * M_PI / 180.0f;
    
    // Calculate camera position
    float camX = shipState.x - camDist * cos(shipRad);
    float camY = shipState.y + camHeight * cos(pitchRad) - camDist * sin(pitchRad);
    float camZ = shipState.z - camDist * sin(shipRad);
    
    // Set up the camera
    gluLookAt(camX, camY, camZ,                    // Camera position
              shipState.x, shipState.y, shipState.z,// Look at ship
              0, 1, 0);                            // Up vector
}