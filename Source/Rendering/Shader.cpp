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

#include "Rendering/Shader.h"

#include <unordered_set>
#include <cassert>

#include "Rendering/DebugUtils.h"

namespace chs
{
    Shader::Shader(const ShaderSettings& shader_settings)
        : shader_program_id{createShaderProgram(shader_settings)},
        uniform_locations{findAllRequiredUniformLocations(shader_settings)} {}

    int Shader::createShaderProgram(const ShaderSettings& shader_settings) const
    {
        unsigned int program_id = glCreateProgram();

        const unsigned int vertex_shader_id = createShader(shader_settings.vertex_shader_code, GL_VERTEX_SHADER);
        assert(vertex_shader_id != INVALID_SHADER_ID);
        GL_CHECK(glAttachShader(program_id, vertex_shader_id));
        
        const unsigned int fragment_shader_id = createShader(shader_settings.fragment_shader_code, GL_FRAGMENT_SHADER);
        assert(fragment_shader_id != INVALID_SHADER_ID);
        GL_CHECK(glAttachShader(program_id, fragment_shader_id));
        
        GL_CHECK(glLinkProgram(program_id));
        GL_CHECK(glValidateProgram(program_id));

        GL_CHECK(glDetachShader(program_id, vertex_shader_id));
        GL_CHECK(glDeleteShader(vertex_shader_id));

        GL_CHECK(glDetachShader(program_id, fragment_shader_id));
        GL_CHECK(glDeleteShader(fragment_shader_id));

        return program_id;
    }
    
    int Shader::createShader(const ShaderCode& shader_code, GLenum shader_type) const
    {
        unsigned int shader_id = glCreateShader(shader_type);
        
        const char* shader_source = shader_code.getShaderCode();
        GL_CHECK(glShaderSource(shader_id, 1, &shader_source, nullptr));
        GL_CHECK(glCompileShader(shader_id));
        
        int result;
        GL_CHECK(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE)
        {
            int length;
            GL_CHECK(glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length));
            auto message = static_cast<char*>(alloca(length * sizeof(char)));
            GL_CHECK(glGetShaderInfoLog(shader_id, length, &length, message));
            GL_CHECK(glDeleteShader(shader_id));
            return INVALID_SHADER_ID;
        }

        return shader_id;
    }

    std::unordered_map<const char*, int> Shader::findAllRequiredUniformLocations(const ShaderSettings& shader_settings)
    {
        std::unordered_map<const char*, int> uniform_locations{};
        for (const auto& uniform_variable_name : shader_settings.uniform_variables)
        {
            assert(!uniform_locations.contains(uniform_variable_name));
            uniform_locations.emplace(uniform_variable_name, findUniformLocation(uniform_variable_name));
        }

        return uniform_locations;
    }

    int Shader::findUniformLocation(const char* uniform_variable_name)
    {
        int uniform_location = glGetUniformLocation(shader_program_id, uniform_variable_name);
        assert(uniform_location != INVALID_UNIFORM_LOCATION && "Unable to get uniform location");

        return uniform_location;
    }

    Shader::~Shader()
    {
        GL_CHECK(glDeleteProgram(shader_program_id));
    }

    void Shader::bind() const
    {
        GL_CHECK(glUseProgram(shader_program_id));
    }

    void Shader::unbind() const
    {
        GL_CHECK(glUseProgram(INVALID_SHADER_ID));
    }

    void Shader::bindTexture(unsigned int slot, const Texture& texture) const
    {
        GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
        texture.bind();
    }

    void Shader::loadMatrix(const char* uniform_name, const glm::mat4& matrix)
    {
        assert(uniform_locations.contains(uniform_name) && "Uniform variable must be present in shader");
        int uniform_location = uniform_locations.at(uniform_name);
        static constexpr GLenum DONT_TRANSPOSE{GL_FALSE};
        GL_CHECK(glUniformMatrix4fv(uniform_location, 1, DONT_TRANSPOSE, &matrix[0][0]));
    }
}