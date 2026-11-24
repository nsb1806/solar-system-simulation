/*
    CS0045 - Final Project
    Project Title: Solar System Simulation
    GROUP 8 MEMBERS:
    - Bumatay, Germione
    - Butaslac, Nj
    - Ferrer, Cypress Angel
    - Munsayac, Jilliane Margaret
    - Radoc, Justine Grace
*/

#define GLEW_STATIC
#define BUFFER_OFFSET(i) ((void *)(uintptr_t)(i))
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <cstdint>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <stdio.h>
#include "stbi_image.h"

using namespace std;

/* ------------------------------------------------------------
                Structures and Global Variables
    Structure is used to store the properties of each planet
------------------------------------------------------------*/

struct Planet
{
    const char *name;
    const char *nickname;
    GLfloat radius;
    GLfloat orbitRadius;
    GLfloat orbitAngle;
    GLfloat spinAngle;
    GLfloat orbitSpeed;
    GLfloat spinSpeed;
    GLfloat color[3];
    bool hasRings;
    GLfloat ringInner;
    GLfloat ringOuter;
    const char *textureFile;
    GLuint textureID;
};

Planet planets[] = {
    // name,      nickname,           rad,  orbRad, orbAng, spAng, orbSpd, spSpd, color[3],          hasRings, ringI, ringO, textureFile          textID
    {"MERCURY", "The Swift Planet", 0.22f, 2.2f, 25.0f, 0.0f, 40.0f, 10.0f, {0.65f, 0.62f, 0.58f}, false, 0.0f, 0.0f, "textures/mercury.jpg", 0},
    {"VENUS", "Earth's Twin", 0.36f, 3.4f, 215.0f, 0.0f, 30.0f, 8.0f, {0.85f, 0.55f, 0.25f}, false, 0.0f, 0.0f, "textures/venus.jpg", 0},
    {"EARTH", "The Blue Planet", 0.40f, 4.8f, 195.0f, 0.0f, 25.0f, 20.0f, {0.20f, 0.55f, 0.90f}, false, 0.0f, 0.0f, "textures/earth.jpg", 0},
    {"MARS", "The Red Planet", 0.30f, 6.2f, 45.0f, 0.0f, 20.0f, 18.0f, {0.85f, 0.35f, 0.20f}, false, 0.0f, 0.0f, "textures/mars.jpg", 0},
    {"JUPITER", "The Gas Giant", 0.85f, 9.2f, 35.0f, 0.0f, 12.0f, 25.0f, {0.82f, 0.62f, 0.42f}, false, 0.0f, 0.0f, "textures/jupiter.jpg", 0},
    {"SATURN", "The Ringed Planet", 0.70f, 11.8f, 190.0f, 0.0f, 10.0f, 22.0f, {0.85f, 0.72f, 0.48f}, true, 0.95f, 1.45f, "textures/saturn.jpg", 0},
    {"URANUS", "The Ice Giant", 0.55f, 14.2f, 325.0f, 0.0f, 8.0f, 16.0f, {0.30f, 0.82f, 0.88f}, true, 0.75f, 1.15f, "textures/uranus.jpg", 0},
    {"NEPTUNE", "The Distant Giant", 0.52f, 16.5f, 95.0f, 0.0f, 6.0f, 14.0f, {0.25f, 0.50f, 0.92f}, false, 0.0f, 0.0f, "textures/neptune.jpg", 0}};

const int NUM_PLANETS = sizeof(planets) / sizeof(planets[0]);

const char *sunName = "SUN";
const char *sunNickname = "The Nearest Star";
GLfloat sunRadius = 1.4f;
GLfloat sunSpinSpeed = 5.0f;
GLfloat sunSpinAngle = 0.0f;
GLfloat sunColor[3] = {1.0f, 0.75f, 0.15f};
const char *sunTextureFile = "textures/sun.jpg";
GLuint sunTextureID = 0;

bool isPaused = false;
float simSpeed = 1.0f;
int selectedBodyIdx = -1; // -1 none, 0 SUN, 1..8 planets[0..7]

// --- Star VBO data ---
const int NUM_STARS = 1500;
GLuint starVBO = 0;
/* ------------------------------------------------------------
                    Function Prototypes
------------------------------------------------------------*/

void init();
void display();
void reshape(int width, int height);
void timer(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

void setLighting();
void initStarVBO();
void resetSimulation();

void setPlanetMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat emission = 0.0f);
void drawSun();
void drawPlanets();
void drawRings();
void drawStars();
void drawBitmapText(float x, float y, const char *text);
void draw2DTextOverlay();

GLuint loadTexture(const char *filename);
void loadAllTextures();

/* ------------------------------------------------------------
                        Main Function
------------------------------------------------------------*/

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Group 8 - Solar System Simulation");

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cerr << "GLEW initialization failed!" << endl;
        return 1;
    }

    init();

    // Register callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}

/* ------------------------------------------------------------
                    Initialization
------------------------------------------------------------*/

void init()
{
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    setLighting();
    loadAllTextures();
    initStarVBO();
    resetSimulation();

    reshape(800, 600);
}

// Generates random star positions and colors, uploads them to a GPU buffer
void initStarVBO()
{
    // Array storing 6 floats per star: x, y, z, r, g, b
    GLfloat data[NUM_STARS * 6];

    for (int i = 0; i < NUM_STARS; ++i)
    {
        float x, y, z, distSq;

        // Pick random points in a big box until one lands 50-70 units from the center
        do
        {
            x = (rand() % 1401 - 700) / 10.0f; // -70.0 to 70.0
            y = (rand() % 1401 - 700) / 10.0f;
            z = (rand() % 1401 - 700) / 10.0f;
            distSq = x * x + y * y + z * z;
        } while (distSq < 50.0f * 50.0f || distSq > 70.0f * 70.0f);
        
        // Generate a base brightness between 0.5 and 1.0
        float brightness = 0.5f + (rand() % 51) / 100.0f; 

        // Store the 3D position 
        data[i * 6 + 0] = x;
        data[i * 6 + 1] = y;
        data[i * 6 + 2] = z;
        // Store the color with slight random variation in tints
        data[i * 6 + 3] = brightness * (0.80f + (rand() % 21) / 100.0f); // Red
        data[i * 6 + 4] = brightness * (0.80f + (rand() % 21) / 100.0f); // Green
        data[i * 6 + 5] = brightness * (0.80f + (rand() % 21) / 100.0f); // Blue
    }

    glGenBuffers(1, &starVBO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* ------------------------------------------------------------
                    Object Drawing Functions
------------------------------------------------------------*/

void drawSun()
{

    glPushMatrix();

    glRotatef(sunSpinAngle, 0.0f, 1.0f, 0.0f); // Spin the sun around its axis
    glColor3fv(sunColor);                      // Set the sun's color
    glutSolidSphere(sunRadius, 40, 40);        // Draw the sun as a solid sphere

    glPopMatrix();
}

void drawPlanets()
{

    for (int i = 0; i < NUM_PLANETS; ++i)
    {
        glPushMatrix();

        glRotatef(planets[i].orbitAngle, 0.0f, 1.0f, 0.0f); // Rotate the planet around the sun based on its orbit angle
        glTranslatef(planets[i].orbitRadius, 0.0f, 0.0f);   // Move the planet to its orbit radius from the sun
        glRotatef(planets[i].spinAngle, 0.0f, 1.0f, 0.0f);  // Spin the planet around its own axis

        glColor3fv(planets[i].color);               // Set the planet's color
        glutSolidSphere(planets[i].radius, 32, 32); // Draw the planet as a solid sphere

        glPopMatrix();
    }
}

void drawRings()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
    glDisable(GL_TEXTURE_2D); 
    glEnable(GL_NORMALIZE);   

    for (int i = 0; i < NUM_PLANETS; ++i)
    {
        if (!planets[i].hasRings)
            continue;
        // Ring's thickness and middle radius converted from its inner and outer edges
        GLfloat tubeRadius = (planets[i].ringOuter - planets[i].ringInner) / 2.0f;
        GLfloat ringRadius = (planets[i].ringOuter + planets[i].ringInner) / 2.0f;

        // Axial tilt: Saturn (~27°) vs. Uranus (~82°)
        GLfloat tilt = (i == 5) ? 27.0f : 82.0f;

        // slightly lighter version of the planet's color
        GLfloat c[3] = {
            planets[i].color[0] * 0.9f + 0.1f,
            planets[i].color[1] * 0.9f + 0.1f,
            planets[i].color[2] * 0.9f + 0.1f};

        glColor3fv(c);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, c);

        glPushMatrix();
        glRotatef(planets[i].orbitAngle, 0.0f, 1.0f, 0.0f); // follow the planet's orbit
        glTranslatef(planets[i].orbitRadius, 0.0f, 0.0f);
        glRotatef(-planets[i].orbitAngle, 0.0f, 1.0f, 0.0f); // keep the tilt fixed in space
        glRotatef(tilt, 0.0f, 0.0f, 1.0f);                   
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);                  // lay the torus flat
        glScalef(1.0f, 1.0f, 0.08f);                         
        glutSolidTorus(tubeRadius, ringRadius, 24, 64);
        glPopMatrix();
    }

    glPopAttrib();
}

void drawStars()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POINT_BIT);
    glDisable(GL_LIGHTING);   // Disabled because stars shine on their own
    glDisable(GL_TEXTURE_2D);
    glPointSize(1.8f);

    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Each star takes 6 floats in the buffer: [x, y, z, r, g, b]
    GLsizei stride = 6 * sizeof(GLfloat);
    glVertexPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(0));
    glColorPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(3 * sizeof(GLfloat)));

    // Draw every star as a single point
    glDrawArrays(GL_POINTS, 0, NUM_STARS);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPopAttrib();
}

/* ------------------------------------------------------------
                    Lighting and Texture Loading
------------------------------------------------------------*/

void setLighting()
{
}

GLuint loadTexture(const char *filename)
{
}

void loadAllTextures()
{
    sunTextureID = loadTexture(sunTextureFile);
    for (int i = 0; i < NUM_PLANETS; ++i)
    {
        planets[i].textureID = loadTexture(planets[i].textureFile);
    }
}

void setPlanetMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat emission)
{
}

/* ------------------------------------------------------------
                        2D Text Overlay
------------------------------------------------------------*/
void drawBitmapText(float x, float y, const char *text)
{
}

void draw2DTextOverlay()
{
}

/* ------------------------------------------------------------
                        Display Callback
------------------------------------------------------------*/
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        0.0f, 12.0f, 24.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);

    drawSun();
    drawPlanets();
    drawRings();
    drawStars();
    draw2DTextOverlay();

    glutSwapBuffers();
}

/* ------------------------------------------------------------
                Window Reshape and Timer
------------------------------------------------------------*/
void reshape(int width, int height)
{
}

void timer(int value)
{

    if (!isPaused)
    {
        // Advance each planet's orbit and spin.
        for (int i = 0; i < NUM_PLANETS; ++i)
        {
            planets[i].orbitAngle += planets[i].orbitSpeed * 0.016f * simSpeed;
            planets[i].spinAngle += planets[i].spinSpeed * 0.016f * simSpeed;

            // Keep the angles within one full rotation.
            if (planets[i].orbitAngle >= 360.0f)
                planets[i].orbitAngle -= 360.0f;

            if (planets[i].spinAngle >= 360.0f)
                planets[i].spinAngle -= 360.0f;
        }

        // Advance the Sun's spin.
        sunSpinAngle += sunSpinSpeed * 0.016f * simSpeed;
        if (sunSpinAngle >= 360.0f)
            sunSpinAngle -= 360.0f;
    }

    glutPostRedisplay();         // Request a redraw of the window
    glutTimerFunc(16, timer, 0); // Set up the next timer callback in ~16 ms (60 FPS)
}

/* ------------------------------------------------------------
                    Keyboard Callback
------------------------------------------------------------*/
void keyboard(unsigned char key, int x, int y)
{
}

/* ------------------------------------------------------------
                        Mouse Callback
------------------------------------------------------------*/
void mouse(int button, int state, int x, int y)
{
}

/* ------------------------------------------------------------
                        Reset Simulation
------------------------------------------------------------*/
void resetSimulation()
{

    // Return every planet's orbit and spin to zero degrees.
    for (int i = 0; i < NUM_PLANETS; ++i)
    {
        planets[i].orbitAngle = 0.0f;
        planets[i].spinAngle = 0.0f;
    }

    sunSpinAngle = 0.0f; // Reset the Sun's spin angle to zero degrees.
    isPaused = false;    // Make sure the simulation is not paused.
    simSpeed = 1.0f;     // Reset the simulation speed to normal.
    selectedBodyIdx = -1;
}