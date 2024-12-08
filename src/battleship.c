#include "include/battleship.h"

Bullet bullets[MAX_BULLETS] = {0};
ShipState shipState = {
    .x = SHIP_START_X,  
    .y = 0,            
    .z = SHIP_START_Z, 
    .yaw = 45,         
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
    shipHealth.explosionFlashTime = 30; 

    float explosionX = shipState.x;
    float explosionY = shipState.y;
    float explosionZ = shipState.z;

    shipHealth.explosionParticles = (ExplosionParticle*)malloc(
        sizeof(ExplosionParticle) * EXPLOSION_PARTICLE_COUNT);

    if (!shipHealth.explosionParticles) {
        printf("Failed to allocate explosion particles!\n");
        return;
    }

    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];

        float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
        float elevation;
        float speed;

        if (i < MUSHROOM_CLOUD_PARTICLES) {

            elevation = ((float)rand() / RAND_MAX) * M_PI * 0.25f + M_PI * 0.25f;
            speed = 10.0f + ((float)rand() / RAND_MAX) * 20.0f;
        } else {

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

        if (i < EXPLOSION_PARTICLE_COUNT / 4) {

            p->r = 1.0f;
            p->g = 1.0f;
            p->b = 0.9f;
        } else if (i < EXPLOSION_PARTICLE_COUNT * 2/4) {

            p->r = 1.0f;
            p->g = 0.8f + ((float)rand() / RAND_MAX) * 0.2f;
            p->b = 0.2f;
        } else if (i < EXPLOSION_PARTICLE_COUNT * 3/4) {

            p->r = 1.0f;
            p->g = 0.4f + ((float)rand() / RAND_MAX) * 0.3f;
            p->b = 0.0f;
        } else {

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

    shipHealth.shockwaveSize += 2.0f;

    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        if (p->lifetime <= 0) continue;

        p->x += p->vx * 0.3f;
        p->y += p->vy * 0.3f;
        p->z += p->vz * 0.3f;

        if (i < MUSHROOM_CLOUD_PARTICLES && p->y > shipState.y + 10.0f) {
            float dx = p->x - shipState.x;
            float dz = p->z - shipState.z;
            float dist = sqrt(dx * dx + dz * dz);

            if (dist < EXPLOSION_SPREAD * 2) {
                float angle = atan2(dz, dx);
                p->vx += cos(angle) * 0.2f;
                p->vz += sin(angle) * 0.2f;
            }

            p->vy *= 0.95f;
        }

        p->x += (((float)rand() / RAND_MAX) - 0.5f) * 0.5f;
        p->y += (((float)rand() / RAND_MAX) - 0.5f) * 0.5f;
        p->z += (((float)rand() / RAND_MAX) - 0.5f) * 0.5f;

        p->vx *= 0.99f;
        p->vy *= 0.99f;
        p->vz *= 0.99f;

        float lifeRatio = p->lifetime / (float)EXPLOSION_LIFETIME;
        p->size *= 0.998f;
        p->a = lifeRatio * lifeRatio;

        if (i >= EXPLOSION_PARTICLE_COUNT * 3/4) {

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

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    for (int i = 0; i < EXPLOSION_PARTICLE_COUNT; i++) {
        ExplosionParticle* p = &shipHealth.explosionParticles[i];
        if (p->lifetime <= 0) continue;

        glPushMatrix();
        glTranslatef(p->x, p->y, p->z);

        glRotatef(-shipState.yaw, 0, 1, 0);
        glRotatef(-shipState.pitch, 0, 0, 1);

        glColor4f(p->r, p->g, p->b, p->a);

        float size = p->size * (1.0f + (float)(i < MUSHROOM_CLOUD_PARTICLES) * 0.5f);

        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);  
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
                collisionRadius = body->radius * 0.9f; 
                damage = SUN_DAMAGE;
                break;

            case CELESTIAL_PLANET:
                collisionRadius = body->radius + COLLISION_CHECK_RADIUS;
                damage = PLANET_DAMAGE;
                break;

            case CELESTIAL_SPACE_STATION:
                collisionRadius = body->radius * 1.5f; 
                damage = STATION_DAMAGE;
                break;

            case CELESTIAL_COMET:

                float cometHeadRadius = body->radius;
                float tailLength = body->radius * 30.0f; 

                if (distSq < (cometHeadRadius * cometHeadRadius)) {
                    damageShip(COMET_DAMAGE);
                    printf("Ship collided with comet head! Distance: %f\n", sqrt(distSq));
                    continue;
                }

                float angleToShip = atan2(dz, dx);
                float cometAngle = body->orbitAngle * M_PI / 180.0f;
                float angleDiff = fabs(angleToShip - cometAngle);
                while (angleDiff > M_PI) angleDiff = 2 * M_PI - angleDiff;

                if (angleDiff < M_PI/6 && 
                    distSq < (tailLength * tailLength) &&
                    distSq > (cometHeadRadius * cometHeadRadius)) {
                    damageShip(COMET_DAMAGE/2); 
                    printf("Ship collided with comet tail! Distance: %f\n", sqrt(distSq));
                }
                continue;

            default:
                continue; 
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

            switch(body->type) {
                case CELESTIAL_SUN:
                    shipHealth.health = 0;
                    if (!shipHealth.isExploding) {
                        createShipExplosion();
                    }
                    break;

                case CELESTIAL_SPACE_STATION:
                    break;

                case CELESTIAL_PLANET:
                    break;
            }

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

    if (isShipDestroyed()) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    int barWidth = 200;
    int barHeight = 20;
    int margin = 20;
    int x = margin;
    int y = glutGet(GLUT_WINDOW_HEIGHT) - margin - barHeight;

    glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + barWidth, y);
    glVertex2f(x + barWidth, y + barHeight);
    glVertex2f(x, y + barHeight);
    glEnd();

    float healthPercent = shipHealth.health / (float)INITIAL_HEALTH;
    float redComponent = 1.0f - healthPercent;
    float greenComponent = healthPercent;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(redComponent, greenComponent, 0.0f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y + 2);  
    glVertex2f(x + 2 + (barWidth - 4) * healthPercent, y + 2);
    glVertex2f(x + 2 + (barWidth - 4) * healthPercent, y + barHeight - 2);
    glVertex2f(x + 2, y + barHeight - 2);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    char healthText[32];
    sprintf(healthText, "HEALTH: %d/%d", shipHealth.health, INITIAL_HEALTH);
    glRasterPos2f(x + 10, y + barHeight/2 + 3);  

    for (const char* c = healthText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    if (healthPercent <= 0.25f) {

        if ((glutGet(GLUT_ELAPSED_TIME) / 250) % 2) {  
            glColor3f(1.0f, 0.0f, 0.0f);
            const char* warningText = "WARNING: CRITICAL DAMAGE";
            int textWidth = strlen(warningText) * 9;  
            glRasterPos2f(x + (barWidth - textWidth)/2, y - 20);

            for (const char* c = warningText; *c != '\0'; c++) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
            }
        }
    }

    if (shipHealth.invulnerabilityFrames > 0) {

        if (shipHealth.invulnerabilityFrames % 10 < 5) {  
            glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
            glBegin(GL_QUADS);

            glVertex2f(0, 0);
            glVertex2f(20, 0);
            glVertex2f(20, glutGet(GLUT_WINDOW_HEIGHT));
            glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT));

            glVertex2f(glutGet(GLUT_WINDOW_WIDTH)-20, 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH)-20, glutGet(GLUT_WINDOW_HEIGHT));

            glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT)-20);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)-20);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT));

            glVertex2f(0, 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0);
            glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 20);
            glVertex2f(0, 20);
            glEnd();
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

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

    cleanupShipHealth();

    resetShipPosition();

    shipHealth.health = INITIAL_HEALTH;
    shipHealth.isExploding = 0;
    shipHealth.invulnerabilityFrames = 0;
    shipHealth.explosionTimer = 0;

    gameOver = 0;
}

void DrawBullet(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(1.0, 0.6, 0.0, 1.0);
    Sphere(0, 0, 0, 0.04);
    
    glColor4f(1.0, 0.3, 0.0, 0.6);
    Sphere(0, 0, 0, 0.06);
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

void FireBullet(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {

            float yawRad = shipState.yaw * M_PI / 180.0f;
            float pitchRad = shipState.pitch * M_PI / 180.0f;

            float forwardX = cosf(yawRad);
            float forwardY = -sinf(pitchRad);
            float forwardZ = -sinf(yawRad);

            bullets[i].x = shipState.x + (2.1f * forwardX);
            bullets[i].y = shipState.y + (2.1f * forwardY);
            bullets[i].z = shipState.z + (2.1f * forwardZ);

            bullets[i].dirX = forwardX;
            bullets[i].dirY = forwardY;
            bullets[i].dirZ = forwardZ;

            bullets[i].active = 1;
            break;
        }
    }
}

void UpdateBullets(void) {
    static float bulletTimers[MAX_BULLETS] = {0};  

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {

            bullets[i].x += BULLET_SPEED * bullets[i].dirX;
            bullets[i].y += BULLET_SPEED * bullets[i].dirY;
            bullets[i].z += BULLET_SPEED * bullets[i].dirZ;

            bulletTimers[i]++;

            float dx = bullets[i].x - shipState.x;
            float dy = bullets[i].y - shipState.y;
            float dz = bullets[i].z - shipState.z;
            float distanceFromShip = sqrt(dx*dx + dy*dy + dz*dz);

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

            if (distSq < (asteroids[j].collisionRadius * asteroids[j].collisionRadius)) {
                printf("Hit asteroid %d! Health: %d\n", j, asteroids[j].health);  
                bullets[i].active = 0;
                asteroids[j].health--;

                if (asteroids[j].health <= 0) {
                    printf("Asteroid %d destroyed!\n", j);  
                    asteroids[j].active = 0;
                    initFragments(&asteroids[j]);
                }
                break;
            }
        }
    }
}

void MoveShip(float dx, float dz) {
    float angle = -shipState.yaw * M_PI / 180.0f;  
    float pitchRad = shipState.pitch * M_PI / 180.0f;

    float forwardX = cos(angle);
    float forwardY = -sin(pitchRad);  
    float forwardZ = sin(angle);

    shipState.x += (dx * forwardX - dz * sin(angle));
    shipState.y += (dx * forwardY);
    shipState.z += (dx * forwardZ + dz * cos(angle));
}

void RotateShip(float yaw, float pitch) {
    shipState.yaw += yaw;

    if (shipState.yaw > 360.0f) shipState.yaw -= 360.0f;
    if (shipState.yaw < 0.0f) shipState.yaw += 360.0f;

    shipState.pitch = fmax(-MAX_PITCH, fmin(MAX_PITCH, shipState.pitch + pitch));
}

void UpdateRoll(void) {

    if (shipState.aPressed) {
        shipState.roll = fmin(MAX_ROLL, shipState.roll + ROTATION_SPEED);
    } else if (shipState.dPressed) {
        shipState.roll = fmax(-MAX_ROLL, shipState.roll - ROTATION_SPEED);
    } else {

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

    if (isShipDestroyed()) {
        updateShipExplosion();
        return;
    }

    if (!shipState.wPressed && !shipState.sPressed) {
        if (shipState.pitch > 0) {
            shipState.pitch = fmax(0, shipState.pitch - PITCH_RETURN_SPEED);
        } else if (shipState.pitch < 0) {
            shipState.pitch = fmin(0, shipState.pitch + PITCH_RETURN_SPEED);
        }
    }

    UpdateRoll();

    UpdateBullets();

    updateShipHealth();

    checkBulletAsteroidCollisions();
}

void drawEngineGlow(void) {

    if (keyStates.upPressed) {
        shipState.engineGlowIntensity = fmin(ENGINE_MAX_GLOW, 
            shipState.engineGlowIntensity + ENGINE_GLOW_BUILDUP);
    } else {
        shipState.engineGlowIntensity = fmax(ENGINE_BASE_GLOW, 
            shipState.engineGlowIntensity - ENGINE_GLOW_DECAY);
    }

    float flicker = 1.0f + ((float)rand() / RAND_MAX - 0.5f) * ENGINE_GLOW_FLICKER;
    float glowIntensity = shipState.engineGlowIntensity * flicker;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    for (int i = 0; i < NUM_GLOW_LAYERS; i++) {
        float layerSize = 0.1f + (i * 0.05f);
        float alpha = glowIntensity * (1.0f - ((float)i / NUM_GLOW_LAYERS));

        glPushMatrix();
        glTranslatef(1.0, -0.05, -0.15);

        glColor4f(1.0f, 0.6f, 0.1f, alpha);
        Cylinder(0, 0, 0, layerSize, 0.1f);

        glColor4f(0.2f, 0.4f, 1.0f, alpha * 0.3f);
        Cylinder(0, 0, 0, layerSize * 1.5f, 0.15f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.0, -0.05, 0.15);

        glColor4f(1.0f, 0.6f, 0.1f, alpha);
        Cylinder(0, 0, 0, layerSize, 0.1f);

        glColor4f(0.2f, 0.4f, 1.0f, alpha * 0.3f);
        Cylinder(0, 0, 0, layerSize * 1.5f, 0.15f);
        glPopMatrix();

        if (keyStates.upPressed) {
            glBegin(GL_TRIANGLE_STRIP);
            float trailLength = 2.0f * glowIntensity;
            float trailWidth = layerSize * 0.8f;

            glColor4f(1.0f, 0.3f, 0.0f, alpha);
            glVertex3f(1.0f, -0.05f, -0.15f - trailWidth);
            glVertex3f(1.0f, -0.05f, -0.15f + trailWidth);
            glColor4f(1.0f, 0.3f, 0.0f, 0.0f);
            glVertex3f(1.0f + trailLength, -0.05f, -0.15f - trailWidth * 1.5f);
            glVertex3f(1.0f + trailLength, -0.05f, -0.15f + trailWidth * 1.5f);
            glEnd();

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

    glTranslatef(shipState.x, shipState.y, shipState.z);
    glRotatef(180, 0, 1, 0);     
    glRotatef(shipState.yaw, 0, 1, 0);    
    glRotatef(shipState.pitch, 0, 0, 1);   
    glRotatef(shipState.roll, 1, 0, 0);    

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 32.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(0.7, 0.72, 0.75);
    GLfloat fuselage_spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, fuselage_spec);
    Cube(0, 0, 0, 1.0, 0.1, 0.25, 0, 0, 1, 0);  

    glColor3f(0.65, 0.67, 0.7);
    GLfloat nose_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, nose_spec);
    Cube(-0.9, 0.05, 0, 0.2, 0.075, 0.15, 15, 0, 1, 0);  
    Cube(-0.9, -0.025, 0, 0.2, 0.05, 0.2, -10, 0, 1, 0);  

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
    Cube(-0.5, 0.1, 0, 0.3, 0.05, 0.15, 5, 0, 1, 0);  
    glDisable(GL_BLEND);

    glColor3f(0.68, 0.7, 0.73);
    GLfloat wing_spec[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat wing_shininess[] = { 16.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, wing_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, wing_shininess);

    Cube(-0.1, 0, -0.6, 0.6, 0.04, 0.4, 10, 0, 1, 0);

    Cube(-0.1, 0, 0.6, 0.6, 0.04, 0.4, -10, 0, 1, 0);

    glColor3f(0.75, 0.77, 0.8);
    GLfloat tip_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, tip_spec);

    Cube(-0.1, 0.1, -0.8, 0.2, 0.15, 0.04, 60, 0, 0, 1);

    Cube(-0.1, 0.1, 0.8, 0.2, 0.15, 0.04, -60, 0, 0, 1);

    glColor3f(0.72, 0.74, 0.77);
    GLfloat tail_spec[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, tail_spec);
    Cube(0.75, 0.15, 0, 0.2, 0.2, 0.04, 30, 0, 0, 1);  

    Cube(0.6, 0, -0.4, 0.3, 0.04, 0.2, 15, 1, 0, 0);  
    Cube(0.6, 0, 0.4, 0.3, 0.04, 0.2, -15, 1, 0, 0);  

    glColor3f(0.45, 0.47, 0.5);
    GLfloat engine_spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat engine_shininess[] = { 64.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, engine_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, engine_shininess);
    Cylinder(0.9, -0.05, -0.15, 0.075, 0.15);  
    Cylinder(0.9, -0.05, 0.15, 0.075, 0.15);   

    GLfloat glow_emission[] = { 1.0f, 0.6f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, glow_emission);
    Cylinder(1.0, -0.05, -0.15, 0.05, 0.05);  
    Cylinder(1.0, -0.05, 0.15, 0.05, 0.05);   

    GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

    glColor3f(0.6, 0.62, 0.65);
    GLfloat cannon_spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat cannon_shininess[] = { 96.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, cannon_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, cannon_shininess);
    Cylinder(-1.0, 0, 0, 0.05, 0.1);  

    glDisable(GL_LIGHTING);
    drawEngineGlow();

    glPopMatrix();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawBullet(bullets[i].x, bullets[i].y, bullets[i].z);
        }
    }
}

void SetupCamera(void) {
    float camDist = 5.0f;    
    float camHeight = 1.0f;   

    float shipRad = -shipState.yaw * M_PI / 180.0f;
    float pitchRad = shipState.pitch * M_PI / 180.0f;

    float camX = shipState.x - camDist * cos(shipRad);
    float camY = shipState.y + camHeight * cos(pitchRad) - camDist * sin(pitchRad);
    float camZ = shipState.z - camDist * sin(shipRad);

    gluLookAt(camX, camY, camZ,                    
              shipState.x, shipState.y, shipState.z,
              0, 1, 0);                            
}