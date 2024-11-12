#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"  // Ignore unused function warnings
#pragma GCC diagnostic ignored "-Wunused-variable"  // Ignore unused variable warnings
#pragma GCC diagnostic ignored "-Wunused-parameter" // Ignore unused parameter warnings

#define STBI_ONLY_PNG
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/utils.h"
#include <math.h>

#define Cos(x) (cos((x)*M_PI/180))
#define Sin(x) (sin((x)*M_PI/180))

GLuint loadTexture(const char* filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);

    if (!data) {
        fprintf(stderr, "Failed to load texture: %s\n", filepath);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture;
}

void Vertex(double th, double ph) {
    // glColor3f(Cos(th) * Cos(th), Sin(ph) * Sin(ph), Sin(th) * Sin(th));
    glVertex3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
}

void Sphere(double x, double y, double z, double radius) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, radius, radius);

    glBegin(GL_TRIANGLE_FAN);
    Vertex(0, -90);
    for (int th = 0; th <= 360; th += d) {
        Vertex(th, d - 90);
    }
    glEnd();

    for (int ph = d - 90; ph <= 90 - 2 * d; ph += d) {
        glBegin(GL_QUAD_STRIP);
        for (int th = 0; th <= 360; th += d) {
            Vertex(th, ph);
            Vertex(th, ph + d);
        }
        glEnd();
    }

    glBegin(GL_TRIANGLE_FAN);
    Vertex(0, 90);
    for (int th = 0; th <= 360; th += d) {
        Vertex(th, 90 - d);
    }
    glEnd();
    glPopMatrix();
}

void Cylinder(double x, double y, double z, double radius, double height) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, height, radius);

    // Cylinder bottom
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 0);
    for (int th = 0; th <= 360; th += d) {
        glVertex3f(Sin(th), 0, Cos(th));
    }
    glEnd();

    // Cylinder sides
    glBegin(GL_QUAD_STRIP);
    for (int th = 0; th <= 360; th += d) {
        glNormal3f(Sin(th), 0, Cos(th));
        glVertex3f(Sin(th), 0, Cos(th));
        glVertex3f(Sin(th), 1, Cos(th));
    }
    glEnd();

    // Cylinder top
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, 1, 0);
    for (int th = 0; th <= 360; th += d) {
        glVertex3f(Sin(th), 1, Cos(th));
    }
    glEnd();

    glPopMatrix();
}

void Cone(double x, double y, double z, double radius, double height) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, height, radius);

    // Cone bottom
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 0);
    for (int th = 0; th <= 360; th += d) {
        glVertex3f(Sin(th), 0, Cos(th));
    }
    glEnd();

    // Cone sides
    glBegin(GL_TRIANGLES);
    for (int th = 0; th <= 360; th += d) {
        float nx = Sin(th);
        float nz = Cos(th);
        glNormal3f(nx, height / sqrt(radius * radius + height * height), nz);
        glVertex3f(Sin(th), 0, Cos(th));
        glVertex3f(0, 1, 0);
    }
    glEnd();

    glPopMatrix();
}

void Cube(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, ax, ay, az);
    glScalef(width, height, depth);

    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0, 0, 1);
    glVertex3f(-1, 0, +1);
    glVertex3f(+1, 0, +1);
    glVertex3f(+1, +1, +1);
    glVertex3f(-1, +1, +1);

    // Back face
    glNormal3f(0, 0, -1);
    glVertex3f(+1, 0, -1);
    glVertex3f(-1, 0, -1);
    glVertex3f(-1, +1, -1);
    glVertex3f(+1, +1, -1);

    // Right face
    glNormal3f(+1, 0, 0);
    glVertex3f(+1, 0, +1);
    glVertex3f(+1, 0, -1);
    glVertex3f(+1, +1, -1);
    glVertex3f(+1, +1, +1);

    // Left face
    glNormal3f(-1, 0, 0);
    glVertex3f(-1, 0, -1);
    glVertex3f(-1, 0, +1);
    glVertex3f(-1, +1, +1);
    glVertex3f(-1, +1, -1);

    // Top face
    glNormal3f(0, 1, 0);
    glVertex3f(-1, +1, +1);
    glVertex3f(+1, +1, +1);
    glVertex3f(+1, +1, -1);
    glVertex3f(-1, +1, -1);

    // Bottom face
    glNormal3f(0, -1, 0);
    glVertex3f(-1, 0, -1);
    glVertex3f(+1, 0, -1);
    glVertex3f(+1, 0, +1);
    glVertex3f(-1, 0, +1);
    glEnd();

    glPopMatrix();
}