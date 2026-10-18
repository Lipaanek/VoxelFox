#include "catch_amalgamated.hpp"
#include "core/util/util.hpp"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

TEST_CASE("Util::File::read returns contents for an existing file") {
    REQUIRE_FALSE(Util::File::read("README.md").empty());
}

TEST_CASE("Util::File::read returns empty string for a missing file") {
    REQUIRE(Util::File::read("does_not_exist.txt").empty());
}

TEST_CASE("Filesystem utilities create and edit directory contents") {
    const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto root = std::filesystem::temp_directory_path()
        / ("voxelfox_file_system_tests_" + std::to_string(suffix));

    Util::Directory project(root);

    const std::vector<std::uint8_t> imageData{0x89, 0x50, 0x4e, 0x47};

    project.folder("assets").folder("textures").file("player.bin", imageData);
    project.folder("config").file("settings.ini", "volume=0.8\n");

    std::ifstream image(root / "assets" / "textures" / "player.bin", std::ios::binary);

    const std::vector<unsigned char> imageBytes{
        std::istreambuf_iterator<char>(image),
        std::istreambuf_iterator<char>()};

    REQUIRE(imageBytes == std::vector<unsigned char>{0x89, 0x50, 0x4e, 0x47});

    image.close();

    REQUIRE(Util::File::read((root / "config" / "settings.ini").string()) == "volume=0.8\n");

    Util::File settings = project.folder("config").file("settings.ini");

    settings.writeText("volume=0.5\n").appendText("muted=false\n");

    REQUIRE(Util::File::read((root / "config" / "settings.ini").string()) == "volume=0.5\nmuted=false\n");

    Util::File numberFile = project.file("number.bin");

    numberFile.writeBinary(std::uint16_t{0x1234});
    numberFile.appendBinary(std::uint8_t{0x56});

    std::ifstream binary(root / "number.bin", std::ios::binary);
    const std::vector<unsigned char> bytes{
        std::istreambuf_iterator<char>(binary),
        std::istreambuf_iterator<char>()};

    REQUIRE(bytes == std::vector<unsigned char>{0x34, 0x12, 0x56});

    binary.close();

    Util::File floatFile = project.file("float.bin");

    floatFile.writeBinary(1.0f);

    std::ifstream floatBinary(root / "float.bin", std::ios::binary);
    const std::vector<unsigned char> floatBytes{
        std::istreambuf_iterator<char>(floatBinary),
        std::istreambuf_iterator<char>()};

    REQUIRE(floatBytes == std::vector<unsigned char>{0x00, 0x00, 0x80, 0x3f});

    floatBinary.close();

    std::filesystem::remove_all(root);
}