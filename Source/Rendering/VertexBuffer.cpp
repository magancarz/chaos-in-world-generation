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

#include "Rendering/VertexBuffer.h"

#include <cassert>

#include <GL/glew.h>

#include "Rendering/DebugUtils.h"

namespace chs
{
    VertexBuffer::VertexBuffer()
    {
        GL_CHECK(glGenBuffers(1, &vertex_buffer));
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : vertex_buffer{other.vertex_buffer},
        buffer_size{other.buffer_size}
    {
        other.vertex_buffer = 0;
        other.buffer_size = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            deleteVertexBufferObjectIfNeeded();

            vertex_buffer = other.vertex_buffer;
            buffer_size = other.buffer_size;

            other.vertex_buffer = 0;
            other.buffer_size = 0;
        }

        return *this;
    }

    void VertexBuffer::deleteVertexBufferObjectIfNeeded()
    {
        if (glIsBuffer(vertex_buffer))
        {
            GL_CHECK(glDeleteBuffers(1, &vertex_buffer));
        }
        vertex_buffer = 0;
    }

    VertexBuffer::~VertexBuffer()
    {
        deleteVertexBufferObjectIfNeeded();
    }

    void VertexBuffer::bindData(int size, void* data)
    {
        buffer_size = size;
        bind();
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
        unbind();
    }

    void VertexBuffer::bind() const
    {
        assert(valid());
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
    }
    
    void VertexBuffer::unbind() const
    {
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, INVALID_VERTEX_BUFFER));
    }
}