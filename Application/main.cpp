// MIT License

// Copyright (c) 2025 Mateusz Gancarz

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <vector>

#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "Editor/Editor.h"
#include "FastNoiseLite/Cpp/FastNoiseLite.h"
#include "Images/Image.h"
#include "Input/InputManager.h"
#include "Managed/ManagedTerrainHost.h"
#include "Rendering/Camera.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderCode.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexArrayEntry.h"
#include "Rendering/VertexBuffer.h"
#include "Window/Window.h"
#include "WorldGeneration.h"
#include "WorldGeneration/NoiseMappingFunction.h"
#include "WorldGeneration/WorldGenerationSettings.h"

namespace {

float sampleNoiseFromManaged(void* context, float x, float y) {
  if (context == nullptr) return 0.0f;
  return static_cast<FastNoiseLite*>(context)->GetNoise(x, y);
}

glm::vec3 terrainColor(float height) {
  if (height < 0.30f) return {0.04f, 0.20f, 0.38f};
  if (height < 0.38f) return {0.08f, 0.38f, 0.58f};
  if (height < 0.43f) return {0.76f, 0.68f, 0.45f};
  if (height < 0.68f) return {0.16f, 0.42f, 0.18f};
  if (height < 0.82f) return {0.32f, 0.29f, 0.25f};
  return {0.88f, 0.91f, 0.92f};
}

bool exportTerrainMaps(const std::vector<glm::vec4>& terrain_data, int width,
                       int height) {
  std::vector<std::uint8_t> color_data(terrain_data.size() * 3);
  std::vector<std::uint8_t> height_data(terrain_data.size());

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const std::size_t source_index = static_cast<std::size_t>(y * width + x);
      const std::size_t output_index =
          static_cast<std::size_t>((height - y - 1) * width + x);
      const glm::vec4& sample = terrain_data[source_index];
      color_data[output_index * 3] = static_cast<std::uint8_t>(
          std::lround(glm::clamp(sample.r, 0.0f, 1.0f) * 255.0f));
      color_data[output_index * 3 + 1] = static_cast<std::uint8_t>(
          std::lround(glm::clamp(sample.g, 0.0f, 1.0f) * 255.0f));
      color_data[output_index * 3 + 2] = static_cast<std::uint8_t>(
          std::lround(glm::clamp(sample.b, 0.0f, 1.0f) * 255.0f));
      height_data[output_index] = static_cast<std::uint8_t>(
          std::lround(glm::clamp(sample.a, 0.0f, 1.0f) * 255.0f));
    }
  }

  const std::filesystem::path export_directory{"Exports"};
  std::error_code directory_error;
  std::filesystem::create_directories(export_directory, directory_error);
  if (directory_error) {
    return false;
  }
  const bool color_saved =
      chs::Image::saveToFile((export_directory / "terrain_color.png").c_str(),
                             color_data.data(), width, height, 3, width * 3);
  const bool height_saved =
      chs::Image::saveToFile((export_directory / "terrain_height.png").c_str(),
                             height_data.data(), width, height, 1, width);
  return color_saved && height_saved;
}
}  // namespace

int main() {
  // Keep relative shader and export paths rooted in the checkout under bazel
  // run.
  if (const char* workspace = std::getenv("BUILD_WORKSPACE_DIRECTORY")) {
    std::filesystem::current_path(workspace);
  }

  chs::Window window{};
  chs::Editor editor{window};
  chs::InputManager input_manager{window};

  chs::Camera camera{input_manager};
  camera.setPerspectiveProjection(chs::Camera::CAMERA_FOV, window.getAspect());

  chs::WorldGeneration world_generation{};
  chs::ManagedTerrainHost managed_terrain_host{};

  chs::WorldGenerationSettings world_generation_settings{};

  FastNoiseLite noise{2025};
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalOctaves(8);

  chs::Texture texture{};

  glPatchParameteri(GL_PATCH_VERTICES, 4);

  std::vector<float> terrain_vertices;

  unsigned resolution = 16;
  for (unsigned z = 0; z <= resolution - 1; ++z) {
    for (unsigned x = 0; x <= resolution - 1; ++x) {
      constexpr int MAP_SCALE = 16;
      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * z) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(0.0f);
      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * x) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(z) /
                                    static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(x) /
                                    static_cast<float>(resolution));

      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * (z + 1)) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(0.0f);
      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * x) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(z + 1) /
                                    static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(x) /
                                    static_cast<float>(resolution));

      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * z) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(0.0f);
      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * (x + 1)) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(z) /
                                    static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(x + 1) /
                                    static_cast<float>(resolution));

      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * (z + 1)) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(0.0f);
      terrain_vertices.emplace_back(-MAP_SCALE / 2.0f +
                                    static_cast<float>(MAP_SCALE * (x + 1)) /
                                        static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(z + 1) /
                                    static_cast<float>(resolution));
      terrain_vertices.emplace_back(static_cast<float>(x + 1) /
                                    static_cast<float>(resolution));
    }
  }

  chs::ShaderCode vertex_shader_code{"Terrain.vert"};
  chs::ShaderCode tesselation_control_shader_code{"Terrain.tesc"};
  chs::ShaderCode tesselation_evaluation_shader_code{"Terrain.tese"};
  chs::ShaderCode fragment_shader_code{"Terrain.frag"};

  chs::ShaderSettings shader_settings{
      .vertex_shader_code = std::move(vertex_shader_code),
      .tesselation_control_shader_code =
          std::move(tesselation_control_shader_code),
      .tesselation_evaluation_shader_code =
          std::move(tesselation_evaluation_shader_code),
      .fragment_shader_code = std::move(fragment_shader_code),
      .uniform_variables = {"view", "projection_view", "height_scale",
                            "minimum_tessellation_level",
                            "maximum_tessellation_level", "sun_position"}};

  chs::Shader shader{shader_settings};

  chs::ShaderSettings skybox_shader_settings{
      .vertex_shader_code = chs::ShaderCode{"Skybox.vert"},
      .tesselation_control_shader_code = chs::ShaderCode{"Skybox.tesc"},
      .tesselation_evaluation_shader_code = chs::ShaderCode{"Skybox.tese"},
      .fragment_shader_code = chs::ShaderCode{"Skybox.frag"},
      .uniform_variables = {"projection_view_rotation", "sun_direction"}};
  chs::Shader skybox_shader{skybox_shader_settings};

  chs::VertexBuffer vertices{};
  vertices.bindData(terrain_vertices.size() * sizeof(float),
                    terrain_vertices.data());

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

  chs::VertexArray vertex_array{
      {vertices_description, texture_coords_description},
      4 * resolution * resolution};

  std::vector<float> skybox_vertices{
      -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f,
      -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f};
  chs::VertexBuffer skybox_vertex_buffer{};
  skybox_vertex_buffer.bindData(skybox_vertices.size() * sizeof(float),
                                skybox_vertices.data());
  chs::VertexArrayEntry skybox_vertices_description{};
  skybox_vertices_description.attribute_size = 3;
  skybox_vertices_description.instance_stride = 3 * sizeof(float);
  skybox_vertices_description.source_buffer = &skybox_vertex_buffer;
  chs::VertexArray skybox_vertex_array{{skybox_vertices_description}, 24};

  static constexpr unsigned int NUM_OF_MAPPING_INTERVALS = 7;
  world_generation_settings.mapping_intervals.resize(NUM_OF_MAPPING_INTERVALS);

  world_generation_settings.mapping_intervals[0].starting_x = 0.0f;
  world_generation_settings.mapping_intervals[0].starting_y = 0.0f;

  world_generation_settings.mapping_intervals[1].starting_x = 0.1f;
  world_generation_settings.mapping_intervals[1].starting_y = 0.365f;

  world_generation_settings.mapping_intervals[2].starting_x = 0.495f;
  world_generation_settings.mapping_intervals[2].starting_y = 0.063f;

  world_generation_settings.mapping_intervals[3].starting_x = 0.51f;
  world_generation_settings.mapping_intervals[3].starting_y = 0.028f;

  world_generation_settings.mapping_intervals[4].starting_x = 0.525f;
  world_generation_settings.mapping_intervals[4].starting_y = 0.051f;

  world_generation_settings.mapping_intervals[5].starting_x = 0.8f;
  world_generation_settings.mapping_intervals[5].starting_y = 0.227f;

  world_generation_settings.mapping_intervals[6].starting_x = 1.0f;
  world_generation_settings.mapping_intervals[6].starting_y = 1.0f;

  // This is the extension point for terrain experiments. Any callable with
  // this signature can provide height and color for a sampled grid position.
  const auto generate_terrain = [&]() {
    world_generation.setWidth(
        static_cast<unsigned int>(world_generation_settings.map_resolution));
    world_generation.setHeight(
        static_cast<unsigned int>(world_generation_settings.map_resolution));
    noise.SetSeed(world_generation_settings.seed);
    noise.SetFractalOctaves(world_generation_settings.octaves);

    const std::size_t sample_count =
        static_cast<std::size_t>(world_generation_settings.map_resolution) *
        static_cast<std::size_t>(world_generation_settings.map_resolution);
    std::vector<chs::TerrainSampleC> managed_samples(sample_count);
    const chs::TerrainGenerationRequestC request{
        .struct_size = sizeof(chs::TerrainGenerationRequestC),
        .width = static_cast<std::uint32_t>(
            world_generation_settings.map_resolution),
        .height = static_cast<std::uint32_t>(
            world_generation_settings.map_resolution),
        .seed = world_generation_settings.seed,
        .octaves = world_generation_settings.octaves,
        .x_offset = world_generation_settings.x_coordinate_offset,
        .y_offset = world_generation_settings.y_coordinate_offset,
        .noise_context = &noise,
        .noise_2d = sampleNoiseFromManaged,
    };
    if (managed_terrain_host.generate(request, managed_samples.data(),
                                      managed_samples.size())) {
      std::vector<glm::vec4> values(sample_count);
      for (std::size_t index = 0; index < sample_count; ++index) {
        const chs::TerrainSampleC& sample = managed_samples[index];
        values[index] =
            glm::vec4{sample.red, sample.green, sample.blue, sample.height};
      }
      return values;
    }

    const chs::NoiseMappingFunction height_curve{
        world_generation_settings.mapping_intervals};
    return world_generation.generate([&](glm::vec2 point) {
      const float raw_noise = noise.GetNoise(
          world_generation_settings.x_coordinate_offset + point.x,
          world_generation_settings.y_coordinate_offset + point.y);
      const float normalized_noise = raw_noise * 0.5f + 0.5f;
      const float height =
          glm::clamp(height_curve.map(normalized_noise), 0.0f, 1.0f);
      return chs::TerrainSample{height, terrainColor(height)};
    });
  };

  std::vector<glm::vec4> terrain_data;
  chs::TerrainStatistics terrain_statistics{};
  int generated_resolution{world_generation_settings.map_resolution};
  const auto regenerate_terrain = [&]() {
    const auto generation_started = std::chrono::steady_clock::now();
    terrain_data = generate_terrain();
    const auto generation_finished = std::chrono::steady_clock::now();
    terrain_statistics.generation_time_ms =
        std::chrono::duration<double, std::milli>(generation_finished -
                                                  generation_started)
            .count();
    terrain_statistics.sample_count = terrain_data.size();
    terrain_statistics.managed_algorithm_status = managed_terrain_host.status();
    generated_resolution = world_generation_settings.map_resolution;
    texture.bindData(static_cast<unsigned int>(generated_resolution),
                     static_cast<unsigned int>(generated_resolution),
                     terrain_data.data());
  };
  regenerate_terrain();

  unsigned int primitive_query{0};
  glGenQueries(1, &primitive_query);
  bool primitive_query_in_flight{false};
  bool regeneration_pending{false};
  auto last_terrain_change = std::chrono::steady_clock::now();

  auto last_time = std::chrono::high_resolution_clock::now();
  while (!window.closeRequested()) {
    window.beginNewFrame();

    auto now = std::chrono::high_resolution_clock::now();
    float frame_delta_time =
        static_cast<float>(
            std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                  last_time)
                .count()) /
        1000.0f;
    last_time = now;

    if (primitive_query_in_flight) {
      int result_available{0};
      glGetQueryObjectiv(primitive_query, GL_QUERY_RESULT_AVAILABLE,
                         &result_available);
      if (result_available != 0) {
        GLuint64 primitive_count{0};
        glGetQueryObjectui64v(primitive_query, GL_QUERY_RESULT,
                              &primitive_count);
        terrain_statistics.triangle_count = primitive_count;
        primitive_query_in_flight = false;
      }
    }

    const chs::EditorActions editor_actions =
        editor.updateGUI(world_generation_settings, terrain_statistics);
    if (editor_actions.build_and_reload_csharp_requested) {
      if (managed_terrain_host.buildAndReload()) {
        regenerate_terrain();
        regeneration_pending = false;
      }
      terrain_statistics.managed_algorithm_status =
          managed_terrain_host.status();
    }
    if (editor_actions.terrain_settings_changed) {
      regeneration_pending = true;
      last_terrain_change = std::chrono::steady_clock::now();
    }

    const auto debounce_elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - last_terrain_change)
            .count();
    const bool automatic_regeneration_due =
        regeneration_pending &&
        world_generation_settings.automatic_regeneration &&
        debounce_elapsed >= world_generation_settings.regeneration_debounce_ms;
    if (editor_actions.regenerate_requested || automatic_regeneration_due) {
      regenerate_terrain();
      regeneration_pending = false;
    }

    if (editor_actions.export_requested) {
      terrain_statistics.export_status =
          exportTerrainMaps(terrain_data, generated_resolution,
                            generated_resolution)
              ? "Saved to Exports/terrain_color.png and terrain_height.png"
              : "Export failed";
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
    shader.loadFloat("height_scale", world_generation_settings.height_scale);
    shader.loadFloat("minimum_tessellation_level",
                     world_generation_settings.minimum_tessellation_level);
    shader.loadFloat("maximum_tessellation_level",
                     world_generation_settings.maximum_tessellation_level);
    shader.loadVec3("sun_position",
                    glm::normalize(glm::vec3{1.0f, 1.0f, 1.0f}));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (!primitive_query_in_flight) {
      glBeginQuery(GL_PRIMITIVES_GENERATED, primitive_query);
    }
    vertex_array.draw();
    if (!primitive_query_in_flight) {
      glEndQuery(GL_PRIMITIVES_GENERATED);
      primitive_query_in_flight = true;
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    vertex_array.unbind();
    shader.unbind();

    const glm::mat4 skybox_projection_view =
        projection * glm::mat4{glm::mat3{view}};
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    skybox_shader.bind();
    skybox_vertex_array.bind();
    skybox_shader.loadMatrix("projection_view_rotation",
                             skybox_projection_view);
    skybox_shader.loadVec3("sun_direction",
                           glm::normalize(glm::vec3{1.0f, 1.0f, 1.0f}));
    skybox_vertex_array.draw();
    skybox_vertex_array.unbind();
    skybox_shader.unbind();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    editor.drawGUI();

    window.finalizeFrame();
  }

  glDeleteQueries(1, &primitive_query);

  return 0;
}
