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

#include "Rendering/Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Input/KeyCodes.h"

namespace chs
{
    Camera::Camera(InputManager& input_manager)
        : input_manager{input_manager} {}

    void Camera::update(float frame_delta_time)
    {
        updateRotation(frame_delta_time);
        updateLocation(frame_delta_time);
        updateViewMatrix();
    }

    void Camera::updateRotation(float frame_delta_time)
    {
        glm::vec3 rotation_input{0.0f};

        rotation_input.x += static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_DOWN));
        rotation_input.x -= static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_UP));

        rotation_input.y += static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_RIGHT));
        rotation_input.y -= static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_LEFT));

        glm::vec3 rotation_delta = look_speed * frame_delta_time * rotation_input;
        constexpr float PITCH_ROTATION_LIMIT = 70.f;
        rotation = glm::vec3
            {
                glm::clamp(rotation.x + rotation_delta.x, -PITCH_ROTATION_LIMIT, PITCH_ROTATION_LIMIT),
                glm::mod(rotation.y + rotation_delta.y, 360.0f),
                0
            };

        front.x = glm::cos(glm::radians(rotation.y - 90.f)) * glm::cos(glm::radians(-rotation.x));
        front.y = glm::sin(glm::radians(-rotation.x));
        front.z = glm::sin(glm::radians(rotation.y - 90.f)) * glm::cos(glm::radians(-rotation.x));

        front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, CAMERA_UP));
    }

    void Camera::updateLocation(float frame_delta_time)
    {
        glm::vec3 translation_input{0.0f};

        translation_input -= right * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_LEFT));
        translation_input += right * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_RIGHT));

        translation_input -= CAMERA_UP * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_DOWN));
        translation_input += CAMERA_UP * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_UP));

        translation_input += front * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_FORWARD));
        translation_input -= front * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_BACKWARD));

        location += move_speed * frame_delta_time * translation_input;
    }

    void Camera::setPerspectiveProjection(float fov_y, float aspect)
    {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        projection = glm::perspective(fov_y, aspect, CAMERA_NEAR, CAMERA_FAR);
    }

    void Camera::updateViewMatrix()
    {
        view = glm::lookAt(location, location + front, CAMERA_UP);
    }
}
