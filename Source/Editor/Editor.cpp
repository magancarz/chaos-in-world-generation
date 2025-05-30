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

#include "Editor/Editor.h"

#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_impl_glfw.h"
#include "imgui/bezie.h"
#include "imgui/imgui_impl_opengl3.h"

namespace chs
{
    Editor::Editor(Window& window)
    {
        initializeGraphicalInterface(window);
    }

    void Editor::initializeGraphicalInterface(Window& window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    Editor::~Editor()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Editor::updateGUI(WorldGenerationSettings& world_generation_settings)
    {
        beginNewFrame();
        updateGUIElements(world_generation_settings);
    }

    void Editor::beginNewFrame() const
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Editor::updateGUIElements(WorldGenerationSettings& world_generation_settings) const
    {
        ImGui::Begin("World Generation Settings");

        if (ImGui::SliderFloat("X Offset", &world_generation_settings.x_coordinate_offset, -100.0f, 100.0f))
        {
            world_generation_settings.settings_updated = true;
        }

        if (ImGui::SliderFloat("Y Offset", &world_generation_settings.y_coordinate_offset, -100.0f, 100.0f))
        {
            world_generation_settings.settings_updated = true;
        }

        if (ImGui::Table("Mapping values",
            reinterpret_cast<glm::vec2*>(world_generation_settings.mapping_intervals.data()),
            world_generation_settings.mapping_intervals.size()))
        {
            world_generation_settings.settings_updated = true;
        }

        ImGui::End();
    }

    void Editor::drawGUI() const
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}