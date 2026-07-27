#pragma once

#include <filesystem>

namespace RC
{
#ifdef __linux__
    // Extract embedded assets (UE4SS-settings.ini, Mods/, etc.) to the working directory
    // Only extracts if assets don't already exist
    void ExtractEmbeddedAssets(const std::filesystem::path& working_dir);
#else
    // No-op on Windows
    inline void ExtractEmbeddedAssets(const std::filesystem::path&) {}
#endif
}
