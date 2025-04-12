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

#include <limits>

namespace chs
{
    class VertexBuffer
    {
    public:
        VertexBuffer(int instance_count, int instance_size, int attribute_size);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;

        void bindData(void* data) const;

        void bind() const;
        void unbind() const;

        int getInstanceCount() const { return instance_count; }
        int getInstanceSize() const { return instance_size; }
        int getAttributeSize() const { return attribute_size; }

        bool valid() const
        {
            return vertex_buffer != INVALID_VERTEX_BUFFER &&
                instance_count != 0 &&
                instance_size != 0 &&
                attribute_size != 0;}

    private:
        static constexpr unsigned int INVALID_VERTEX_BUFFER{0};

        void deleteVertexBufferObjectIfNeeded();

        unsigned int vertex_buffer{INVALID_VERTEX_BUFFER};

        int instance_count{0};
        int instance_size{0};
        int attribute_size{0};
    };
}