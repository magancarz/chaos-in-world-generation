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
#include <sstream>

namespace chs
{
    class CodeBuilder
    {
    public:
        static constexpr int DEFAULT_INDENTATION_AMOUNT{4};
        static constexpr char DEFAULT_NEWLINE_CHARACTER{'\n'};

        explicit CodeBuilder(
            int indentation_size = DEFAULT_INDENTATION_AMOUNT,
            char newline_character = DEFAULT_NEWLINE_CHARACTER);

        void indentation() { code_builder << generateIndentation(); }
        void append(const char* text) { code_builder << text; }
        void newline() { code_builder << newline_character; }
        void incrementIndentation() { total_indentation_count += indentation_size; }
        void incrementIndentation(int n) { total_indentation_count += n; }
        void decrementIndentation() { total_indentation_count = std::max(0, total_indentation_count - indentation_size); }
        void decrementIndentation(int n) { total_indentation_count = std::max(0, total_indentation_count - n); }

        std::string generate() const { return code_builder.str(); }
        void clear() { code_builder.clear(); }

    private:
        std::stringstream code_builder;
        char newline_character;
        int indentation_size;
        int total_indentation_count{0};

        std::string generateIndentation() const { return std::string(total_indentation_count, ' '); }
    };
}
