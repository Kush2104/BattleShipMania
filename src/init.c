#include <stdlib.h>
#include <math.h>
#include "include/init.h"

Star stars[NUM_STARS];
static float lastRegenX = 0.0f;
static float lastRegenY = 0.0f;
static float lastRegenZ = 0.0f;

float randomFloat(float min, float max) {
    float scale = rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

void generateStarInBox(int index, float centerX, float centerY, float centerZ) {
    // Generate star within viewing box centered on the ship
    stars[index].x = randomFloat(centerX - STAR_VIEW_DISTANCE, centerX + STAR_VIEW_DISTANCE);
    stars[index].y = randomFloat(centerY - STAR_VIEW_DISTANCE, centerY + STAR_VIEW_DISTANCE);
    stars[index].z = randomFloat(centerZ - STAR_VIEW_DISTANCE, centerZ + STAR_VIEW_DISTANCE);
    stars[index].brightness = randomFloat(0.2f, 0.6f);  // More subtle brightness range
    stars[index].twinkleSpeed = randomFloat(0.001f, 0.003f);  // Slower twinkling
}

void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        generateStarInBox(i, 0, 0, 0);
    }
    lastRegenX = lastRegenY = lastRegenZ = 0.0f;
}

void regenerateStarField(float shipX, float shipY, float shipZ) {
    // Check if we've moved far enough to need new stars
    float dx = shipX - lastRegenX;
    float dy = shipY - lastRegenY;
    float dz = shipZ - lastRegenZ;
    float distMoved = sqrt(dx*dx + dy*dy + dz*dz);
    
    if (distMoved > STAR_SPACING) {
        // Calculate how many stars to regenerate based on movement
        int starsToRegen = (int)(NUM_STARS * (distMoved / STAR_VIEW_DISTANCE * 0.5f));
        starsToRegen = (starsToRegen < NUM_STARS/4) ? starsToRegen : NUM_STARS/4;
        
        for (int i = 0; i < starsToRegen; i++) {
            int starIndex = rand() % NUM_STARS;
            
            // Check if star is too far from ship
            float starDx = stars[starIndex].x - shipX;
            float starDy = stars[starIndex].y - shipY;
            float starDz = stars[starIndex].z - shipZ;
            float starDist = sqrt(starDx*starDx + starDy*starDy + starDz*starDz);
            
            if (starDist > STAR_VIEW_DISTANCE * 0.8f) {
                // Generate new star in front of ship's movement direction
                generateStarInBox(starIndex, 
                                shipX + (dx/distMoved) * STAR_VIEW_DISTANCE * 0.5f,
                                shipY + (dy/distMoved) * STAR_VIEW_DISTANCE * 0.5f,
                                shipZ + (dz/distMoved) * STAR_VIEW_DISTANCE * 0.5f);
            }
        }
        
        lastRegenX = shipX;
        lastRegenY = shipY;
        lastRegenZ = shipZ;
    }
}