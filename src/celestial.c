#include "include/celestial.h"

CelestialBody solarBodies[MAX_BODIES];
int bodyCount = 0;
Asteroid* asteroids = NULL;
int asteroidBeltInitialized = 0;

UFO* ufos = NULL;
int ufoInitialized = 0;


static GLuint asteroidTexture = 0;
static GLuint earthTexture = 0;
static GLuint marsTexture = 0;
static GLuint mercuryTexture = 0;
static GLuint venusTexture = 0;
static GLuint jupiterTexture = 0;
static GLuint saturnTexture = 0;
static GLuint uranusTexture = 0;
static GLuint neptuneTexture = 0;
static GLuint ufoBaseTexture = 0;
static GLuint ufoDomeTexture = 0;

float smoothNoise(float x, float y, float z) {
    return (float)rand() / RAND_MAX;
}

void initSolarSystem(void) {
    bodyCount = 0;

    CelestialBody* sun = &solarBodies[bodyCount++];
    sun->originalRadius = REAL_SUN_RADIUS;
    sun->radius = getScaledRadius(REAL_SUN_RADIUS);
    sun->x = sun->y = sun->z = 0;
    sun->rotationSpeed = 0.1f;
    sun->currentRotation = 0;
    sun->type = CELESTIAL_SUN;
    sun->color[0] = 1.0f;
    sun->color[1] = 0.85f;
    sun->color[2] = 0.4f;
    sun->color[3] = 1.0f;
    sun->name = "Sol";

    mercuryTexture = LoadBMP("src/assets/textures/mercury.bmp");
    venusTexture = LoadBMP("src/assets/textures/venus.bmp");
    earthTexture = LoadBMP("src/assets/textures/earth.bmp");
    marsTexture = LoadBMP("src/assets/textures/mars.bmp");
    jupiterTexture = LoadBMP("src/assets/textures/jupiter.bmp");
    saturnTexture = LoadBMP("src/assets/textures/saturn.bmp");
    uranusTexture = LoadBMP("src/assets/textures/uranus.bmp");
    neptuneTexture = LoadBMP("src/assets/textures/neptune.bmp");
    asteroidTexture = LoadBMP("src/assets/textures/rock.bmp");
    ufoBaseTexture = LoadBMP("src/assets/textures/ufobase.bmp");
    ufoDomeTexture = LoadBMP("src/assets/textures/glass.bmp");

    CelestialBody* mercury = &solarBodies[bodyCount++];
    mercury->originalRadius = REAL_EARTH_RADIUS * 0.383f;
    mercury->radius = 20.0f;
    mercury->orbitRadius = 500.0f;
    mercury->orbitSpeed = 0.02f;
    mercury->orbitAngle = 0;
    mercury->rotationSpeed = 0.3f;
    mercury->currentRotation = 0;
    mercury->type = CELESTIAL_PLANET;
    mercury->color[0] = mercury->color[1] = mercury->color[2] = mercury->color[3] = 1.0f;
    mercury->name = "Mercury";

    CelestialBody* venus = &solarBodies[bodyCount++];
    venus->originalRadius = REAL_EARTH_RADIUS * 0.949f;
    venus->radius = 25.0f;
    venus->orbitRadius = 1000.0f;
    venus->orbitSpeed = 0.015f;
    venus->orbitAngle = 45;
    venus->rotationSpeed = 0.27f;
    venus->currentRotation = 0;
    venus->type = CELESTIAL_PLANET;
    venus->color[0] = venus->color[1] = venus->color[2] = venus->color[3] = 1.0f;
    venus->name = "Venus";

    CelestialBody* earth = &solarBodies[bodyCount++];
    earth->originalRadius = REAL_EARTH_RADIUS;
    earth->radius = 30.0f;
    earth->orbitRadius = 1500.0f;
    earth->orbitSpeed = 0.01f;
    earth->orbitAngle = 90;
    earth->rotationSpeed = 0.5f;
    earth->currentRotation = 0;
    earth->type = CELESTIAL_PLANET;
    earth->color[0] = earth->color[1] = earth->color[2] = earth->color[3] = 1.0f;
    earth->name = "Earth";

    CelestialBody* mars = &solarBodies[bodyCount++];
    mars->originalRadius = REAL_EARTH_RADIUS * 0.532f;
    mars->radius = 22.0f;
    mars->orbitRadius = 1800.0f;
    mars->orbitSpeed = 0.008f;
    mars->orbitAngle = 135;
    mars->rotationSpeed = 0.48f;
    mars->currentRotation = 0;
    mars->type = CELESTIAL_PLANET;
    mars->color[0] = mars->color[1] = mars->color[2] = mars->color[3] = 1.0f;
    mars->name = "Mars";

    CelestialBody* jupiter = &solarBodies[bodyCount++];
    jupiter->originalRadius = REAL_JUPITER_RADIUS;
    jupiter->radius = 80.0f;
    jupiter->orbitRadius = 2500.0f;
    jupiter->orbitSpeed = 0.005f;
    jupiter->orbitAngle = 180;
    jupiter->rotationSpeed = 0.8f;
    jupiter->currentRotation = 0;
    jupiter->type = CELESTIAL_PLANET;
    jupiter->color[0] = jupiter->color[1] = jupiter->color[2] = jupiter->color[3] = 1.0f;
    jupiter->name = "Jupiter";

    CelestialBody* saturn = &solarBodies[bodyCount++];
    saturn->originalRadius = REAL_SATURN_RADIUS;
    saturn->radius = 70.0f;
    saturn->orbitRadius = 3000.0f;
    saturn->orbitSpeed = 0.003f;
    saturn->orbitAngle = 270;
    saturn->rotationSpeed = 0.7f;
    saturn->currentRotation = 0;
    saturn->type = CELESTIAL_PLANET;
    saturn->color[0] = saturn->color[1] = saturn->color[2] = saturn->color[3] = 1.0f;
    saturn->name = "Saturn";

    CelestialBody* uranus = &solarBodies[bodyCount++];
    uranus->originalRadius = REAL_URANUS_RADIUS;
    uranus->radius = 45.0f;
    uranus->orbitRadius = 3500.0f;
    uranus->orbitSpeed = 0.002f;
    uranus->orbitAngle = 45;
    uranus->rotationSpeed = 0.6f;
    uranus->currentRotation = 0;
    uranus->type = CELESTIAL_PLANET;
    uranus->color[0] = uranus->color[1] = uranus->color[2] = uranus->color[3] = 1.0f;
    uranus->name = "Uranus";

    CelestialBody* neptune = &solarBodies[bodyCount++];
    neptune->originalRadius = REAL_NEPTUNE_RADIUS;
    neptune->radius = 43.0f;
    neptune->orbitRadius = 4000.0f;
    neptune->orbitSpeed = 0.001f;
    neptune->orbitAngle = 135;
    neptune->rotationSpeed = 0.5f;
    neptune->currentRotation = 0;
    neptune->type = CELESTIAL_PLANET;
    neptune->color[0] = neptune->color[1] = neptune->color[2] = neptune->color[3] = 1.0f;
    neptune->name = "Neptune";

    CelestialBody* asteroidBelt = &solarBodies[bodyCount++];
    asteroidBelt->originalRadius = REAL_EARTH_ORBIT * 1.2f;
    asteroidBelt->radius = getScaledDistance(REAL_EARTH_ORBIT * 1.2f);
    asteroidBelt->orbitRadius = getScaledDistance(REAL_EARTH_ORBIT * 1.2f);
    asteroidBelt->x = asteroidBelt->y = asteroidBelt->z = 0;
    asteroidBelt->rotationSpeed = 0.05f;
    asteroidBelt->currentRotation = 0;
    asteroidBelt->type = CELESTIAL_ASTEROID;
    asteroidBelt->color[0] = asteroidBelt->color[1] = asteroidBelt->color[2] = 0.7f;
    asteroidBelt->color[3] = 1.0f;
    asteroidBelt->name = "Asteroid Belt";

    CelestialBody* comet = &solarBodies[bodyCount++];
    comet->originalRadius = REAL_EARTH_RADIUS * 0.2f;
    comet->radius = 25.0f;
    comet->orbitRadius = 3000.0f;
    comet->orbitSpeed = 0.05f;
    comet->orbitAngle = 0;
    comet->rotationSpeed = 0.5f;
    comet->currentRotation = 0;
    comet->type = CELESTIAL_COMET;
    comet->color[0] = comet->color[1] = comet->color[2] = 1.0f;
    comet->color[3] = 1.0f;
    comet->name = "Halley's Comet";
    comet->specialEffectTimer = 0;

    CelestialBody* spaceStation = &solarBodies[bodyCount++];
    spaceStation->originalRadius = REAL_EARTH_RADIUS * 0.002f;
    spaceStation->radius = 5.0f;
    spaceStation->rotationSpeed = 0.5f;
    spaceStation->currentRotation = 0;
    spaceStation->type = CELESTIAL_SPACE_STATION;
    spaceStation->color[0] = spaceStation->color[1] = spaceStation->color[2] = 0.8f;
    spaceStation->color[3] = 1.0f;
    spaceStation->name = "ISS"; 
}

void setupSolarLighting(void) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light_diffuse[] = { 1.2f, 1.2f, 1.2f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0000005);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void drawPlanetRings(float innerRadius, float outerRadius, float r, float g, float b, float alpha) {
    glPushMatrix();

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float ringWidth = outerRadius - innerRadius;
    int numRings = 8;  

    for(int i = 0; i < numRings; i++) {
        float localInner = innerRadius + (ringWidth * i / numRings);
        float localOuter = innerRadius + (ringWidth * (i + 1) / numRings);
        float localAlpha = alpha * (1.0f - (float)i/numRings * 0.3f);  

        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j <= RING_SEGMENTS; j++) {
            float angle = (float)j/RING_SEGMENTS * 2.0f * M_PI;
            float cos_angle = cos(angle);
            float sin_angle = sin(angle);

            glColor4f(r, g, b, localAlpha);
            glVertex3f(cos_angle * localInner, 0, sin_angle * localInner);
            glVertex3f(cos_angle * localOuter, 0, sin_angle * localOuter);
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawBody(CelestialBody* body) {
    if(body->type == CELESTIAL_SUN) {
        glPushMatrix();
        glTranslatef(body->x, body->y, body->z);
        glRotatef(body->currentRotation, 0, 1, 0);

        glDisable(GL_LIGHTING);

        glColor4f(body->color[0], body->color[1], body->color[2], body->color[3]);
        Sphere(0, 0, 0, body->radius);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        const int CORONA_LAYERS = 8;
        for(int i = 0; i < CORONA_LAYERS; i++) {
            float scale = 1.0f + (i * 0.15f);
            float alpha = 0.3f * (1.0f - ((float)i / CORONA_LAYERS));

            glColor4f(
                body->color[0],
                body->color[1] * (1.0f - (i * 0.1f)),
                body->color[2] * (1.0f - (i * 0.15f)),
                alpha
            );
            Sphere(0, 0, 0, body->radius * scale);
        }

        glDisable(GL_BLEND);
        glPopMatrix();

        GLfloat light_position[] = { body->x, body->y, body->z, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        glEnable(GL_LIGHTING);
    }
    else if(body->type == CELESTIAL_PLANET) {
        glPushMatrix();

        float orbitAngleRad = body->orbitAngle * M_PI / 180.0f;
        body->x = cos(orbitAngleRad) * body->orbitRadius;
        body->z = sin(orbitAngleRad) * body->orbitRadius;

        glTranslatef(body->x, body->y, body->z);
        glRotatef(body->currentRotation, 0, 1, 0);

        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GLfloat mat_shininess[] = { 32.0f };

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glEnable(GL_TEXTURE_2D);

        if (strcmp(body->name, "Earth") == 0) {
            glBindTexture(GL_TEXTURE_2D, earthTexture);
        } else if (strcmp(body->name, "Mars") == 0) {
            glBindTexture(GL_TEXTURE_2D, marsTexture);
        } else if (strcmp(body->name, "Mercury") == 0) {
            glBindTexture(GL_TEXTURE_2D, mercuryTexture);
        } else if (strcmp(body->name, "Venus") == 0) {
            glBindTexture(GL_TEXTURE_2D, venusTexture);
        } else if (strcmp(body->name, "Jupiter") == 0) {
            glBindTexture(GL_TEXTURE_2D, jupiterTexture);
        } else if (strcmp(body->name, "Saturn") == 0) {
            glBindTexture(GL_TEXTURE_2D, saturnTexture);
        } else if (strcmp(body->name, "Uranus") == 0) {
            glBindTexture(GL_TEXTURE_2D, uranusTexture);
        } else if (strcmp(body->name, "Neptune") == 0) {
            glBindTexture(GL_TEXTURE_2D, neptuneTexture);
        }

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        const int stacks = 32;
        const int slices = 32;
        const float r = body->radius;

        for (int i = 0; i < stacks; i++) {
            float lat0 = M_PI * (-0.5 + (float)i / stacks);
            float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);

            glBegin(GL_QUAD_STRIP);
            for (int j = 0; j <= slices; j++) {
                float lng = 2 * M_PI * (float)j / slices;

                for (int k = 0; k <= 1; k++) {
                    float lat = k ? lat1 : lat0;
                    float y = sin(lat);
                    float x = cos(lng) * cos(lat);
                    float z = sin(lng) * cos(lat);

                    glTexCoord2f((float)j / slices, (float)(i + k) / stacks);
                    glNormal3f(x, y, z);
                    glVertex3f(r * x, r * y, r * z);
                }
            }
            glEnd();
        }

        glDisable(GL_TEXTURE_2D);

        if (strcmp(body->name, "Saturn") == 0) {
            glRotatef(26.73f, 1, 0, 0);
            drawPlanetRings(body->radius * SATURN_RING_INNER, body->radius * SATURN_RING_OUTER, 
                           0.9f, 0.8f, 0.7f, 0.7f);
        }
        else if (strcmp(body->name, "Uranus") == 0) {
            glRotatef(97.77f, 1, 0, 0);
            drawPlanetRings(body->radius * URANUS_RING_INNER, body->radius * URANUS_RING_OUTER,
                           0.6f, 0.6f, 0.6f, 0.4f);
        }
        else if (strcmp(body->name, "Neptune") == 0) {
            drawPlanetRings(body->radius * NEPTUNE_RING_INNER, body->radius * NEPTUNE_RING_OUTER,
                           0.3f, 0.3f, 0.3f, 0.2f);
        }

        glPopMatrix();
    }
    else if(body->type == CELESTIAL_ASTEROID) {
        drawAsteroidBelt(body);
    }
    else if(body->type == CELESTIAL_COMET) {
        drawComet(body);
    }
    else if(body->type == CELESTIAL_SPACE_STATION) {
        drawSpaceStation(body);
    }
}

void updateSolarSystem(void) {
    for(int i = 0; i < bodyCount; i++) {
        CelestialBody* body = &solarBodies[i];

        body->currentRotation += body->rotationSpeed;
        if(body->currentRotation >= 360.0f) body->currentRotation -= 360.0f;

        if(body->type == CELESTIAL_PLANET) {
            body->orbitAngle += body->orbitSpeed;
            if(body->orbitAngle >= 360.0f) body->orbitAngle -= 360.0f;

            float angleRad = body->orbitAngle * M_PI / 180.0f;
            body->x = cos(angleRad) * body->orbitRadius;
            body->z = sin(angleRad) * body->orbitRadius;
            body->y = 0;
        }
    }
}

void drawSolarSystem(void) {
    setupSolarLighting();

    GLfloat light_position[] = {
        solarBodies[0].x,
        solarBodies[0].y,
        solarBodies[0].z,
        1.0f
    };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    for(int i = 0; i < bodyCount; i++) {
        drawBody(&solarBodies[i]);
    }
    renderUFOs();
}

float getScaledDistance(float realDistance) {
    return realDistance * SPACE_SCALE_FACTOR;
}

float getScaledRadius(float realRadius) {
    return realRadius * BODY_SCALE_FACTOR;
}

void drawAsteroid(Asteroid* asteroid) {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_shininess[] = { 32.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, asteroid->textureId);

    int horizontalSegments = (MIN_ASTEROID_VERTICES / 2);
    int verticalSegments = 6;

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.0f);
    glVertex3f(asteroid->vertices[0].x, 
              asteroid->vertices[0].y, 
              asteroid->vertices[0].z);

    for (int j = 0; j <= horizontalSegments; j++) {
        int idx = 1 + (j % horizontalSegments);
        Vertex3D v = asteroid->vertices[idx];
        float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        glNormal3f(v.x/len, v.y/len, v.z/len);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();

    for (int i = 0; i < verticalSegments - 2; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= horizontalSegments; j++) {
            for (int k = 0; k < 2; k++) {
                int row = i + k;
                int idx = 1 + row * horizontalSegments + (j % horizontalSegments);
                Vertex3D v = asteroid->vertices[idx];
                float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
                glNormal3f(v.x/len, v.y/len, v.z/len);
                glTexCoord2f(v.u, v.v);
                glVertex3f(v.x, v.y, v.z);
            }
        }
        glEnd();
    }

    glBegin(GL_TRIANGLE_FAN);
    int lastIdx = asteroid->numVertices - 1;
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(asteroid->vertices[lastIdx].x, 
              asteroid->vertices[lastIdx].y, 
              asteroid->vertices[lastIdx].z);

    int startIdx = lastIdx - horizontalSegments;
    for (int j = horizontalSegments; j >= 0; j--) {
        int idx = startIdx + (j % horizontalSegments);
        Vertex3D v = asteroid->vertices[idx];
        float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        glNormal3f(v.x/len, v.y/len, v.z/len);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawAsteroidBelt(CelestialBody* belt) {
    static float* fixedY = NULL;

    if (!asteroidBeltInitialized) {
        srand(1234);
        asteroids = (Asteroid*)malloc(sizeof(Asteroid) * NUM_ASTEROIDS);
        fixedY = (float*)malloc(sizeof(float) * NUM_ASTEROIDS);

        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            asteroids[i].numVertices = 0;
            asteroids[i].vertices = generateAsteroidVertices(&asteroids[i].numVertices);
            asteroids[i].rotation = ((float)rand() / RAND_MAX) * 360.0f;
            asteroids[i].rotationSpeed = 0.0f;
            asteroids[i].orbitAngle = ((float)rand() / RAND_MAX) * 2 * M_PI;
            asteroids[i].orbitRadius = ASTEROID_BELT_DISTANCE + ((float)rand() / RAND_MAX - 0.5) * BELT_WIDTH;
            asteroids[i].orbitSpeed = MIN_ORBIT_SPEED;
            asteroids[i].active = 1;
            asteroids[i].health = MAX_HEALTH;
            asteroids[i].fragmentsActive = 0;
            fixedY[i] = ((float)rand() / RAND_MAX - 0.5) * BELT_HEIGHT;
            asteroids[i].textureId = asteroidTexture;
        }
        asteroidBeltInitialized = 1;
    }

    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            asteroids[i].orbitAngle += asteroids[i].orbitSpeed;

            float x = cos(asteroids[i].orbitAngle) * asteroids[i].orbitRadius;
            float z = sin(asteroids[i].orbitAngle) * asteroids[i].orbitRadius;

            asteroids[i].x = x;
            asteroids[i].y = fixedY[i];
            asteroids[i].z = z;

            glPushMatrix();
            glTranslatef(x, fixedY[i], z);
            glRotatef(asteroids[i].rotation, 0, 1, 0);
            drawAsteroid(&asteroids[i]);
            glPopMatrix();
        }

        if (asteroids[i].fragmentsActive) {
            updateFragments(&asteroids[i]);
            drawFragments(&asteroids[i]);

            static float explosionRadius = 0.0f;
            static float explosionAlpha = 1.0f;

            if (explosionRadius < EXPLOSION_RADIUS) {
                explosionRadius += 0.5f;
                explosionAlpha = 1.0f - (explosionRadius / EXPLOSION_RADIUS);
                drawExplosionEffect(asteroids[i].x, asteroids[i].y, asteroids[i].z,
                                  explosionRadius, explosionAlpha);
            } else {
                explosionRadius = 0.0f;
                explosionAlpha = 1.0f;
            }
        }
    }
}

Vertex3D* generateAsteroidVertices(int* numVertices) {
    *numVertices = MIN_ASTEROID_VERTICES;
    int verticalSegments = 6;
    int horizontalSegments = *numVertices / 2;

    int totalVertices = (verticalSegments - 1) * horizontalSegments + 2;
    Vertex3D* vertices = (Vertex3D*)malloc(sizeof(Vertex3D) * totalVertices);

    float baseRadius = MIN_ASTEROID_RADIUS + 
                      ((float)rand() / RAND_MAX) * (MAX_ASTEROID_RADIUS - MIN_ASTEROID_RADIUS);

    int idx = 0;

    vertices[idx].x = 0;
    vertices[idx].y = baseRadius;
    vertices[idx].z = 0;
    vertices[idx].u = 0.5f;
    vertices[idx].v = 0.0f;
    idx++;

    for (int i = 1; i < verticalSegments; i++) {
        float phi = M_PI * i / verticalSegments;
        float y = baseRadius * cos(phi);
        float radius = baseRadius * sin(phi);
        float v = (float)i / verticalSegments;

        for (int j = 0; j < horizontalSegments; j++) {
            float theta = 2 * M_PI * j / horizontalSegments;
            float u = (float)j / horizontalSegments;

            float x = radius * cos(theta);
            float z = radius * sin(theta);

            if (rand() % 2) {
                x *= ELONGATION_FACTOR;
            } else {
                z *= ELONGATION_FACTOR;
            }

            float distortion = 1.0f + ((float)rand() / RAND_MAX - 0.5f) * SURFACE_ROUGHNESS;

            vertices[idx].x = x * distortion;
            vertices[idx].y = y * (1.0f + ((float)rand() / RAND_MAX - 0.5f) * SURFACE_ROUGHNESS);
            vertices[idx].z = z * distortion;
            vertices[idx].u = u;
            vertices[idx].v = v;

            idx++;
        }
    }

    vertices[idx].x = 0;
    vertices[idx].y = -baseRadius;
    vertices[idx].z = 0;
    vertices[idx].u = 0.5f;
    vertices[idx].v = 1.0f;

    *numVertices = totalVertices;
    return vertices;
}

void initFragments(Asteroid* asteroid) {
    asteroid->fragmentsActive = 1;

    for (int i = 0; i < MAX_FRAGMENTS; i++) {
        Fragment* f = &asteroid->fragments[i];

        int fragVertices = asteroid->numVertices / 3;
        f->vertices = generateAsteroidVertices(&fragVertices);
        f->numVertices = fragVertices;

        f->x = asteroid->x;
        f->y = asteroid->y;
        f->z = asteroid->z;

        float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
        float elevation = ((float)rand() / RAND_MAX - 0.5f) * M_PI;
        float speed = 5.0f + ((float)rand() / RAND_MAX) * 7.0f;  

        f->vx = speed * cos(angle) * cos(elevation);
        f->vy = speed * sin(elevation);
        f->vz = speed * sin(angle) * cos(elevation);

        f->rotX = ((float)rand() / RAND_MAX) * 360.0f;
        f->rotY = ((float)rand() / RAND_MAX) * 360.0f;
        f->rotZ = ((float)rand() / RAND_MAX) * 360.0f;

        f->rotVelX = ((float)rand() / RAND_MAX - 0.5f) * 30.0f;
        f->rotVelY = ((float)rand() / RAND_MAX - 0.5f) * 30.0f;
        f->rotVelZ = ((float)rand() / RAND_MAX - 0.5f) * 30.0f;

        f->scale = 0.3f + ((float)rand() / RAND_MAX) * 0.2f;

        f->lifetime = FRAGMENT_LIFETIME;
        f->active = 1;
    }
}

void updateFragments(Asteroid* asteroid) {
    if (!asteroid->fragmentsActive) return;

    int activeCount = 0;
    for (int i = 0; i < MAX_FRAGMENTS; i++) {
        Fragment* f = &asteroid->fragments[i];
        if (!f->active) continue;

        f->x += f->vx;
        f->y += f->vy;
        f->z += f->vz;

        f->vy -= 0.01f;

        f->rotX += f->rotVelX;
        f->rotY += f->rotVelY;
        f->rotZ += f->rotVelZ;

        f->lifetime--;
        if (f->lifetime <= 0) {
            f->active = 0;
            if (f->vertices) {
                free(f->vertices);
                f->vertices = NULL;
            }
        } else {
            activeCount++;
        }
    }

    if (activeCount == 0) {
        asteroid->fragmentsActive = 0;
    }
}

void drawFragments(Asteroid* asteroid) {
    if (!asteroid->fragmentsActive) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, asteroid->textureId);

    GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_shininess[] = { 32.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    for (int i = 0; i < MAX_FRAGMENTS; i++) {
        Fragment* f = &asteroid->fragments[i];
        if (!f->active) continue;

        glPushMatrix();
        glTranslatef(f->x, f->y, f->z);
        glRotatef(f->rotX, 1, 0, 0);
        glRotatef(f->rotY, 0, 1, 0);
        glRotatef(f->rotZ, 0, 0, 1);

        float visualScale = f->scale * 2.0f;  
        glScalef(visualScale, visualScale, visualScale);

        const int stacks = 12;
        const int slices = 12;
        const float r = 1.0f;

        for (int stack = 0; stack < stacks; stack++) {
            float phi1 = M_PI * (-0.5 + (float)stack / stacks);
            float phi2 = M_PI * (-0.5 + (float)(stack + 1) / stacks);

            glBegin(GL_QUAD_STRIP);
            for (int slice = 0; slice <= slices; slice++) {
                float theta = 2.0f * M_PI * (float)slice / slices;

                for (int k = 0; k <= 1; k++) {
                    float phi = k ? phi2 : phi1;
                    float y = sin(phi);
                    float c = cos(phi);
                    float x = c * cos(theta);
                    float z = c * sin(theta);

                    glTexCoord2f((float)slice / slices, (float)(stack + k) / stacks);
                    glNormal3f(x, y, z);
                    glVertex3f(r * x, r * y, r * z);
                }
            }
            glEnd();
        }

        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
}

void drawExplosionEffect(float x, float y, float z, float radius, float alpha) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float innerRadius = radius * 0.7f;
    float outerRadius = radius;

    glColor4f(1.0f, 0.8f, 0.2f, alpha);
    glutSolidSphere(innerRadius, 20, 20);

    glColor4f(1.0f, 0.4f, 0.0f, alpha * 0.5f);
    glutSolidSphere(outerRadius, 20, 20);

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void cleanupAsteroids(void) {
    if (asteroids != NULL) {
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            if (asteroids[i].vertices != NULL) {
                free(asteroids[i].vertices);
            }
            if (asteroids[i].fragmentsActive) {
                for (int j = 0; j < MAX_FRAGMENTS; j++) {
                    if (asteroids[i].fragments[j].vertices != NULL) {
                        free(asteroids[i].fragments[j].vertices);
                    }
                }
            }
        }
        free(asteroids);
        asteroids = NULL;
        asteroidBeltInitialized = 0;
    }
}

void drawComet(CelestialBody* comet) {
    glPushMatrix();

    float orbitAngleRad = comet->orbitAngle * M_PI / 180.0f;
    comet->x = cos(orbitAngleRad) * comet->orbitRadius;
    comet->z = sin(orbitAngleRad) * comet->orbitRadius;
    comet->y = sin(orbitAngleRad * 0.5f) * comet->orbitRadius * 0.3f;

    glTranslatef(comet->x, comet->y, comet->z);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(1.0f, 0.95f, 0.8f, 1.0f);
    glutSolidSphere(comet->radius * 0.3f, 20, 20);

    float glowSizes[] = {0.4f, 0.5f, 0.7f};
    float glowAlphas[] = {0.7f, 0.5f, 0.3f};
    for(int i = 0; i < 3; i++) {
        glColor4f(1.0f, 0.95f, 0.8f, glowAlphas[i]);
        glutSolidSphere(comet->radius * glowSizes[i], 20, 20);
    }

    float tailLength = comet->radius * 30.0f;
    float spreadBase = comet->radius * 0.5f;
    int numParticles = 500;

    glPointSize(2.0f);
    glBegin(GL_POINTS);

    for(int i = 0; i < numParticles; i++) {
        float t = (float)i / numParticles;
        float spread = spreadBase + (t * comet->radius * 2.0f);

        float x = -tailLength * t;

        float rand1 = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float rand2 = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float y = rand1 * spread;
        float z = rand2 * spread;

        float brightness = 1.0f - (t * 0.8f);
        float alpha = 1.0f - (t * t);

        if(rand() % 4 == 0) {
            glColor4f(1.0f, 1.0f, 1.0f, alpha);
            glPointSize(3.0f);
        } else {
            glColor4f(0.8f * brightness, 0.9f * brightness, 1.0f, alpha * 0.5f);
            glPointSize(2.0f);
        }

        glVertex3f(x, y, z);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    float streamWidth = comet->radius * 0.5f;
    float streamLength = tailLength * 0.7f;
    int segments = 30;

    for(int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float x = -streamLength * t;
        float alpha = 1.0f - (t * t);

        glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.5f);
        glVertex3f(x, streamWidth * (1.0f - t), 0.0f);
        glVertex3f(x, -streamWidth * (1.0f - t), 0.0f);
    }
    glEnd();

    glBegin(GL_LINES);
    for(int i = 0; i < 50; i++) {
        float t = (float)rand() / RAND_MAX;
        float x1 = -tailLength * t;
        float y1 = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * comet->radius;
        float z1 = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * comet->radius;

        float length = comet->radius * (1.0f + t);
        float alpha = 1.0f - t;

        glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.3f);
        glVertex3f(x1, y1, z1);
        glVertex3f(x1 - length, y1, z1);
    }
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPointSize(1.0f);
    glPopMatrix();
}

void drawSpaceStation(CelestialBody* station) {
    glPushMatrix();

    CelestialBody* earth = &solarBodies[3];
    float offsetAngle = station->currentRotation * M_PI / 180.0f;

    station->x = earth->x + STATION_ORBIT_DISTANCE * cos(offsetAngle);
    station->z = earth->z + STATION_ORBIT_DISTANCE * sin(offsetAngle);
    station->y = earth->y + STATION_ORBIT_DISTANCE * 0.1f * sin(offsetAngle * 2.0f);

    glTranslatef(station->x, station->y, station->z);
    glRotatef(station->currentRotation, 0, 1, 0);

    GLfloat mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat mat_shininess[] = { 64.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    float baseSize = STATION_BASE_SIZE;

    glColor3f(0.7f, 0.7f, 0.7f);
    Cylinder(0, 0, 0, baseSize * 1.2f, baseSize * 3.0f);

    glPushMatrix();
    glRotatef(station->currentRotation * STATION_RING_SPEED, 0, 1, 0);

    glColor3f(0.65f, 0.65f, 0.65f);
    for(int i = 0; i < 8; i++) {
        float angle = i * 45.0f;
        glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        Cube(baseSize * 4, 0, 0, 
             baseSize * 1.5f, baseSize * 0.8f, baseSize * 0.8f,
             angle, 0, 1, 0);
        glPopMatrix();
    }

    glColor3f(0.6f, 0.6f, 0.6f);
    for(int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(i * 90, 0, 1, 0);
        Cylinder(baseSize * 4, 0, 0, baseSize * 0.3f, baseSize * 0.3f);
        glPopMatrix();
    }
    glPopMatrix();

    glPushMatrix();
    glRotatef(-station->currentRotation * STATION_RING_SPEED * 0.5f, 0, 1, 0);

    for(int i = -1; i <= 1; i += 2) {

        glColor3f(0.7f, 0.7f, 0.7f);
        Cube(0, 0, i * baseSize * 3,
             baseSize * 0.3f, baseSize * 0.3f, baseSize * 2.0f,
             0, 0, 0, 1);

        glColor3f(0.2f, 0.3f, 0.8f);
        Cube(baseSize * 2, 0, i * baseSize * 3,
             baseSize * 4.0f, baseSize * 0.1f, baseSize * 1.5f,
             0, 0, 0, 1);

        Cube(-baseSize * 2, 0, i * baseSize * 3,
             baseSize * 4.0f, baseSize * 0.1f, baseSize * 1.5f,
             0, 0, 0, 1);
    }
    glPopMatrix();

    glColor3f(0.75f, 0.75f, 0.75f);
    Cylinder(0, baseSize * 2, 0, baseSize * 0.8f, baseSize * 1.0f);

    glPushMatrix();
    glTranslatef(0, baseSize * 3, 0);
    glRotatef(station->currentRotation * 2, 0, 1, 0);
    glColor3f(0.8f, 0.8f, 0.8f);

    for(int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(i * 90, 0, 1, 0);
        Cube(baseSize * 0.8f, 0, 0,
             baseSize * 0.8f, baseSize * 0.1f, baseSize * 0.1f,
             45, 0, 0, 1);
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix();
}

void initUFOs(void) {
    if (!ufoInitialized) {
        ufos = (UFO*)malloc(sizeof(UFO) * NUM_UFOS);
        
        for (int i = 0; i < NUM_UFOS; i++) {
            UFO* ufo = &ufos[i];
            
            // Set initial position closer to the ship's starting position
            float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
            float distance = 500.0f + ((float)rand() / RAND_MAX) * 1000.0f; // Between 500 and 1500 units
            
            ufo->radius = UFO_MIN_RADIUS + ((float)rand() / RAND_MAX) * (UFO_MAX_RADIUS - UFO_MIN_RADIUS);
            ufo->height = UFO_MIN_HEIGHT + ((float)rand() / RAND_MAX) * (UFO_MAX_HEIGHT - UFO_MIN_HEIGHT);
            ufo->rotation = ((float)rand() / RAND_MAX) * 360.0f;
            ufo->rotationSpeed = 0.5f + ((float)rand() / RAND_MAX); // Make rotation more noticeable
            
            // Set initial position relative to ship start position
            ufo->orbitRadius = distance;
            ufo->orbitAngle = angle;
            ufo->x = SHIP_START_X + cos(angle) * distance;
            ufo->y = ((float)rand() / RAND_MAX) * 200.0f - 100.0f; // Random height between -100 and 100
            ufo->z = SHIP_START_Z + sin(angle) * distance;
            
            ufo->orbitSpeed = (UFO_SPEED_MIN + ((float)rand() / RAND_MAX) * (UFO_SPEED_MAX - UFO_SPEED_MIN)) * 0.01f;
            ufo->active = 1;
            ufo->health = MAX_HEALTH;
            ufo->fragmentsActive = 0;
            ufo->textureId = asteroidTexture;
        }
        ufoInitialized = 1;
    }
}

// Update the updateUFOs() function to make movement more noticeable:
void updateUFOs(void) {
    for (int i = 0; i < NUM_UFOS; i++) {
        UFO* ufo = &ufos[i];
        if (!ufo->active) continue;

        ufo->rotation += ufo->rotationSpeed;
        ufo->orbitAngle += ufo->orbitSpeed;

        // Make UFOs orbit around their initial positions
        float baseX = SHIP_START_X + cos(ufo->orbitAngle * 0.1f) * ufo->orbitRadius;
        float baseZ = SHIP_START_Z + sin(ufo->orbitAngle * 0.1f) * ufo->orbitRadius;
        
        // Add a smaller circular motion to make movement more interesting
        float secondaryRadius = 100.0f;
        float secondaryAngle = ufo->orbitAngle * 2.0f;
        
        ufo->x = baseX + cos(secondaryAngle) * secondaryRadius;
        ufo->y += sin(ufo->orbitAngle * 3.0f) * 0.5f; // Gentle vertical bobbing
        ufo->y = fmax(-100, fmin(100, ufo->y)); // Keep height within bounds
        ufo->z = baseZ + sin(secondaryAngle) * secondaryRadius;
    }
}

void UFODome(double radius, int slices, int stacks) {
    for (int i = 0; i < stacks; ++i) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            for (int k = 0; k <= 1; ++k) {
                double phi = (i + k) * M_PI / (2 * stacks);
                double theta = j * 2.0 * M_PI / slices;
                
                double x = radius * cos(theta) * sin(phi);
                double y = radius * cos(phi);
                double z = radius * sin(theta) * sin(phi);
                
                glNormal3d(x/radius, y/radius, z/radius);
                glTexCoord2f(theta / (2 * M_PI), phi / M_PI);
                glVertex3d(x, y, z);
            }
        }
        glEnd();
    }
}

void drawUFO(UFO* ufo) {
    if (!ufo->active || ufo->fragmentsActive) return;

    glPushMatrix();
    glTranslatef(ufo->x, ufo->y, ufo->z);
    glRotatef(ufo->rotation, 0, 1, 0);

    // Ensure proper texture and material state
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Reset color to white

    // Material properties
    GLfloat mat_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat mat_shininess[] = { 64.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // Ensure texture parameters are set for each UFO
    glBindTexture(GL_TEXTURE_2D, ufoBaseTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Draw the main bottom disk with ridges
    const int NUM_RIDGES = 12;
    float ridgeHeight = ufo->height * 0.1f;
    
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 36; ++i) {
        float angle = i * (2.0 * M_PI / 36);
        float cos_angle = cos(angle);
        float sin_angle = sin(angle);
        
        float ridge = (i % (36/NUM_RIDGES) == 0) ? ridgeHeight : 0.0f;
        
        glNormal3f(cos_angle, -0.2f, sin_angle);
        glTexCoord2f((float)i / 36, 0);
        glVertex3f(ufo->radius * cos_angle, ridge, ufo->radius * sin_angle);
        
        float innerRadius = ufo->radius * 0.8f;
        glTexCoord2f((float)i / 36, 1);
        glVertex3f(innerRadius * cos_angle, 0, innerRadius * sin_angle);
    }
    glEnd();

    // Add rotating ring around the base
    static float ringRotation = 0;
    ringRotation += 0.5f;
    
    glPushMatrix();
    glRotatef(ringRotation, 0, 1, 0);
    
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= 36; ++i) {
        float angle = i * (2.0 * M_PI / 36);
        float cos_angle = cos(angle);
        float sin_angle = sin(angle);
        float ringWidth = ufo->radius * 1.1f;
        
        glNormal3f(cos_angle, 0.0f, sin_angle);
        glTexCoord2f((float)i / 36, 0);
        glVertex3f(ringWidth * cos_angle, ufo->height * 0.15f, ringWidth * sin_angle);
        glTexCoord2f((float)i / 36, 1);
        glVertex3f(ringWidth * cos_angle, ufo->height * 0.2f, ringWidth * sin_angle);
    }
    glEnd();
    glPopMatrix();

    // Add detailed mid-section with panels
    const int NUM_PANELS = 8;
    float panelWidth = 2.0f * M_PI / NUM_PANELS;
    
    glBegin(GL_QUADS);
    for (int i = 0; i < NUM_PANELS; ++i) {
        float startAngle = i * panelWidth;
        float endAngle = startAngle + panelWidth * 0.8f;
        
        float cos_start = cos(startAngle);
        float sin_start = sin(startAngle);
        float cos_end = cos(endAngle);
        float sin_end = sin(endAngle);
        
        glNormal3f(cos_start, 0.2f, sin_start);
        glTexCoord2f(0, 0);
        glVertex3f(ufo->radius * cos_start, ufo->height * 0.2f, ufo->radius * sin_start);
        glTexCoord2f(1, 0);
        glVertex3f(ufo->radius * cos_end, ufo->height * 0.2f, ufo->radius * sin_end);
        glTexCoord2f(1, 1);
        glVertex3f(ufo->radius * 0.9f * cos_end, ufo->height * 0.3f, ufo->radius * 0.9f * sin_end);
        glTexCoord2f(0, 1);
        glVertex3f(ufo->radius * 0.9f * cos_start, ufo->height * 0.3f, ufo->radius * 0.9f * sin_start);
    }
    glEnd();

    // Draw dome with glass texture
    glBindTexture(GL_TEXTURE_2D, ufoDomeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set material properties for glass
    GLfloat glass_ambient[] = { 0.4f, 0.4f, 0.4f, 0.6f };
    GLfloat glass_diffuse[] = { 0.8f, 0.8f, 0.8f, 0.6f };
    GLfloat glass_specular[] = { 1.0f, 1.0f, 1.0f, 0.8f };
    GLfloat glass_shininess[] = { 128.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, glass_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glass_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glass_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, glass_shininess);

    glTranslatef(0.0, ufo->height * 0.3f, 0.0);
    
    // Draw layered dome with internal structure
    const int stacks = 20;
    const int slices = 36;
    float dome_radius = ufo->radius * 0.7f;
    float dome_height = ufo->height * 0.7f;
    
    // Inner dome
    glPushMatrix();
    glScalef(0.9f, 0.9f, 0.9f);
    UFODome(dome_radius, slices, stacks);
    glPopMatrix();
    
    // Outer dome
    UFODome(dome_radius, slices, stacks);

    // Add support struts
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);  // Disable texturing for struts
    
    // Save current color
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);
    
    glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
    
    for (int i = 0; i < 6; ++i) {
        glPushMatrix();
        glRotatef(i * 60, 0, 1, 0);
        
        glBegin(GL_QUADS);
        float strutWidth = dome_radius * 0.05f;
        glVertex3f(-strutWidth, 0, 0);
        glVertex3f(strutWidth, 0, 0);
        glVertex3f(strutWidth, dome_height * 0.8f, 0);
        glVertex3f(-strutWidth, dome_height * 0.8f, 0);
        glEnd();
        
        glPopMatrix();
    }

    // Add pulsing lights around the base
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    float pulseIntensity = (sin(glutGet(GLUT_ELAPSED_TIME) * 0.003f) + 1.0f) * 0.5f;
    
    for (int i = 0; i < NUM_PANELS; ++i) {
        float angle = i * (2.0f * M_PI / NUM_PANELS);
        float x = ufo->radius * 1.05f * cos(angle);
        float z = ufo->radius * 1.05f * sin(angle);
        
        glPushMatrix();
        glTranslatef(x, -ufo->height * 0.2f, z);
        
        glColor4f(0.2f, 0.8f, 1.0f, pulseIntensity * 0.6f);
        glutSolidSphere(ufo->radius * 0.05f, 8, 8);
        
        glColor4f(0.2f, 0.8f, 1.0f, pulseIntensity * 0.3f);
        glutSolidSphere(ufo->radius * 0.08f, 8, 8);
        
        glPopMatrix();
    }

    // Restore previous state
    glColor4fv(currentColor);
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    
    glPopMatrix();

    if (ufo->fragmentsActive) {
        updateFragments((Asteroid*)ufo);
        drawFragments((Asteroid*)ufo);
    }
}

void checkBulletUFOCollisions(void) {
    if (!ufoInitialized || ufos == NULL) return;

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        for (int j = 0; j < NUM_UFOS; j++) {
            if (!ufos[j].active || ufos[j].fragmentsActive) continue;

            float dx = bullets[i].x - ufos[j].x;
            float dy = bullets[i].y - ufos[j].y;
            float dz = bullets[i].z - ufos[j].z;
            float distSq = dx*dx + dy*dy + dz*dz;
            float collisionRadius = ufos[j].radius * 1.5f;

            if (distSq < (collisionRadius * collisionRadius)) {
                bullets[i].active = 0;
                ufos[j].health--;

                if (ufos[j].health <= 0) {
                    ufos[j].active = 0;
                    initFragments((Asteroid*)&ufos[j]); // Reuse asteroid explosion
                }
                break;
            }
        }
    }
}

void cleanupUFOs(void) {
    if (ufos != NULL) {
        free(ufos);
        ufos = NULL;
        ufoInitialized = 0;
        
        // Delete textures
        if (ufoBaseTexture) {
            glDeleteTextures(1, &ufoBaseTexture);
            ufoBaseTexture = 0;
        }
        if (ufoDomeTexture) {
            glDeleteTextures(1, &ufoDomeTexture);
            ufoDomeTexture = 0;
        }
    }
}


void renderUFOs(void) {
    if (!ufoInitialized || ufos == NULL) return;
    
    for (int i = 0; i < NUM_UFOS; i++) {
        drawUFO(&ufos[i]);
    }
}