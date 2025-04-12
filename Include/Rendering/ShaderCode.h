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

#include <string>
#include <unordered_map>

#include "Rendering/ShaderType.h"

namespace chs
{
    class ShaderCode
    {
    public:
        static constexpr const char* SHADERS_DIRECTORY{"Shaders"};

        static constexpr const char* VERTEX_SHADER_FILE_EXTENSION{".vert"};
        static constexpr const char* FRAGMENT_SHADER_FILE_EXTENSION{".frag"};

        inline static const std::unordered_map<const char*, ShaderType> SHADER_TYPE_MAPPINGS
        {
            {VERTEX_SHADER_FILE_EXTENSION, ShaderType::VERTEX},
            {FRAGMENT_SHADER_FILE_EXTENSION, ShaderType::FRAGMENT}
        };

        explicit ShaderCode(const char* shader_file_name);

        const char* getShaderCode() const { return shader_code.c_str(); }
        ShaderType getShaderType() const { return shader_type; }

    private:
        std::string loadShaderCodeFromFile(const char* shader_file_name) const;
        ShaderType detectShaderTypeFromFileName(const char* shader_file_name) const;

        std::string shader_code;
        ShaderType shader_type;
    };
}