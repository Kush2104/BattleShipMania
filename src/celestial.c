// solar_system.c
#include "include/celestial.h"

CelestialBody solarBodies[MAX_BODIES];
int bodyCount = 0;

static GLuint asteroidTexture = 0;
static GLuint earthTexture = 0;
static GLuint marsTexture = 0;
static GLuint mercuryTexture = 0;
static GLuint venusTexture = 0;

Asteroid* asteroids = NULL;
int asteroidBeltInitialized = 0;

float smoothNoise(float x, float y, float z) {
    return (float)rand() / RAND_MAX;
}

void initSolarSystem(void) {
    bodyCount = 0;  // Reset counter
    
    // Initialize Sun (kept exactly as before)
    CelestialBody* sun = &solarBodies[bodyCount++];
    sun->originalRadius = REAL_SUN_RADIUS;
    sun->radius = getScaledRadius(REAL_SUN_RADIUS);
    sun->x = sun->y = sun->z = 0;
    sun->rotationSpeed = 0.1f;
    sun->currentRotation = 0;
    sun->type = CELESTIAL_SUN;
    sun->color[0] = 1.0f;    // R
    sun->color[1] = 0.85f;   // G
    sun->color[2] = 0.4f;    // B
    sun->color[3] = 1.0f;    // A
    sun->name = "Sol";

     mercuryTexture = LoadBMP("src/assets/textures/mercury.bmp");
    if (!mercuryTexture) {
        printf("Failed to load Mercury texture\n");
        exit(1);
    }

    // Mercury
    CelestialBody* mercury = &solarBodies[bodyCount++];
    mercury->originalRadius = REAL_EARTH_RADIUS * 0.383f;
    mercury->radius = 20.0f;
    mercury->orbitRadius = 500.0f;
    mercury->orbitSpeed = 0.02f;
    mercury->orbitAngle = 0;
    mercury->rotationSpeed = 0.3f;
    mercury->currentRotation = 0;
    mercury->type = CELESTIAL_PLANET;
    mercury->color[0] = 1.0f;  // Set to white for texture
    mercury->color[1] = 1.0f;
    mercury->color[2] = 1.0f;
    mercury->color[3] = 1.0f;
    mercury->name = "Mercury";

    venusTexture = LoadBMP("src/assets/textures/venus.bmp");
    if (!venusTexture) {
        printf("Failed to load Venus texture\n");
        exit(1);
    }

    // Venus
    CelestialBody* venus = &solarBodies[bodyCount++];
    venus->originalRadius = REAL_EARTH_RADIUS * 0.949f;
    venus->radius = 25.0f;
    venus->orbitRadius = 1000.0f;
    venus->orbitSpeed = 0.015f;
    venus->orbitAngle = 45;
    venus->rotationSpeed = 0.27f;
    venus->currentRotation = 0;
    venus->type = CELESTIAL_PLANET;
    venus->color[0] = 1.0f;  // Set to white for texture
    venus->color[1] = 1.0f;
    venus->color[2] = 1.0f;
    venus->color[3] = 1.0f;
    venus->name = "Venus";

    // Earth

    earthTexture = LoadBMP("src/assets/textures/earth.bmp");
    if (!earthTexture) {
        printf("Failed to load Earth texture\n");
        exit(1);
    }

    CelestialBody* earth = &solarBodies[bodyCount++];
    earth->originalRadius = REAL_EARTH_RADIUS;
    earth->radius = 30.0f;  // Visible size
    earth->orbitRadius = 1500.0f;  // Three-quarters of the way to asteroid belt
    earth->orbitSpeed = 0.01f;
    earth->orbitAngle = 90;
    earth->rotationSpeed = 0.5f;
    earth->currentRotation = 0;
    earth->type = CELESTIAL_PLANET;
    earth->color[0] = 0.2f;
    earth->color[1] = 0.5f;
    earth->color[2] = 0.8f;
    earth->color[3] = 1.0f;
    earth->name = "Earth";

    marsTexture = LoadBMP("src/assets/textures/mars.bmp");
    if (!marsTexture) {
        printf("Failed to load Mars texture\n");
        exit(1);
    }

    // Mars
    CelestialBody* mars = &solarBodies[bodyCount++];
    mars->originalRadius = REAL_EARTH_RADIUS * 0.532f;
    mars->radius = 22.0f;
    mars->orbitRadius = 1800.0f;
    mars->orbitSpeed = 0.008f;
    mars->orbitAngle = 135;
    mars->rotationSpeed = 0.48f;
    mars->currentRotation = 0;
    mars->type = CELESTIAL_PLANET;
    mars->color[0] = 1.0f;  // Set to white for texture
    mars->color[1] = 1.0f;
    mars->color[2] = 1.0f;
    mars->color[3] = 1.0f;
    mars->name = "Mars";

    // Initialize Asteroid Belt (kept exactly as before)
    asteroidTexture = LoadBMP("src/assets/textures/rock.bmp");
    if (!asteroidTexture) {
        printf("Failed to load asteroid texture\n");
        exit(1);
    }

    CelestialBody* asteroidBelt = &solarBodies[bodyCount++];
    asteroidBelt->originalRadius = REAL_EARTH_ORBIT * 1.2f;
    asteroidBelt->radius = getScaledDistance(REAL_EARTH_ORBIT * 1.2f);
    asteroidBelt->orbitRadius = getScaledDistance(REAL_EARTH_ORBIT * 1.2f);
    asteroidBelt->x = 0;
    asteroidBelt->y = 0;
    asteroidBelt->z = 0;
    asteroidBelt->rotationSpeed = 0.05f;
    asteroidBelt->currentRotation = 0;
    asteroidBelt->type = CELESTIAL_ASTEROID;
    asteroidBelt->color[0] = 0.7f;
    asteroidBelt->color[1] = 0.7f;
    asteroidBelt->color[2] = 0.7f;
    asteroidBelt->color[3] = 1.0f;
    asteroidBelt->name = "Asteroid Belt";
    asteroidBelt->specialEffectTimer = 0.0f;
    asteroidBelt->specialEffectIntensity = 1.0f;
}

Vertex3D* generateAsteroidVertices(int* numVertices) {
    *numVertices = MIN_ASTEROID_VERTICES;
    int verticalSegments = 6;
    int horizontalSegments = *numVertices / 2;
    
    int totalVertices = (verticalSegments - 1) * horizontalSegments + 2;
    Vertex3D* vertices = (Vertex3D*)malloc(sizeof(Vertex3D) * totalVertices);
    
    float baseRadius = MIN_ASTEROID_RADIUS + 
                      ((float)rand() / RAND_MAX) * (MAX_ASTEROID_RADIUS - MIN_ASTEROID_RADIUS);

    int idx = 0;
    
    // Top vertex
    vertices[idx].x = 0;
    vertices[idx].y = baseRadius;
    vertices[idx].z = 0;
    vertices[idx].u = 0.5f;  // Center of texture for pole
    vertices[idx].v = 0.0f;  // Top of texture
    idx++;
    
    // Generate vertices in rings
    for (int i = 1; i < verticalSegments; i++) {
        float phi = M_PI * i / verticalSegments;
        float y = baseRadius * cos(phi);
        float radius = baseRadius * sin(phi);
        float v = (float)i / verticalSegments;  // Vertical texture coordinate
        
        for (int j = 0; j < horizontalSegments; j++) {
            float theta = 2 * M_PI * j / horizontalSegments;
            float u = (float)j / horizontalSegments;  // Horizontal texture coordinate
            
            // Calculate base spherical position
            float x = radius * cos(theta);
            float z = radius * sin(theta);
            
            // Add controlled random variation
            float distortion = 1.0f + ((float)rand() / RAND_MAX - 0.5f) * SURFACE_ROUGHNESS;
            
            vertices[idx].x = x * distortion;
            vertices[idx].y = y * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SURFACE_ROUGHNESS);
            vertices[idx].z = z * distortion;
            vertices[idx].u = u;
            vertices[idx].v = v;
            
            idx++;
        }
    }
    
    // Bottom vertex
    vertices[idx].x = 0;
    vertices[idx].y = -baseRadius;
    vertices[idx].z = 0;
    vertices[idx].u = 0.5f;  // Center of texture for pole
    vertices[idx].v = 1.0f;  // Bottom of texture
    
    *numVertices = totalVertices;
    return vertices;
}

void drawAsteroid(Asteroid* asteroid) {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Set material properties for lighting
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // White for proper texture color
    GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_shininess[] = { 32.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // Enable texturing before setting texture environment
    glEnable(GL_TEXTURE_2D);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture environment mode to modulate
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Bind the asteroid texture
    glBindTexture(GL_TEXTURE_2D, asteroidTexture);

    int horizontalSegments = (MIN_ASTEROID_VERTICES / 2);
    int verticalSegments = 6;
    
    // Draw top cap
    glBegin(GL_TRIANGLE_FAN);
    // Center vertex - calculate normal pointing straight up
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.0f);
    glVertex3f(asteroid->vertices[0].x, 
              asteroid->vertices[0].y, 
              asteroid->vertices[0].z);
              
    // Edge vertices
    for (int j = 0; j <= horizontalSegments; j++) {
        int idx = 1 + (j % horizontalSegments);
        Vertex3D v = asteroid->vertices[idx];
        // Calculate normal as normalized position
        float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        glNormal3f(v.x/len, v.y/len, v.z/len);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
    
    // Draw body segments
    for (int i = 0; i < verticalSegments - 2; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= horizontalSegments; j++) {
            for (int k = 0; k < 2; k++) {
                int row = i + k;
                int idx = 1 + row * horizontalSegments + (j % horizontalSegments);
                Vertex3D v = asteroid->vertices[idx];
                float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
                glNormal3f(v.x/len, v.y/len, v.z/len);
                glTexCoord2f(v.u, v.v);
                glVertex3f(v.x, v.y, v.z);
            }
        }
        glEnd();
    }
    
    // Draw bottom cap
    glBegin(GL_TRIANGLE_FAN);
    int lastIdx = asteroid->numVertices - 1;
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(asteroid->vertices[lastIdx].x, 
              asteroid->vertices[lastIdx].y, 
              asteroid->vertices[lastIdx].z);
              
    int startIdx = lastIdx - horizontalSegments;
    for (int j = horizontalSegments; j >= 0; j--) {
        int idx = startIdx + (j % horizontalSegments);
        Vertex3D v = asteroid->vertices[idx];
        float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        glNormal3f(v.x/len, v.y/len, v.z/len);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();

    // Disable texturing
    glDisable(GL_TEXTURE_2D);
}

void drawAsteroidBelt(CelestialBody* belt) {
    static float* fixedY = NULL;
    
    // Initialize asteroids if not already done
    if (!asteroidBeltInitialized) {
        srand(1234);  // Fixed seed for consistent generation
        asteroids = (Asteroid*)malloc(sizeof(Asteroid) * NUM_ASTEROIDS);
        fixedY = (float*)malloc(sizeof(float) * NUM_ASTEROIDS);
        
        if (!asteroids || !fixedY) {
            printf("Failed to allocate memory for asteroids\n");
            exit(1);
        }
        
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            asteroids[i].numVertices = 0;
            asteroids[i].vertices = generateAsteroidVertices(&asteroids[i].numVertices);
            asteroids[i].rotation = ((float)rand() / RAND_MAX) * 360.0f;
            asteroids[i].rotationSpeed = 0.0f;
            asteroids[i].orbitAngle = ((float)rand() / RAND_MAX) * 2 * M_PI;
            asteroids[i].orbitRadius = ASTEROID_BELT_DISTANCE + ((float)rand() / RAND_MAX - 0.5) * BELT_WIDTH;
            asteroids[i].orbitSpeed = MIN_ORBIT_SPEED;
            asteroids[i].active = 1;
            asteroids[i].explosion.active = 0;
            fixedY[i] = ((float)rand() / RAND_MAX - 0.5) * BELT_HEIGHT;
        }
        
        asteroidBeltInitialized = 1;  // Mark as initialized
    }
    
    // Draw each asteroid
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            asteroids[i].orbitAngle += asteroids[i].orbitSpeed;
            
            float x = cos(asteroids[i].orbitAngle) * asteroids[i].orbitRadius;
            float z = sin(asteroids[i].orbitAngle) * asteroids[i].orbitRadius;
            
            // Store position for collision detection
            asteroids[i].x = x;
            asteroids[i].y = fixedY[i];
            asteroids[i].z = z;
            
            glPushMatrix();
            glTranslatef(x, fixedY[i], z);
            glRotatef(asteroids[i].rotation, 0, 1, 0);
            drawAsteroid(&asteroids[i]);
            glPopMatrix();
        }
        
        // Draw explosion if active
        if (asteroids[i].explosion.active) {
            updateExplosion(&asteroids[i].explosion);
            drawExplosion(&asteroids[i].explosion);
        }
    }
}

void cleanupAsteroids(void) {
    if (asteroids) {
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            if (asteroids[i].vertices) {
                free(asteroids[i].vertices);
            }
        }
        free(asteroids);
        asteroids = NULL;
        asteroidBeltInitialized = 0;
    }
}

void setupSolarLighting(void) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    
    // Set up light properties
    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light_diffuse[] = { 1.2f, 1.2f, 1.2f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Adjusted attenuation for strong directional lighting
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0000005);

    // Make sure we're using GL_MODULATE for proper lighting of textures
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void printCameraInfo(void) {
    // Call this from your display function
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    printf("Camera Position: %.2f, %.2f, %.2f\n", 
           -modelview[12], -modelview[13], -modelview[14]);
}

float getScaledDistance(float realDistance) {
    return realDistance * SPACE_SCALE_FACTOR;
}

float getScaledRadius(float realRadius) {
    return realRadius * BODY_SCALE_FACTOR;
}

void updateSolarSystem(void) {
    for(int i = 0; i < bodyCount; i++) {
        CelestialBody* body = &solarBodies[i];
        
        // Update rotation
        body->currentRotation += body->rotationSpeed;
        if(body->currentRotation >= 360.0f) body->currentRotation -= 360.0f;
        
        // Update orbit for planets
        if(body->type == CELESTIAL_PLANET) {
            body->orbitAngle += body->orbitSpeed;
            if(body->orbitAngle >= 360.0f) body->orbitAngle -= 360.0f;
            
            // Calculate position based on orbit
            float angleRad = body->orbitAngle * M_PI / 180.0f;
            body->x = cos(angleRad) * body->orbitRadius;
            body->z = sin(angleRad) * body->orbitRadius;
            body->y = 0;  // Keeping orbits in the XZ plane for now
        }
        
        // Update special effects timer for asteroid belt
        if(body->type == CELESTIAL_ASTEROID) {
            body->specialEffectTimer += 0.1f;
            if(body->specialEffectTimer >= 360.0f) {
                body->specialEffectTimer = 0.0f;
            }
        }
    }
}

void drawBody(CelestialBody* body) {
    if(body->type == CELESTIAL_SUN) {
        glPushMatrix();
        glTranslatef(body->x, body->y, body->z);
        glRotatef(body->currentRotation, 0, 1, 0);
        
        glDisable(GL_LIGHTING);
        
        // Draw the sun's core
        glColor4f(body->color[0], body->color[1], body->color[2], body->color[3]);
        Sphere(0, 0, 0, body->radius);
        
        // Draw sun's glow
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        const int CORONA_LAYERS = 8;
        for(int i = 0; i < CORONA_LAYERS; i++) {
            float scale = 1.0f + (i * 0.15f);
            float alpha = 0.3f * (1.0f - ((float)i / CORONA_LAYERS));
            
            glColor4f(
                body->color[0],
                body->color[1] * (1.0f - (i * 0.1f)),
                body->color[2] * (1.0f - (i * 0.15f)),
                alpha
            );
            Sphere(0, 0, 0, body->radius * scale);
        }
        
        glDisable(GL_BLEND);
        glPopMatrix();

        // Position the light at the sun's center
        GLfloat light_position[] = { body->x, body->y, body->z, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        
        glEnable(GL_LIGHTING);
    }
    else if(body->type == CELESTIAL_PLANET) {
        glPushMatrix();
        
        float orbitAngleRad = body->orbitAngle * M_PI / 180.0f;
        body->x = cos(orbitAngleRad) * body->orbitRadius;
        body->z = sin(orbitAngleRad) * body->orbitRadius;
        
        glTranslatef(body->x, body->y, body->z);
        glRotatef(body->currentRotation, 0, 1, 0);

        if (strcmp(body->name, "Earth") == 0 || 
            strcmp(body->name, "Mars") == 0 || 
            strcmp(body->name, "Mercury") == 0 || 
            strcmp(body->name, "Venus") == 0) {
            
            // Material properties
            GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
            GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
            GLfloat mat_shininess[] = { 32.0f };

            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
            glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

            // Enable texturing
            glEnable(GL_TEXTURE_2D);
            
            // Bind appropriate texture
            if (strcmp(body->name, "Earth") == 0) {
                glBindTexture(GL_TEXTURE_2D, earthTexture);
            } else if (strcmp(body->name, "Mars") == 0) {
                glBindTexture(GL_TEXTURE_2D, marsTexture);
            } else if (strcmp(body->name, "Mercury") == 0) {
                glBindTexture(GL_TEXTURE_2D, mercuryTexture);
            } else if (strcmp(body->name, "Venus") == 0) {
                glBindTexture(GL_TEXTURE_2D, venusTexture);
            }
            
            // Ensure texture environment is set correctly
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            
            // Draw planet with full bright color to show texture properly
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            
            // Draw the sphere with texture coordinates
            const int stacks = 32;
            const int slices = 32;
            const float r = body->radius;
            
            for (int i = 0; i < stacks; i++) {
                float lat0 = M_PI * (-0.5 + (float)i / stacks);
                float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);
                
                glBegin(GL_QUAD_STRIP);
                for (int j = 0; j <= slices; j++) {
                    float lng = 2 * M_PI * (float)j / slices;
                    
                    for (int k = 0; k <= 1; k++) {
                        float lat = k ? lat1 : lat0;
                        float y = sin(lat);
                        float x = cos(lng) * cos(lat);
                        float z = sin(lng) * cos(lat);
                        
                        // Texture coordinates
                        glTexCoord2f((float)j / slices, (float)(i + k) / stacks);
                        
                        // Normal for lighting
                        glNormal3f(x, y, z);
                        
                        // Vertex
                        glVertex3f(r * x, r * y, r * z);
                    }
                }
                glEnd();
            }
            
            glDisable(GL_TEXTURE_2D);
        }
        else {
            // Default drawing for any other planets
            GLfloat mat_ambient[] = { body->color[0] * 0.2f, body->color[1] * 0.2f, body->color[2] * 0.2f, 1.0f };
            GLfloat mat_diffuse[] = { body->color[0], body->color[1], body->color[2], 1.0f };
            GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
            GLfloat mat_shininess[] = { 32.0f };

            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
            glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

            glColor4f(body->color[0], body->color[1], body->color[2], body->color[3]);
            Sphere(0, 0, 0, body->radius);
        }
        
        glPopMatrix();
    }
    else if(body->type == CELESTIAL_ASTEROID) {
        drawAsteroidBelt(body);
    }
}

void drawSolarSystem(void) {
    // Set up lighting before drawing
    setupSolarLighting();
    
    // Position the light at the sun's location
    GLfloat light_position[] = {
        solarBodies[0].x,
        solarBodies[0].y,
        solarBodies[0].z,
        1.0f  // Positional light
    };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    // Draw all bodies
    for(int i = 0; i < bodyCount; i++) {
        drawBody(&solarBodies[i]);
    }
}

void initExplosion(Explosion* explosion, float x, float y, float z) {
    explosion->x = x;
    explosion->y = y;
    explosion->z = z;
    explosion->active = 1;
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &explosion->particles[i];
        
        // Start at explosion center
        p->x = x;
        p->y = y;
        p->z = z;
        
        // Random velocity in all directions
        float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
        float elevation = ((float)rand() / RAND_MAX - 0.5f) * M_PI;
        
        p->vx = EXPLOSION_SPEED * cos(angle) * cos(elevation);
        p->vy = EXPLOSION_SPEED * sin(elevation);
        p->vz = EXPLOSION_SPEED * sin(angle) * cos(elevation);
        
        p->alpha = 1.0f;
        p->active = 1;
    }
}

void updateExplosion(Explosion* explosion) {
    if (!explosion->active) return;
    
    int activeCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &explosion->particles[i];
        if (!p->active) continue;
        
        // Update position
        p->x += p->vx;
        p->y += p->vy;
        p->z += p->vz;
        
        // Add slight gravity effect
        p->vy -= 0.01f;
        
        // Fade out
        p->alpha -= 0.02f;
        
        if (p->alpha <= 0) {
            p->active = 0;
        } else {
            activeCount++;
        }
    }
    
    // If no active particles, explosion is done
    if (activeCount == 0) {
        explosion->active = 0;
    }
}

void drawExplosion(Explosion* explosion) {
    if (!explosion->active) return;
    
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glPointSize(2.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &explosion->particles[i];
        if (!p->active) continue;
        
        // Orange-red color for explosion
        glColor4f(1.0f, 0.4f, 0.0f, p->alpha);
        glVertex3f(p->x, p->y, p->z);
    }
    glEnd();
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}