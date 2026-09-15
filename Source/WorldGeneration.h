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

#include <functional>
#include <vector>

#include <glm/glm.hpp>

namespace chs
{
    struct TerrainSample
    {
        // The current renderer expects normalized values in [0, 1].
        float height{0.0f};
        glm::vec3 color{0.0f};
    };

    class WorldGeneration
    {
    public:
        using TerrainFunction = std::function<TerrainSample(glm::vec2)>;

        // Samples the function at integer grid positions and packs RGB color and
        // height into a GPU-ready RGBA texture (height is stored in alpha).
        [[nodiscard]] std::vector<glm::vec4> generate(const TerrainFunction& terrain_function) const;

        static constexpr unsigned int MAX_WIDTH_VALUE{4096};
        static constexpr unsigned int MAX_HEIGHT_VALUE{4096};

        void setWidth(unsigned int value) { width = value; }
        void setHeight(unsigned int value) { height = value; }
    private:
        unsigned int width{64};
        unsigned int height{64};
    };
}
