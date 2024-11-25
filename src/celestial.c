// solar_system.c
#include "include/celestial.h"
#include "include/battleship.h"
#include "include/utils.h"
#include <math.h>

CelestialBody solarBodies[MAX_BODIES];
int bodyCount = 0;

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

void drawAsteroidBelt(CelestialBody* belt) {
    glPushMatrix();
    glTranslatef(belt->x, belt->y, belt->z);
    glRotatef(belt->currentRotation, 0, 1, 0);
    
    // Enable lighting
    glEnable(GL_LIGHTING);
    
    // Material properties for asteroids
    GLfloat mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 25.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    const int NUM_ASTEROIDS = 100;
    const float BELT_RADIUS = belt->orbitRadius;
    const float BELT_WIDTH = BELT_RADIUS * 0.2f;
    const float BELT_HEIGHT = BELT_RADIUS * 0.05f;
    
    // First, draw the orbit line (more visible)
    glDisable(GL_LIGHTING);
    glColor4f(0.6f, 0.6f, 0.6f, 1.0f);  // Solid color, no transparency
    
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 360; i += 5) {
        float angle = i * M_PI / 180.0f;
        glVertex3f(cos(angle) * BELT_RADIUS, 0, sin(angle) * BELT_RADIUS);
    }
    glEnd();
    
    // Now draw the asteroids
    glEnable(GL_LIGHTING);
    for(int i = 0; i < NUM_ASTEROIDS; i++) {
        float angle = (float)i * (360.0f / NUM_ASTEROIDS);
        float radius = BELT_RADIUS + sin(i * 3.14159f/7) * BELT_WIDTH;
        float height = cos(i * 3.14159f/5) * BELT_HEIGHT;
        
        glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        glTranslatef(radius, height, 0);
        
        // Rotate each asteroid
        float rotAngle = belt->currentRotation * (1.0f + (i % 5) * 0.2f);
        glRotatef(rotAngle, 1, 1, 1);
        
        // Make asteroids much larger
        float size = 50.0f + (float)(i % 5) * 10.0f;  // Much larger size
        glScalef(size, size, size);
        
        // Draw the asteroid
        glColor3f(0.7f + (i % 3) * 0.1f, 
                 0.7f + (i % 3) * 0.1f, 
                 0.7f + (i % 3) * 0.1f);
        Sphere(0, 0, 0, 1.0);
        
        glPopMatrix();
    }
    
    glPopMatrix();
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