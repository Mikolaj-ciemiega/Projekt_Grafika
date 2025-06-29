// Wymagane biblioteki: GLUT (np. FreeGLUT), OpenGL, stb_image

#include <GL/glut.h>
#include <cmath>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Prototypy
void specialKeys(int key, int x, int y);
GLuint loadTexture(const char* filename);

// Parametry obrotu i animacji
float anglePlanet = 0.0f, angleMoon1 = 0.0f, angleMoon2 = 0.0f;
float selfRotateSun = 0.0f, selfRotatePlanet = 0.0f, selfRotateMoon1 = 0.0f, selfRotateMoon2 = 0.0f;
int rotationDirection = 1; // 1 = zgodnie z ruchem wskazówek, -1 = przeciwnie, 0 = stop

bool lightSun = true;
bool lightMoon2 = true;

// Tekstury - id
GLuint texSun, texPlanet, texMoon1, texMoon2;

GLuint loadTexture(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

void loadTextures() {
    texSun = loadTexture("sun.jpg");
    texPlanet = loadTexture("planet.jpg");
    texMoon1 = loadTexture("moon1.jpg");
    texMoon2 = loadTexture("moon2.jpg");
}

void drawTexturedSphere(GLuint textureID, float radius) {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    gluSphere(quad, radius, 40, 40);
    gluDeleteQuadric(quad);
    glDisable(GL_TEXTURE_2D);
    if (textureID == texMoon2) {
        GLfloat emission[] = { 1.0f, 1.0f, 0.8f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    else {
        GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
    }
}

void drawOrbit(float radius) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1, 1, 1, 0.4f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; ++i) {
        float theta = 2.0f * M_PI * i / 100;
        glVertex3f(cos(theta) * radius, 0.0f, sin(theta) * radius);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 5.0, 30.0,   // Zwiêkszono dystans kamery
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    if (lightSun) {
        GLfloat lightPos0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    }
    else {
        glDisable(GL_LIGHT0);
    }

    if (lightMoon2) {
        GLfloat lightPos1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glEnable(GL_LIGHT1);
        glPushMatrix();
        glRotatef(anglePlanet, 0, 1, 0);
        glTranslatef(12.0f, 0.0f, 0.0f);  // dalej od s³oñca
        glRotatef(angleMoon2, 0, 1, 0);
        glTranslatef(4.0f, 0.0f, 0.0f);   // dalej od planety
        glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
        glPopMatrix();
    }
    else {
        glDisable(GL_LIGHT1);
    }

    // S³oñce
    glPushMatrix();
    glRotatef(selfRotateSun, 0, 1, 0);
    drawTexturedSphere(texSun, 3.0f);
    glPopMatrix();

    drawOrbit(12.0f); // nowa orbita planety

    // Planeta
    glPushMatrix();
    glRotatef(anglePlanet, 0, 1, 0);
    glTranslatef(12.0f, 0.0f, 0.0f);
    glRotatef(selfRotatePlanet, 0, 1, 0);
    drawTexturedSphere(texPlanet, 1.5f);

    drawOrbit(3.0f); // ksiê¿yc 1
    drawOrbit(4.0f); // ksiê¿yc 2

    // Ksiê¿yc 1
    glPushMatrix();
    glRotatef(angleMoon1, 0, 1, 0);
    glTranslatef(3.0f, 0.0f, 0.0f);
    glRotatef(selfRotateMoon1, 0, 1, 0);
    drawTexturedSphere(texMoon1, 0.5f);
    glPopMatrix();

    // Ksiê¿yc 2
    glPushMatrix();
    glRotatef(angleMoon2, 0, 1, 0);
    glTranslatef(4.0f, 0.0f, 0.0f);
    glRotatef(selfRotateMoon2, 0, 1, 0);
    drawTexturedSphere(texMoon2, 0.3f);
    glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
}

void update(int value) {
    if (rotationDirection != 0) {
        anglePlanet += 0.2f * rotationDirection;
        angleMoon1 += 0.6f * rotationDirection;
        angleMoon2 += 0.3f * rotationDirection;

        selfRotateSun += 0.2f * rotationDirection;
        selfRotatePlanet += 1.0f * rotationDirection;
        selfRotateMoon1 += 1.2f * rotationDirection;
        selfRotateMoon2 += 1.4f * rotationDirection;
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'p':
    case 'P': rotationDirection = 0; break;
    case '1': lightSun = false; break;
    case '2': lightMoon2 = false; break;
    case '3': lightSun = true; lightMoon2 = true; break;
    }
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_RIGHT) rotationDirection = 1;
    if (key == GLUT_KEY_LEFT) rotationDirection = -1;
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    loadTextures();


    GLfloat lightColor1[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightColor1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Uk³ad planetarny - OpenGL");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}