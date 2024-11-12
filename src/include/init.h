#ifndef INIT_H
#define INIT_H

typedef struct {
    float x, y, z;       // Position
    float brightness;    // Brightness for twinkling
    float twinkleSpeed;  // Unique speed for each star
} Star;

#define NUM_STARS 1000
extern Star stars[NUM_STARS];

void initStars();

#endif
