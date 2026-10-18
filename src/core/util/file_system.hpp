#pragma once

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Util {
    class File {
    public:
        explicit File(std::filesystem::path path);

        static std::string read(const std::string& path);

        const std::filesystem::path& path() const noexcept;

        File& writeText(std::string_view text);
        File& appendText(std::string_view text);

        File& writeBinary(std::span<const std::byte> bytes);
        File& appendBinary(std::span<const std::byte> bytes);

        template <std::ranges::contiguous_range Range>
            requires (
                std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, std::byte>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, char>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, signed char>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, unsigned char>
            )
        File& writeBinary(const Range& bytes) {
            const std::span<const std::ranges::range_value_t<Range>> view(
                std::ranges::data(bytes), std::ranges::size(bytes));
            return writeBinary(std::as_bytes(view));
        }

        template <std::ranges::contiguous_range Range>
            requires (
                std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, std::byte>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, char>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, signed char>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, unsigned char>
            )
        File& appendBinary(const Range& bytes) {
            const std::span<const std::ranges::range_value_t<Range>> view(
                std::ranges::data(bytes), std::ranges::size(bytes));
            return appendBinary(std::as_bytes(view));
        }

        template <std::integral T>
            requires (!std::same_as<T, bool> && sizeof(T) <= sizeof(std::uint64_t))
        File& writeBinary(T value) {
            using Unsigned = std::make_unsigned_t<T>;
            Unsigned encoded = static_cast<Unsigned>(value);
            std::array<std::byte, sizeof(T)> bytes{};
            for (std::size_t i = 0; i < bytes.size(); ++i) {
                bytes[i] = static_cast<std::byte>((encoded >> (i * 8)) & Unsigned{0xff});
            }
            return writeBinary(std::span<const std::byte>(bytes));
        }

        File& writeBinary(bool value);
        File& writeBinary(float value);
        File& writeBinary(double value);

        template <std::integral T>
            requires (!std::same_as<T, bool> && sizeof(T) <= sizeof(std::uint64_t))
        File& appendBinary(T value) {
            using Unsigned = std::make_unsigned_t<T>;
            Unsigned encoded = static_cast<Unsigned>(value);
            std::array<std::byte, sizeof(T)> bytes{};
            for (std::size_t i = 0; i < bytes.size(); ++i) {
                bytes[i] = static_cast<std::byte>((encoded >> (i * 8)) & Unsigned{0xff});
            }
            return appendBinary(std::span<const std::byte>(bytes));
        }

        File& appendBinary(bool value);
        File& appendBinary(float value);
        File& appendBinary(double value);

        template <typename T>
            requires std::is_enum_v<T>
        File& writeBinary(T value) {
            return writeBinary(static_cast<std::underlying_type_t<T>>(value));
        }

        template <typename T>
            requires std::is_enum_v<T>
        File& appendBinary(T value) {
            return appendBinary(static_cast<std::underlying_type_t<T>>(value));
        }

    private:
        std::filesystem::path path_;
    };

    class Directory {
    public:
        explicit Directory(std::filesystem::path path);

        Directory folder(const std::string& name) const;
        File file(const std::string& name) const;

        Directory& file(const std::string& name, std::string_view text);
        Directory& file(const std::string& name, const std::string& text);
        Directory& file(const std::string& name, const char* text);

        template <std::ranges::contiguous_range Range>
            requires (
                std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, std::byte>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, char>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, signed char>
                || std::same_as<std::remove_cv_t<std::ranges::range_value_t<Range>>, unsigned char>
            )
        Directory& file(const std::string& name, const Range& bytes) {
            file(name).writeBinary(bytes);
            return *this;
        }

        template <std::integral T>
            requires (!std::same_as<T, bool> && sizeof(T) <= sizeof(std::uint64_t))
        Directory& file(const std::string& name, T value) {
            file(name).writeBinary(value);
            return *this;
        }

        Directory& file(const std::string& name, bool value);
        Directory& file(const std::string& name, float value);
        Directory& file(const std::string& name, double value);

        template <typename T>
            requires std::is_enum_v<T>
        Directory& file(const std::string& name, T value) {
            file(name).writeBinary(value);
            return *this;
        }

        const std::filesystem::path& path() const noexcept;

    private:
        std::filesystem::path path_;
    };
}
