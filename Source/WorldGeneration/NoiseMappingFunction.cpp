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

#include <cstdio>

#include "glm/common.hpp"

namespace chs
{
    NoiseMappingFunction::NoiseMappingFunction(std::vector<MappingInterval> mapping_intervals)
        : mapping_intervals{std::move(mapping_intervals)} {}

    float NoiseMappingFunction::map(float noise_value)
    {
        if (mapping_intervals.empty())
        {
            return noise_value;
        }

        int valid_mapping_interval_index = 0;
        for (; valid_mapping_interval_index < mapping_intervals.size(); ++valid_mapping_interval_index)
        {
            if (mapping_intervals[valid_mapping_interval_index].starting_x > noise_value)
            {
                valid_mapping_interval_index = valid_mapping_interval_index - 1;
                break;
            }
        }

        if (valid_mapping_interval_index < 0)
        {
            return noise_value;
        }

        MappingInterval& mapping_interval = mapping_intervals[valid_mapping_interval_index];
        return mapping_interval.coefficients.a * noise_value + mapping_interval.coefficients.b;
    }
}
