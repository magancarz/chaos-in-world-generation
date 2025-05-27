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

#include "WorldGeneration/GLSLCodeGenerator.h"

#include <format>

namespace chs
{
    GLSLCodeGenerator::GLSLCodeGenerator(const NoiseMappingFunction& noise_mapping_function)
        : noise_mapping_function{noise_mapping_function} {}

    std::string GLSLCodeGenerator::generate()
    {
        if (noise_mapping_function.mapping_intervals.empty())
        {
            return "";
        }

        generateFunctionOpening();
        for (unsigned int index = noise_mapping_function.mapping_intervals.size() - 1; index > 0; --index)
        {
            const MappingInterval& mapping_interval = noise_mapping_function.mapping_intervals[index];
            generateCodeForMappingInterval(mapping_interval);
        }
        generateCodeForLastMappingInterval(noise_mapping_function.mapping_intervals[0]);
        generateFunctionClosure();

        std::string result = code_builder.generate();
        code_builder.clear();

        return result;
    }

    void GLSLCodeGenerator::generateFunctionOpening()
    {
        std::string function_opening = std::format("float {}(float x)", function_name);
        code_builder.append(function_opening.c_str());
        code_builder.newline();
        code_builder.append("{");
        code_builder.newline();
        code_builder.incrementIndentation();
    }

    void GLSLCodeGenerator::generateCodeForMappingInterval(const MappingInterval& mapping_interval)
    {
        code_builder.indentation();

        std::string mapping = std::format(
            "if (x >= {:8f}f) return {:8f}f * x + ({:8f}f);",
            mapping_interval.starting_x,
            mapping_interval.coefficients.a,
            mapping_interval.coefficients.b);
        code_builder.append(mapping.c_str());

        code_builder.newline();
    }

    void GLSLCodeGenerator::generateCodeForLastMappingInterval(const MappingInterval& mapping_interval)
    {
        code_builder.indentation();

        std::string mapping = std::format(
            "return {:8f}f * x + ({:8f}f);",
            mapping_interval.starting_x,
            mapping_interval.coefficients.a,
            mapping_interval.coefficients.b);
        code_builder.append(mapping.c_str());

        code_builder.newline();
    }

    void GLSLCodeGenerator::generateFunctionClosure()
    {
        code_builder.decrementIndentation();
        code_builder.append("}");
        code_builder.newline();
    }
}
