#ifndef INIT_H
#define INIT_H

#include "utils.h"

typedef struct {
    float x, y, z;       // Position
    float brightness;    // Brightness for twinkling
    float twinkleSpeed;  // Unique speed for each star
} Star;

#define NUM_STARS 5000  // More stars for better effect
#define STAR_VIEW_DISTANCE 10000.0f  // Much further away
#define STAR_SPACING 1000.0f  // Space between star field regeneration

extern Star stars[NUM_STARS];

void initStars();
void regenerateStarField(float x, float y, float z);
float randomFloat(float min, float max);

#endif