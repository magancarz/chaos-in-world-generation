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

#include <cstdint>
#include <string>
#include <vector>

#include "MappingInterval.h"

namespace chs {

struct WorldGenerationSettings {
  float x_coordinate_offset = 0;
  float y_coordinate_offset = 0;
  int seed = 2025;
  int octaves = 8;
  int map_resolution = 512;

  std::vector<MappingInterval> mapping_intervals;
  float unit_size = 1.0f;

  float water_height = 0.6f;
  float minimum_tessellation_level = 4.0f;
  float maximum_tessellation_level = 64.0f;

  bool automatic_regeneration = true;
  int regeneration_debounce_ms = 250;
};

struct TerrainStatistics {
  double generation_time_ms = 0.0;
  std::uint64_t sample_count = 0;
  std::uint64_t triangle_count = 0;
  float minimum_generated_height = 0.0f;
  float maximum_generated_height = 0.0f;
  const char* export_status = "Not exported";
  std::string managed_algorithm_status = "C# algorithm not loaded";
};

}  // namespace chs
