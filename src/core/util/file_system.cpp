#include "file_system.hpp"

#include <bit>
#include <fstream>
#include <format>
#include <iterator>
#include <limits>
#include <stdexcept>

#include "util.hpp"

namespace {
    std::filesystem::path childPath(const std::filesystem::path& parent, const std::string& name) {
        const std::filesystem::path child(name);
        if (name.empty() || child == "." || child == ".." || child.has_parent_path()
            || child.has_root_name() || child.has_root_directory()) {
            throw std::invalid_argument(std::format("Expected a single path component, got: {}", name));
        }
        return parent / child;
    }

    void prepareParentDirectory(const std::filesystem::path& path) {
        const auto parent = path.parent_path();
        if (!parent.empty())
            std::filesystem::create_directories(parent);
    }

    void writeBytes(const std::filesystem::path& path, std::span<const std::byte> bytes, std::ios::openmode mode) {
        prepareParentDirectory(path);
        std::ofstream output(path, mode | std::ios::binary);
        if (!output.is_open())
            throw std::runtime_error(std::format("Cannot open file for writing: {}", path.string()));

        if (!bytes.empty())
            output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        output.flush();
        if (!output)
            throw std::runtime_error(std::format("Failed writing file: {}", path.string()));
    }
}

namespace Util {
    File::File(std::filesystem::path path)
        : path_(std::move(path)) {
        if (this->path_.empty())
            throw std::invalid_argument("File path cannot be empty");
    }

    std::string File::read(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            Util::Log::error(std::format("Cannot open file: {}", path));
            return "";
        }

        std::string contents{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()};
        return contents;
    }

    const std::filesystem::path& File::path() const noexcept {
        return this->path_;
    }

    File& File::writeText(std::string_view text) {
        prepareParentDirectory(this->path_);
        std::ofstream output(this->path_, std::ios::out | std::ios::trunc);
        if (!output.is_open())
            throw std::runtime_error(std::format("Cannot open file for writing: {}", this->path_.string()));

        if (!text.empty())
            output.write(text.data(), static_cast<std::streamsize>(text.size()));
        output.flush();
        if (!output)
            throw std::runtime_error(std::format("Failed writing file: {}", this->path_.string()));
        return *this;
    }

    File& File::appendText(std::string_view text) {
        prepareParentDirectory(this->path_);
        std::ofstream output(this->path_, std::ios::out | std::ios::app);
        if (!output.is_open())
            throw std::runtime_error(std::format("Cannot open file for writing: {}", this->path_.string()));

        if (!text.empty())
            output.write(text.data(), static_cast<std::streamsize>(text.size()));
        output.flush();
        if (!output)
            throw std::runtime_error(std::format("Failed writing file: {}", this->path_.string()));
        return *this;
    }

    File& File::writeBinary(std::span<const std::byte> bytes) {
        writeBytes(this->path_, bytes, std::ios::out | std::ios::trunc);
        return *this;
    }

    File& File::appendBinary(std::span<const std::byte> bytes) {
        writeBytes(this->path_, bytes, std::ios::out | std::ios::app);
        return *this;
    }

    File& File::writeBinary(bool value) {
        return writeBinary(static_cast<std::uint8_t>(value ? 1 : 0));
    }

    File& File::writeBinary(float value) {
        static_assert(sizeof(float) == sizeof(std::uint32_t) && std::numeric_limits<float>::is_iec559);
        return writeBinary(std::bit_cast<std::uint32_t>(value));
    }

    File& File::writeBinary(double value) {
        static_assert(sizeof(double) == sizeof(std::uint64_t) && std::numeric_limits<double>::is_iec559);
        return writeBinary(std::bit_cast<std::uint64_t>(value));
    }

    File& File::appendBinary(bool value) {
        return appendBinary(static_cast<std::uint8_t>(value ? 1 : 0));
    }

    File& File::appendBinary(float value) {
        static_assert(sizeof(float) == sizeof(std::uint32_t) && std::numeric_limits<float>::is_iec559);
        return appendBinary(std::bit_cast<std::uint32_t>(value));
    }

    File& File::appendBinary(double value) {
        static_assert(sizeof(double) == sizeof(std::uint64_t) && std::numeric_limits<double>::is_iec559);
        return appendBinary(std::bit_cast<std::uint64_t>(value));
    }

    Directory::Directory(std::filesystem::path path)
        : path_(std::move(path)) {
        if (this->path_.empty())
            throw std::invalid_argument("Directory path cannot be empty");
        std::filesystem::create_directories(this->path_);
    }

    Directory Directory::folder(const std::string& name) const {
        return Directory(childPath(this->path_, name));
    }

    File Directory::file(const std::string& name) const {
        return File(childPath(this->path_, name));
    }

    Directory& Directory::file(const std::string& name, std::string_view text) {
        this->file(name).writeText(text);
        return *this;
    }

    Directory& Directory::file(const std::string& name, const std::string& text) {
        return this->file(name, std::string_view(text));
    }

    Directory& Directory::file(const std::string& name, const char* text) {
        if (!text)
            throw std::invalid_argument("Text file contents cannot be null");
        return this->file(name, std::string_view(text));
    }

    Directory& Directory::file(const std::string& name, bool value) {
        this->file(name).writeBinary(value);
        return *this;
    }

    Directory& Directory::file(const std::string& name, float value) {
        this->file(name).writeBinary(value);
        return *this;
    }

    Directory& Directory::file(const std::string& name, double value) {
        this->file(name).writeBinary(value);
        return *this;
    }

    const std::filesystem::path& Directory::path() const noexcept {
        return this->path_;
    }
}
