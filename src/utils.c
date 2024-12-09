#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"  
#pragma GCC diagnostic ignored "-Wunused-variable"  
#pragma GCC diagnostic ignored "-Wunused-parameter" 

#include "include/utils.h"

#define Cos(x) (cos((x)*M_PI/180))
#define Sin(x) (sin((x)*M_PI/180))

GLuint LoadBMP(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Unable to open %s\n", filename);
        return 0;
    }

    unsigned char header[54];
    if (fread(header, sizeof(unsigned char), 54, file) != 54) {
        printf("Error while readin\n");
        fclose(file);
        return 0;
    }

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bpp = *(short*)&header[28];  

    int size = 3 * width * height;
    unsigned char* data = (unsigned char*)malloc(size);
    if (!data) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return 0;
    }

    int padding = (4 - (width * 3) % 4) % 4;
    int rowSize = width * 3 + padding;

    unsigned char* tempRow = (unsigned char*)malloc(rowSize);

    for (int i = height - 1; i >= 0; i--) {
        if (fread(tempRow, 1, rowSize, file) != rowSize) {
            printf("Error: Failed to read image data\n");
            free(tempRow);
            free(data);
            fclose(file);
            return 0;
        }

        memcpy(&data[i * width * 3], tempRow, width * 3);
    }

    free(tempRow);
    fclose(file);

    for (int i = 0; i < size; i += 3) {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
    return texture;
}

void Vertex(double th, double ph) {
    glVertex3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
}

void Sphere(double x, double y, double z, double radius) {
    const int stacks = 32;  
    const int slices = 32;

    glPushMatrix();
    glTranslated(x, y, z);

    for(int i = 0; i < stacks; i++) {
        double lat0 = M_PI * (-0.5 + (double)i / stacks);
        double lat1 = M_PI * (-0.5 + (double)(i + 1) / stacks);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j <= slices; j++) {
            double lng = 2 * M_PI * (double)j / slices;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3d(x * zr0, z0, y * zr0);
            if(glIsEnabled(GL_TEXTURE_2D)) {
                glTexCoord2f((float)j/slices, (float)i/stacks);
            }
            glVertex3d(radius * x * zr0, radius * z0, radius * y * zr0);

            glNormal3d(x * zr1, z1, y * zr1);
            if(glIsEnabled(GL_TEXTURE_2D)) {
                glTexCoord2f((float)j/slices, (float)(i+1)/stacks);
            }
            glVertex3d(radius * x * zr1, radius * z1, radius * y * zr1);
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

    // Bottom
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0, 0, 0);
    for (int th = 0; th <= 360; th += d) {
        float s = sin(th * M_PI/180);
        float c = cos(th * M_PI/180);
        glTexCoord2f(0.5f + 0.5f*c, 0.5f + 0.5f*s);
        glVertex3f(s, 0, c);
    }
    glEnd();

    // Sides
    glBegin(GL_QUAD_STRIP);
    for (int th = 0; th <= 360; th += d) {
        float s = sin(th * M_PI/180);
        float c = cos(th * M_PI/180);
        glNormal3f(s, 0, c);
        glTexCoord2f(th/360.0f, 0); glVertex3f(s, 0, c);
        glTexCoord2f(th/360.0f, 1); glVertex3f(s, 1, c);
    }
    glEnd();

    // Top
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0, 1, 0);
    for (int th = 0; th <= 360; th += d) {
        float s = sin(th * M_PI/180);
        float c = cos(th * M_PI/180);
        glTexCoord2f(0.5f + 0.5f*c, 0.5f + 0.5f*s);
        glVertex3f(s, 1, c);
    }
    glEnd();

    glPopMatrix();
}

void Cone(double x, double y, double z, double radius, double height) {
    const int d = 5;
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, height, radius);

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 0);
    for (int th = 0; th <= 360; th += d) {
        glVertex3f(Sin(th), 0, Cos(th));
    }
    glEnd();

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
    // Front
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, 0, +1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+1, 0, +1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+1, +1, +1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1, +1, +1);

    // Back
    glNormal3f(0, 0, -1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+1, 0, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, 0, -1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1, +1, -1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+1, +1, -1);

    // Right
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(+1, 0, +1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+1, 0, -1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+1, +1, -1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(+1, +1, +1);

    // Left
    glNormal3f(-1, 0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1, 0, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, 0, +1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1, +1, +1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1, +1, -1);

    // Top
    glNormal3f(0, +1, 0);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1, +1, +1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+1, +1, +1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+1, +1, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, +1, -1);

    // Bottom
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, 0, -1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+1, 0, -1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+1, 0, +1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1, 0, +1);
    glEnd();

    glPopMatrix();
}