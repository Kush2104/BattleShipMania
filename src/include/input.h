#ifndef INPUT_H
#define INPUT_H

// Key states for continuous movement
typedef struct {
    int upPressed;
    int downPressed;
    int leftPressed;
    int rightPressed;
} KeyStates;

extern KeyStates keyStates;
extern float cameraAngle;  // Defined in display.c

// Function declarations
void key(unsigned char ch, int x, int y);
void keyUp(unsigned char ch, int x, int y);
void specialKey(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void updateMovement(void);
void initializeInput(void);

#endif 
