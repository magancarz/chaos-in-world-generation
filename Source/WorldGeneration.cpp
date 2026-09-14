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

#include <stdexcept>

namespace chs
{
    std::vector<glm::vec4> WorldGeneration::generate(const TerrainFunction& terrain_function) const
    {
        if (width == 0 || width > MAX_WIDTH_VALUE || height == 0 || height > MAX_HEIGHT_VALUE)
        {
            throw std::invalid_argument{"Terrain dimensions are outside the supported range"};
        }
        if (!terrain_function)
        {
            throw std::invalid_argument{"A terrain function is required"};
        }

        unsigned int index = 0;
        std::vector<glm::vec4> values(width * height);
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                const TerrainSample sample = terrain_function(glm::vec2{x, y});
                values[index] = glm::vec4{sample.color, sample.height};
                index += 1;
            }
        }

        return values;
    }
}
