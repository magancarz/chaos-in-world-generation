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

#include "WorldGeneration.h"

#include <cassert>
#include <iostream>

#include "WorldGeneration/CoefficientsCalculator.h"
#include "WorldGeneration/GLSLCodeGenerator.h"
#include "WorldGeneration/MappingInterval.h"
#include "WorldGeneration/NoiseMappingFunction.h"

namespace chs
{
    WorldGeneration::WorldGeneration()
    {
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        noise.SetFractalOctaves(8);
    }

    std::vector<glm::vec4> WorldGeneration::generate(const WorldGenerationSettings& world_generation_settings)
    {
        assert(width > 0 && width <= MAX_WIDTH_VALUE && height > 0 && height <= MAX_HEIGHT_VALUE);

        unsigned int index = 0;
        std::vector<glm::vec4> values(width * height);
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                float noise_value = noise.GetNoise(
                    world_generation_settings.x_coordinate_offset + static_cast<float>(x),
                    world_generation_settings.y_coordinate_offset + static_cast<float>(y));
                noise_value = (noise_value * 0.5f) + 0.5f;
                values.at(index) = glm::vec4{noise_value, noise_value, noise_value, 255.0f};
                index += 1;
            }
        }

        return values;
    }
}
