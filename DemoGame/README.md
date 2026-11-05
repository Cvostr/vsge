# House of Shadows

House of Shadows — playable horror demo built on VSGE. The player explores a sealed manor, recovers five cursed artifacts and escapes through the north door while three supernatural creatures hunt by sight.

## Objective

1. Find all **5 artifacts** hidden in five manor areas: the west bedroom, west study, east library, east cellar and central hall. Their exact positions vary between runs.
2. Return to the **north door** once every artifact is recovered.
3. Avoid or fight the three creatures. They cannot see through walls, but they pursue if they see you.

## Controls

- **WASD** — move
- **Mouse** — look / aim flashlight
- **Shift** — sprint (uses stamina)
- **Space / Left mouse button** — attack
- **Esc** — pause
- **R** — restart after winning, losing or pausing
- **Enter** — start from the menu
- On the win/loss screen the cursor is released; click or press **R** to restart. Gameplay HUD and objectives are hidden until play resumes.

## Features

- 3D third-person horror built with the VSGE renderer
- Deferred PBR lighting, cascaded shadow support, bloom and SSAO
- Procedural textures and materials (brick, wood, plaster, carpet, scarred mutant hide, relics, night sky)
- Dynamic flashlight spotlight with shadow casting
- Three distinct procedural mutant silhouettes: a tall, narrow stalker with a parasite growth; a broad, plated brute; and a low, segmented, six-legged crawler. Their enlarged, forward-facing skulls have visible deep-set glowing eyes, heavy brows, gaping jaws and orderly rows of teeth; archetype and minor proportions vary by run without losing each creature's defining shape.
- Replaceable OGG sound effects and music from category folders under `sounds/`, with synthesized fallback sounds when no valid custom file is present
- Living monster animation: breathing and pulsing bodies, searching head movements, and independently moving limbs
- Randomized monster spawn points and artifact locations within their respective manor areas
- Six randomized supply pickups per run when safe spawn locations are available: three blood tonics restore health and three salt pouches restore stamina. Supplies avoid artifact locations and are restored on restart.
- Blood smears, claw marks, swaying hanging roots, a pulsing wall growth, and differently colored flickering torches for a more oppressive manor atmosphere
- Physical manor collision and character-controller player movement
- Monster collision capsules are kept narrow for confined rooms; melee hits test the actual body, head, growths, and limbs instead of a broad point target
- Attacks deal 32 damage within 2.45 m when the aim ray intersects the monster's visible hit regions and is not blocked by a wall; target HP and hit damage are shown in the HUD
- HUD with objective, health, stamina, artifact count and contextual warnings
- Menu, pause, win and loss states
- Player and hunters are returned to their last safe floor positions if they fall below the manor or leave its bounds; restarting restores the player and camera to the spawn point.

## Launch

The game is built as the **DemoGame** target.

### Windows

From a Visual Studio developer prompt:

```bat
build.bat Release
```

The script creates `out\release\HouseOfShadows-win64.zip`. Share that archive;
after extracting it, run `DemoGame.exe` from the extracted folder.

Or configure and build with CMake:

```bat
cmake -S . -B out\build\x64-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out\build\x64-Debug
out\build\x64-Debug\DemoGame\DemoGame.exe
```

### Linux

```bash
cmake -S . -B out/build/linux-Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build/linux-Debug --target DemoGame --parallel
./out/build/linux-Debug/DemoGame/DemoGame
```

The Linux build uses the prebuilt libraries under `vendor/` and requires `mono` plus the usual Vulkan runtime and development libraries. If your checkout is missing `vendor/static-x64/linux/glslang/libMachineIndependent.a`, extract it locally from the included `libMachineIndependent.a.zip` before building.

## Notes

- Put `.ogg` files directly into `sounds/steps`, `sounds/attack`, `sounds/monsters_attack`, `sounds/monsters`, `sounds/ambient`, `sounds/pickups`, `sounds/scare`, `sounds/doors`, `sounds/ui`, or `sounds/ending`. Multiple sound effects in a category are picked randomly. `ambient` and `ending` select one track per run and loop it.
- If an effects folder has no valid OGG, its built-in procedural sound remains in use. Background music falls back to `sounds/duhovoy-vopl--latunnaya-pechal.ogg`; if that file is also absent or invalid, the game runs without background music. Ending music falls back to the built-in synthesized track.
- CMake and `build.bat Release` copy/package the entire `sounds` folder beside `DemoGame.exe`, preserving category folders.
- No mouse-look calibration is needed; relative mouse mode is enabled automatically.
- If running through a remote session without OpenGL/Vulkan support, the window may fail to open; run on a graphics-capable machine.
