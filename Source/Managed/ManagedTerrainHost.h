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

#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

#include "Managed/TerrainManagedApi.h"

namespace chs
{
    class ManagedTerrainHost
    {
    public:
        ManagedTerrainHost() = default;
        ManagedTerrainHost(const ManagedTerrainHost&) = delete;
        ManagedTerrainHost& operator=(const ManagedTerrainHost&) = delete;

        [[nodiscard]] bool buildAndReload();
        [[nodiscard]] bool loadAndReload(
            const std::filesystem::path& bridge_directory,
            const std::filesystem::path& algorithm_assembly);
        [[nodiscard]] bool generate(
            const TerrainGenerationRequestC& request,
            TerrainSampleC* output,
            std::size_t output_count);

        [[nodiscard]] bool ready() const { return generate_function != nullptr; }
        [[nodiscard]] const std::string& status() const { return status_message; }

    private:
        using LoadAssemblyFunction = std::int32_t (*)(
            const char* assembly_path,
            const char* type_name,
            const char* method_name,
            const char* delegate_type_name,
            void* reserved,
            void** delegate);
        using ReloadFunction = std::int32_t (*)(const char* path, std::uint32_t path_length);
        using GenerateFunction = std::int32_t (*)(
            const TerrainGenerationRequestC* request,
            TerrainSampleC* output,
            std::size_t output_count);
        using GetLastErrorFunction = std::uint32_t (*)(char* output, std::uint32_t capacity);

        [[nodiscard]] bool initializeRuntime(const std::filesystem::path& bridge_directory);
        [[nodiscard]] bool loadBridgeFunction(const char* method_name, void** function);
        [[nodiscard]] std::string managedError() const;
        void setStatus(std::string message);

        void* hostfxr_library{nullptr};
        LoadAssemblyFunction load_assembly{nullptr};
        ReloadFunction reload_function{nullptr};
        GenerateFunction generate_function{nullptr};
        GetLastErrorFunction get_last_error_function{nullptr};
        std::filesystem::path bridge_assembly_path;
        std::string status_message{"C# algorithm not loaded"};
        std::uint64_t build_revision{0};
    };
}
