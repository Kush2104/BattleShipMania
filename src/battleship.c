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

ShipHealth shipHealth;
int gameOver = 0;

void initializeShipHealth(void) {
    shipHealth.health = INITIAL_HEALTH;
    shipHealth.isExploding = 0;
    shipHealth.invulnerabilityFrames = 0;
    shipHealth.explosionTimer = 0;
    shipHealth.explosionParticles = NULL;
}

void damageShip(int damage) {
    if (shipHealth.invulnerabilityFrames > 0) return;
    
    shipHealth.health -= damage;
    shipHealth.invulnerabilityFrames = COLLISION_INVULNERABILITY_TIME;
    
    if (shipHealth.health <= 0) {
        shipHealth.health = 0;
        if (!shipHealth.isExploding) {
            createShipExplosion();
        }
    }
    
    printf("Ship damaged! Health: %d\n", shipHealth.health);
}

void createShipExplosion(void) {
    // Clean up any existing explosion particles first
    cleanupShipHealth();
    
    shipHealth.isExploding = 1;
    shipHealth.explosionTimer = EXPLOSION_LIFETIME;
    gameOver = 0;
    
    // Store ship's position at time of explosion
    float explosionX = shipState.x;
    float explosionY = shipState.y;
    float explosionZ = shipState.z;
    
    // Allocate memory for explosion particles
    shipHealth.explosionParticles = (ExplosionParticle*)malloc(
        sizeof(ExplosionParticle) * EXPLOSION_PARTICLE_COUNT);
    
    if (!shipHealth.explosionParticles) {
        printf("Failed to allocate explosion particles!\n");
        return;
    }
    
    // Create large initial flash
    printf("Ship exploding at position: %f, %f, %f\n", explosionX, explosionY, explosionZ);
    
    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        
        // Start particles in a sphere around explosion center
        float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
        float elevation = ((float)rand() / RAND_MAX - 0.5f) * M_PI;
        float distance = ((float)rand() / RAND_MAX) * 2.0f;
        
        p->x = explosionX + cos(angle) * cos(elevation) * distance;
        p->y = explosionY + sin(elevation) * distance;
        p->z = explosionZ + sin(angle) * cos(elevation) * distance;
        
        // Velocity outward from center
        float speed = 5.0f + ((float)rand() / RAND_MAX) * 10.0f;
        p->vx = cos(angle) * cos(elevation) * speed;
        p->vy = sin(elevation) * speed;
        p->vz = sin(angle) * cos(elevation) * speed;
        
        // Larger initial size
        p->size = INITIAL_EXPLOSION_SIZE * (0.5f + ((float)rand() / RAND_MAX));
        p->lifetime = EXPLOSION_LIFETIME * (0.5f + ((float)rand() / RAND_MAX) * 0.5f);
        
        // More dramatic colors
        if (i < EXPLOSION_PARTICLE_COUNT / 3) {  // One third bright white-yellow
            p->r = 1.0f;
            p->g = 1.0f;
            p->b = 0.8f;
        } else if (i < EXPLOSION_PARTICLE_COUNT * 2/3) {  // One third orange
            p->r = 1.0f;
            p->g = 0.5f + ((float)rand() / RAND_MAX) * 0.3f;
            p->b = 0.0f;
        } else {  // One third dark red
            p->r = 1.0f;
            p->g = 0.2f + ((float)rand() / RAND_MAX) * 0.2f;
            p->b = 0.0f;
        }
        p->a = 1.0f;
    }
}

void updateShipExplosion(void) {
    if (!shipHealth.isExploding || !shipHealth.explosionParticles) return;
    
    shipHealth.explosionTimer--;
    
    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        if (p->lifetime <= 0) continue;
        
        // Update position with velocity
        p->x += p->vx * 0.3f;
        p->y += p->vy * 0.3f;
        p->z += p->vz * 0.3f;
        
        // Add gravity and drag
        p->vy -= 0.15f;
        p->vx *= 0.98f;
        p->vy *= 0.98f;
        p->vz *= 0.98f;
        
        // Reduce size over time
        p->size *= 0.98f;
        
        // Update alpha based on lifetime
        float lifeRatio = p->lifetime / (float)EXPLOSION_LIFETIME;
        p->a = lifeRatio * lifeRatio;  // Quadratic falloff
        
        // Decrease lifetime
        p->lifetime--;
    }
    
    // Only set game over after explosion finishes
    if (shipHealth.explosionTimer <= 0) {
        gameOver = 1;
    }
}

void drawShipExplosion(void) {
    if (!shipHealth.isExploding || !shipHealth.explosionParticles) return;
    
    glPushMatrix();
    
    // Draw initial flash in first few frames
    if (shipHealth.explosionTimer > EXPLOSION_LIFETIME - 10) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDisable(GL_LIGHTING);
        
        float flashSize = EXPLOSION_SPREAD * 3.0f;
        float flashAlpha = (shipHealth.explosionTimer - (EXPLOSION_LIFETIME - 10)) / 10.0f;
        
        glPushMatrix();
        glTranslatef(shipState.x, shipState.y, shipState.z);
        glColor4f(1.0f, 0.9f, 0.3f, flashAlpha);
        glutSolidSphere(flashSize, 16, 16);
        glPopMatrix();
    }
    
    // Draw particles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);
    
    // Draw main explosion particles
    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        if (p->lifetime <= 0) continue;
        
        glPushMatrix();
        glTranslatef(p->x, p->y, p->z);
        
        // Billboarding - make particles face camera
        glRotatef(-shipState.yaw, 0, 1, 0);
        glRotatef(-shipState.pitch, 0, 0, 1);
        
        glColor4f(p->r, p->g, p->b, p->a);
        
        // Draw particle as two crossed triangles for better visibility
        float size = p->size;
        glBegin(GL_TRIANGLES);
        // First triangle
        glVertex3f(-size, -size, 0);
        glVertex3f(size, -size, 0);
        glVertex3f(0, size, 0);
        // Second triangle (crossed)
        glVertex3f(-size, size, 0);
        glVertex3f(size, size, 0);
        glVertex3f(0, -size, 0);
        glEnd();
        
        glPopMatrix();
    }
    
    // Draw smoke particles
    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i += 2) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        if (p->lifetime <= 0) continue;
        
        glPushMatrix();
        // Fixed: Use p->size instead of undefined size
        glTranslatef(p->x, p->y + p->size * 0.5f, p->z);
        glRotatef(-shipState.yaw, 0, 1, 0);
        
        float smokeAlpha = p->a * 0.4f;
        glColor4f(0.2f, 0.2f, 0.2f, smokeAlpha);
        float smokeSize = p->size * 2.0f;  // Double the particle's size for smoke
        
        glBegin(GL_TRIANGLES);
        glVertex3f(-smokeSize, -smokeSize, 0);
        glVertex3f(smokeSize, -smokeSize, 0);
        glVertex3f(0, smokeSize, 0);
        glEnd();
        
        glPopMatrix();
    }
    
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void checkShipCollisions(void) {
    if (shipHealth.isExploding) return;
    
    // Check asteroid collisions
    extern Asteroid* asteroids;
    extern int asteroidBeltInitialized;
    
    if (asteroidBeltInitialized && asteroids != NULL) {
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            if (!asteroids[i].active || asteroids[i].fragmentsActive) continue;
            
            float dx = shipState.x - asteroids[i].x;
            float dy = shipState.y - asteroids[i].y;
            float dz = shipState.z - asteroids[i].z;
            float distSq = dx*dx + dy*dy + dz*dz;
            
            if (distSq < (COLLISION_CHECK_RADIUS * COLLISION_CHECK_RADIUS)) {
                damageShip(ASTEROID_DAMAGE);
                break;
            }
        }
    }
    
    // Check planet collisions
    extern CelestialBody solarBodies[];
    extern int bodyCount;
    
    for (int i = 0; i < bodyCount; i++) {
        CelestialBody* body = &solarBodies[i];
        if (body->type != CELESTIAL_PLANET && body->type != CELESTIAL_SUN) continue;
        
        float dx = shipState.x - body->x;
        float dy = shipState.y - body->y;
        float dz = shipState.z - body->z;
        float distSq = dx*dx + dy*dy + dz*dz;
        float collisionRadius = body->radius + COLLISION_CHECK_RADIUS;
        
        if (distSq < (collisionRadius * collisionRadius)) {
            damageShip(PLANET_DAMAGE);  // Instant death for planet collision
            break;
        }
    }
}

void updateShipHealth(void) {
    if (shipHealth.invulnerabilityFrames > 0) {
        shipHealth.invulnerabilityFrames--;
    }
    
    if (shipHealth.isExploding) {
        updateShipExplosion();
    } else {
        checkShipCollisions();
    }
}

void drawHealthBar(void) {
    // Don't draw health if ship is destroyed
    if (isShipDestroyed()) return;
    
    // Switch to orthographic projection for 2D drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable lighting and depth test for 2D elements
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Draw health bar background
    int barWidth = 200;
    int barHeight = 20;
    int margin = 20;
    int x = margin;
    int y = glutGet(GLUT_WINDOW_HEIGHT) - margin - barHeight;
    
    // Black background
    glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + barWidth, y);
    glVertex2f(x + barWidth, y + barHeight);
    glVertex2f(x, y + barHeight);
    glEnd();
    
    // Health bar (green to red based on health)
    float healthPercent = shipHealth.health / (float)INITIAL_HEALTH;
    float redComponent = 1.0f - healthPercent;
    float greenComponent = healthPercent;
    
    // Enable blending for smooth colors
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(redComponent, greenComponent, 0.0f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y + 2);  // Add small padding
    glVertex2f(x + 2 + (barWidth - 4) * healthPercent, y + 2);
    glVertex2f(x + 2 + (barWidth - 4) * healthPercent, y + barHeight - 2);
    glVertex2f(x + 2, y + barHeight - 2);
    glEnd();
    
    // Draw health text
    glColor3f(1.0f, 1.0f, 1.0f);
    char healthText[32];
    sprintf(healthText, "HEALTH: %d/%d", shipHealth.health, INITIAL_HEALTH);
    glRasterPos2f(x + 10, y + barHeight/2 + 3);  // Center text vertically
    
    for (const char* c = healthText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
    
    // Draw warning text if health is low
    if (healthPercent <= 0.25f) {
        // Flash the warning (use explosion timer for timing)
        if ((glutGet(GLUT_ELAPSED_TIME) / 250) % 2) {  // Flash every 250ms
            glColor3f(1.0f, 0.0f, 0.0f);
            const char* warningText = "WARNING: CRITICAL DAMAGE";
            int textWidth = strlen(warningText) * 9;  // Approximate width
            glRasterPos2f(x + (barWidth - textWidth)/2, y - 20);
            
            for (const char* c = warningText; *c != '\0'; c++) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
            }
        }
    }
    
    // If recently damaged, show damage indicator
    if (shipHealth.invulnerabilityFrames > 0) {
        // Flash the damage border
        if (shipHealth.invulnerabilityFrames % 10 < 5) {  // Flash every few frames
            glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
            glBegin(GL_QUADS);
            // Left border
            glVertex2f(0, 0);
            glVertex2f(20, 0);
            glVertex2f(20, glutGet(GLUT_WINDOW_HEIGHT));
            glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT));
            // Right border
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH)-20, 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH)-20, glutGet(GLUT_WINDOW_HEIGHT));
            // Top border
            glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT)-20);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)-20);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT));
            // Bottom border
            glVertex2f(0, 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 20);
            glVertex2f(0, 20);
            glEnd();
        }
    }
    
    // Restore states
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    // Restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void cleanupShipHealth(void) {
    if (shipHealth.explosionParticles) {
        free(shipHealth.explosionParticles);
        shipHealth.explosionParticles = NULL;
    }
    shipHealth.isExploding = 0;
}

void resetShipPosition(void) {
    shipState.x = SHIP_START_X;
    shipState.y = 0;
    shipState.z = SHIP_START_Z;
    shipState.yaw = 45;
    shipState.pitch = 0;
    shipState.roll = 0;
}

int isShipDestroyed(void) {
    return shipHealth.isExploding || shipHealth.health <= 0;
}

void resetGame(void) {
    // Clean up existing explosion
    cleanupShipHealth();
    
    // Reset ship position
    resetShipPosition();
    
    // Reset ship health
    shipHealth.health = INITIAL_HEALTH;
    shipHealth.isExploding = 0;
    shipHealth.invulnerabilityFrames = 0;
    shipHealth.explosionTimer = 0;
    
    // Reset game state
    gameOver = 0;
}

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

void checkBulletAsteroidCollisions(void) {
    extern Asteroid* asteroids;
    extern int asteroidBeltInitialized;
    
    if (!asteroidBeltInitialized || asteroids == NULL) return;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        
        for (int j = 0; j < NUM_ASTEROIDS; j++) {
            if (!asteroids[j].active || asteroids[j].fragmentsActive) continue;
            
            float dx = bullets[i].x - asteroids[j].x;
            float dy = bullets[i].y - asteroids[j].y;
            float dz = bullets[i].z - asteroids[j].z;
            float distSq = dx*dx + dy*dy + dz*dz;
            float collisionRadius = MAX_ASTEROID_RADIUS * 1.5f;
            
            if (distSq < (collisionRadius * collisionRadius)) {
                // Hit detected!
                printf("Hit asteroid %d! Health: %d\n", j, asteroids[j].health);  // Debug print
                
                bullets[i].active = 0;
                asteroids[j].health--;
                
                if (asteroids[j].health <= 0) {
                    printf("Asteroid %d destroyed!\n", j);  // Debug print
                    asteroids[j].active = 0;
                    initFragments(&asteroids[j]);
                }
                break;
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
    // Don't update ship controls if destroyed
    if (isShipDestroyed()) {
        updateShipExplosion();
        return;
    }

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
    
    // Update bullets
    UpdateBullets();
    
    // Update health and check collisions
    updateShipHealth();
    
    // Check for collisions
    checkBulletAsteroidCollisions();
}

void drawBattleship(void) {
    glPushMatrix();
    
    // Move to ship's position and apply rotations in correct order
    glTranslatef(shipState.x, shipState.y, shipState.z);
    glRotatef(180, 0, 1, 0);     // Initial -90 degree Y rotation
    glRotatef(shipState.yaw, 0, 1, 0);    // Yaw (left/right)
    glRotatef(shipState.pitch, 0, 0, 1);   // Pitch (up/down)
    glRotatef(shipState.roll, 1, 0, 0);    // Roll (banking)
    
    // Enable lighting and set material properties
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    
    // Increase base ambient light for better visibility
    GLfloat mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };  // Doubled ambient values
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 32.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // Main fuselage - lighter metallic silver-gray
    glColor3f(0.7, 0.72, 0.75);  // Much lighter base color
    GLfloat fuselage_spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, fuselage_spec);
    Cube(0, 0, 0, 2.0, 0.2, 0.5, 0, 0, 1, 0);  // Main body
    
    // Nose section - slightly darker but still visible
    glColor3f(0.65, 0.67, 0.7);  // Lighter nose color
    GLfloat nose_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, nose_spec);
    Cube(-1.8, 0.1, 0, 0.4, 0.15, 0.3, 15, 0, 1, 0);  // Upper nose
    Cube(-1.8, -0.05, 0, 0.4, 0.1, 0.4, -10, 0, 1, 0);  // Lower nose
    
    // Cockpit - bright reflective glass
    GLfloat glass_ambient[] = { 0.2f, 0.3f, 0.4f, 1.0f };  // Increased ambient
    GLfloat glass_diffuse[] = { 0.4f, 0.7f, 0.9f, 0.8f };  // Brighter blue
    GLfloat glass_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat glass_shininess[] = { 128.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, glass_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glass_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glass_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, glass_shininess);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Cube(-1.0, 0.2, 0, 0.6, 0.1, 0.3, 5, 0, 1, 0);  // Canopy
    glDisable(GL_BLEND);
    
    // Wings - lighter metallic with subtle color variation
    glColor3f(0.68, 0.7, 0.73);  // Slightly different shade for visual interest
    GLfloat wing_spec[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat wing_shininess[] = { 16.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, wing_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, wing_shininess);
    
    // Left wing
    Cube(-0.2, 0, -1.2, 1.2, 0.08, 0.8, 10, 0, 1, 0);
    // Right wing
    Cube(-0.2, 0, 1.2, 1.2, 0.08, 0.8, -10, 0, 1, 0);
    
    // Wing tips - accented edges
    glColor3f(0.75, 0.77, 0.8);  // Slightly lighter for emphasis
    GLfloat tip_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, tip_spec);
    // Left tip
    Cube(-0.2, 0.2, -1.6, 0.4, 0.3, 0.08, 60, 0, 0, 1);
    // Right tip
    Cube(-0.2, 0.2, 1.6, 0.4, 0.3, 0.08, -60, 0, 0, 1);
    
    // Tail section - bright metallic
    glColor3f(0.72, 0.74, 0.77);
    GLfloat tail_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, tail_spec);
    Cube(1.5, 0.3, 0, 0.4, 0.4, 0.08, 30, 0, 0, 1);  // Center tail
    // Horizontal stabilizers
    Cube(1.2, 0, -0.8, 0.6, 0.08, 0.4, 15, 1, 0, 0);  // Left
    Cube(1.2, 0, 0.8, 0.6, 0.08, 0.4, -15, 1, 0, 0);  // Right
    
    // Engine nozzles - darker contrast but still visible
    glColor3f(0.45, 0.47, 0.5);  // Darker but not black
    GLfloat engine_spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat engine_shininess[] = { 64.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, engine_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, engine_shininess);
    Cylinder(1.8, -0.1, -0.3, 0.15, 0.3);  // Left engine
    Cylinder(1.8, -0.1, 0.3, 0.15, 0.3);   // Right engine
    
    // Engine glow - brighter emission
    GLfloat glow_emission[] = { 1.0f, 0.6f, 0.2f, 1.0f };  // Brighter orange glow
    glMaterialfv(GL_FRONT, GL_EMISSION, glow_emission);
    Cylinder(2.0, -0.1, -0.3, 0.1, 0.1);  // Left glow
    Cylinder(2.0, -0.1, 0.3, 0.1, 0.1);   // Right glow
    
    // Reset emission
    GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
    
    // Front laser cannon - bright metallic
    glColor3f(0.6, 0.62, 0.65);  // Lighter cannon color
    GLfloat cannon_spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat cannon_shininess[] = { 96.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, cannon_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, cannon_shininess);
    Cylinder(-2.0, 0, 0, 0.1, 0.2);  // Cannon housing
    
    glDisable(GL_LIGHTING);
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
    
    // Calculate camera position
    float camX = shipState.x - camDist * cos(shipRad);
    float camY = shipState.y + camHeight * cos(pitchRad) - camDist * sin(pitchRad);
    float camZ = shipState.z - camDist * sin(shipRad);
    
    // Set up the camera
    gluLookAt(camX, camY, camZ,                    // Camera position
              shipState.x, shipState.y, shipState.z,// Look at ship
              0, 1, 0);                            // Up vector
}