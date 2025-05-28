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

#include "Rendering/VertexArray.h"

#include <cassert>

#include <GL/glew.h>

#include "Rendering/DebugUtils.h"

namespace chs
{
    VertexArray::VertexArray(
            const std::vector<VertexArrayEntry>& vertex_array_entries,
            unsigned int num_of_patches)
        : vertex_array{createVertexArrayObject()},
        num_of_array_entries{static_cast<unsigned int>(vertex_array_entries.size())},
        num_of_patches{num_of_patches}
    {
        bindEachVertexArrayEntry(vertex_array_entries);
    }

    unsigned int VertexArray::createVertexArrayObject() const
    {
        unsigned int id;
        GL_CHECK(glGenVertexArrays(1, &id));

        return id;
    }

    void VertexArray::bindEachVertexArrayEntry(const std::vector<VertexArrayEntry>& vertex_array_entries) const
    {
        GL_CHECK(glBindVertexArray(vertex_array));
        for (unsigned int index = 0; index < static_cast<unsigned int>(vertex_array_entries.size()); ++index)
        {
            bindVertexArrayEntry(index, vertex_array_entries.at(index));
        }
        GL_CHECK(glBindVertexArray(INVALID_VERTEX_ARRAY));
    }

    void VertexArray::bindVertexArrayEntry(unsigned int attribute_index, const VertexArrayEntry& vertex_array_entry) const
    {
        static constexpr bool NOT_NORMALIZED{false};
        vertex_array_entry.source_buffer->bind();
        GL_CHECK(glVertexAttribPointer(attribute_index, vertex_array_entry.attribute_size, GL_FLOAT, NOT_NORMALIZED, vertex_array_entry.instance_stride, (void*)vertex_array_entry.instance_offset));
        vertex_array_entry.source_buffer->unbind();
    }

    VertexArray::~VertexArray()
    {
        GL_CHECK(glDeleteVertexArrays(1, &vertex_array));
    }

    void VertexArray::bind() const
    {
        GL_CHECK(glBindVertexArray(vertex_array));
        for (unsigned int attribute_index = 0; attribute_index < num_of_array_entries; ++attribute_index)
        {
            GL_CHECK(glEnableVertexAttribArray(attribute_index));
        }
    }

    void VertexArray::draw() const
    {
        assert(num_of_patches > 0 && "Sanity check");
        glDrawArrays(GL_PATCHES, 0, static_cast<int>(num_of_patches));
    }

    void VertexArray::unbind() const
    {
        for (unsigned int attribute_index = 0; attribute_index < num_of_array_entries; ++attribute_index)
        {
            GL_CHECK(glDisableVertexAttribArray(attribute_index));
        }
        GL_CHECK(glBindVertexArray(INVALID_VERTEX_ARRAY));
    }
}