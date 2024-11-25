// celestial.h
#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

#include "battleship.h"
#include <math.h>
#include "utils.h"

// Real scales (in km) - we'll apply a scale factor to make it playable
#define REAL_SUN_RADIUS 696340.0f
#define REAL_EARTH_RADIUS 6371.0f
#define REAL_EARTH_ORBIT 149600000.0f  // Average distance from Sun

#define SPACE_SCALE_FACTOR 0.00001f    // Back to original
#define BODY_SCALE_FACTOR 0.0001f      // Back to original

// Celestial body types
#define CELESTIAL_SUN 0
#define CELESTIAL_PLANET 1
#define CELESTIAL_ASTEROID 2
#define CELESTIAL_COMET 3
#define CELESTIAL_NEBULA 4
#define CELESTIAL_BLACK_HOLE 5
#define CELESTIAL_SPACE_STATION 6

#define ASTEROID_SCALE_FACTOR 5.0f
#define MIN_ASTEROID_VERTICES 24  // More vertices for smoother shape
#define MAX_ASTEROID_VERTICES 32
#define NUM_ASTEROIDS 50
#define MIN_ASTEROID_RADIUS 8.0
#define MAX_ASTEROID_RADIUS 15.0
#define SURFACE_ROUGHNESS 0.15f
#define NOISE_FACTOR 0.4f 
#define ASTEROID_BELT_DISTANCE 2000.0f
#define BELT_WIDTH 500.0f
#define BELT_HEIGHT 50.0f  // Reduced height for less vertical spread
#define ASTEROID_Y_SCALE 2.0f  // New constant for vertical scaling

#define MIN_ORBIT_SPEED 0.0001f  // Much slower minimum orbit speed
#define MAX_ORBIT_SPEED 0.0002f  // Much slower maximum orbit speed

#define MAX_PARTICLES 100
#define PARTICLE_LIFETIME 50.0f
#define EXPLOSION_SPEED 2.0f

typedef struct {
    float x, y, z;        // Position
    float vx, vy, vz;     // Velocity
    float alpha;          // Transparency
    int active;           // Whether particle is active
} Particle;

typedef struct {
    float x, y, z;         // Position
    Particle particles[MAX_PARTICLES];
    int active;           // Whether explosion is currently happening
} Explosion;

// New structure for asteroid vertices
typedef struct {
    float x, y, z;
    float u, v;    // Texture coordinates
} Vertex3D;

typedef struct {
    Vertex3D* vertices;
    int numVertices;
    float rotation;
    float rotationSpeed;
    float orbitAngle;
    float orbitRadius;
    float orbitSpeed;
    GLuint textureId;
    float x, y, z;        // Current position
    int active;          // Whether asteroid is still active
    Explosion explosion;  // Explosion effect when destroyed
} Asteroid;

extern Asteroid* asteroids;

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

void drawAsteroid(Asteroid* asteroid);
Vertex3D* generateAsteroidVertices(int* numVertices);

extern Asteroid* asteroids;  // Global declaration
extern int asteroidBeltInitialized;

// New function declarations for special celestial bodies
void drawAsteroidBelt(CelestialBody* belt);
void drawComet(CelestialBody* comet);
void drawSpaceStation(CelestialBody* station);
void drawNebula(CelestialBody* nebula);
void drawBlackHole(CelestialBody* blackHole);

//Explosions
void initExplosion(Explosion* explosion, float x, float y, float z);
void updateExplosion(Explosion* explosion);
void drawExplosion(Explosion* explosion);
void cleanupAsteroids(void);



#endif