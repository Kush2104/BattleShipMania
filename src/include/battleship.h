#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include "celestial.h"

// Gameplay constants
#define MAX_BULLETS 50           
#define BULLET_LIFETIME 500     
#define BULLET_VIEW_DISTANCE 1000.0f
#define BULLET_SPEED 5  
#define FIRE_RATE 10  
#define ROTATION_SPEED 2.0  
#define MOVE_SPEED 2.0  
#define MAX_PITCH 15.0  
#define MAX_ROLL 45.0  
#define PITCH_RETURN_SPEED 2.0  
#define ROLL_RETURN_SPEED 2.0  

// Starting position
#define SHIP_START_X 2000.0f  
#define SHIP_START_Z 2000.0f 

// Health and damage system
#define INITIAL_HEALTH 5
#define SUN_DAMAGE 10        
#define COMET_DAMAGE 3       
#define STATION_DAMAGE 2     
#define ASTEROID_DAMAGE 1    
#define PLANET_DAMAGE 5 
#define COLLISION_INVULNERABILITY_TIME 60  
#define COLLISION_CHECK_RADIUS 5.0f  

// Explosion system
#define EXPLOSION_PARTICLE_COUNT 1000   
#define EXPLOSION_LIFETIME 1000          
#define INITIAL_EXPLOSION_SIZE 4.0f     
#define EXPLOSION_SPREAD 20.0f          
#define SHOCKWAVE_COUNT 3              
#define MUSHROOM_CLOUD_PARTICLES 400

extern int gameOver;

#define M_PI 3.14159265358979323846

// Particle system for explosions
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float lifetime;
    float size;
    float r, g, b, a;
} ExplosionParticle;

// Ship health and explosion state
typedef struct {
    int health;
    int isExploding;
    int invulnerabilityFrames;
    int explosionTimer;
    ExplosionParticle* explosionParticles;
    int explosionStage;     
    float shockwaveSize;    
    int explosionFlashTime;
} ShipHealth;

// Ship state including position and controls
typedef struct {
    float x, y, z;         
    float yaw;            
    float pitch;          
    float roll;           
    int wPressed;         
    int sPressed;         
    int aPressed;         
    int dPressed;         
    ShipHealth health;
} ShipState;

// Projectile structure
typedef struct {
    float x, y, z;     
    float dirX, dirY, dirZ;  
    int active;        
} Bullet;

extern ShipState shipState;
extern Bullet bullets[MAX_BULLETS];

// Core gameplay functions
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
void checkBulletAsteroidCollisions(void);

// Health and explosion system
void initializeShipHealth(void);
void updateShipHealth(void);
void damageShip(int damage);
void createShipExplosion(void);
void updateShipExplosion(void);
void drawShipExplosion(void);
void checkShipCollisions(void);
void cleanupShipHealth(void);

// Game state management
void resetShipPosition(void);
int isShipDestroyed(void);
void drawHealthBar(void);
void resetGame(void);

#endif