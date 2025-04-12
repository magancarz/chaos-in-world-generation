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

#include "Rendering/ShaderCode.h"

#include <fstream>
#include <filesystem>

namespace chs
{
    ShaderCode::ShaderCode(const char* shader_file_name)
        : shader_code{loadShaderCodeFromFile(shader_file_name)} {}

    std::string ShaderCode::loadShaderCodeFromFile(const char* shader_file_name) const
    {
        std::string shader_file_path = std::filesystem::path{SHADERS_DIRECTORY} / std::filesystem::path{shader_file_name};
        std::ifstream stream{shader_file_path.c_str()};
        std::string line;
        std::stringstream ss;

        while (getline(stream, line))
        {
            ss << line << '\n';
        }
        return ss.str();
    }

    ShaderType ShaderCode::detectShaderTypeFromFileName(const char* shader_file_name) const
    {
        std::filesystem::path file_name{shader_file_name};
        return SHADER_TYPE_MAPPINGS.at(file_name.extension().c_str());
    }
}