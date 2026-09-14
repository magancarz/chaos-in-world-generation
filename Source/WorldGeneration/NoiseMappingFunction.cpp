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

#include "WorldGeneration/NoiseMappingFunction.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include "glm/common.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/compatibility.hpp"

namespace chs
{
    NoiseMappingFunction::NoiseMappingFunction(std::vector<MappingInterval> mapping_intervals)
        : mapping_intervals{std::move(mapping_intervals)}
    {
        std::ranges::sort(this->mapping_intervals, {}, &MappingInterval::starting_x);
    }

    float NoiseMappingFunction::map(float noise_value) const
    {
        if (mapping_intervals.size() < 2)
        {
            return noise_value;
        }

        if (noise_value <= mapping_intervals.front().starting_x)
        {
            return mapping_intervals.front().starting_y;
        }
        if (noise_value >= mapping_intervals.back().starting_x)
        {
            return mapping_intervals.back().starting_y;
        }

        const auto right = std::ranges::upper_bound(
            mapping_intervals, noise_value, {}, &MappingInterval::starting_x);
        const MappingInterval& left_mapping_interval = *std::prev(right);
        const MappingInterval& right_mapping_interval = *right;

        if (left_mapping_interval.starting_x == right_mapping_interval.starting_x)
        {
            return right_mapping_interval.starting_y;
        }

        return glm::mix(
                left_mapping_interval.starting_y,
                right_mapping_interval.starting_y,
                glm::smoothstep(
                    left_mapping_interval.starting_x,
                    right_mapping_interval.starting_x,
                    noise_value));
    }
}
