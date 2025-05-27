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

#include "Input/InputManager.h"

#include <cmath>

#include "Input/KeyMappings.h"

namespace chs
{
    InputManager::InputManager(const Window& window)
        : window_ptr{window.getGLFWwindow()}{
        glfwSetKeyCallback(window_ptr, &InputManager::keyCallback);
    }

    void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key_values.size() <= key)
        {
            return;
        }

        if (action == GLFW_PRESS)
        {
            key_values.at(key) = true;
        }
        else if (action == GLFW_RELEASE)
        {
            key_values.at(key) = false;
        }
    }

    bool InputManager::isKeyPressed(KeyCode key_mapping)
    {
        return key_values.at(key_mappings.getKeyCodeFor(key_mapping));
    }
}
