#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#define MAX_BULLETS 20  // Maximum number of bullets on screen
#define BULLET_SPEED 0.5  // Speed of bullets
#define FIRE_RATE 10  // Cooldown between shots
#define ROTATION_SPEED 2.0  // Ship rotation speed
#define MOVE_SPEED 0.1  // Ship movement speed
#define MAX_PITCH 30.0
#define PITCH_RETURN_SPEED 2.0

#define M_PI 3.14159265358979323846


typedef struct {
    float x, y, z;         // Position
    float yaw;            // Left/right rotation
    float pitch;          // Up/down rotation
    int wPressed;         // W key state
    int sPressed;         // S key state
} ShipState;

typedef struct {
    float x, y, z;     // Position
    int active;        // Whether bullet is currently flying
} Bullet;

typedef struct {
    float distance;        // Distance from ship
    float height;         // Height offset from ship
    float yaw;            // Camera rotation
    float pitch;          // Camera pitch
} CameraState;

extern ShipState shipState;
extern CameraState cameraState;

void SetupCamera(void);

void drawBattleship(void);
void FireBullet(void);
void UpdateBullets(void);
void MoveShip(float dx, float dz);
void RotateShip(float yaw, float pitch);
void SetPitchPressed(int isW, int state);

extern Bullet bullets[MAX_BULLETS];
extern int cooldown;    // Time between shots when holding F


#endif
