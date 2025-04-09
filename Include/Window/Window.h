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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace chs
{
    class Window
    {
    public:
        static constexpr int DEFAULT_WINDOW_WIDTH{1280};
        static constexpr int DEFAULT_WINDOW_HEIGHT{800};
        static constexpr const char* DEFAULT_WINDOW_TITLE{"Chaos in world generation"};

        Window(int width = DEFAULT_WINDOW_WIDTH, int height = DEFAULT_WINDOW_HEIGHT);
        ~Window();

        void beginNewFrame();
        void finalizeFrame();

        bool closeRequested() const { return glfwWindowShouldClose(window); }

        GLFWwindow* getGLFWwindow() const { return window; }

    private:
        static constexpr float CLEAR_COLOR_RED{0.1f};
        static constexpr float CLEAR_COLOR_GREEN{0.1f};
        static constexpr float CLEAR_COLOR_BLUE{0.1f};
        static constexpr float CLEAR_COLOR_ALPHA{1.0f};

        int width;
        int height;

        GLFWwindow* initializeGLFWWindow();

        GLFWwindow* window{nullptr};
    };
}