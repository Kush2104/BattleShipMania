// solar_system.h
#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

// Real scales (in km) - we'll apply a scale factor to make it playable
#define REAL_SUN_RADIUS 696340.0f
#define REAL_EARTH_RADIUS 6371.0f
#define REAL_EARTH_ORBIT 149600000.0f  // Average distance from Sun

// Scale factors to make the game playable while maintaining relative sizes
#define SPACE_SCALE_FACTOR 0.00001f    // For distances between bodies
#define BODY_SCALE_FACTOR 0.0001f      // For celestial body sizes
#define SHIP_SCALE_FACTOR 0.5f         // Make ship smaller relative to planets

typedef struct {
    float x, y, z;           // Position
    float radius;           // Actual radius after scaling
    float originalRadius;   // Original radius before scaling
    float rotationSpeed;    // Degrees per frame
    float currentRotation;  // Current rotation angle
    float orbitRadius;      // Distance from sun
    float orbitSpeed;      // Speed of orbit
    float orbitAngle;      // Current position in orbit
    int type;              // 0 = sun, 1 = planet
    float color[4];        // RGBA color
    char* name;           // Body name
} CelestialBody;

#define MAX_BODIES 10
extern CelestialBody solarBodies[MAX_BODIES];
extern int bodyCount;

// Function declarations
void initSolarSystem(void);
void updateSolarSystem(void);
void drawSolarSystem(void);
void drawBody(CelestialBody* body);
float getScaledDistance(float realDistance);
float getScaledRadius(float realRadius);

#endif