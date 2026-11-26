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
#include <algorithm>
#include <cmath>
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

// Member 5: mouse controls share simSpeed with Member 1's timer.
const float MIN_SIM_SPEED = 0.25f;
const float MAX_SIM_SPEED = 5.0f;
const float SIM_SPEED_STEP = 0.25f;
const float DEFAULT_CAMERA_DISTANCE = std::sqrt(12.0f * 12.0f + 24.0f * 24.0f);
const float MAX_CAMERA_DISTANCE = 80.0f;
float cameraDistance = DEFAULT_CAMERA_DISTANCE;
bool isZoomDragging = false;
int lastMouseY = 0;

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
void motion(int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
float minimumCameraDistance();
void zoomCamera(float factor);
void getCameraTarget(float &x, float &y, float &z);
void applyCamera();

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
    glutMotionFunc(motion);
    glutMouseWheelFunc(mouseWheel); // FreeGLUT supplies wheel events separately.
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
    snprintf(statusLine, sizeof(statusLine), "Simulation: %s | Speed: %.2fx",
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
    applyCamera(); // Member 5: preserve the original viewing angle while zooming.

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
    (void)x;

    // Middle drag is reserved for zoom so left/right clicks keep their
    // README-defined speed controls. Button release must not change speed.
    if (button == GLUT_MIDDLE_BUTTON)
    {
        isZoomDragging = (state == GLUT_DOWN);
        lastMouseY = y;
        return;
    }

    if (state != GLUT_DOWN)
        return;

    if (button == GLUT_LEFT_BUTTON)
        simSpeed = std::max(MIN_SIM_SPEED, simSpeed - SIM_SPEED_STEP);
    else if (button == GLUT_RIGHT_BUTTON)
        simSpeed = std::min(MAX_SIM_SPEED, simSpeed + SIM_SPEED_STEP);
    else
        return;

    // Updating speed while paused does not resume Member 1's timer.
    glutPostRedisplay();
}

void motion(int x, int y)
{
    (void)x;
    if (!isZoomDragging)
        return;

    // Drag up to move closer; drag down to move away. Limit a single event
    // so a large pointer jump cannot overflow the exponential zoom factor.
    const int deltaY = std::max(-100, std::min(100, y - lastMouseY));
    lastMouseY = y;
    zoomCamera(std::exp(static_cast<float>(deltaY) * 0.01f));
}

void mouseWheel(int wheel, int direction, int x, int y)
{
    (void)x;
    (void)y;
    if (wheel != 0 || direction == 0)
        return;

    zoomCamera(direction > 0 ? 0.9f : 1.0f / 0.9f);
}

float minimumCameraDistance()
{
    float extent = sunRadius;
    if (selectedBodyIdx > 0 && selectedBodyIdx <= NUM_PLANETS)
    {
        const Planet &planet = planets[selectedBodyIdx - 1];
        extent = planet.hasRings ? std::max(planet.radius, planet.ringOuter)
                                : planet.radius;
    }

    // Keep the camera outside the selected sphere and its rings.
    return std::max(1.5f, 3.0f * extent);
}

void zoomCamera(float factor)
{
    cameraDistance = std::max(minimumCameraDistance(),
                             std::min(MAX_CAMERA_DISTANCE, cameraDistance * factor));
    glutPostRedisplay();
}

void getCameraTarget(float &x, float &y, float &z)
{
    x = y = z = 0.0f; // No selection, or Sun selection: look at the origin.
    if (selectedBodyIdx > 0 && selectedBodyIdx <= NUM_PLANETS)
    {
        const Planet &planet = planets[selectedBodyIdx - 1];
        const float angle = planet.orbitAngle * 3.14159265358979323846f / 180.0f;

        // Match Member 1's glRotatef(Y) then glTranslatef(orbitRadius, 0, 0).
        // Recompute each frame so the camera follows the orbit without
        // changing any planet's position, spin, or animation speed.
        x = planet.orbitRadius * std::cos(angle);
        z = -planet.orbitRadius * std::sin(angle);
    }
}

void applyCamera()
{
    float targetX, targetY, targetZ;
    getCameraTarget(targetX, targetY, targetZ);

    // Recheck after keyboard selection, e.g. Mercury to the larger Sun.
    cameraDistance = std::max(minimumCameraDistance(),
                             std::min(MAX_CAMERA_DISTANCE, cameraDistance));
    const float scale = cameraDistance / DEFAULT_CAMERA_DISTANCE;
    gluLookAt(targetX, targetY + 12.0f * scale, targetZ + 24.0f * scale,
              targetX, targetY, targetZ,
              0.0f, 1.0f, 0.0f);
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
    cameraDistance = DEFAULT_CAMERA_DISTANCE;
    isZoomDragging = false;
    lastMouseY = 0;
}
