# Chaos in World Generation

An interactive OpenGL sandbox for testing terrain-generation algorithms. The
current stage samples a user-provided C++ function into a floating-point terrain
texture and renders the result with adaptive GPU tessellation.

## Terrain function API

A terrain function receives an integer grid position as a `glm::vec2` and
returns normalized height and RGB color values:

```cpp
auto terrain_data = world_generation.generate([](glm::vec2 point)
{
    const float height = myHeightAlgorithm(point.x, point.y);
    const glm::vec3 color = myColorAlgorithm(point, height);
    return chs::TerrainSample{height, color};
});
```

The returned data is packed as RGB + height and uploaded as an `RGBA32F`
texture. The tessellation evaluation shader reads alpha for displacement and
passes RGB to the fragment shader. `Application/main.cpp` contains a complete
FastNoiseLite example with an editable height curve.

The settings window provides seed, octave, resolution, height-scale, and
tessellation controls; solid, wireframe, height-only, and color-only views; generation
timing and triangle statistics; automatic debounced or manual regeneration; and
PNG export to `Exports/terrain_color.png` and `Exports/terrain_height.png`.

## Build

Use Bazelisk (which selects Bazel 7.7.1 from `.bazelversion`) and a C++23
compiler with standard-library support for `<format>` (for example GCC 13+).
The application currently builds on Linux using GLFW's X11 backend; Wayland
sessions need XWayland. Running it requires OpenGL 4.6.

Install the graphics development dependencies on Ubuntu/Debian:

```sh
sudo apt install build-essential libglew-dev libgl1-mesa-dev libx11-dev \
    libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev
```

Initialize the pinned source dependencies and build/run the application:

```sh
git submodule update --init --recursive
bazel build //:chaos-in-world-generation
bazel run //:chaos-in-world-generation
```

GLFW, GLM, FastNoiseLite, ImGui, and stb use the sources in `ThirdParty`.
GLEW and OpenGL use system libraries. Bazel downloads its pinned build rules
on the first build. Shaders are included in the application's Bazel runfiles.
PNG exports from `bazel run` are written to the checkout's `Exports/` directory.
To launch the binary directly, run `./bazel-bin/chaos-in-world-generation`
from the repository root so it can find `Shaders/`.

Core tests require only the compiler and initialized source dependencies,
without OpenGL, GLEW, or X11 development packages:

```sh
bazel test //:terrain-core-tests
```

Build everything with `bazel build //...`. Add `-c opt` for an optimized build
or `-c dbg` for debug symbols; test assertions remain enabled in either mode.
