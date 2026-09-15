#nullable enable

using System;
using Terrain.Abstractions;

namespace Terrain.Algorithm;

// Edit this file while the engine is running, then press "Build and reload C#"
// in the settings window. Noise2D calls FastNoiseLite in the C++ application.
public sealed unsafe class TerrainAlgorithm : ITerrainAlgorithm
{
    public string Name => "C# FastNoise terrain";

    public int Generate(
        TerrainGenerationRequest* request,
        TerrainSample* output,
        nuint outputCount)
    {
        if (request->Noise2D == null)
            return -10;

        nuint required = (nuint)request->Width * request->Height;
        if (outputCount < required)
            return -11;

        for (uint y = 0; y < request->Height; ++y)
        {
            for (uint x = 0; x < request->Width; ++x)
            {
                float rawNoise = request->Noise2D(
                    request->NoiseContext,
                    request->XOffset + x,
                    request->YOffset + y);
                float height = Math.Clamp(rawNoise * 0.5f + 0.5f, 0.0f, 1.0f);
                (float red, float green, float blue) = ColorForHeight(height);
                nuint index = (nuint)y * request->Width + x;
                output[index] = new TerrainSample
                {
                    Red = red,
                    Green = green,
                    Blue = blue,
                    Height = height,
                };
            }
        }
        return 0;
    }

    private static (float Red, float Green, float Blue) ColorForHeight(float height)
    {
        if (height < 0.02f) return (0.04f, 0.20f, 0.38f);
        if (height < 0.40f) return (0.08f, 0.38f, 0.58f);
        if (height < 0.43f) return (0.76f, 0.68f, 0.45f);
        if (height < 0.68f) return (0.16f, 0.42f, 0.18f);
        if (height < 0.82f) return (0.32f, 0.29f, 0.25f);
        return (0.88f, 0.91f, 0.92f);
    }
}
