# Solar System Simulation Video Presentation Script

**Course:** CS0045  
**Group:** 8  
**Script prepared by:** Cypress Angel O. Ferrer  
**Suggested duration:** 6–8 minutes, including demonstrations

## Recording preparation

Use the final reviewed group build. Finish the missing image-loader header, texture files, lighting, and material functions before recording a demonstration of those features. The lighting section below includes separate wording for a completed build and a progress presentation; use only the wording that matches the actual program.

Open the application, press R, and begin with the complete scene visible. Capture the application and briefly show relevant code when explaining implementation. The bracketed directions are production cues, not spoken lines. Coordinate slide titles with Member 2. The duration is a suggested pacing target because the repository does not specify a required video length.

## 1 Introduction by Cypress

**Suggested time:** 0:00–0:40

[Show the project title and group members, then the running simulation.]

“Good day! We are Group 8, and this is our Solar System Simulation for CS0045. Our members are Justine Grace Radoc, Jilliane Margaret Munsayac, Nj Butaslac, Germione Bumatay, and Cypress Angel Ferrer.

Our project uses C++ and OpenGL to display the Sun and eight planets in an interactive three-dimensional scene. We combine geometric objects, transformations, animation, and user input. The sizes and speeds are adjusted for demonstration, so the simulation is not an exact astronomical model.

First, Justine will explain how the objects are drawn and animated.”

## 2 Objects and animation by Radoc

**Suggested time:** 0:40–1:45

[Show `drawSun()`, `drawPlanets()`, and `timer()`, then return to the scene.]

“The Sun and planets are drawn as spheres. Each planet stores its own radius, orbital distance, orbit angle, spin angle, and movement speeds in a Planet structure.

For each planet, we first rotate around the Y axis to establish its orbital position. We then translate it away from the Sun using its orbit radius. Another rotation gives the planet its axial spin. We save and restore the model matrix so these transformations do not affect other planets.

Our timer requests another update approximately every 16 milliseconds. When the simulation is running, it updates orbit and spin angles using each object's speed and the shared simulation-speed multiplier. The display callback then draws the updated scene.

Reset returns the angles to zero and restores the normal simulation settings. Mouse speed controls use the same multiplier, so they remain connected to this animation logic.”

[Demonstrate visible orbital motion. Do not rely on axial spin being visible on an untextured, uniformly colored sphere.]

## 3 Stars and rings by Munsayac

**Suggested time:** 1:45–2:35

[Show the star background, then select Saturn with 7 and Uranus with 8. Zoom as needed.]

“The background contains 1,500 stars drawn as points. Each star has three position values and three color values. We place this data in a vertex buffer object and draw the stars together using glDrawArrays.

Saturn and Uranus use torus objects for their rings. We calculate each torus's main radius and tube thickness from the stored inner and outer ring radii. We also apply different tilts and flatten the rings with scaling.

The rings follow their planets' orbital positions, while their orientation is handled separately. This gives both planets a recognizable appearance within the scene.”

## 4 Lighting and textures by Butaslac

**Suggested time:** 2:35–3:25

[Before recording, check the final source and demonstrate only the features actually implemented.]

**Use after the group has implemented and verified lighting and textures:**

“Our lighting combines ambient, diffuse, specular, and emissive components. Ambient light keeps shadowed areas visible. Diffuse lighting shows how surfaces face the light source, while specular lighting produces highlights. Emission gives an object a self-lit appearance.

Texture mapping adds image details to the Sun and planets. These details help distinguish the bodies and make their axial rotation easier to observe. We also check that render states are restored correctly when switching between planets, stars, rings, and the text overlay.”

[Show the working lighting and texture result. Adjust this explanation if the final implementation differs.]

**Use instead if presenting the current incomplete group build:**

“The project plan includes lighting and image texture mapping, but these functions are still being integrated in this build. The current demonstration uses colored spheres. We will complete the texture loader, supply the images, and verify the lighting before the final presentation.”

## 5 Keyboard controls and overlays by Bumatay

**Suggested time:** 3:25–4:15

[Press R, pause with Space, resume, and select Earth with 4. Show the bottom-right text.]

“The keyboard callback connects key presses to simulation controls. Space pauses or resumes the animation. R resets it, and keys 1 through 9 select the Sun and the eight planets in order.

The text overlay displays the selected body's name and nickname, together with the simulation status and current speed. For example, selecting Earth displays The Blue Planet.

The reshape callback updates the viewport and projection aspect ratio when the window changes size. This prevents the 3D scene from stretching. The overlay uses a separate two-dimensional projection so its placement is based on screen coordinates.”

[Resize the window. Mention selection glow only if Member 3 has implemented and verified it.]

## 6 Mouse controls and camera by Cypress

**Suggested time:** 4:15–5:55

[Press R. Left-click twice to show 0.5x; right-click twice to return to 1.0x.]

“My programming contribution is the mouse interaction. A left-click slows the simulation by 0.25x, while a right-click increases its speed by the same amount. The speed is limited between 0.25x and 5.0x.

These controls update the same simSpeed variable used by the timer. That means the planets' orbits, their spins, and the Sun's spin use the selected speed consistently.”

[Pause with Space, right-click once, and show that the objects remain paused. Resume.]

“Changing the speed does not automatically resume a paused simulation. We can choose a speed while paused and then continue using Space.”

[Select Earth with 4. Scroll up to zoom in, then scroll down.]

“For a closer inspection, select a body using the existing number keys, then use the mouse wheel to zoom in or out. The camera follows the selected planet as it moves. It calculates the planet's center from the same orbit radius and angle used by the drawing function.

This changes the camera's position and viewing target. It does not enlarge the planet or change its orbit.”

[Hold the middle button and drag up, then down. Select Saturn with 7 and demonstrate the closest permitted view.]

“Middle-button dragging provides another way to zoom. Dragging upward moves closer, and dragging downward moves away. The minimum camera distance accounts for the selected body's size, including rings, while the maximum distance limits how far we can move away.”

[Press R to restore the original view.]

“Finally, reset restores the camera distance, cancels dragging, clears the selection, and returns the simulation to its initial settings. This connects my mouse callbacks with the existing display and reset functions.”

## 7 Validation and closing by Cypress

**Suggested time:** 5:55–6:35

[Show the automated test result, followed by the full-system view.]

“We checked the mouse callbacks for speed limits, button-release behavior, pause and timer integration, planet tracking, zoom limits, and reset behavior. The automated checks exercise the actual callbacks without opening a graphics window. Visual rendering and textures also need to be checked in the final integrated application.

This project shows how transformations, animation, geometry, and input callbacks work together in an interactive scene. I also prepared Documentation Part I and this presentation script, while each group member contributed their assigned implementation and documentation tasks.

Thank you for watching our Solar System Simulation.”

## Short Cypress only recording option

Use this if each member records their contribution separately. Allow about 90 seconds plus demonstrations.

“Hi, I'm Cypress Angel Ferrer, and my part covers the mouse callbacks, the video presentation script, and Documentation Part I.

In the application, left-click decreases the animation speed and right-click increases it. Each click changes the speed by 0.25x, within a range of 0.25x to 5.0x. The mouse callback updates the same speed variable used by the animation timer, and changing speed does not unpause the simulation.

I also added camera zoom. After selecting a body with keys 1 to 9, we can scroll to zoom in and out. Holding the middle mouse button and dragging vertically provides an alternative. The camera follows the selected planet by calculating its current center from its orbit angle and radius.

The camera has distance limits that account for the selected body's size and rings. Pressing R restores the original camera view, clears the selection, and cancels dragging along with the existing simulation reset.

For documentation, Part I explains the project's background, objectives, scope, structure, and mouse implementation. I also wrote the group's presentation script with demonstration cues for each member. Thank you.”
