#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

#include "utils.h"
#include "battleship.h"
#include "shader_util.h"

#define REAL_SUN_RADIUS 696340.0f
#define REAL_EARTH_RADIUS 6371.0f
#define REAL_EARTH_ORBIT 149600000.0f
#define REAL_JUPITER_RADIUS 69911.0f
#define REAL_SATURN_RADIUS 58232.0f
#define REAL_URANUS_RADIUS 25362.0f
#define REAL_NEPTUNE_RADIUS 24622.0f

#define SATURN_RING_INNER 1.2f   
#define SATURN_RING_OUTER 2.5f   
#define URANUS_RING_INNER 1.4f
#define URANUS_RING_OUTER 2.0f
#define NEPTUNE_RING_INNER 1.1f  
#define NEPTUNE_RING_OUTER 1.5f
#define RING_SEGMENTS 180

#define MAX_FLARE_ALPHA 0.7f
#define NUM_FLARE_ELEMENTS 4

#define SPACE_SCALE_FACTOR 0.00002f    
#define BODY_SCALE_FACTOR 0.0001f      

#define CELESTIAL_SUN 0
#define CELESTIAL_PLANET 1
#define CELESTIAL_ASTEROID 2
#define CELESTIAL_COMET 3
#define CELESTIAL_SPACE_STATION 4

#define ASTEROID_SCALE_FACTOR 5.0f
#define MIN_ASTEROID_VERTICES 24
#define MAX_ASTEROID_VERTICES 32
#define NUM_ASTEROIDS 100
#define MIN_ASTEROID_RADIUS 8.0
#define MAX_ASTEROID_RADIUS 15.0
#define SURFACE_ROUGHNESS 0.4f
#define NOISE_FACTOR 0.8f 
#define ASTEROID_BELT_DISTANCE 2000.0f
#define BELT_WIDTH 500.0f
#define BELT_HEIGHT 50.0f
#define ASTEROID_Y_SCALE 3.0f
#define ELONGATION_FACTOR 2.8f

#define STATION_ROTATION_SPEED 0.1f
#define STATION_RING_SPEED 0.5f
#define STATION_ORBIT_DISTANCE 50.0f  
#define STATION_BASE_SIZE 1.1f

#define MIN_ORBIT_SPEED 0.0001f
#define MAX_ORBIT_SPEED 0.0002f

#define MAX_FRAGMENTS 8
#define FRAGMENT_LIFETIME 200
#define MAX_HEALTH 3
#define EXPLOSION_RADIUS 15.0f

#define CELESTIAL_UFO 5
#define NUM_UFOS 20
#define UFO_DAMAGE 1
#define UFO_MIN_RADIUS 10.0
#define UFO_MAX_RADIUS 20.0
#define UFO_MIN_HEIGHT 5.0
#define UFO_MAX_HEIGHT 10.0
#define UFO_ORBIT_DISTANCE 2000.0f
#define UFO_SPEED_MIN 0.5f
#define UFO_SPEED_MAX 2.0f

#define MAX_COMET_PARTICLES 100000

typedef struct {
    float position[4];  // xyz + lifetime
    float velocity[4];  // xyz + size
    float color[4];     // rgba
} CometParticle;

typedef struct {
    GLuint computeProgram;
    GLuint particleBuffer;
    GLuint particleVAO;
    GLuint particleVBO;
    CometParticle* particles;
    float deltaTime;
} CometTailData;

typedef struct {
    float x, y, z;
    float u, v;
} Vertex3D;

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
    CometTailData* tailData;
} CelestialBody;

typedef struct {
    float x, y, z;
    float radius;
    float height;
    float rotation;
    float rotationSpeed;
    float orbitAngle;
    float orbitRadius;
    float orbitSpeed;
    int active;
    int health;
    Fragment fragments[MAX_FRAGMENTS];
    int fragmentsActive;
    GLuint textureId;
} UFO;

#define MAX_BODIES 20




extern CelestialBody solarBodies[MAX_BODIES];
extern int bodyCount;
extern Asteroid* asteroids;
extern int asteroidBeltInitialized;
extern UFO* ufos;
extern int ufoInitialized;

void initSolarSystem(void);
void updateSolarSystem(void);
void drawSolarSystem(void);
void drawBody(CelestialBody* body);
void drawPlanetRings(float innerRadius, float outerRadius, float r, float g, float b, float alpha);
float getScaledDistance(float realDistance);
float getScaledRadius(float realRadius);

void drawAsteroid(Asteroid* asteroid);
Vertex3D* generateAsteroidVertices(int* numVertices);
void initFragments(Asteroid* asteroid);
void updateFragments(Asteroid* asteroid);
void drawFragments(Asteroid* asteroid);
void drawExplosionEffect(float x, float y, float z, float radius, float alpha);

void drawAsteroidBelt(CelestialBody* belt);
void drawComet(CelestialBody* comet);
void drawSpaceStation(CelestialBody* station);

void initUFOs(void);
void drawUFO(UFO* ufo);
void updateUFOs(void);
void checkBulletUFOCollisions(void);
void cleanupUFOs(void);
void renderUFOs(void);


void cleanupAsteroids(void);

void initCometTail(CelestialBody* comet);
void updateCometTail(CelestialBody* comet);
void drawCometTail(CelestialBody* comet);
void cleanupCometTail(CelestialBody* comet);

#endif