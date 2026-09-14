#include "WorldGeneration.h"
#include "WorldGeneration/NoiseMappingFunction.h"

#include "FastNoiseLite/Cpp/FastNoiseLite.h"

#include <cassert>
#include <stdexcept>
#include <vector>

namespace
{
    void testCoordinatesAndPacking()
    {
        chs::WorldGeneration generation;
        generation.setWidth(2);
        generation.setHeight(2);

        const auto data = generation.generate([](glm::vec2 point)
        {
            return chs::TerrainSample{point.x + point.y, {point.x, point.y, 0.25f}};
        });

        assert(data.size() == 4);
        assert(data[0] == glm::vec4(0.0f, 0.0f, 0.25f, 0.0f));
        assert(data[3] == glm::vec4(1.0f, 1.0f, 0.25f, 2.0f));
    }

    void testInvalidDimensions()
    {
        chs::WorldGeneration generation;
        generation.setWidth(0);

        bool rejected = false;
        try
        {
            const auto unused = generation.generate([](glm::vec2) { return chs::TerrainSample{}; });
            (void)unused;
        }
        catch (const std::invalid_argument&)
        {
            rejected = true;
        }
        assert(rejected);
    }

    void testCurveBoundariesAndOrdering()
    {
        const chs::NoiseMappingFunction curve({{1.0f, 0.8f}, {0.0f, 0.2f}, {0.5f, 0.4f}});
        assert(curve.map(-1.0f) == 0.2f);
        assert(curve.map(2.0f) == 0.8f);
        assert(curve.map(0.5f) == 0.4f);
    }

    std::vector<glm::vec4> generateSeededTerrain(int seed)
    {
        FastNoiseLite noise{seed};
        chs::WorldGeneration generation;
        generation.setWidth(8);
        generation.setHeight(8);
        return generation.generate([&](glm::vec2 point)
        {
            const float height = noise.GetNoise(point.x, point.y) * 0.5f + 0.5f;
            return chs::TerrainSample{height, glm::vec3{height}};
        });
    }

    void testSeedDeterminism()
    {
        assert(generateSeededTerrain(42) == generateSeededTerrain(42));
        assert(generateSeededTerrain(42) != generateSeededTerrain(43));
    }
}

int main()
{
    testCoordinatesAndPacking();
    testInvalidDimensions();
    testCurveBoundariesAndOrdering();
    testSeedDeterminism();
}
