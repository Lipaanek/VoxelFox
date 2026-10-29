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

TEST_CASE("ProjectFileHandler loads sections and typed values from a file") {
    const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path()
        / ("voxelfox_project_file_load_tests_" + std::to_string(suffix))
        / "project.vfp";

    ProjectFile source{ Util::File(path), {} };
    ProjectFileHandler sourceHandler{ source };
    sourceHandler.addSection("application");
    sourceHandler.addVariable("application", "name", std::string{ "A ; quoted name\nnext" });
    sourceHandler.addVariable("application", "version", 1);
    sourceHandler.addVariable("application", "scale", 1.5f);
    sourceHandler.addVariable("application", "enabled", true);
    sourceHandler.save();

    ProjectFile loaded{ Util::File("unused.vfp"), {} };
    ProjectFileHandler loadedHandler{ loaded };
    loadedHandler.load(path);

    REQUIRE(loaded.environment.at("application").at("name") == VariableValue{ "A ; quoted name\nnext" });
    REQUIRE(loaded.environment.at("application").at("version") == VariableValue{ 1 });
    REQUIRE(loaded.environment.at("application").at("scale") == VariableValue{ 1.5f });
    REQUIRE(loaded.environment.at("application").at("enabled") == VariableValue{ true });
    REQUIRE(loaded.file.path() == path);

    std::filesystem::remove_all(path.parent_path());
}

TEST_CASE("ProjectFileHandler rejects malformed project values without replacing the environment") {
    const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path()
        / ("voxelfox_invalid_project_tests_" + std::to_string(suffix))
        / "project.vfp";

    const auto previousPath = path.parent_path() / "previous.vfp";
    ProjectFile project{ Util::File(previousPath), {} };
    ProjectFileHandler handler{ project };
    handler.addSection("existing");
    handler.addVariable("existing", "value", 7);
    Util::Directory(path.parent_path()).file("project.vfp", "[broken\n");

    REQUIRE_THROWS(handler.load(path));
    REQUIRE(project.environment.at("existing").at("value") == VariableValue{ 7 });
    REQUIRE(project.file.path() == previousPath);

    std::filesystem::remove_all(path.parent_path());
}
