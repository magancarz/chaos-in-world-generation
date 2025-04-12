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
            std::vector<VertexBuffer> vertex_buffers,
            unsigned int num_of_vertices)
        : vertex_array{createVertexArrayObject()},
        vertex_buffers{std::move(vertex_buffers)},
        num_of_vertices{num_of_vertices}
    {
        bindEachVertexArrayEntry(this->vertex_buffers);
    }

    unsigned int VertexArray::createVertexArrayObject() const
    {
        unsigned int id;
        GL_CHECK(glGenVertexArrays(1, &id));

        return id;
    }

    void VertexArray::bindEachVertexArrayEntry(const std::vector<VertexBuffer>& vertex_buffers_to_bind) const
    {
        GL_CHECK(glBindVertexArray(vertex_array));
        for (unsigned int index = 0; index < static_cast<unsigned int>(vertex_buffers_to_bind.size()); ++index)
        {
            bindVertexArrayEntry(index, vertex_buffers_to_bind.at(index));
        }
        GL_CHECK(glBindVertexArray(INVALID_VERTEX_ARRAY));
    }

    void VertexArray::bindVertexArrayEntry(unsigned int attribute_index, const VertexBuffer& vertex_buffer) const
    {
        static constexpr bool NOT_NORMALIZED{false};
        static constexpr int STRIDE{0};
        static constexpr const void* const FIRST_ELEMENT_OFFSET{nullptr};
        vertex_buffer.bind();
        GL_CHECK(glVertexAttribPointer(attribute_index, vertex_buffer.getAttributeSize(), GL_FLOAT, NOT_NORMALIZED, STRIDE, FIRST_ELEMENT_OFFSET));
        vertex_buffer.unbind();
    }

    VertexArray::~VertexArray()
    {
        GL_CHECK(glDeleteVertexArrays(1, &vertex_array));
    }

    void VertexArray::bind() const
    {
        assert(vertex_buffers.size() > 0 && "Something went wrong");
        GL_CHECK(glBindVertexArray(vertex_array));
        for (unsigned int attribute_index = 0; attribute_index < vertex_buffers.size(); ++attribute_index)
        {
            GL_CHECK(glEnableVertexAttribArray(attribute_index));
        }
    }

    void VertexArray::draw() const
    {
        assert(num_of_vertices > 0 && "Sanity check");
        glDrawArrays(GL_TRIANGLES, 0, num_of_vertices);
    }

    void VertexArray::unbind() const
    {
        assert(vertex_buffers.size() > 0 && "Something went wrong");
        for (unsigned int attribute_index = 0; attribute_index < vertex_buffers.size(); ++attribute_index)
        {
            GL_CHECK(glDisableVertexAttribArray(attribute_index));
        }
        GL_CHECK(glBindVertexArray(INVALID_VERTEX_ARRAY));
    }
}