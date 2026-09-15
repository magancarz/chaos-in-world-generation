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

#pragma once

#include "Window/Window.h"
#include "WorldGeneration/WorldGenerationSettings.h"

namespace chs {

struct EditorActions {
  bool terrain_settings_changed{false};
  bool regenerate_requested{false};
  bool export_requested{false};
  bool build_and_reload_csharp_requested{false};
};

class Editor {
 public:
  explicit Editor(Window& window);
  ~Editor();

  [[nodiscard]] EditorActions updateGUI(
      WorldGenerationSettings& world_generation_settings,
      const TerrainStatistics& terrain_statistics);
  void drawGUI() const;

 private:
  void initializeGraphicalInterface(Window& window);

  void beginNewFrame() const;
  [[nodiscard]] EditorActions updateGUIElements(
      WorldGenerationSettings& world_generation_settings,
      const TerrainStatistics& terrain_statistics) const;
};

}  // namespace chs
