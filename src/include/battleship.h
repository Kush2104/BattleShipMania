#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include "celestial.h"

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

#define SHIP_START_X 2000.0f  // Starting X position
#define SHIP_START_Z 2000.0f 

#define M_PI 3.14159265358979323846

typedef struct {
    float x, y, z;         // Position
    float yaw;            // Left/right rotation
    float pitch;          // Up/down rotation
    float roll;           // Left/right tilt (banking)
    int wPressed;         // W key state
    int sPressed;         // S key state
    int aPressed;         // A key state
    int dPressed;         // D key state
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

#endif