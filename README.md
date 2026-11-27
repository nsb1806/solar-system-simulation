# 🌌 Group 8: 3D Solar System Simulation

An interactive 3D Solar System Simulation built in C++ using OpenGL, GLUT, and GLEW.

This project was developed as the final requirement for the **CS0045** course. It features real-time planetary revolution and rotation, texture mapping, dynamic lighting, a background starfield, and interactive camera controls.

---

## 🚀 Features

### Geometric Primitives & Objects

- **Spheres:** The Sun and all eight planets.
- **Torus:** 3D rings for Saturn and Uranus.
- **Points:** A background starfield.
- **Bonus Feature:** A 1,200-particle Asteroid Belt orbiting between Mars and Jupiter.

### Keyboard Callbacks

- **Space:** Pause / Resume the simulation.
- **R:** Reset the simulation (returns planets to their starting positions and resets the camera).
- **1 to 9:** Select the Sun or a specific planet to view its name, nickname, and trigger a self-glow (emission) effect.
  - _1: SUN — "The Nearest Star"_
  - _2: MERCURY — "The Swift Planet"_
  - _3: VENUS — "Earth's Twin"_
  - _4: EARTH — "The Blue Planet"_
  - _5: MARS — "The Red Planet"_
  - _6: JUPITER — "The Gas Giant"_
  - _7: SATURN — "The Ringed Planet"_
  - _8: URANUS — "The Ice Giant"_
  - _9: NEPTUNE — "The Distant Giant"_

### Mouse Callbacks

- **Left-Click:** Slow down the simulation speed.
- **Right-Click:** Speed up the simulation speed.
- **Click & Drag:** Orbit the camera around the solar system.
- **Scroll Wheel:** Zoom the camera in and out.

### 2D Elements

- **Bitmap Text Overlay:** Displays the selected body's name, nickname, simulation status (Paused/Running), and current speed multiplier in the bottom-right corner.

### 3D Elements & Transformations

- **Translation:** Planets revolve around the Sun along their respective orbital paths.
- **Rotation:** Planets spin on their own axes.
- **Scaling:** Planets and the Sun are scaled to their respective sizes.

### Lighting

- **Ambient Light:** General background light so shadowed sides are not pitch black.
- **Diffuse Light:** Simulates sunlight illuminating surfaces based on their angle to the light source.
- **Specular Light:** Adds shiny highlights to the planets.
- **Emission:** Simulates self-glow (applied to the Sun and the currently selected body).

### Texture Mapping

High-resolution, equirectangular image textures are mapped onto the spheres for the Sun and all eight planets, providing realistic surface details.

### VBO (Vertex Buffer Object) Implementation

- **Stars & Asteroids:** To optimize performance, the background stars and the asteroid belt are rendered using VBOs (`glDrawArrays(GL_POINTS)`), ensuring fast GPU rendering without overloading the CPU.

---

## 🛠️ How to Run

1. **Prerequisites:** Ensure you have a C++ compiler installed (e.g., MinGW/GCC or MSVC) along with GLEW, GLUT/FreeGLUT, and OpenGL libraries.
2. **Textures:** Ensure the `textures/` folder containing the `.jpg` images for the Sun and all 8 planets is in the same directory as the executable.
3. **Compile & Run:** Build the `G8finalproject.cpp` file and run the executable.
   _(Note: Make sure your IDE's working directory is set to the project folder so the texture paths load correctly)._

---

## 👥 Group 8 Members

- **Bumatay** - Keyboard Callbacks, 2D Overlay, Resize Logic
- **Butaslac** - Lighting, Texture Mapping, Code Debugging & Finalization
- **Ferrer** - Mouse Controls, Camera System
- **Munsayac** - Star VBOs, Asteroid Belt, Torus Rings, Slides
- **Radoc** - Planets & Sun Drawing, Animation Timer

---

## 📚 Technologies Used

- **C++**
- **OpenGL** (Legacy Pipeline)
- **GLEW** (OpenGL Extension Wrangler)
- **FreeGLUT** (OpenGL Utility Toolkit)
- **stb_image** (Image loading library)
