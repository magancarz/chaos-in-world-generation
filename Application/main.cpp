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

#include "WorldGeneration.h"

#include <vector>
#include <array>
#include <cstdint>
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>

#include "Images/Image.h"
#include "Window/Window.h"
#include "Editor/Editor.h"
#include "Rendering/ShaderCode.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexArrayEntry.h"
#include "Input/InputManager.h"
#include "Rendering/Camera.h"
#include "WorldGeneration/WorldGenerationSettings.h"

#include <glm/ext/matrix_transform.hpp>

int main()
{
    chs::Window window{};
    chs::Editor editor{window};
    chs::InputManager input_manager{window};

    chs::Camera camera{input_manager};
    camera.setPerspectiveProjection(chs::Camera::CAMERA_FOV, window.getAspect());

    static constexpr int MAP_SIZE = 128;

    chs::WorldGeneration world_generation{};
    world_generation.setWidth(MAP_SIZE);
    world_generation.setHeight(MAP_SIZE);

    chs::WorldGenerationSettings world_generation_settings{};
    auto height_map_data = world_generation.generate(world_generation_settings);

    chs::Texture texture{};
    texture.bindData(MAP_SIZE, MAP_SIZE, height_map_data.data());

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    std::vector<float> terrain_vertices;

    const int MAP_SCALE = 16;
    unsigned resolution = 20;
    for(unsigned z = 0; z <= resolution - 1; ++z)
    {
        for(unsigned x = 0; x <= resolution - 1; ++x)
        {
            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * z) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(0.0f);
            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * x) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(z) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(x) / static_cast<float>(resolution));

            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * (z + 1)) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(0.0f);
            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * x) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(z + 1) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(x) / static_cast<float>(resolution));

            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * z) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(0.0f);
            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * (x + 1)) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(z) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(x + 1) / static_cast<float>(resolution));

            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * (z + 1)) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(0.0f);
            terrain_vertices.emplace_back(-MAP_SCALE / 2.0f + static_cast<float>(MAP_SCALE * (x + 1)) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(z + 1) / static_cast<float>(resolution));
            terrain_vertices.emplace_back(static_cast<float>(x + 1) / static_cast<float>(resolution));
        }
    }

    chs::ShaderCode vertex_shader_code{"Terrain.vert"};
    chs::ShaderCode tesselation_control_shader_code{"Terrain.tesc"};
    chs::ShaderCode tesselation_evaluation_shader_code{"Terrain.tese"};
    chs::ShaderCode fragment_shader_code{"Terrain.frag"};

    chs::ShaderSettings shader_settings
    {
        .vertex_shader_code = std::move(vertex_shader_code),
        .tesselation_control_shader_code = std::move(tesselation_control_shader_code),
        .tesselation_evaluation_shader_code = std::move(tesselation_evaluation_shader_code),
        .fragment_shader_code = std::move(fragment_shader_code),
        .uniform_variables = {"view", "projection_view"}
    };

    chs::Shader shader{shader_settings};

    chs::VertexBuffer vertices{};
    vertices.bindData(terrain_vertices.size() * sizeof(float), terrain_vertices.data());

    chs::VertexArrayEntry vertices_description{};
    vertices_description.attribute_size = 3;
    vertices_description.instance_offset = 0;
    vertices_description.instance_stride = 5 * sizeof(float);
    vertices_description.source_buffer = &vertices;

    chs::VertexArrayEntry texture_coords_description{};
    texture_coords_description.attribute_size = 2;
    texture_coords_description.instance_offset = 3 * sizeof(float);
    texture_coords_description.instance_stride = 5 * sizeof(float);
    texture_coords_description.source_buffer = &vertices;

    chs::VertexArray vertex_array{{vertices_description, texture_coords_description}, 4 * resolution * resolution};

    auto last_time = std::chrono::high_resolution_clock::now();
    while (!window.closeRequested())
    {
        window.beginNewFrame();

        auto now = std::chrono::high_resolution_clock::now();
        float frame_delta_time = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count()) / 1000.0f;
        last_time = now;

        editor.updateGUI(world_generation_settings);

        if (world_generation_settings.settings_updated)
        {
            auto height_map_data = world_generation.generate(world_generation_settings);
            texture.bindData(MAP_SIZE, MAP_SIZE, height_map_data.data());
            world_generation_settings.settings_updated = false;
        }

        camera.update(frame_delta_time);

        glm::mat4 projection = camera.getProjection();
        glm::mat4 view = camera.getView();
        glm::mat4 projection_view = projection * view;

        shader.bind();
        vertex_array.bind();
        shader.bindTexture(0, texture);
        shader.loadMatrix("view", view);
        shader.loadMatrix("projection_view", projection_view);

        vertex_array.draw();

        vertex_array.unbind();
        shader.unbind();

        editor.drawGUI();

        window.finalizeFrame();
    }

    return 0;
}