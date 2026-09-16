#nullable enable

using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Terrain.Abstractions;

public static class TerrainAbi
{
  public const uint Version = 3;
}

[StructLayout(LayoutKind.Sequential)]
public struct TerrainMappingPoint
{
  public float X;
  public float Y;
}

[StructLayout(LayoutKind.Sequential)]
public unsafe struct TerrainGenerationRequest
{
  public uint AbiVersion;
  public uint StructSize;
  public uint Width;
  public uint Height;
  public int Seed;
  public int Octaves;
  public float XOffset;
  public float YOffset;
  public void* NoiseContext;
  public delegate* unmanaged[Cdecl]<void*, float, float, float> Noise2D;
  public TerrainMappingPoint* MappingPoints;
  public uint MappingPointCount;
  public float WaterLevel;  // Water level in meters.
  public float UnitSize;  // Unit size in meters.
}

[StructLayout(LayoutKind.Sequential)]
public struct TerrainSample
{
  public float Red;
  public float Green;
  public float Blue;
  public float Height;
}

public unsafe interface ITerrainAlgorithm
{
  string Name { get; }

  int Generate(
      TerrainGenerationRequest* request,
      TerrainSample* output,
      nuint outputCount);
}
