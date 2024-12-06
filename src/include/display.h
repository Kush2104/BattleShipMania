#ifndef DISPLAY_H
#define DISPLAY_H

#include "init.h"
#include "battleship.h"
#include "input.h"
#include "celestial.h"
#include "utils.h"

void display(void);
void reshape(int width, int height);
void update(int value);
void drawAxes();
void updateStarBrightness();
void displayStars();
void idle(void);

#endif
