# VSGE

VSGE is a C++17 Vulkan game engine with an ImGui editor and C# scripting through Mono. It includes a renderer with deferred PBR lighting, shadows, IBL, bloom/SSAO, Bullet physics, OpenAL audio, resource management and a scene/entity component model.

## Playable game: House of Shadows

The repository contains **House of Shadows**, a playable 3D horror game built on VSGE.

- Explore a sealed manor from a third-person camera.
- Recover five cursed artifacts hidden in the house.
- Survive three supernatural hunters with line-of-sight AI.
- Escape through the north door once the artifacts are recovered.

See [`DemoGame/README.md`](DemoGame/README.md) for controls, launch instructions and implementation notes.

## Build

Windows:

```bat
build.bat DemoGame
out\build\x64-Debug\DemoGame\DemoGame.exe
```

Run `build.bat` without an argument to build all targets.
Run `build.bat Release` to build the Windows release and create
`out\release\HouseOfShadows-win64.zip` for distribution.

Linux:

```bash
cmake -S . -B out/build/linux-Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build/linux-Debug --target DemoGame --parallel
./out/build/linux-Debug/DemoGame/DemoGame
```

Linux users may need to extract the included `vendor/static-x64/linux/glslang/libMachineIndependent.a.zip` locally before building.

## Editor

The editor targets are also available in `Editor/`. The playable demo is built directly as the `DemoGame` target and does not require external assets.

## Layout

- `Engine/` — core engine, rendering, physics, audio, resources and scene system
- `Editor/` — ImGui-based editor
- `EngineApplication/` — runtime entrypoint for packaged projects
- `DemoGame/` — House of Shadows playable game
- `Shaders/` — GLSL shaders and shader bundle builder
- `MonoScripting/` — C# scripting API sources
- `vendor/` — prebuilt dependencies
