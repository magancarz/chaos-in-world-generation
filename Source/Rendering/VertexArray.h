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

#include "Rendering/VertexArrayEntry.h"

namespace chs
{
    class VertexArray
    {
    public:
        explicit VertexArray(
            const std::vector<VertexArrayEntry>& vertex_buffers,
            unsigned int num_of_patches);
        ~VertexArray();

        void bind() const;
        void draw() const;
        void unbind() const;

    private:
        [[nodiscard]] unsigned int createVertexArrayObject() const;
        void bindEachVertexArrayEntry(const std::vector<VertexArrayEntry>& vertex_array_entries) const;
        void bindVertexArrayEntry(unsigned int attribute_index, const VertexArrayEntry& vertex_array_entry) const;

        static constexpr unsigned int INVALID_VERTEX_ARRAY{0};
        unsigned int vertex_array{INVALID_VERTEX_ARRAY};
        unsigned int num_of_array_entries{0};
        unsigned int num_of_patches{0};
    };
}