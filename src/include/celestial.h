// celestial.h
#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

// Real scales (in km) - we'll apply a scale factor to make it playable
#define REAL_SUN_RADIUS 696340.0f
#define REAL_EARTH_RADIUS 6371.0f
#define REAL_EARTH_ORBIT 149600000.0f  // Average distance from Sun

#define SPACE_SCALE_FACTOR 0.00001f    // Back to original
#define BODY_SCALE_FACTOR 0.0001f      // Back to original
#define ASTEROID_SCALE_FACTOR 0.05f    // New scale factor specifically for asteroids

// Celestial body types
#define CELESTIAL_SUN 0
#define CELESTIAL_PLANET 1
#define CELESTIAL_ASTEROID 2
#define CELESTIAL_COMET 3
#define CELESTIAL_NEBULA 4
#define CELESTIAL_BLACK_HOLE 5
#define CELESTIAL_SPACE_STATION 6

typedef struct {
    float x, y, z;           // Position
    float radius;           // Actual radius after scaling
    float originalRadius;   // Original radius before scaling
    float rotationSpeed;    // Degrees per frame
    float currentRotation;  // Current rotation angle
    float orbitRadius;      // Distance from sun
    float orbitSpeed;      // Speed of orbit
    float orbitAngle;      // Current position in orbit
    int type;              // Type of celestial body
    float color[4];        // RGBA color
    char* name;           // Body name
    // New fields for special effects
    float specialEffectTimer;  // For animation effects
    float specialEffectIntensity;  // For varying effect intensity
} CelestialBody;

#define MAX_BODIES 20  // Increased from 10 to accommodate new objects
extern CelestialBody solarBodies[MAX_BODIES];
extern int bodyCount;

// Function declarations
void initSolarSystem(void);
void updateSolarSystem(void);
void drawSolarSystem(void);
void drawBody(CelestialBody* body);
float getScaledDistance(float realDistance);
float getScaledRadius(float realRadius);

// New function declarations for special celestial bodies
void drawAsteroidBelt(CelestialBody* belt);
void drawComet(CelestialBody* comet);
void drawSpaceStation(CelestialBody* station);
void drawNebula(CelestialBody* nebula);
void drawBlackHole(CelestialBody* blackHole);

#endif