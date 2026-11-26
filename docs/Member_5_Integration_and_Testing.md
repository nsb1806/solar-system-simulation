# Member 5 Integration and Testing

## Review notes for Member 1

This contribution uses the `main` source downloaded on November 26, 2026, at commit `1e7c325f55d21a13140990958a148ca6e0cf01ef` as its baseline.

- `mouse()` updates the existing `simSpeed`; `timer()` is unchanged.
- `display()` replaces only the fixed `gluLookAt` call with `applyCamera()`.
- `resetSimulation()` additionally resets `cameraDistance`, `isZoomDragging`, and `lastMouseY`.
- `getCameraTarget()` derives the current world position from the same Y rotation and X translation used by `drawPlanets()`. If that transform changes, update the target calculation with it.
- Selecting a body now centers the camera on it. Keyboard mappings are unchanged. The speed overlay uses two decimal places to display 0.25x steps accurately. R clears selection and restores the original view.
- FreeGLUT's motion and wheel callbacks are registered in `main()`. No second wheel implementation is added to `mouse()`, preventing duplicate zoom events.

These notes are the coordination handoff for review; they do not claim a separate discussion or approval from another member.

## Automated regression checks

`tests/mouse_callbacks_test.cpp` includes the actual application source and intercepts only redraw scheduling, timer scheduling, and `gluLookAt` so callbacks can run without a graphics window. It uses the installed OpenGL, GLEW, FreeGLUT, and GLU headers and libraries.

On macOS with the dependencies in the locations used for this check, run from the repository root:

```sh
clang++ -std=c++11 -I/opt/homebrew/include -Itests/support \
  tests/mouse_callbacks_test.cpp \
  -L/opt/homebrew/lib -L/opt/X11/lib -lGLEW -lglut -lGL -lGLU \
  -o /tmp/member5-mouse-test
/tmp/member5-mouse-test
```

On a Linux development machine with the equivalent development packages installed:

```sh
g++ -std=c++11 -Itests/support tests/mouse_callbacks_test.cpp \
  -lGLEW -lglut -lGL -lGLU -o /tmp/member5-mouse-test
/tmp/member5-mouse-test
```

The Linux command is provided for portability and was not run in this environment.

**Observed result on macOS:** PASS for speed changes and bounds, release events, pause/timer interaction, default camera coordinates, all eight planet target mappings, wheel direction and limits, ring-aware selection changes, inactive and active drag behavior, large pointer movement, ignored inputs, and reset. The same checks also passed with `-fsanitize=address,undefined -fno-omit-frame-pointer`.

The source currently includes `stbi_image.h`, which is absent from the baseline. The test-only header in `tests/support` allows this callback test to compile while the texture functions remain empty. It deliberately requires `MEMBER5_CALLBACK_TEST`; it is not an image loader and must never be included in an application build. Remove it once Member 3 supplies the real loader. The test does not call the unfinished texture functions.

## Existing group integration blockers

- `stbi_image.h` and the `textures/` image files are missing.
- `setLighting()`, `setPlanetMaterial()`, and `loadTexture()` are empty. The non-void texture loader produces a compiler warning because it has no return statement.
- The current drawing functions do not yet bind textures or apply selected-body emission.
- `drawRings()` passes a three-element color array to `glMaterialfv(GL_AMBIENT_AND_DIFFUSE)`, which expects four values. Member 3 should include an alpha component during finalization.
- macOS reports deprecation warnings for the existing GLU projection calls.

The callback test passes, but it does not verify a rendered frame or prove the whole application is ready. The complete application remains dependent on the above group work. This branch does not implement another member's lighting, textures, or finalization assignment.

## Visual checks for the final integrated build

These are pending manual checks, not claimed test results:

1. Start the application and confirm its original view and 1.0x speed.
2. Left-click to the 0.25x lower limit and right-click to the 5.0x upper limit; verify the displayed speed and visible motion.
3. Pause, change speed, verify no motion, then resume.
4. Select each body with 1–9, zoom in and out, and verify that the camera follows its center during animation.
5. Zoom close to Mercury, switch to the Sun and Saturn, and verify the camera remains outside each body and its rings.
6. Try middle-button dragging, release it, and confirm ordinary pointer movement no longer changes the view.
7. Resize the window while zoomed and verify the projection remains usable.
8. Reset while dragging and verify speed, selection, view, and animation return to their initial settings.
9. Confirm the completed textures, lighting, selection glow, ring materials, and overlays still render correctly.

## Document handoff

`Part_I_Cypress.md` contains the first part of the project documentation. `Video_Presentation_Script.md` contains the full group script and a shorter Cypress-only recording option. The repository has no instructor-provided Part I/II outline; check the headings against any separate class template before combining them with Member 4's Part II.
