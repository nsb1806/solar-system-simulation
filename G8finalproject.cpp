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
#include <cmath>
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

// Store the current window dimensions for the 2D overlay and reshape function.
int windowWidth = 800;
int windowHeight = 600;

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
void drawTexturedSphere(GLdouble radius, int slices, int stacks);
void drawSun();
void drawPlanets();
void drawRings();
void drawStars();
void drawOrbits();
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
void drawTexturedSphere(GLdouble radius, int slices, int stacks) 
{ 
    GLUquadric *quad = gluNewQuadric(); 
    gluQuadricTexture(quad, GL_TRUE); 
    gluQuadricNormals(quad, GLU_SMOOTH); 
    gluSphere(quad, radius, slices, stacks); 
    gluDeleteQuadric(quad); 
}

    void drawSun()
{

    GLfloat glowIntensity = (selectedBodyIdx == 0) ? 1.35f : 0.95f; // Sun emits light when selected
    glPushMatrix();
    glRotatef(sunSpinAngle, 0.0f, 1.0f, 0.0f); // Spin the sun around its axis

if (sunTextureID != 0)
    {
        setPlanetMaterial(1.0f, 0.90f, 0.65f, 0.35f * glowIntensity); // Sun emits light when selected
        
        GLfloat sunEmission[] = {
            1.0f * glowIntensity, 
            0.78f * glowIntensity, 
            0.22f * glowIntensity, 
            1.0f
        };
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sunEmission);
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sunTextureID);
        glColor3f(1.0f, 1.0f, 1.0f);  // Set the sun's color
        drawTexturedSphere(sunRadius, 40, 40);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        GLfloat noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission); // Reset emission
    }
    else
    {
        setPlanetMaterial(sunColor[0], sunColor[1], sunColor[2], glowIntensity * 0.35f); // Sun emits light when selected
        glColor3fv(sunColor);  // Set the sun's color
        glutSolidSphere(sunRadius, 40, 40); // Draw the sun as a solid sphere
    }       

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

        GLfloat glow = (selectedBodyIdx == i + 1) ? 0.55f : 0.0f; // Glow effect when selected

        if (planets[i].textureID != 0)
        {
            setPlanetMaterial(1.0f, 1.0f, 1.0f, glow);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, planets[i].textureID);
            glColor3f(1.0f, 1.0f, 1.0f); // Set the planet's color to white for texture rendering
            drawTexturedSphere(planets[i].radius, 32, 32);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        }
        else
        {
            setPlanetMaterial(planets[i].color[0], planets[i].color[1], planets[i].color[2], glow);
            glColor3fv(planets[i].color);               // Set the planet's color
            glutSolidSphere(planets[i].radius, 32, 32); // Draw the planet as a solid sphere
        }

        glPopMatrix();
    }
}

void drawRings()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
    glDisable(GL_TEXTURE_2D); 
    glEnable(GL_NORMALIZE);   

    GLfloat noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);

    for (int i = 0; i < NUM_PLANETS; ++i)
    {
        if (!planets[i].hasRings)
            continue;
        // Ring's thickness and middle radius converted from its inner and outer edges
        GLfloat tubeRadius = (planets[i].ringOuter - planets[i].ringInner) / 2.0f;
        GLfloat ringRadius = (planets[i].ringOuter + planets[i].ringInner) / 2.0f;

        // Axial tilt: Saturn (~27°) vs. Uranus (~82°)
        GLfloat tilt = (i == 5) ? 27.0f : 82.0f;

        GLfloat c[3];
        GLfloat spec[4];

        if (i == 6) // Uranus: soft icy blue-grey 
        {
            c[0] = 0.44f;
            c[1] = 0.54f;
            c[2] = 0.62f;
            spec[0] = 0.12f; spec[1] = 0.14f; spec[2] = 0.16f; spec[3] = 1.0f;
        }
        else // Saturn (or any other ringed planet)
        {
            c[0] = 0.58f;
            c[1] = 0.58f;
            c[2] = 0.60f;
            spec[0] = 0.10f; spec[1] = 0.10f; spec[2] = 0.10f; spec[3] = 1.0f;
        }

        glColor3fv(c);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, c);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12.0f);

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


void drawOrbits()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glLineWidth(1.0f);

    const float pi = 3.14159265f;
    const int segments = 128;

    for (int i = 0; i < NUM_PLANETS; ++i)
    {
        bool isSelected = (selectedBodyIdx == i + 1);
        float alpha = isSelected ? 0.55f : 0.22f;

        if (isSelected)
            glColor4f(0.85f, 0.90f, 1.0f, alpha);
        else
            glColor4f(0.55f, 0.60f, 0.72f, alpha);

        glBegin(GL_LINE_LOOP);
        for (int s = 0; s < segments; ++s)
        {
            float angle = (2.0f * pi * s) / segments;
            float x = cosf(angle) * planets[i].orbitRadius;
            float z = sinf(angle) * planets[i].orbitRadius;
            glVertex3f(x, 0.0f, z);
        }
        glEnd();
    }

    glPopAttrib();
}

/* ------------------------------------------------------------
                    Lighting and Texture Loading
------------------------------------------------------------*/

void setLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    GLfloat sceneAmbient[] = {0.42f, 0.42f, 0.48, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sceneAmbient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    GLfloat lightAmbient[] = {0.25f, 0.22f, 0.18f, 1.0f};
    GLfloat lightDiffuse[] = {1.45f, 1.30f, 1.00f, 1.0f};
    GLfloat lightSpecular[] = {0.9f, 0.85f, 0.7f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

GLuint loadTexture(const char *filename)
{
    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data)
    {
        cerr << "Failed to load texture: " << filename << stbi_failure_reason()<< endl;
        return 0;
    }

    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, 
        format, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return textureID;
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
    GLfloat matAmbient[] = {r * 0.25f, g * 0.25f, b * 0.25f, 1.0f};
    GLfloat matDiffuse[] = {r, g, b, 1.0f};
    GLfloat matSpecular[] = {0.45f, 0.45f, 0.45f, 1.0f};
    GLfloat matEmission[] = {emission * r, emission * g, emission * b, 1.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

}


/* ------------------------------------------------------------
                        2D Text Overlay
------------------------------------------------------------*/
void drawBitmapText(float x, float y, const char *text)
{
    glRasterPos2f(x, y);
    for (const char *character = text; *character != '\0'; ++character)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *character);
}

void draw2DTextOverlay()
{
    // Save the 3D rendering state before switching to screen-space drawing.
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<GLdouble>(windowWidth),
               0.0, static_cast<GLdouble>(windowHeight));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    const char *selectedName = "None";
    const char *selectedNickname = "Press 1-9 to select a body";

    if (selectedBodyIdx == 0)
    {
        selectedName = sunName;
        selectedNickname = sunNickname;
    }
    else if (selectedBodyIdx > 0 && selectedBodyIdx <= NUM_PLANETS)
    {
        selectedName = planets[selectedBodyIdx - 1].name;
        selectedNickname = planets[selectedBodyIdx - 1].nickname;
    }

    char selectedLine[128];
    snprintf(selectedLine, sizeof(selectedLine), "%s - %s",
             selectedName, selectedNickname);

    char statusLine[80];
    snprintf(statusLine, sizeof(statusLine), "Simulation: %s | Speed: %.1fx",
             isPaused ? "PAUSED" : "RUNNING", simSpeed);

    const char *controlsLine = "Space: Pause/Resume   R: Reset   1-9: Select   Esc: Exit";
    const float rightMargin = 15.0f;

    int selectedWidth = 0;
    for (const char *character = selectedLine; *character != '\0'; ++character)
        selectedWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, *character);

    int statusWidth = 0;
    for (const char *character = statusLine; *character != '\0'; ++character)
        statusWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, *character);

    int controlsWidth = 0;
    for (const char *character = controlsLine; *character != '\0'; ++character)
        controlsWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, *character);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(windowWidth - selectedWidth - rightMargin,
                   48.0f, selectedLine);
    drawBitmapText(windowWidth - statusWidth - rightMargin,
                   30.0f, statusLine);
    drawBitmapText(windowWidth - controlsWidth - rightMargin,
                   12.0f, controlsLine);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
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
        0.0f, 7.5f, 15.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);
    GLfloat lightPos[] = {0.0f, 4.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawSun();
    drawPlanets();
    drawRings();
    drawStars();
    drawOrbits();
    draw2DTextOverlay();

    glutSwapBuffers();
}

/* ------------------------------------------------------------
                Window Reshape and Timer
------------------------------------------------------------*/
void reshape(int width, int height)
{
    // Update the viewport and perspective projection whenever the window is
    // resized, preventing the solar system from being stretched.
    windowWidth = (width > 0) ? width : 1;
    windowHeight = (height > 0) ? height : 1;

    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,
                   static_cast<GLdouble>(windowWidth) /
                       static_cast<GLdouble>(windowHeight),
                   0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    (void)x;
    (void)y;

    // Handle simulation controls and body selection from keyboard input.
    switch (key)
    {
    case ' ':
        isPaused = !isPaused;
        break;

    case 'r':
    case 'R':
        resetSimulation();
        break;

    case '1':
        selectedBodyIdx = 0; // Sun
        break;

    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        selectedBodyIdx = key - '1';
        break;

    case 27: // Escape
        exit(0);
        break;

    default:
        return;
    }

    glutPostRedisplay();
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
