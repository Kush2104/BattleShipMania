#ifndef INIT_H
#define INIT_H

#include "utils.h"

typedef struct {
    float x, y, z;       
    float brightness;    
    float twinkleSpeed;  
} Star;

#define NUM_STARS 5000  
#define STAR_VIEW_DISTANCE 10000.0f  
#define STAR_SPACING 1000.0f  

extern Star stars[NUM_STARS];

void initStars();
void regenerateStarField(float x, float y, float z);
float randomFloat(float min, float max);

#endif