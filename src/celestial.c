// solar_system.c
#include "include/celestial.h"

CelestialBody solarBodies[MAX_BODIES];
int bodyCount = 0;

static GLuint asteroidTexture = 0;

float smoothNoise(float x, float y, float z) {
    return (float)rand() / RAND_MAX;
}

void initSolarSystem(void) {
    bodyCount = 0;  // Reset counter
    
    // Initialize Sun (kept the same)
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

    // Initialize Earth with adjusted position
    CelestialBody* earth = &solarBodies[bodyCount++];
    earth->originalRadius = REAL_EARTH_RADIUS;
    earth->radius = getScaledRadius(REAL_EARTH_RADIUS);
    earth->orbitRadius = getScaledDistance(REAL_EARTH_ORBIT);
    earth->orbitSpeed = 0.01f;
    earth->orbitAngle = 0;
    earth->rotationSpeed = 0.5f;
    earth->currentRotation = 0;
    earth->type = CELESTIAL_PLANET;
    earth->color[0] = 0.2f;    // Blue color for Earth
    earth->color[1] = 0.5f;
    earth->color[2] = 0.8f;
    earth->color[3] = 1.0f;
    earth->name = "Earth";
    earth->x = earth->orbitRadius;
    earth->y = 0;
    earth->z = 0;

    asteroidTexture = LoadBMP("src/assets/textures/rock.bmp");
    if (!asteroidTexture) {
        printf("Failed to load asteroid texture\n");
        exit(1);
    }

    // Initialize Asteroid Belt with adjusted position
    CelestialBody* asteroidBelt = &solarBodies[bodyCount++];
    asteroidBelt->originalRadius = REAL_EARTH_ORBIT * 1.2f;  // Closer to Earth
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
    // Set material properties
    float ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};  // Increased ambient for better texture visibility
    float diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};  // Full diffuse for texture color
    float specular[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Low specularity for rocky appearance
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0);

    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, asteroidTexture);

    int horizontalSegments = (MIN_ASTEROID_VERTICES / 2);
    int verticalSegments = 6;
    
    // Draw top cap
    glBegin(GL_TRIANGLE_FAN);
    // Center vertex
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.0f);
    glVertex3f(asteroid->vertices[0].x, 
              asteroid->vertices[0].y, 
              asteroid->vertices[0].z);
              
    // Edge vertices
    for (int j = 0; j <= horizontalSegments; j++) {
        int idx = 1 + (j % horizontalSegments);
        Vertex3D v = asteroid->vertices[idx];
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
    static Asteroid* asteroids = NULL;
    static float* fixedY = NULL;
    
    // Initialize asteroids if not already done
    if (asteroids == NULL) {
        srand(1234);  // Fixed seed for consistent generation
        asteroids = (Asteroid*)malloc(sizeof(Asteroid) * NUM_ASTEROIDS);
        fixedY = (float*)malloc(sizeof(float) * NUM_ASTEROIDS);
        
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            asteroids[i].numVertices = 0;
            asteroids[i].vertices = generateAsteroidVertices(&asteroids[i].numVertices);
            asteroids[i].rotation = ((float)rand() / RAND_MAX) * 360.0f;
            asteroids[i].rotationSpeed = 0.0f;  // No rotation
            asteroids[i].orbitAngle = ((float)rand() / RAND_MAX) * 2 * M_PI;
            asteroids[i].orbitRadius = ASTEROID_BELT_DISTANCE + ((float)rand() / RAND_MAX - 0.5) * BELT_WIDTH;
            asteroids[i].orbitSpeed = MIN_ORBIT_SPEED;
            fixedY[i] = ((float)rand() / RAND_MAX - 0.5) * BELT_HEIGHT;
        }
    }
    
    // Set up lighting for the asteroids
    GLfloat light_position[] = {0.0, 1000.0, 0.0, 1.0};
    GLfloat light_ambient[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular[] = {0.5, 0.5, 0.5, 1.0};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    // Draw each asteroid
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        asteroids[i].orbitAngle += asteroids[i].orbitSpeed;
        
        float x = cos(asteroids[i].orbitAngle) * asteroids[i].orbitRadius;
        float z = sin(asteroids[i].orbitAngle) * asteroids[i].orbitRadius;
        
        glPushMatrix();
        glTranslatef(x, fixedY[i], z);
        glRotatef(asteroids[i].rotation, 0, 1, 0);
        drawAsteroid(&asteroids[i]);
        glPopMatrix();
    }
}

void setupSolarLighting(void) {
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);  // Important for correct normal scaling
    
    // Set up material properties
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // Setup sun's light properties
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.1f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 0.95f, 0.8f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 0.9f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    // Enable light attenuation
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0000001);
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
        // Existing sun drawing code...
        glPushMatrix();
        glTranslatef(body->x, body->y, body->z);
        glRotatef(body->currentRotation, 0, 1, 0);
        
        // Disable lighting for the sun
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
    }
    else if(body->type == CELESTIAL_PLANET) {
        // Existing planet drawing code...
        glPushMatrix();
        glTranslatef(body->x, body->y, body->z);
        glRotatef(body->currentRotation, 0, 1, 0);
        
        // Enable lighting for planets
        glEnable(GL_LIGHTING);
        
        // Set material properties
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = {
            body->color[0],
            body->color[1],
            body->color[2],
            body->color[3]
        };
        GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GLfloat mat_shininess[] = { 25.0f };
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        
        Sphere(0, 0, 0, body->radius);
        
        glPopMatrix();
    }
    else if(body->type == CELESTIAL_ASTEROID) {
        drawAsteroidBelt(body);  // Call our new asteroid belt function
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