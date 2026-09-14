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

The project requires CMake 3.26+, a C++23 compiler, OpenGL 4.6, and GLEW.

```sh
git submodule update --init --recursive
cmake -S . -B build
cmake --build build
./build/chaos-in-world-generation
```

Core tests do not require OpenGL or GLEW:

```sh
cmake -S . -B build-tests -DCIWG_BUILD_APP=OFF
cmake --build build-tests
ctest --test-dir build-tests
```
