// MIT License

// Copyright (c) 2025 Mateusz Gancarz

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <cstdint>

namespace chs
{
    inline constexpr std::uint32_t TERRAIN_MANAGED_ABI_VERSION = 1;

    using TerrainNoise2DFunction = float (*)(void* context, float x, float y);

    struct TerrainGenerationRequestC
    {
        std::uint32_t abi_version{TERRAIN_MANAGED_ABI_VERSION};
        std::uint32_t struct_size{0};
        std::uint32_t width{0};
        std::uint32_t height{0};
        std::int32_t seed{0};
        std::int32_t octaves{0};
        float x_offset{0.0f};
        float y_offset{0.0f};
        void* noise_context{nullptr};
        TerrainNoise2DFunction noise_2d{nullptr};
    };

    struct TerrainSampleC
    {
        float red{0.0f};
        float green{0.0f};
        float blue{0.0f};
        float height{0.0f};
    };
}
