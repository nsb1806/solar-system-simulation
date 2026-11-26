// Exercise the real callbacks without creating an OpenGL window.
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <cassert>
#include <cmath>
#include <iostream>

static int redraws = 0;
static double eye[3], target[3];
static void testRedisplay() { ++redraws; }
static void testTimer(unsigned int, void (*)(int), int) {}
static void testLookAt(double ex, double ey, double ez,
                       double tx, double ty, double tz, double, double, double)
{
    eye[0] = ex; eye[1] = ey; eye[2] = ez;
    target[0] = tx; target[1] = ty; target[2] = tz;
}

#define MEMBER5_CALLBACK_TEST
#define main solarSystemMain
#define glutPostRedisplay testRedisplay
#define glutTimerFunc testTimer
#define gluLookAt testLookAt
#include "../G8finalproject.cpp"
#undef main
#undef glutPostRedisplay
#undef glutTimerFunc
#undef gluLookAt

static bool near(double a, double b) { return std::fabs(a - b) < 0.0001; }

int main()
{
    resetSimulation();
    applyCamera();
    assert(near(eye[0], 0) && near(eye[1], 12) && near(eye[2], 24));
    mouse(GLUT_LEFT_BUTTON, GLUT_DOWN, 0, 0);
    assert(near(simSpeed, 0.75));
    mouse(GLUT_LEFT_BUTTON, GLUT_UP, 0, 0);
    assert(near(simSpeed, 0.75));
    mouse(GLUT_RIGHT_BUTTON, GLUT_DOWN, 0, 0);
    assert(near(simSpeed, 1));
    for (int i = 0; i < 100; ++i) mouse(GLUT_LEFT_BUTTON, GLUT_DOWN, 0, 0);
    assert(near(simSpeed, MIN_SIM_SPEED));
    for (int i = 0; i < 100; ++i) mouse(GLUT_RIGHT_BUTTON, GLUT_DOWN, 0, 0);
    assert(near(simSpeed, MAX_SIM_SPEED));

    keyboard(' ', 0, 0);
    mouse(GLUT_LEFT_BUTTON, GLUT_DOWN, 0, 0);
    timer(0);
    assert(isPaused && near(planets[0].orbitAngle, 0) && near(sunSpinAngle, 0));
    keyboard(' ', 0, 0);
    timer(0);
    assert(near(planets[0].orbitAngle, planets[0].orbitSpeed * 0.016f * simSpeed));
    assert(near(planets[0].spinAngle, planets[0].spinSpeed * 0.016f * simSpeed));

    resetSimulation();
    keyboard('4', 0, 0); // Earth uses planets[2], not planets[3].
    planets[2].orbitAngle = 90;
    float x, y, z;
    getCameraTarget(x, y, z);
    assert(near(x, 0) && near(y, 0) && near(z, -4.8));
    planets[2].orbitAngle = 180;
    applyCamera();
    assert(near(target[0], -4.8) && near(target[2], 0));
    for (int body = 1; body <= NUM_PLANETS; ++body)
    {
        selectedBodyIdx = body;
        planets[body - 1].orbitAngle = 270;
        getCameraTarget(x, y, z);
        assert(near(x, 0) && near(z, planets[body - 1].orbitRadius));
    }

    keyboard('2', 0, 0);
    float distance = cameraDistance;
    mouseWheel(0, 1, 0, 0);
    assert(cameraDistance < distance);
    mouseWheel(0, -1, 0, 0);
    assert(near(cameraDistance, distance));
    for (int i = 0; i < 300; ++i) mouseWheel(0, 1, 0, 0);
    assert(near(cameraDistance, minimumCameraDistance()));
    keyboard('7', 0, 0); // Switching from Mercury must leave room for Saturn's rings.
    applyCamera();
    assert(near(cameraDistance, 3 * planets[5].ringOuter));
    keyboard('1', 0, 0);
    applyCamera();
    assert(near(target[0], 0) && near(target[1], 0) && near(target[2], 0));
    for (int i = 0; i < 300; ++i) mouseWheel(0, -1, 0, 0);
    assert(near(cameraDistance, MAX_CAMERA_DISTANCE));

    resetSimulation();
    distance = cameraDistance;
    motion(0, 50); // No held middle button: no zoom.
    mouseWheel(1, 1, 0, 0); // Horizontal wheel: ignored.
    mouseWheel(0, 0, 0, 0);
    mouse(99, GLUT_DOWN, 0, 0);
    assert(near(cameraDistance, distance) && near(simSpeed, 1));
    mouse(GLUT_MIDDLE_BUTTON, GLUT_DOWN, 10, 100);
    motion(10, 80);
    assert(cameraDistance < distance && near(simSpeed, 1));
    motion(10, 100);
    assert(near(cameraDistance, distance));
    mouse(GLUT_MIDDLE_BUTTON, GLUT_UP, 10, 100);
    motion(10, 200);
    assert(near(cameraDistance, distance));
    mouse(GLUT_MIDDLE_BUTTON, GLUT_DOWN, 10, 100);
    motion(10, 1000000);
    assert(std::isfinite(cameraDistance) && cameraDistance <= MAX_CAMERA_DISTANCE);
    keyboard('R', 0, 0);
    assert(!isZoomDragging && lastMouseY == 0 && selectedBodyIdx == -1);
    assert(!isPaused && near(simSpeed, 1) && near(cameraDistance, DEFAULT_CAMERA_DISTANCE));
    assert(redraws > 0);
    std::cout << "PASS: mouse speed, pause/timer integration, camera tracking, zoom limits, drag and reset\n";
}
