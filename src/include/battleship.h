#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include "celestial.h"
#include "utils.h"
#include "init.h"

#define MAX_BULLETS 50           // Increased from 20
#define BULLET_LIFETIME 500     // New constant for bullet lifetime
#define BULLET_VIEW_DISTANCE 1000.0f
#define BULLET_SPEED 5  // Speed of bullets
#define FIRE_RATE 10  // Cooldown between shots
#define ROTATION_SPEED 2.0  // Ship rotation speed
#define MOVE_SPEED 2.0  // Ship movement speed
#define MAX_PITCH 15.0  // Maximum pitch angle
#define MAX_ROLL 45.0  // Maximum roll angle
#define PITCH_RETURN_SPEED 2.0  // Speed at which pitch returns to neutral
#define ROLL_RETURN_SPEED 2.0  // Speed at which roll returns to neutral

#define SHIP_START_X 1000.0f  // Closer to Earth's orbit
#define SHIP_START_Z 1000.0f 

#define INITIAL_HEALTH 5
#define SUN_DAMAGE 10        // Instant death
#define COMET_DAMAGE 3       // Significant damage
#define STATION_DAMAGE 2     // Medium damage
#define ASTEROID_DAMAGE 1    // Small damage
#define PLANET_DAMAGE 5 
#define COLLISION_INVULNERABILITY_TIME 60  // Frames of invulnerability after hit
#define COLLISION_CHECK_RADIUS 5.0f  // Radius for checking collisions

#define EXPLOSION_PARTICLE_COUNT 1000   // More particles for bigger explosion
#define EXPLOSION_LIFETIME 1000          // 4 seconds at 60fps
#define INITIAL_EXPLOSION_SIZE 4.0f     // Much bigger initial size
#define EXPLOSION_SPREAD 20.0f          // Much wider spread
#define SHOCKWAVE_COUNT 3              // Multiple shockwave rings
#define MUSHROOM_CLOUD_PARTICLES 400

extern int gameOver;

#define M_PI 3.14159265358979323846

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float lifetime;
    float size;
    float r, g, b, a;  // Color with alpha
} ExplosionParticle;

typedef struct {
    int health;
    int isExploding;
    int invulnerabilityFrames;
    int explosionTimer;
    ExplosionParticle* explosionParticles;
    int explosionStage;     // Current stage of explosion
    float shockwaveSize;    // Size of expanding shockwave
    int explosionFlashTime;
} ShipHealth;

typedef struct {
    float x, y, z;         // Position
    float yaw;            // Left/right rotation
    float pitch;          // Up/down rotation
    float roll;           // Left/right tilt (banking)
    int wPressed;         // W key state
    int sPressed;         // S key state
    int aPressed;         // A key state
    int dPressed;         // D key state
    ShipHealth health;
} ShipState;

typedef struct {
    float x, y, z;     // Position
    float dirX, dirY, dirZ;  // Direction vector
    int active;        // Whether bullet is currently flying
} Bullet;

// Global variables
extern ShipState shipState;
extern Bullet bullets[MAX_BULLETS];

// Function declarations
void SetupCamera(void);
void drawBattleship(void);
void DrawBullet(float x, float y, float z);
void FireBullet(void);
void UpdateBullets(void);
void MoveShip(float dx, float dz);
void RotateShip(float yaw, float pitch);
void SetPitchPressed(int isW, int state);
void SetRollPressed(int isA, int state);
void UpdateShipState(void);
void UpdateRoll(void);
void checkBulletAsteroidCollisions(void);  // Add this declaration

void initializeShipHealth(void);
void updateShipHealth(void);
void damageShip(int damage);
void createShipExplosion(void);
void updateShipExplosion(void);
void drawShipExplosion(void);
void checkShipCollisions(void);
void cleanupShipHealth(void);

void resetShipPosition(void);
int isShipDestroyed(void);
void drawHealthBar(void);
void resetGame(void);

#endif