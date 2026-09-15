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

#include "Managed/ManagedTerrainHost.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <sstream>
#include <string_view>
#include <sys/wait.h>
#include <utility>
#include <vector>

namespace
{
    constexpr int LOAD_ASSEMBLY_AND_GET_FUNCTION_POINTER = 5;
    const char* const UNMANAGED_CALLERS_ONLY_METHOD = reinterpret_cast<const char*>(-1);

    using HostfxrHandle = void*;
    using HostfxrInitializeFunction = std::int32_t (*)(const char*, const void*, HostfxrHandle*);
    using HostfxrGetDelegateFunction = std::int32_t (*)(HostfxrHandle, int, void**);
    using HostfxrCloseFunction = std::int32_t (*)(HostfxrHandle);

    std::vector<int> versionParts(const std::string& version)
    {
        std::vector<int> parts;
        std::size_t position = 0;
        while (position < version.size())
        {
            const std::size_t end = version.find('.', position);
            const std::string_view part{version.data() + position,
                (end == std::string::npos ? version.size() : end) - position};
            int value = 0;
            for (const char character : part)
            {
                if (character < '0' || character > '9') break;
                value = value * 10 + character - '0';
            }
            parts.push_back(value);
            if (end == std::string::npos) break;
            position = end + 1;
        }
        return parts;
    }

    bool newerVersion(const std::filesystem::path& left, const std::filesystem::path& right)
    {
        return versionParts(left.filename().string()) > versionParts(right.filename().string());
    }

    void addRunfilesDotnetRoots(
        std::vector<std::filesystem::path>& roots,
        const std::filesystem::path& runfiles_directory)
    {
        std::error_code error;
        if (!std::filesystem::is_directory(runfiles_directory, error)) return;
        for (const auto& entry : std::filesystem::directory_iterator(runfiles_directory, error))
        {
            if (!entry.is_directory()) continue;
            if (std::filesystem::exists(entry.path() / "dotnet", error) &&
                std::filesystem::is_directory(entry.path() / "host" / "fxr", error))
            {
                roots.push_back(entry.path());
            }
        }
    }

    std::vector<std::filesystem::path> dotnetRoots()
    {
        std::vector<std::filesystem::path> roots;
        for (const char* variable : {"DOTNET_ROOT", "DOTNET_ROOT_X64"})
        {
            if (const char* value = std::getenv(variable); value != nullptr && *value != '\0')
            {
                roots.emplace_back(value);
            }
        }
        for (const char* variable : {"RUNFILES_DIR", "TEST_SRCDIR"})
        {
            if (const char* value = std::getenv(variable); value != nullptr && *value != '\0')
            {
                addRunfilesDotnetRoots(roots, value);
            }
        }
        std::error_code executable_error;
        const std::filesystem::path executable = std::filesystem::canonical("/proc/self/exe", executable_error);
        if (!executable_error)
        {
            addRunfilesDotnetRoots(roots, executable.string() + ".runfiles");
        }

        roots.emplace_back("/usr/share/dotnet");
        roots.emplace_back("/usr/local/share/dotnet");

        if (const char* path = std::getenv("PATH"))
        {
            std::stringstream entries{path};
            std::string entry;
            while (std::getline(entries, entry, ':'))
            {
                std::error_code error;
                const std::filesystem::path executable = std::filesystem::path{entry} / "dotnet";
                if (std::filesystem::exists(executable, error))
                {
                    const std::filesystem::path resolved = std::filesystem::canonical(executable, error);
                    if (!error) roots.push_back(resolved.parent_path());
                }
            }
        }
        return roots;
    }

    std::filesystem::path findDotnetExecutable()
    {
        std::error_code error;
        for (const std::filesystem::path& root : dotnetRoots())
        {
            const std::filesystem::path executable = root / "dotnet";
            if (std::filesystem::is_regular_file(executable, error)) return executable;
        }
        return {};
    }

    std::string shellQuote(const std::filesystem::path& path)
    {
        std::string quoted{"'"};
        for (const char character : path.string())
        {
            if (character == '\'') quoted += "'\\''";
            else quoted += character;
        }
        quoted += '\'';
        return quoted;
    }

    std::filesystem::path findHostfxr()
    {
        for (const std::filesystem::path& root : dotnetRoots())
        {
            const std::filesystem::path versions_directory = root / "host" / "fxr";
            std::error_code error;
            if (!std::filesystem::is_directory(versions_directory, error)) continue;

            std::vector<std::filesystem::path> versions;
            for (const auto& entry : std::filesystem::directory_iterator(versions_directory, error))
            {
                if (entry.is_directory()) versions.push_back(entry.path());
            }
            std::sort(versions.begin(), versions.end(), newerVersion);
            for (const std::filesystem::path& version : versions)
            {
                const std::filesystem::path library = version / "libhostfxr.so";
                if (std::filesystem::exists(library, error)) return library;
            }
        }
        return {};
    }

    struct CommandResult
    {
        int exit_code{-1};
        std::string output;
    };

    CommandResult runCommand(const std::string& command)
    {
        CommandResult result;
        std::array<char, 512> buffer{};
        FILE* process = popen((command + " 2>&1").c_str(), "r");
        if (process == nullptr)
        {
            result.output = "Failed to start dotnet build";
            return result;
        }
        while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), process) != nullptr)
        {
            result.output.append(buffer.data());
        }
        const int status = pclose(process);
        result.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        return result;
    }

    std::string summarizeBuildOutput(const std::string& output)
    {
        std::stringstream lines{output};
        std::string line;
        while (std::getline(lines, line))
        {
            if (line.find(": error ") != std::string::npos) return line;
        }

        const std::size_t end = output.find_last_not_of("\r\n");
        if (end == std::string::npos) return {};
        const std::size_t start = output.find_last_of("\r\n", end);
        return output.substr(start == std::string::npos ? 0 : start + 1, end - (start == std::string::npos ? 0 : start + 1) + 1);
    }
}

namespace chs
{
    bool ManagedTerrainHost::buildAndReload()
    {
        const std::filesystem::path bridge_directory{"Managed/Build/Bridge"};
        const std::filesystem::path algorithm_directory =
            std::filesystem::path{"Managed/Build"} / ("Algorithm-" + std::to_string(++build_revision));
        const std::filesystem::path dotnet_executable = findDotnetExecutable();
        if (dotnet_executable.empty())
        {
            setStatus("The Bazel-managed .NET SDK was not found in the application runfiles");
            return false;
        }
        const std::string dotnet = shellQuote(dotnet_executable);

        const CommandResult bridge_build = runCommand(
            dotnet + " build Managed/Terrain.Bridge/Terrain.Bridge.csproj --configuration Debug "
            "--output Managed/Build/Bridge --nologo");
        if (bridge_build.exit_code != 0)
        {
            setStatus("C# bridge build failed: " + summarizeBuildOutput(bridge_build.output));
            return false;
        }

        const CommandResult algorithm_build = runCommand(
            dotnet + " build Managed/Terrain.Algorithm/Terrain.Algorithm.csproj --configuration Debug "
            "--output " + algorithm_directory.string() + " --nologo");
        if (algorithm_build.exit_code != 0)
        {
            setStatus("C# algorithm build failed: " + summarizeBuildOutput(algorithm_build.output));
            return false;
        }

        const std::filesystem::path algorithm_path =
            std::filesystem::absolute(algorithm_directory / "Terrain.Algorithm.dll");
        if (!loadAndReload(bridge_directory, algorithm_path)) return false;

        setStatus("Loaded C# algorithm revision " + std::to_string(build_revision));
        return true;
    }

    bool ManagedTerrainHost::loadAndReload(
        const std::filesystem::path& bridge_directory,
        const std::filesystem::path& algorithm_assembly)
    {
        if (load_assembly == nullptr && !initializeRuntime(bridge_directory)) return false;

        const std::string path = std::filesystem::absolute(algorithm_assembly).string();
        const std::int32_t result = reload_function(path.data(), static_cast<std::uint32_t>(path.size()));
        if (result != 0)
        {
            setStatus("C# reload failed: " + managedError());
            return false;
        }

        setStatus("Loaded C# algorithm " + algorithm_assembly.filename().string());
        return true;
    }

    bool ManagedTerrainHost::generate(
        const TerrainGenerationRequestC& request,
        TerrainSampleC* output,
        std::size_t output_count)
    {
        if (generate_function == nullptr)
        {
            setStatus("C# algorithm is enabled but has not been built and loaded");
            return false;
        }
        const std::int32_t result = generate_function(&request, output, output_count);
        if (result != 0)
        {
            setStatus("C# generation failed: " + managedError());
            return false;
        }
        return true;
    }

    bool ManagedTerrainHost::initializeRuntime(const std::filesystem::path& bridge_directory)
    {
        const std::filesystem::path hostfxr_path = findHostfxr();
        if (hostfxr_path.empty())
        {
            setStatus(".NET runtime was not found; install the .NET 8 SDK or set DOTNET_ROOT");
            return false;
        }

        hostfxr_library = dlopen(hostfxr_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (hostfxr_library == nullptr)
        {
            setStatus(std::string{"Could not load hostfxr: "} + dlerror());
            return false;
        }

        const auto initialize = reinterpret_cast<HostfxrInitializeFunction>(
            dlsym(hostfxr_library, "hostfxr_initialize_for_runtime_config"));
        const auto get_delegate = reinterpret_cast<HostfxrGetDelegateFunction>(
            dlsym(hostfxr_library, "hostfxr_get_runtime_delegate"));
        const auto close = reinterpret_cast<HostfxrCloseFunction>(
            dlsym(hostfxr_library, "hostfxr_close"));
        if (initialize == nullptr || get_delegate == nullptr || close == nullptr)
        {
            setStatus("The installed hostfxr does not provide the required hosting API");
            return false;
        }

        const std::filesystem::path runtime_config =
            std::filesystem::absolute(bridge_directory / "Terrain.Bridge.runtimeconfig.json");
        HostfxrHandle context = nullptr;
        if (initialize(runtime_config.c_str(), nullptr, &context) != 0 || context == nullptr)
        {
            setStatus("Could not initialize .NET using " + runtime_config.string());
            return false;
        }

        void* load_assembly_pointer = nullptr;
        const std::int32_t delegate_result = get_delegate(
            context, LOAD_ASSEMBLY_AND_GET_FUNCTION_POINTER, &load_assembly_pointer);
        close(context);
        if (delegate_result != 0 || load_assembly_pointer == nullptr)
        {
            setStatus("Could not obtain the .NET assembly-loading function");
            return false;
        }
        load_assembly = reinterpret_cast<LoadAssemblyFunction>(load_assembly_pointer);
        bridge_assembly_path = std::filesystem::absolute(bridge_directory / "Terrain.Bridge.dll");

        if (!loadBridgeFunction("Reload", reinterpret_cast<void**>(&reload_function)) ||
            !loadBridgeFunction("Generate", reinterpret_cast<void**>(&generate_function)) ||
            !loadBridgeFunction("GetLastError", reinterpret_cast<void**>(&get_last_error_function)))
        {
            return false;
        }
        return true;
    }

    bool ManagedTerrainHost::loadBridgeFunction(const char* method_name, void** function)
    {
        const std::int32_t result = load_assembly(
            bridge_assembly_path.c_str(),
            "Terrain.Bridge.NativeApi, Terrain.Bridge",
            method_name,
            UNMANAGED_CALLERS_ONLY_METHOD,
            nullptr,
            function);
        if (result != 0 || *function == nullptr)
        {
            setStatus(std::string{"Could not load C# bridge method "} + method_name);
            return false;
        }
        return true;
    }

    std::string ManagedTerrainHost::managedError() const
    {
        if (get_last_error_function == nullptr) return "No managed diagnostic available";
        const std::uint32_t required = get_last_error_function(nullptr, 0);
        if (required <= 1) return "Unknown managed error";
        std::vector<char> message(required);
        get_last_error_function(message.data(), static_cast<std::uint32_t>(message.size()));
        return message.data();
    }

    void ManagedTerrainHost::setStatus(std::string message)
    {
        status_message = std::move(message);
    }
}
