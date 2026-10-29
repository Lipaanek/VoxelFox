#include "catch_amalgamated.hpp"
#include "core/readable_files/project_file_handler.hpp"

#include <chrono>
#include <filesystem>

TEST_CASE("ProjectFileHandler saves updated project values") {
    const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto root = std::filesystem::temp_directory_path()
        / ("voxelfox_project_file_tests_" + std::to_string(suffix));

    ProjectFile project{ Util::File(root / "My Project" / "project.vfp"), {} };
    ProjectFileHandler handler{ project };

    handler.addSection("application");
    handler.addVariable("application", "project_name", std::string{ "Old name" });
    handler.addVariable("application", "project_name", std::string{ "My Project" });
    handler.addVariable("application", "version", std::string{ "1.9.1" });
    handler.save();

    const std::string contents = Util::File::read((root / "My Project" / "project.vfp").string());
    REQUIRE(contents.find("[application]") != std::string::npos);
    REQUIRE(contents.find("project_name=\"My Project\"") != std::string::npos);
    REQUIRE(contents.find("version=\"1.9.1\"") != std::string::npos);

    handler.removeSection("application");
    handler.save();
    REQUIRE(Util::File::read((root / "My Project" / "project.vfp").string()) == "; VoxelFox project\n");

    std::filesystem::remove_all(root);
}
