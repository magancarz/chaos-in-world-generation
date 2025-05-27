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

#include "Input/KeyMappings.h"

#include <utility>

#include "GLFW/glfw3.h"

namespace chs
{
    KeyMappings::KeyMappings()
    {
        key_mappings.emplace(KeyCode::MOVE_LEFT, GLFW_KEY_A);
        key_mappings.emplace(KeyCode::MOVE_RIGHT, GLFW_KEY_D);
        key_mappings.emplace(KeyCode::MOVE_FORWARD, GLFW_KEY_W);
        key_mappings.emplace(KeyCode::MOVE_BACKWARD, GLFW_KEY_S);
        key_mappings.emplace(KeyCode::MOVE_UP, GLFW_KEY_E);
        key_mappings.emplace(KeyCode::MOVE_DOWN, GLFW_KEY_Q);
        key_mappings.emplace(KeyCode::LOOK_LEFT, GLFW_KEY_LEFT);
        key_mappings.emplace(KeyCode::LOOK_RIGHT, GLFW_KEY_RIGHT);
        key_mappings.emplace(KeyCode::LOOK_UP, GLFW_KEY_UP);
        key_mappings.emplace(KeyCode::LOOK_DOWN, GLFW_KEY_DOWN);
        key_mappings.emplace(KeyCode::JUMP, GLFW_KEY_SPACE);

        key_mappings.emplace(KeyCode::ESCAPE, GLFW_KEY_ESCAPE);
        key_mappings.emplace(KeyCode::F1, GLFW_KEY_F1);
        key_mappings.emplace(KeyCode::F2, GLFW_KEY_F2);
        key_mappings.emplace(KeyCode::F3, GLFW_KEY_F3);
        key_mappings.emplace(KeyCode::F4, GLFW_KEY_F4);
        key_mappings.emplace(KeyCode::F5, GLFW_KEY_F5);
        key_mappings.emplace(KeyCode::F6, GLFW_KEY_F6);
        key_mappings.emplace(KeyCode::F7, GLFW_KEY_F7);
        key_mappings.emplace(KeyCode::F8, GLFW_KEY_F8);
        key_mappings.emplace(KeyCode::F9, GLFW_KEY_F9);

        key_mappings.emplace(KeyCode::NUM_0, GLFW_KEY_0);
        key_mappings.emplace(KeyCode::NUM_1, GLFW_KEY_1);
        key_mappings.emplace(KeyCode::NUM_2, GLFW_KEY_2);
        key_mappings.emplace(KeyCode::NUM_3, GLFW_KEY_3);
        key_mappings.emplace(KeyCode::NUM_4, GLFW_KEY_4);
        key_mappings.emplace(KeyCode::NUM_5, GLFW_KEY_5);
        key_mappings.emplace(KeyCode::NUM_6, GLFW_KEY_6);
        key_mappings.emplace(KeyCode::NUM_7, GLFW_KEY_7);
        key_mappings.emplace(KeyCode::NUM_8, GLFW_KEY_8);
        key_mappings.emplace(KeyCode::NUM_9, GLFW_KEY_9);
        key_mappings.emplace(KeyCode::NUM_0, GLFW_KEY_0);

        key_mappings.emplace(KeyCode::NUMPAD_2, GLFW_KEY_KP_2);
        key_mappings.emplace(KeyCode::NUMPAD_4, GLFW_KEY_KP_4);
        key_mappings.emplace(KeyCode::NUMPAD_6, GLFW_KEY_KP_6);
        key_mappings.emplace(KeyCode::NUMPAD_8, GLFW_KEY_KP_8);

        key_mappings.emplace(KeyCode::ALT, GLFW_KEY_LEFT_ALT);
    }

    int KeyMappings::getKeyCodeFor(KeyCode key_code) const
    {
        if (!key_mappings.contains(key_code))
        {
            return std::to_underlying(KeyCode::NO_KEY);
        }

        return key_mappings.at(key_code);
    }
}
