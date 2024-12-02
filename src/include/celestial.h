#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

#include "battleship.h"
#include <math.h>
#include "utils.h"

// Real astronomical scales in km
#define REAL_SUN_RADIUS 696340.0f
#define REAL_EARTH_RADIUS 6371.0f
#define REAL_EARTH_ORBIT 149600000.0f

// Scale factors to make the game playable
#define SPACE_SCALE_FACTOR 0.00001f    
#define BODY_SCALE_FACTOR 0.0001f      

// Types of celestial objects
#define CELESTIAL_SUN 0
#define CELESTIAL_PLANET 1
#define CELESTIAL_ASTEROID 2
#define CELESTIAL_COMET 3
#define CELESTIAL_NEBULA 4
#define CELESTIAL_BLACK_HOLE 5
#define CELESTIAL_SPACE_STATION 6

// Asteroid belt configuration
#define ASTEROID_SCALE_FACTOR 5.0f
#define MIN_ASTEROID_VERTICES 24
#define MAX_ASTEROID_VERTICES 32
#define NUM_ASTEROIDS 50
#define MIN_ASTEROID_RADIUS 8.0
#define MAX_ASTEROID_RADIUS 15.0
#define SURFACE_ROUGHNESS 0.15f
#define NOISE_FACTOR 0.4f 
#define ASTEROID_BELT_DISTANCE 2000.0f
#define BELT_WIDTH 500.0f
#define BELT_HEIGHT 50.0f
#define ASTEROID_Y_SCALE 2.0f

#define MIN_ORBIT_SPEED 0.0001f
#define MAX_ORBIT_SPEED 0.0002f

// Asteroid destruction system
#define MAX_FRAGMENTS 8
#define FRAGMENT_LIFETIME 200
#define MAX_HEALTH 3
#define EXPLOSION_RADIUS 15.0f

// Basic 3D vertex with texture coordinates
typedef struct {
    float x, y, z;
    float u, v;
} Vertex3D;

// Fragment system for asteroid destruction
typedef struct {
    Vertex3D* vertices;
    int numVertices;
    float x, y, z;
    float vx, vy, vz;
    float rotX, rotY, rotZ;
    float rotVelX, rotVelY, rotVelZ;
    float scale;
    float lifetime;
    int active;
} Fragment;

// Main asteroid structure
typedef struct {
    Vertex3D* vertices;
    int numVertices;
    float rotation;
    float rotationSpeed;
    float orbitAngle;
    float orbitRadius;
    float orbitSpeed;
    GLuint textureId;
    float x, y, z;
    int active;
    int health;
    Fragment fragments[MAX_FRAGMENTS];
    int fragmentsActive;
} Asteroid;

// Structure for all celestial bodies (planets, sun, etc)
typedef struct {
    float x, y, z;
    float radius;
    float originalRadius;
    float rotationSpeed;
    float currentRotation;
    float orbitRadius;
    float orbitSpeed;
    float orbitAngle;
    int type;
    float color[4];
    char* name;
    float specialEffectTimer;
    float specialEffectIntensity;
} CelestialBody;

#define MAX_BODIES 20

extern CelestialBody solarBodies[MAX_BODIES];
extern int bodyCount;
extern Asteroid* asteroids;
extern int asteroidBeltInitialized;

// Core system functions
void initSolarSystem(void);
void updateSolarSystem(void);
void drawSolarSystem(void);
void drawBody(CelestialBody* body);
float getScaledDistance(float realDistance);
float getScaledRadius(float realRadius);

// Asteroid related functions
void drawAsteroid(Asteroid* asteroid);
Vertex3D* generateAsteroidVertices(int* numVertices);
void initFragments(Asteroid* asteroid);
void updateFragments(Asteroid* asteroid);
void drawFragments(Asteroid* asteroid);
void drawExplosionEffect(float x, float y, float z, float radius, float alpha);

// Draw functions for different celestial objects
void drawAsteroidBelt(CelestialBody* belt);
void drawComet(CelestialBody* comet);
void drawSpaceStation(CelestialBody* station);
void drawNebula(CelestialBody* nebula);
void drawBlackHole(CelestialBody* blackHole);

void cleanupAsteroids(void);

#endif