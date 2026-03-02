# M3DS - A 3DS Game Development Framework

M3DS is a library created to simplify the 3DS game development process. It provides simple utilities for:

- Loading, rendering, and animation of 3D models through a custom `.mod3ds` format.
- Loading and playback of WAV audio files (with some sample rate requirements).
- Creating customisable User Interfaces.
- Obtaining input from the user.
- Safely and efficiently reading and writing of binary files.
- Serialisation and Deserialisation to binary files.
- Creating custom animations based off any reasonable public fields on a `Node` object.
- Simple script interface for trivial extensibility.
- Complicated Node inheritance for complex extensibility.

M3DS provides a Godot-like Node system, making organising projects simple. Additionally, M3DS has its own simple physics engine, SPhys, designed primarily for Kinematic Bodies rather than Rigid Bodies.

M3DS does not use exceptions or runtime type information, however it can be recompiled with the former enabled to support catching `std::bad_alloc`.

## Build Instructions
1. Ensure `libctru` is installed and up to date.
    - On Windows, use the devkitPro Updater at https://github.com/devkitPro/installer/releases
    - On Linux or macOS, follow the instructions at https://devkitpro.org/wiki/Getting_Started
2. Open a terminal in the `m3ds` folder.
3. Run `make`. To install system-wide, run `make install` instead.

The final `libm3ds.a` will be found in the `lib` directory.

## Example: Hello World
```c++
#include <m3ds/M3DS.hpp>

int main() {
    // Initialises systems, sets bottom screen as console (configurable)
    M3DS::Init _ {};

    // Supports formatting through std::format
    M3DS::Debug::log("Hello, {}!", "World");

    // Create a root node and begin main loop (so program doesn't exit immediately)
    // Press START to exit in default mainLoop.
    M3DS::Root root {};
    root.mainLoop();
}
```
Compile under regular libctru rules, linking against M3DS with `-lm3ds` and Citro3D with `-lcitro3d`, using C++26 with `-std=c++26`.

A repository with example projects can be found [here](https://github.com/HyperDir/M3DS-Examples).
