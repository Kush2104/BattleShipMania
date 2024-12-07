#include "include/init.h"

Star stars[NUM_STARS];
static float lastRegenX = 0.0f;
static float lastRegenY = 0.0f;
static float lastRegenZ = 0.0f;

float randomFloat(float min, float max) {
    float scale = rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

void generateStarSpherical(int index, float centerX, float centerY, float centerZ) {

    float phi = randomFloat(0, 2 * M_PI);           
    float theta = randomFloat(0, M_PI);             
    float distance = randomFloat(STAR_VIEW_DISTANCE * 0.5f, STAR_VIEW_DISTANCE);

    float x = distance * sin(theta) * cos(phi);
    float y = distance * sin(theta) * sin(phi);
    float z = distance * cos(theta);

    stars[index].x = centerX + x;
    stars[index].y = centerY + y;
    stars[index].z = centerZ + z;

    stars[index].brightness = randomFloat(2.3f, 2.8f);
    stars[index].twinkleSpeed = randomFloat(0.001f, 0.002f);
}

void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        generateStarSpherical(i, 0, 0, 0);
    }
    lastRegenX = lastRegenY = lastRegenZ = 0.0f;
}

void regenerateStarField(float shipX, float shipY, float shipZ) {

    float dx = shipX - lastRegenX;
    float dy = shipY - lastRegenY;
    float dz = shipZ - lastRegenZ;
    float distMoved = sqrt(dx*dx + dy*dy + dz*dz);

    if (distMoved > STAR_SPACING) {

        for(int i = 0; i < NUM_STARS/4; i++) {
            int starIndex = rand() % NUM_STARS;
            generateStarSpherical(starIndex, shipX, shipY, shipZ);
        }

        lastRegenX = shipX;
        lastRegenY = shipY;
        lastRegenZ = shipZ;
    }
}