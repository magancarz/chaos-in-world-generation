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

#include "Editor/Editor.h"

#include <algorithm>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/bezie.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <imgui.h>

namespace chs {

Editor::Editor(Window& window) { initializeGraphicalInterface(window); }

void Editor::initializeGraphicalInterface(Window& window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
  ImGui_ImplOpenGL3_Init("#version 460");
}

Editor::~Editor() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

EditorActions Editor::updateGUI(
    WorldGenerationSettings& world_generation_settings,
    const TerrainStatistics& terrain_statistics) {
  beginNewFrame();
  return updateGUIElements(world_generation_settings, terrain_statistics);
}

void Editor::beginNewFrame() const {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

EditorActions Editor::updateGUIElements(
    WorldGenerationSettings& world_generation_settings,
    const TerrainStatistics& terrain_statistics) const {
  EditorActions actions{};
  ImGui::Begin("World Generation Settings");

  if (ImGui::SliderFloat("X Offset",
                         &world_generation_settings.x_coordinate_offset,
                         -100.0f, 100.0f)) {
    actions.terrain_settings_changed = true;
  }

  if (ImGui::SliderFloat("Y Offset",
                         &world_generation_settings.y_coordinate_offset,
                         -100.0f, 100.0f)) {
    actions.terrain_settings_changed = true;
  }

  actions.terrain_settings_changed |=
      ImGui::InputInt("Seed", &world_generation_settings.seed);
  actions.terrain_settings_changed |=
      ImGui::SliderInt("Octaves", &world_generation_settings.octaves, 1, 12);

  static constexpr const char* RESOLUTION_LABELS[] = {"64",  "128",  "256",
                                                      "512", "1024", "2048"};
  static constexpr int RESOLUTIONS[] = {64, 128, 256, 512, 1024, 2048};
  int resolution_index = 0;
  while (resolution_index < 5 && RESOLUTIONS[resolution_index] !=
                                     world_generation_settings.map_resolution) {
    ++resolution_index;
  }
  if (ImGui::Combo("Map Resolution", &resolution_index, RESOLUTION_LABELS, 6)) {
    world_generation_settings.map_resolution = RESOLUTIONS[resolution_index];
    actions.terrain_settings_changed = true;
  }

  if (ImGui::Table("Mapping values",
                   reinterpret_cast<glm::vec2*>(
                       world_generation_settings.mapping_intervals.data()),
                   world_generation_settings.mapping_intervals.size())) {
    actions.terrain_settings_changed = true;
  }

  actions.terrain_settings_changed |=
      ImGui::InputFloat("Unit size", &world_generation_settings.unit_size);

  ImGui::SeparatorText("Terrain Algorithm");
  actions.build_and_reload_csharp_requested = ImGui::Button("Build and reload");
  ImGui::TextDisabled("Managed/Terrain.Algorithm/TerrainAlgorithm.cs");
  ImGui::TextWrapped("C# status: %s",
                     terrain_statistics.managed_algorithm_status.c_str());

  ImGui::SeparatorText("Rendering");
  if (ImGui::InputFloat("Water Height", &world_generation_settings.water_height,
                        0.1f, 1.0f, "%.2f")) {
    actions.terrain_settings_changed = true;
  }
  if (ImGui::SliderFloat("Minimum Tessellation",
                         &world_generation_settings.minimum_tessellation_level,
                         1.0f, 64.0f, "%.0f")) {
    world_generation_settings.minimum_tessellation_level =
        std::min(world_generation_settings.minimum_tessellation_level,
                 world_generation_settings.maximum_tessellation_level);
  }
  if (ImGui::SliderFloat("Maximum Tessellation",
                         &world_generation_settings.maximum_tessellation_level,
                         1.0f, 64.0f, "%.0f")) {
    world_generation_settings.maximum_tessellation_level =
        std::max(world_generation_settings.maximum_tessellation_level,
                 world_generation_settings.minimum_tessellation_level);
  }

  ImGui::SeparatorText("Regeneration");
  ImGui::Checkbox("Automatic",
                  &world_generation_settings.automatic_regeneration);
  if (world_generation_settings.automatic_regeneration) {
    ImGui::SliderInt("Debounce (ms)",
                     &world_generation_settings.regeneration_debounce_ms, 50,
                     1000);
  }
  actions.regenerate_requested = ImGui::Button("Regenerate now");
  ImGui::SameLine();
  actions.export_requested = ImGui::Button("Export PNG maps");

  ImGui::SeparatorText("Statistics");
  ImGui::Text("Generation: %.2f ms", terrain_statistics.generation_time_ms);
  ImGui::Text("Samples: %llu",
              static_cast<unsigned long long>(terrain_statistics.sample_count));
  ImGui::Text("Minimum height: %.3f",
              terrain_statistics.minimum_generated_height);
  ImGui::Text("Maximum height: %.3f",
              terrain_statistics.maximum_generated_height);
  ImGui::Text("Triangles: %llu", static_cast<unsigned long long>(
                                     terrain_statistics.triangle_count));
  ImGui::Text("Frame rate: %.1f FPS", ImGui::GetIO().Framerate);
  ImGui::TextWrapped("Export: %s", terrain_statistics.export_status);

  ImGui::End();

  return actions;
}

void Editor::drawGUI() const {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace chs
