#ifndef INPUT_H
#define INPUT_H

#include "utils.h"
#include "battleship.h"

typedef struct {
    int upPressed;      // Up arrow key state
    int downPressed;    // Down arrow key state
    int leftPressed;    // Left arrow key state
    int rightPressed;   // Right arrow key state
} KeyStates;

extern KeyStates keyStates;

// Function declarations
void key(unsigned char ch, int x, int y);           // Regular key press
void keyUp(unsigned char ch, int x, int y);         // Regular key release
void specialKey(int key, int x, int y);            // Special key press (arrows)
void specialKeyUp(int key, int x, int y);          // Special key release (arrows)
void updateMovement(void);                         // Update ship position based on keys
void initializeInput(void);                        // Initialize input callbacks

#endif