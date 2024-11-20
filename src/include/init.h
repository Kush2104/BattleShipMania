#ifndef INIT_H
#define INIT_H

typedef struct {
    float x, y, z;       // Position
    float brightness;    // Brightness for twinkling
    float twinkleSpeed;  // Unique speed for each star
} Star;

#define NUM_STARS 3000  // Increased for better density
#define STAR_VIEW_DISTANCE 100.0f  // How far stars are visible
#define STAR_SPACING 40.0f  // Space between star field regeneration

extern Star stars[NUM_STARS];

void initStars();
void regenerateStarField(float x, float y, float z);
float randomFloat(float min, float max);

#endif