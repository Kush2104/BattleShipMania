#include "include/battleship.h"


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
    shipHealth.explosionStage = 0;
    shipHealth.shockwaveSize = 0;
    shipHealth.explosionFlashTime = 0;
    shipState.yaw = 45;
    shipState.pitch = 0;
    shipState.roll = 0;
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
    cleanupShipHealth();
    
    shipHealth.isExploding = 1;
    shipHealth.explosionTimer = EXPLOSION_LIFETIME;
    shipHealth.explosionStage = 0;
    shipHealth.shockwaveSize = 0.0f;
    shipHealth.explosionFlashTime = 30; // Half second flash
    
    float explosionX = shipState.x;
    float explosionY = shipState.y;
    float explosionZ = shipState.z;
    
    shipHealth.explosionParticles = (ExplosionParticle*)malloc(
        sizeof(ExplosionParticle) * EXPLOSION_PARTICLE_COUNT);
    
    if (!shipHealth.explosionParticles) {
        printf("Failed to allocate explosion particles!\n");
        return;
    }
    
    // Initialize particles in layers for mushroom cloud effect
    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        
        float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
        float elevation;
        float speed;
        
        if (i < MUSHROOM_CLOUD_PARTICLES) {
            // Mushroom cloud cap particles
            elevation = ((float)rand() / RAND_MAX) * M_PI * 0.25f + M_PI * 0.25f;
            speed = 10.0f + ((float)rand() / RAND_MAX) * 20.0f;
        } else {
            // Regular explosion particles
            elevation = ((float)rand() / RAND_MAX - 0.5f) * M_PI;
            speed = 5.0f + ((float)rand() / RAND_MAX) * 15.0f;
        }
        
        p->x = explosionX;
        p->y = explosionY;
        p->z = explosionZ;
        
        p->vx = cos(angle) * cos(elevation) * speed;
        p->vy = sin(elevation) * speed;
        p->vz = sin(angle) * cos(elevation) * speed;
        
        p->size = INITIAL_EXPLOSION_SIZE * (0.8f + ((float)rand() / RAND_MAX) * 0.4f);
        p->lifetime = EXPLOSION_LIFETIME * (0.3f + ((float)rand() / RAND_MAX) * 0.7f);
        
        // Color distribution for nuclear effect
        if (i < EXPLOSION_PARTICLE_COUNT / 4) {
            // Core: Intense white-yellow
            p->r = 1.0f;
            p->g = 1.0f;
            p->b = 0.9f;
        } else if (i < EXPLOSION_PARTICLE_COUNT * 2/4) {
            // Mid: Bright orange-yellow
            p->r = 1.0f;
            p->g = 0.8f + ((float)rand() / RAND_MAX) * 0.2f;
            p->b = 0.2f;
        } else if (i < EXPLOSION_PARTICLE_COUNT * 3/4) {
            // Outer: Deep orange-red
            p->r = 1.0f;
            p->g = 0.4f + ((float)rand() / RAND_MAX) * 0.3f;
            p->b = 0.0f;
        } else {
            // Smoke: Dark grey with slight orange tint
            float grey = 0.3f + ((float)rand() / RAND_MAX) * 0.2f;
            p->r = grey + 0.1f;
            p->g = grey;
            p->b = grey - 0.1f;
        }
        p->a = 1.0f;
    }
}

void updateShipExplosion(void) {
    if (!shipHealth.isExploding || !shipHealth.explosionParticles) return;
    
    shipHealth.explosionTimer--;
    if (shipHealth.explosionFlashTime > 0) shipHealth.explosionFlashTime--;
    
    // Update shockwave (faster expansion)
    shipHealth.shockwaveSize += 2.0f;
    
    // Update particles with mushroom cloud behavior
    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        if (p->lifetime <= 0) continue;
        
        // More dramatic particle movement
        p->x += p->vx * 0.3f;
        p->y += p->vy * 0.3f;
        p->z += p->vz * 0.3f;
        
        // Mushroom cloud formation
        if (i < MUSHROOM_CLOUD_PARTICLES && p->y > shipState.y + 10.0f) {
            float dx = p->x - shipState.x;
            float dz = p->z - shipState.z;
            float dist = sqrt(dx * dx + dz * dz);
            
            // Spread particles outward at the top
            if (dist < EXPLOSION_SPREAD * 2) {
                float angle = atan2(dz, dx);
                p->vx += cos(angle) * 0.2f;
                p->vz += sin(angle) * 0.2f;
            }
            
            // Slow vertical velocity at top
            p->vy *= 0.95f;
        }
        
        // Add turbulence
        p->x += (((float)rand() / RAND_MAX) - 0.5f) * 0.5f;
        p->y += (((float)rand() / RAND_MAX) - 0.5f) * 0.5f;
        p->z += (((float)rand() / RAND_MAX) - 0.5f) * 0.5f;
        
        // Slow particles gradually
        p->vx *= 0.99f;
        p->vy *= 0.99f;
        p->vz *= 0.99f;
        
        // Size and alpha changes
        float lifeRatio = p->lifetime / (float)EXPLOSION_LIFETIME;
        p->size *= 0.998f;
        p->a = lifeRatio * lifeRatio;
        
        // Color evolution
        if (i >= EXPLOSION_PARTICLE_COUNT * 3/4) {
            // Smoke particles get darker
            p->r *= 0.995f;
            p->g *= 0.995f;
            p->b *= 0.995f;
        }
        
        p->lifetime--;
    }
    
    if (shipHealth.explosionTimer <= 0) {
        gameOver = 1;
    }
}

void drawShipExplosion(void) {
    if (!shipHealth.isExploding || !shipHealth.explosionParticles) return;
    
    glPushMatrix();
    
    // Draw initial flash (bigger and brighter)
    if (shipHealth.explosionFlashTime > 0) {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        float flashAlpha = shipHealth.explosionFlashTime / 30.0f;
        glColor4f(1.0f, 1.0f, 1.0f, flashAlpha);
        glPushMatrix();
        glTranslatef(shipState.x, shipState.y, shipState.z);
        glutSolidSphere(EXPLOSION_SPREAD * 2, 32, 32);
        glPopMatrix();
    }
    
    // Draw multiple expanding shockwaves
    for (int i = 0; i < SHOCKWAVE_COUNT; i++) {
        float offset = (float)i * (EXPLOSION_SPREAD * 2.0f / SHOCKWAVE_COUNT);
        float currentSize = shipHealth.shockwaveSize - offset;
        
        if (currentSize > 0 && currentSize < EXPLOSION_SPREAD * 6.0f) {
            float shockwaveAlpha = 1.0f - (currentSize / (EXPLOSION_SPREAD * 6.0f));
            glColor4f(1.0f, 0.8f, 0.5f, shockwaveAlpha * 0.5f);
            
            glPushMatrix();
            glTranslatef(shipState.x, shipState.y, shipState.z);
            glutSolidTorus(0.2f, currentSize, 32, 32);
            glPopMatrix();
        }
    }
    
    // Draw explosion particles
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        if (p->lifetime <= 0) continue;
        
        glPushMatrix();
        glTranslatef(p->x, p->y, p->z);
        
        // Billboarding
        glRotatef(-shipState.yaw, 0, 1, 0);
        glRotatef(-shipState.pitch, 0, 0, 1);
        
        glColor4f(p->r, p->g, p->b, p->a);
        
        float size = p->size * (1.0f + (float)(i < MUSHROOM_CLOUD_PARTICLES) * 0.5f);
        
        // Draw larger, more dramatic particles
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);  // Center
        for (int j = 0; j <= 12; j++) {
            float angle = j * (2 * M_PI / 12);
            glVertex3f(cos(angle) * size, sin(angle) * size, 0);
        }
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
            
            float asteroidRadius = MIN_ASTEROID_RADIUS + 
                ((float)rand() / RAND_MAX) * (MAX_ASTEROID_RADIUS - MIN_ASTEROID_RADIUS);
            float collisionDistance = COLLISION_CHECK_RADIUS + asteroidRadius;
            
            if (distSq < (collisionDistance * collisionDistance)) {
                damageShip(ASTEROID_DAMAGE);
                printf("Ship collided with asteroid! Distance: %f\n", sqrt(distSq));
                break;
            }
        }
    }
    
    // Check all celestial bodies
    extern CelestialBody solarBodies[];
    extern int bodyCount;
    
    for (int i = 0; i < bodyCount; i++) {
        CelestialBody* body = &solarBodies[i];
        float dx = shipState.x - body->x;
        float dy = shipState.y - body->y;
        float dz = shipState.z - body->z;
        float distSq = dx*dx + dy*dy + dz*dz;
        float collisionRadius;
        int damage = 0;
        
        switch(body->type) {
            case CELESTIAL_SUN:
                collisionRadius = body->radius * 0.9f; // Slightly smaller than visual radius
                damage = SUN_DAMAGE;
                break;
                
            case CELESTIAL_PLANET:
                collisionRadius = body->radius + COLLISION_CHECK_RADIUS;
                damage = PLANET_DAMAGE;
                break;
                
            case CELESTIAL_SPACE_STATION:
                collisionRadius = body->radius * 1.5f; // Larger collision box for station
                damage = STATION_DAMAGE;
                break;
                
            case CELESTIAL_COMET:
                // For comet, check both head and tail collision
                float cometHeadRadius = body->radius;
                float tailLength = body->radius * 30.0f; // Match the comet's tail length
                
                // Check head collision first
                if (distSq < (cometHeadRadius * cometHeadRadius)) {
                    damageShip(COMET_DAMAGE);
                    printf("Ship collided with comet head! Distance: %f\n", sqrt(distSq));
                    continue;
                }
                
                // Check tail collision using a cone shape
                float angleToShip = atan2(dz, dx);
                float cometAngle = body->orbitAngle * M_PI / 180.0f;
                float angleDiff = fabs(angleToShip - cometAngle);
                while (angleDiff > M_PI) angleDiff = 2 * M_PI - angleDiff;
                
                // If within tail angle and distance
                if (angleDiff < M_PI/6 && // 30-degree cone
                    distSq < (tailLength * tailLength) &&
                    distSq > (cometHeadRadius * cometHeadRadius)) {
                    damageShip(COMET_DAMAGE/2); // Less damage for tail collision
                    printf("Ship collided with comet tail! Distance: %f\n", sqrt(distSq));
                }
                continue;
                
            default:
                continue; // Skip other types
        }
        
        if (damage > 0 && distSq < (collisionRadius * collisionRadius)) {
            damageShip(damage);
            const char* bodyType;
            switch(body->type) {
                case CELESTIAL_SUN: bodyType = "sun"; break;
                case CELESTIAL_PLANET: bodyType = "planet"; break;
                case CELESTIAL_SPACE_STATION: bodyType = "space station"; break;
                default: bodyType = "unknown"; break;
            }
            printf("Ship collided with %s (%s)! Distance: %f\n", 
                   body->name ? body->name : "unnamed",
                   bodyType,
                   sqrt(distSq));
            
            // Special effects for different collisions
            switch(body->type) {
                case CELESTIAL_SUN:
                    // Instant explosion effect
                    shipHealth.health = 0;
                    if (!shipHealth.isExploding) {
                        createShipExplosion();
                    }
                    break;
                    
                case CELESTIAL_SPACE_STATION:
                    // Maybe add some spark effects here
                    // For now, just applying damage is enough
                    break;
                    
                case CELESTIAL_PLANET:
                    // Already handled by damage system
                    break;
            }
            
            break; // Exit after first collision
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
    shipHealth.explosionTimer = 0;
    shipHealth.explosionStage = 0;
    shipHealth.shockwaveSize = 0;
    shipHealth.explosionFlashTime = 0;
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
    Sphere(0, 0, 0, 0.04);  // Increased from 0.02
    
    // Orange glow
    glColor4f(1.0, 0.3, 0.0, 0.6);
    Sphere(0, 0, 0, 0.06);  // Increased from 0.03
    
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

void drawEngineGlow(void) {
    // Update glow intensity based on forward movement
    if (keyStates.upPressed) {
        shipState.engineGlowIntensity = fmin(ENGINE_MAX_GLOW, 
            shipState.engineGlowIntensity + ENGINE_GLOW_BUILDUP);
    } else {
        shipState.engineGlowIntensity = fmax(ENGINE_BASE_GLOW, 
            shipState.engineGlowIntensity - ENGINE_GLOW_DECAY);
    }
    
    // Add random flicker
    float flicker = 1.0f + ((float)rand() / RAND_MAX - 0.5f) * ENGINE_GLOW_FLICKER;
    float glowIntensity = shipState.engineGlowIntensity * flicker;
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    // Draw multiple layers of engine glow
    for (int i = 0; i < NUM_GLOW_LAYERS; i++) {
        float layerSize = 0.1f + (i * 0.05f);
        float alpha = glowIntensity * (1.0f - ((float)i / NUM_GLOW_LAYERS));
        
        // Left engine glow
        glPushMatrix();
        glTranslatef(1.0, -0.05, -0.15);
        
        // Core glow (bright orange)
        glColor4f(1.0f, 0.6f, 0.1f, alpha);
        Cylinder(0, 0, 0, layerSize, 0.1f);
        
        // Outer glow (blue tint)
        glColor4f(0.2f, 0.4f, 1.0f, alpha * 0.3f);
        Cylinder(0, 0, 0, layerSize * 1.5f, 0.15f);
        glPopMatrix();
        
        // Right engine glow
        glPushMatrix();
        glTranslatef(1.0, -0.05, 0.15);
        
        // Core glow
        glColor4f(1.0f, 0.6f, 0.1f, alpha);
        Cylinder(0, 0, 0, layerSize, 0.1f);
        
        // Outer glow
        glColor4f(0.2f, 0.4f, 1.0f, alpha * 0.3f);
        Cylinder(0, 0, 0, layerSize * 1.5f, 0.15f);
        glPopMatrix();

        // Draw exhaust trails when moving
        if (keyStates.upPressed) {
            glBegin(GL_TRIANGLE_STRIP);
            float trailLength = 2.0f * glowIntensity;
            float trailWidth = layerSize * 0.8f;
            
            // Left engine trail
            glColor4f(1.0f, 0.3f, 0.0f, alpha);
            glVertex3f(1.0f, -0.05f, -0.15f - trailWidth);
            glVertex3f(1.0f, -0.05f, -0.15f + trailWidth);
            glColor4f(1.0f, 0.3f, 0.0f, 0.0f);
            glVertex3f(1.0f + trailLength, -0.05f, -0.15f - trailWidth * 1.5f);
            glVertex3f(1.0f + trailLength, -0.05f, -0.15f + trailWidth * 1.5f);
            glEnd();
            
            // Right engine trail
            glBegin(GL_TRIANGLE_STRIP);
            glColor4f(1.0f, 0.3f, 0.0f, alpha);
            glVertex3f(1.0f, -0.05f, 0.15f - trailWidth);
            glVertex3f(1.0f, -0.05f, 0.15f + trailWidth);
            glColor4f(1.0f, 0.3f, 0.0f, 0.0f);
            glVertex3f(1.0f + trailLength, -0.05f, 0.15f - trailWidth * 1.5f);
            glVertex3f(1.0f + trailLength, -0.05f, 0.15f + trailWidth * 1.5f);
            glEnd();
        }
    }
    
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
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
    GLfloat mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 32.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // Main fuselage - lighter metallic silver-gray
    glColor3f(0.7, 0.72, 0.75);
    GLfloat fuselage_spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, fuselage_spec);
    Cube(0, 0, 0, 1.0, 0.1, 0.25, 0, 0, 1, 0);  // Main body
    
    // Nose section - slightly darker but still visible
    glColor3f(0.65, 0.67, 0.7);
    GLfloat nose_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, nose_spec);
    Cube(-0.9, 0.05, 0, 0.2, 0.075, 0.15, 15, 0, 1, 0);  // Upper nose
    Cube(-0.9, -0.025, 0, 0.2, 0.05, 0.2, -10, 0, 1, 0);  // Lower nose
    
    // Cockpit - bright reflective glass
    GLfloat glass_ambient[] = { 0.2f, 0.3f, 0.4f, 1.0f };
    GLfloat glass_diffuse[] = { 0.4f, 0.7f, 0.9f, 0.8f };
    GLfloat glass_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat glass_shininess[] = { 128.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, glass_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glass_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glass_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, glass_shininess);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Cube(-0.5, 0.1, 0, 0.3, 0.05, 0.15, 5, 0, 1, 0);  // Canopy
    glDisable(GL_BLEND);
    
    // Wings - lighter metallic with subtle color variation
    glColor3f(0.68, 0.7, 0.73);
    GLfloat wing_spec[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat wing_shininess[] = { 16.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, wing_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, wing_shininess);
    
    // Left wing
    Cube(-0.1, 0, -0.6, 0.6, 0.04, 0.4, 10, 0, 1, 0);
    // Right wing
    Cube(-0.1, 0, 0.6, 0.6, 0.04, 0.4, -10, 0, 1, 0);
    
    // Wing tips - accented edges
    glColor3f(0.75, 0.77, 0.8);
    GLfloat tip_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, tip_spec);
    // Left tip
    Cube(-0.1, 0.1, -0.8, 0.2, 0.15, 0.04, 60, 0, 0, 1);
    // Right tip
    Cube(-0.1, 0.1, 0.8, 0.2, 0.15, 0.04, -60, 0, 0, 1);
    
    // Tail section - bright metallic
    glColor3f(0.72, 0.74, 0.77);
    GLfloat tail_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, tail_spec);
    Cube(0.75, 0.15, 0, 0.2, 0.2, 0.04, 30, 0, 0, 1);  // Center tail
    // Horizontal stabilizers
    Cube(0.6, 0, -0.4, 0.3, 0.04, 0.2, 15, 1, 0, 0);  // Left
    Cube(0.6, 0, 0.4, 0.3, 0.04, 0.2, -15, 1, 0, 0);  // Right
    
    // Engine nozzles - darker contrast but still visible
    glColor3f(0.45, 0.47, 0.5);
    GLfloat engine_spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat engine_shininess[] = { 64.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, engine_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, engine_shininess);
    Cylinder(0.9, -0.05, -0.15, 0.075, 0.15);  // Left engine
    Cylinder(0.9, -0.05, 0.15, 0.075, 0.15);   // Right engine
    
    // Engine glow - brighter emission
    GLfloat glow_emission[] = { 1.0f, 0.6f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, glow_emission);
    Cylinder(1.0, -0.05, -0.15, 0.05, 0.05);  // Left glow
    Cylinder(1.0, -0.05, 0.15, 0.05, 0.05);   // Right glow
    
    // Reset emission
    GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
    
    // Front laser cannon - bright metallic
    glColor3f(0.6, 0.62, 0.65);
    GLfloat cannon_spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat cannon_shininess[] = { 96.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, cannon_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, cannon_shininess);
    Cylinder(-1.0, 0, 0, 0.05, 0.1);  // Cannon housing
    
    glDisable(GL_LIGHTING);
    drawEngineGlow();

    glPopMatrix();
    
    // Draw all active bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawBullet(bullets[i].x, bullets[i].y, bullets[i].z);
        }
    }
}

void SetupCamera(void) {
    float camDist = 5.0f;    // Reduced from 10.0f - closer to ship
    float camHeight = 1.0f;   // Reduced from 2.0f - lower camera angle
    
    // Calculate camera position based on ship's orientation
    float shipRad = -shipState.yaw * M_PI / 180.0f;
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