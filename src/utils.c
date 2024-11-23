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
    const int stacks = 40;  // Increase detail level
    const int slices = 40;
    
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(radius, radius, radius);
    
    for(int i = 0; i < stacks; i++) {
        double lat0 = M_PI * (-0.5 + (double)i / stacks);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);
        
        double lat1 = M_PI * (-0.5 + (double)(i + 1) / stacks);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);
        
        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j <= slices; j++) {
            double lng = 2 * M_PI * (double)j / slices;
            double x = cos(lng);
            double y = sin(lng);
            
            // Normal vector for proper lighting
            glNormal3d(x * zr0, y * zr0, z0);
            glVertex3d(x * zr0, y * zr0, z0);
            
            // Normal vector for next vertex
            glNormal3d(x * zr1, y * zr1, z1);
            glVertex3d(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
    
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