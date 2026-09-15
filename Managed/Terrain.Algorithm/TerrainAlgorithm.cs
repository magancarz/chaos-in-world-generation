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
    if (request->MappingPointCount > 0 && request->MappingPoints == null)
      return -12;

    var mappingPoints = new TerrainMappingPoint[checked((int)request->MappingPointCount)];
    for (int index = 0; index < mappingPoints.Length; ++index)
      mappingPoints[index] = request->MappingPoints[index];
    Array.Sort(mappingPoints, static (left, right) => left.X.CompareTo(right.X));

    for (uint y = 0; y < request->Height; ++y)
    {
      for (uint x = 0; x < request->Width; ++x)
      {
        float rawNoise = request->Noise2D(
            request->NoiseContext,
            request->XOffset + x,
            request->YOffset + y);
        float normalizedNoise = rawNoise * 0.5f + 0.5f;
        float height = Math.Clamp(
            MapHeight(mappingPoints, normalizedNoise), 0.0f, 1.0f);
        (float red, float green, float blue) = ColorForHeight(
            request->WaterLevel, height);
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

  private static float MapHeight(
      ReadOnlySpan<TerrainMappingPoint> points,
      float value)
  {
    if (points.Length < 2)
      return value;
    if (value <= points[0].X)
      return points[0].Y;
    if (value >= points[^1].X)
      return points[^1].Y;

    for (int index = 1; index < points.Length; ++index)
    {
      TerrainMappingPoint right = points[index];
      if (value >= right.X)
        continue;

      TerrainMappingPoint left = points[index - 1];
      if (left.X == right.X)
        return right.Y;

      float amount = (value - left.X) / (right.X - left.X);
      amount = amount * amount * (3.0f - 2.0f * amount);
      return left.Y + (right.Y - left.Y) * amount;
    }

    return value;
  }

  private static (float Red, float Green, float Blue) ColorForHeight(
      float water_level, float height)
  {
    if (height < water_level) return (0.04f, 0.20f, 0.38f);
    return (0.16f, 0.42f, 0.18f);
  }
}
