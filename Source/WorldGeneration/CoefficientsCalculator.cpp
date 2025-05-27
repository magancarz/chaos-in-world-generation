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

#include "WorldGeneration/CoefficientsCalculator.h"

#include "glm/ext/scalar_constants.hpp"

namespace chs
{
    std::vector<Coefficients> CoefficientsCalculator::calculate(const std::vector<Spline>& splines)
    {
        std::vector<Coefficients> result_coefficients;
        result_coefficients.resize(splines.size());

        for (unsigned int current_spline_index = 0; current_spline_index < splines.size() - 1; ++current_spline_index)
        {
            const Spline& first_spline = splines.at(current_spline_index);
            const Spline& second_spline = splines.at(current_spline_index + 1);

            float dx = second_spline.x - first_spline.x;
            float dy = second_spline.y - first_spline.y;

            if (dx <= glm::epsilon<float>()) dx += glm::epsilon<float>();

            Coefficients& coefficients = result_coefficients.at(current_spline_index);
            coefficients.a = dy / dx;
            coefficients.b = first_spline.y - coefficients.a * first_spline.x;
        }

        return result_coefficients;
    }
}
