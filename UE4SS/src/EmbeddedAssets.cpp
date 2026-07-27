#ifdef __linux__
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// Embedded assets archive (generated at build time)
#include "embedded_assets.h"

namespace RC
{
    // Extract embedded assets to the working directory if they don't exist
    void ExtractEmbeddedAssets(const std::filesystem::path& working_dir)
    {
        // Check if UE4SS-settings.ini already exists — if so, assets are already extracted
        auto settings_path = working_dir / "UE4SS-settings.ini";
        if (std::filesystem::exists(settings_path))
        {
            return;
        }

        fprintf(stderr, "[UE4SS] Extracting embedded assets to %s\n", working_dir.string().c_str());

        // Write the embedded tar.gz to a temp file
        auto temp_archive = working_dir / ".ue4ss_assets.tar.gz";
        {
            std::ofstream out(temp_archive, std::ios::binary);
            if (!out)
            {
                fprintf(stderr, "[UE4SS] Failed to create temp archive: %s\n", temp_archive.string().c_str());
                return;
            }
            out.write(reinterpret_cast<const char*>(embedded_assets_data), embedded_assets_size);
            out.close();
        }

        // Extract using tar
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            chdir(working_dir.string().c_str());
            execlp("tar", "tar", "-xzf", temp_archive.string().c_str(), nullptr);
            _exit(1); // exec failed
        }
        else if (pid > 0)
        {
            // Parent process — wait for tar to finish
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                fprintf(stderr, "[UE4SS] Assets extracted successfully\n");
            }
            else
            {
                fprintf(stderr, "[UE4SS] Failed to extract assets (tar exit code: %d)\n", WEXITSTATUS(status));
            }
        }
        else
        {
            fprintf(stderr, "[UE4SS] Failed to fork for tar extraction\n");
        }

        // Clean up temp archive
        std::filesystem::remove(temp_archive);
    }
}
#endif
