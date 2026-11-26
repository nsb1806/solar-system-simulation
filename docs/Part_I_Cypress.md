# Solar System Simulation Documentation Part I

**Course:** CS0045  
**Group:** 8  
**Prepared by:** Cypress Angel O. Ferrer  
**Contribution:** Member 5

## 1 Introduction

Our project is an interactive three-dimensional solar system simulation developed in C++ using OpenGL. It represents the Sun and eight planets using spheres, with a star background and torus-shaped rings for Saturn and Uranus. Users can observe orbital revolution and axial rotation, identify a celestial body, control animation speed, and inspect a selected body using the camera.

The project applies computer graphics concepts through a familiar subject. Rather than treating drawing, animation, and interaction as separate exercises, the simulation combines them in one scene. The displayed sizes, orbital distances, and speeds are chosen for visibility and demonstration. They are not astronomical measurements or predictions.

## 2 Project objectives

The general objective is to demonstrate an interactive 3D scene using OpenGL and event-driven C++ programming.

The specific objectives are to:

1. Represent the Sun and planets with spheres, planetary rings with tori, and stars with points.
2. Use translation, rotation, and scaling to position and transform scene objects.
3. Animate each planet's orbit and axial spin through a timer callback.
4. Provide keyboard controls for pause, reset, body selection, and text identification.
5. Provide mouse controls for bounded animation speed changes and camera zoom.
6. Use a vertex buffer object to store star positions and colors.
7. Integrate ambient, diffuse, specular, and emissive lighting with image texture mapping as part of the final group implementation.

## 3 Scope and limitations

The scene covers the Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, and Neptune. Saturn and Uranus have rings. The star background contains 1,500 points stored in a VBO. A 2D text overlay reports the selected body, its nickname, the simulation's running or paused state, and its speed.

Users can adjust the speed from 0.25x to 5.0x, pause and resume animation, select a body with keys 1–9, and zoom toward or away from the selected body. The camera follows the selected planet as it orbits. Reset restores the full-system starting view and clears the selection.

This is an educational visualization. It does not calculate gravitational forces, elliptical orbital mechanics, collisions, or real-time astronomical positions. Moons and an asteroid belt are outside the implemented Member 5 scope; the README treats an asteroid belt as optional. The camera maintains a fixed viewing direction, so mouse dragging changes distance rather than rotating the view.

At this stage, the shared source contains empty lighting, material, and texture-loading functions. It also references an image-loader header and texture images that are absent from the repository. Those items belong to Member 3's final integration and must be completed before claiming the full textured and illuminated application is ready.

## 4 Development tools and program structure

| Component | Purpose |
| --- | --- |
| C++ | Stores object properties, updates animation state, and handles callbacks |
| OpenGL | Renders geometry and applies transformations and render state |
| GLU | Provides the perspective projection and camera view |
| FreeGLUT | Creates the window and dispatches keyboard, mouse, motion, wheel, reshape, and timer callbacks |
| GLEW | Initializes access to OpenGL extension functions, including VBO operations |
| GitHub | Maintains the shared source and reviews contributions through branches and pull requests |

The main source file is `G8finalproject.cpp`. The `Planet` structure stores a body's name, nickname, radius, orbit radius, orbit angle, spin angle, movement speeds, color, ring properties, and texture information. Shared state includes the pause flag, simulation speed, selected body index, and camera distance.

Initialization creates the window, initializes GLEW, prepares scene data, resets simulation state, and registers callbacks. `timer()` advances orbit and spin angles when the simulation is running. `display()` sets the camera and draws the Sun, planets, rings, stars, and text overlay. Input callbacks update shared state and request a redraw.

## 5 Scene design and transformations

Each planet is drawn inside a saved model matrix. Rotation around the Y axis establishes its orbital angle, and translation along the local X axis places it at its orbit radius. A further Y-axis rotation applies axial spin. Restoring the matrix after each planet prevents its transformations from affecting the next object.

The current planet drawing functions pass different radii to `glutSolidSphere` to create visible size differences. Ring rendering additionally uses `glScalef` to flatten the torus. This distinction explains the actual source implementation without implying that every sphere uses an explicit scaling call.

The projection uses a 45-degree field of view and updates its aspect ratio when the window is resized. A separate orthographic projection draws text in screen coordinates, then restores the 3D drawing state.

## 6 Mouse interaction implementation

### 6.1 Animation speed control

`mouse()` handles left and right button presses. A left-click reduces `simSpeed` by 0.25, and a right-click increases it by 0.25. The result is bounded between 0.25 and 5.0 to prevent reverse motion, an accidental zero-speed state, or excessive acceleration. Button releases do not apply a second speed change.

The existing timer multiplies orbit and spin increments by `simSpeed`, so the mouse controls affect the Sun's spin and every planet's motion consistently. They leave `isPaused` unchanged. A user can therefore choose a new speed while paused and apply it when Space resumes the simulation.

### 6.2 Zoom controls

`mouseWheel()` handles the primary wheel. Scrolling up multiplies camera distance by 0.9; scrolling down divides it by 0.9. FreeGLUT's wheel callback is registered separately from the button callback to avoid treating a wheel event as a speed click.

As an alternative, pressing the middle button starts a zoom drag. `motion()` compares the current vertical pointer position with the previous one: dragging up moves closer and dragging down moves farther away. Releasing the middle button ends the drag. Ordinary pointer movement does not change zoom.

The minimum distance depends on the selected body's size and includes its outer ring radius when applicable. The maximum distance is 80 scene units. Each drag event is bounded before calculating its zoom factor, preventing a large pointer jump from producing an unbounded value.

### 6.3 Following a selected body

Member 4's keyboard callback sets `selectedBodyIdx`: zero selects the Sun and values one through eight select the planets. `getCameraTarget()` reads that state and computes the planet's current center from the same orbital transformation used by Member 1:

`targetX = orbitRadius * cos(orbitAngleInRadians)`  
`targetY = 0`  
`targetZ = -orbitRadius * sin(orbitAngleInRadians)`

The negative Z sign matches OpenGL's positive rotation about the Y axis. Recalculating this position every frame keeps the camera centered on a moving planet. With no selection or with the Sun selected, the target is the origin.

`applyCamera()` changes the eye position and look-at target while retaining the original elevated viewing angle. It does not change a planet's radius, orbit, spin, or texture coordinates. It also rechecks the minimum distance after a new selection, ensuring that a close view of Mercury does not place the camera inside the larger Sun or Saturn's rings.

### 6.4 Reset and callback registration

The program registers `mouse`, `motion`, and `mouseWheel` using `glutMouseFunc`, `glutMotionFunc`, and `glutMouseWheelFunc`. `display()` calls `applyCamera()` where the original fixed `gluLookAt` call appeared.

`resetSimulation()` restores the original eye position of `(0, 12, 24)` looking toward the origin, clears the selection, and cancels any active zoom drag. It retains Member 1's existing reset behavior of setting orbit and spin angles to zero and returning to a running simulation at 1.0x speed.

## 7 User controls

| Input | Action |
| --- | --- |
| Space | Pause or resume animation |
| R | Reset animation, speed, selection, camera, and drag state |
| 1 | Select and follow the Sun |
| 2–9 | Select and follow Mercury through Neptune in order |
| Left-click | Decrease animation speed by 0.25x |
| Right-click | Increase animation speed by 0.25x |
| Wheel up or down | Zoom in or out |
| Middle-button drag up or down | Alternative zoom control |
| Escape | Exit the application |

## 8 Division of responsibilities

| Member | Assigned responsibilities |
| --- | --- |
| Justine Grace Radoc | Sun and planet drawing, transformations, timer, reset, and display |
| Jilliane Margaret Munsayac | Star VBO, Saturn and Uranus rings, and presentation slides |
| Nj Butaslac | Lighting, texture mapping, debugging, and final code comments |
| Germione Bumatay | Keyboard callback, text overlays, reshape, and Documentation Part II |
| Cypress Angel Ferrer | Mouse and zoom callbacks, video presentation script, and Documentation Part I |

Member 5's integration points are the existing `simSpeed`, `selectedBodyIdx`, and planet orbit data, plus the camera call in `display()` and camera-state reset in `resetSimulation()`. The accompanying integration notes explain these points for Member 1's review and the regression test results for finalization.

## 9 Project references

- Group 8 project specification: [Repository README](../README.md).
- Shared implementation: [G8finalproject.cpp](../G8finalproject.cpp).
- Member 5 verification and remaining group integration: [Integration and testing](Member_5_Integration_and_Testing.md).

Documentation Part II is maintained separately by Member 4 and can follow this section in the group's final submission.
