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

#include "Rendering/Texture.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Rendering/DebugUtils.h"

namespace chs
{
    Texture::Texture()
        : texture{createTextureObject()} {}

    unsigned int Texture::createTextureObject() const
    {
        unsigned int id{INVALID_TEXTURE};
        GL_CHECK(glGenTextures(1, &id));

        return id;
    }

    Texture::Texture(Texture&& other) noexcept
        : texture{other.texture}
    {
        other.texture = INVALID_TEXTURE;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        deleteTextureObjectIfNeeded();

        texture = other.texture;
        other.texture = INVALID_TEXTURE;

        return *this;
    }

    void Texture::deleteTextureObjectIfNeeded()
    {
        if (texture != INVALID_TEXTURE)
        {
            GL_CHECK(glDeleteTextures(1, &texture));
            texture = INVALID_TEXTURE;
        }
    }

    Texture::~Texture()
    {
        deleteTextureObjectIfNeeded();
    }

    void Texture::bindData(unsigned int width, unsigned int height, const void* data) const
    {
        bind();

        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        
        float lod_levels = glm::log2(static_cast<float>(std::max(width, height)));
        GL_CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, lod_levels));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

        unbind();
    }

    void Texture::bind() const
    {
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));
    }
    
    void Texture::unbind() const
    {
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, INVALID_TEXTURE));
    }
}