#nullable enable

using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using System.Text;
using Terrain.Abstractions;

namespace Terrain.Bridge;

public static unsafe class NativeApi
{
    private static readonly object Sync = new();
    private static LoadedAlgorithm? current;
    private static string lastError = "";

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static int Reload(byte* pathUtf8, uint pathLength)
    {
        try
        {
            string path = Encoding.UTF8.GetString(pathUtf8, checked((int)pathLength));
            LoadedAlgorithm replacement = LoadedAlgorithm.Load(path);

            lock (Sync)
            {
                LoadedAlgorithm? previous = current;
                current = replacement;
                previous?.Unload();
            }

            lastError = "";
            return 0;
        }
        catch (Exception exception)
        {
            lastError = exception.ToString();
            return -1;
        }
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static int Generate(
        TerrainGenerationRequest* request,
        TerrainSample* output,
        nuint outputCount)
    {
        try
        {
            if (request is null || output is null)
                return Fail("The native request or output pointer was null", -1);
            if (request->AbiVersion != TerrainAbi.Version)
                return Fail($"Unsupported terrain ABI version {request->AbiVersion}", -2);
            if (request->StructSize != (uint)sizeof(TerrainGenerationRequest))
                return Fail("The native and managed request layouts do not match", -3);

            lock (Sync)
            {
                if (current is null)
                    return Fail("No C# terrain algorithm is loaded", -4);
                int result = current.Instance.Generate(request, output, outputCount);
                if (result != 0)
                    lastError = $"{current.Instance.Name} returned error {result}";
                return result;
            }
        }
        catch (Exception exception)
        {
            lastError = exception.ToString();
            return -5;
        }
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static uint GetLastError(byte* output, uint capacity)
    {
        byte[] message = Encoding.UTF8.GetBytes(lastError);
        uint required = checked((uint)message.Length + 1);
        if (output is null || capacity == 0)
            return required;

        uint copied = Math.Min((uint)message.Length, capacity - 1);
        for (uint index = 0; index < copied; ++index)
            output[index] = message[index];
        output[copied] = 0;
        return required;
    }

    private static int Fail(string message, int result)
    {
        lastError = message;
        return result;
    }
}

internal sealed class TerrainPluginLoadContext : AssemblyLoadContext
{
    private readonly AssemblyDependencyResolver resolver;

    public TerrainPluginLoadContext(string pluginPath)
        : base($"Terrain-{Path.GetFileName(Path.GetDirectoryName(pluginPath))}", isCollectible: true)
    {
        resolver = new AssemblyDependencyResolver(pluginPath);
    }

    protected override Assembly? Load(AssemblyName assemblyName)
    {
        Assembly abstractions = typeof(ITerrainAlgorithm).Assembly;
        if (assemblyName.Name == abstractions.GetName().Name)
            return abstractions;

        string? path = resolver.ResolveAssemblyToPath(assemblyName);
        return path is null ? null : LoadFromAssemblyPath(path);
    }
}

internal sealed class LoadedAlgorithm
{
    private TerrainPluginLoadContext? context;
    private ITerrainAlgorithm? instance;

    public ITerrainAlgorithm Instance =>
        instance ?? throw new ObjectDisposedException(nameof(LoadedAlgorithm));

    private LoadedAlgorithm(TerrainPluginLoadContext context, ITerrainAlgorithm instance)
    {
        this.context = context;
        this.instance = instance;
    }

    public static LoadedAlgorithm Load(string path)
    {
        string absolutePath = Path.GetFullPath(path);
        var context = new TerrainPluginLoadContext(absolutePath);
        try
        {
            Assembly assembly = context.LoadFromAssemblyPath(absolutePath);
            Type[] implementations = assembly.GetTypes()
                .Where(type => !type.IsAbstract && typeof(ITerrainAlgorithm).IsAssignableFrom(type))
                .ToArray();
            if (implementations.Length != 1)
                throw new InvalidOperationException(
                    $"Expected exactly one ITerrainAlgorithm implementation, found {implementations.Length}");

            var instance = (ITerrainAlgorithm?)Activator.CreateInstance(implementations[0])
                ?? throw new InvalidOperationException("Could not create the terrain algorithm");
            return new LoadedAlgorithm(context, instance);
        }
        catch
        {
            context.Unload();
            throw;
        }
    }

    public void Unload()
    {
        if (instance is IDisposable disposable)
            disposable.Dispose();
        instance = null;
        context?.Unload();
        context = null;
    }
}
