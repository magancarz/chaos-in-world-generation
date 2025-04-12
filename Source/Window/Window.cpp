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

#include "Window/Window.h"

#include <iostream>

namespace chs
{
    static void GLFWErrorCallback([[maybe_unused]] int error_code, const char* error_description)
    {
        printf("GLFW error: %s\n", error_description);
    }

    Window::Window(int width, int height)
        : width{width}, height{height}, window{initializeGLFWWindow()} {}

    GLFWwindow* Window::initializeGLFWWindow()
    {
        glfwSetErrorCallback(GLFWErrorCallback);
 
        if (!glfwInit())
        {
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        static constexpr GLFWmonitor* WINDOWED_MODE{nullptr};
        static constexpr GLFWwindow* DO_NOT_SHARE_RESOURCES{nullptr};
        GLFWwindow* window = glfwCreateWindow(width, height, DEFAULT_WINDOW_TITLE, WINDOWED_MODE, DO_NOT_SHARE_RESOURCES);
        if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK)
        {
            std::cout << "Failed to initialize GLEW!" << std::endl;
            exit(EXIT_FAILURE);
        }

        return window;
    }

    Window::~Window()
    {
        glfwTerminate();
    }

    void Window::beginNewFrame()
    {
        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &height);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
        glClearColor(CLEAR_COLOR_RED, CLEAR_COLOR_GREEN, CLEAR_COLOR_BLUE, CLEAR_COLOR_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::finalizeFrame()
    {
        glfwSwapBuffers(window);
    }
}