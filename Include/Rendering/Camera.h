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

#include <glm/glm.hpp>

#include "Input/InputManager.h"

namespace chs
{
    class Camera
    {
    public:
        explicit Camera(InputManager& input_manager);

        Camera(const Camera&) = delete;
        Camera& operator=(const Camera& other) = delete;
        Camera(Camera&&) = default;
        Camera& operator=(Camera&& other) = delete;

        void setPerspectiveProjection(float fov_y, float aspect);

        void update(float frame_delta_time);

        [[nodiscard]] const glm::mat4& getProjection() const { return projection; }
        [[nodiscard]] const glm::mat4& getView() const { return view; }

        static constexpr float CAMERA_FOV = 70.0f;
        static constexpr float CAMERA_NEAR = 0.001f;
        static constexpr float CAMERA_FAR = 1000.0f;
        static constexpr glm::vec3 CAMERA_FRONT{0, 0, -1};
        static constexpr glm::vec3 CAMERA_RIGHT{1, 0, 0};
        static constexpr glm::vec3 CAMERA_UP{0, 1, 0};

    private:
        InputManager& input_manager;

        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};

        void updateRotation(float frame_delta_time);
        void updateLocation(float frame_delta_time);
        void updateViewMatrix();

        glm::vec3 location{0.0f};
        glm::vec3 rotation{0.0f};

        glm::vec3 front{CAMERA_FRONT};
        glm::vec3 right{CAMERA_FRONT};

        float move_speed{1.0f};
        float look_speed{50.0f};
    };
}
