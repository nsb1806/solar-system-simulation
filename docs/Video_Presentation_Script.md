# Solar System Simulation Video Presentation Script

## Recording setup

Show a title card with the project name and group members. Start the program in its full-system view with the simulation running. The bracketed directions below are actions for the recording, not spoken lines. The five parts can be recorded as separate clips and joined in this order.

The current source still has empty lighting, material, and texture-loading functions. Use the matching Member 3 line for the build you actually record; do not show those effects unless they work in that build.

## Demo script

### Member 1 — Radoc: Scene and animation

[Show the full system moving for a few seconds.]

“Here is our Solar System Simulation, built with C++ and OpenGL. The Sun and eight planets move in a 3D scene. A timer updates each planet’s orbit and spin. The sizes and speeds are adjusted for viewing, so this is not an astronomical scale model.”

### Member 2 — Munsayac: Stars and rings

[Show the star background, then select Saturn with 7 and Uranus with 8.]

“The background contains 1,500 stars stored in a vertex buffer. Saturn and Uranus have tilted, torus-shaped rings.”

### Member 3 — Butaslac: Lighting and textures

[Show the matching result only if it is present in the recorded build.]

**If completed and verified:** “Lighting adds depth to the planets, and textures show surface details on the Sun and planets.”

**If using the current source:** “Lighting and texture mapping are still being integrated, so this build shows the Sun and planets with solid colors.”

### Member 4 — Bumatay: Keyboard controls and overlay

[Press Space to pause. Press 4 to select Earth and show the overlay. Press Space to resume. Resize the window, then press R.]

“Space pauses or resumes the simulation. Press 1 to 9 to select the Sun or a planet; the overlay shows its name and nickname. R resets the animation, and resizing the window keeps the scene proportional.”

### Member 5 — Cypress: Mouse controls and documentation

[Press 4 to select Earth. Left-click once and show 0.75x; right-click once to return to 1.0x. Scroll up, then down. Hold the middle button and drag vertically. Press R to restore the starting view.]

“Left-click slows the animation by 0.25x, and right-click speeds it up. With Earth selected, the mouse wheel zooms in and out, and the camera follows the planet. Middle-button dragging also changes the zoom. I also prepared Part I of our documentation.”

[End card: Group 8 • CS0045 • Thanks for watching]
