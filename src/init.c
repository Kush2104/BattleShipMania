#include <stdlib.h>
#include <math.h>
#include "include/init.h"

#define NUM_STARS 1000

Star stars[NUM_STARS];  // Array to hold star positions and properties

void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        float theta = 2.0f * M_PI * (rand() % 1000) / 1000.0f;  // Random angle
        float phi = acos(2.0f * (rand() % 1000) / 1000.0f - 1.0f);  // Random inclination angle
        float radius = 10.0f;  // Distance from the center

        stars[i].x = radius * sinf(phi) * cosf(theta);
        stars[i].y = radius * sinf(phi) * sinf(theta);
        stars[i].z = radius * cosf(phi);
        stars[i].brightness = 0.5f + (rand() % 50) / 100.0f;  // Initial random brightness
        stars[i].twinkleSpeed = 0.005f + (rand() % 10) / 2000.0f;  // Random twinkle speed
    }
}
