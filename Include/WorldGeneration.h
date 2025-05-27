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

#include <vector>

#include "FastNoiseLite/Cpp/FastNoiseLite.h"
#include <glm/glm.hpp>

#include "WorldGeneration/WorldGenerationSettings.h"

namespace chs
{
    class WorldGeneration
    {
    public:
        WorldGeneration();

        std::vector<glm::vec4> generate(const WorldGenerationSettings& world_generation_settings);

        static constexpr unsigned int MAX_WIDTH_VALUE{4096};
        static constexpr unsigned int MAX_HEIGHT_VALUE{4096};

        void setWidth(unsigned int value) { width = value; }
        void setHeight(unsigned int value) { height = value; }
        void setSeed(unsigned int value) { seed = value; }

        void setOctaves(unsigned int value) { noise.SetFractalOctaves(value); }

    private:
        FastNoiseLite noise;

        unsigned int width{64};
        unsigned int height{64};
        unsigned int seed{2025};
    };
}