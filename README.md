# G8 CS0045 Final Project: Solar System Simulation

An interactive 3D Solar System Simulation built in C++ using OpenGL

---

## Features

### GEOMETRIC PRIMITIVES/OBJECTS:

- Spheres (Sun and Planets)
- Torus (Saturn and Uranus Rings)
- Points (Stars)
- Asteroid Belt \*Additional (if possible)

### KEYBOARD CALLBACK:

- Press Space to pause/resume the simulation.
- Press "r" to reset the simulation to its initial state.
- Press 1-9 to view names of Sun and Planets and their nicknames and make them glow to stand out.

  _[1 == SUN; "The Nearest Star", 2 == MERCURY; "The Swift Planet", 3 == VENUS; "Earth's Twin", 4 == EARTH; "The Blue Planet", 5 == MARS; "The Red Planet", 6 == JUPITER; "The Gas Giant", 7 == SATURN; "The Ringed Planet", 8 == URANUS; "The Ice Giant", 9 == NEPTUNE; "The Distant Giant"]_

### MOUSE CALLBACK:

- Click Left Button to slow down the simulation speed.
- Click Right Button to speed up the simulation speed.

### 2D ELEMENTS:

- Bitmap Text (Names and Nicknames of Sun and Planets) on Bottom Right corner of the screen.

### 3D ELEMENTS:

- Spheres (Sun and Planets)
- Torus (Saturn and Uranus Rings)
- Points (Stars) on the background of the simulation.

### TRANSLATION, ROTATION, SCALING:

- **Translation:** Planets revolve around the Sun in their respective orbits.
- **Rotation:** Planets spin on their own axes.
- **Scaling:** Planets are scaled to their respective sizes.

### LIGHTING:

- **Ambient Light:** Simulates the general background light in the scene so the shadowed sides are not pitch black.
- **Diffuse Light:** Simulates light coming from a light source (Sun) that illuminates surfaces based on their angle to the light.
- **Specular Light:** Simulates the shiny highlights on surfaces in the scene.
- **Emission:** Simulates the light given by an object itself (such as the Sun's self-glow and the selected body during keyboard callback), independent of external light sources.

### TEXTURE MAPPING (image):

- Sun: Sun texture image
- Mercury: Mercury texture image
- Venus: Venus texture image
- Earth: Earth texture image
- Mars: Mars texture image
- Jupiter: Jupiter texture image
- Saturn: Saturn texture image
- Uranus: Uranus texture image
- Neptune: Neptune texture image

### VBO IMPLEMENTATION:

- Points (Stars)
