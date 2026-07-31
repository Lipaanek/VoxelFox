#include "catch_amalgamated.hpp"
#include "core/util/util.hpp"

TEST_CASE("Util::File::read returns contents for an existing file") {
    REQUIRE_FALSE(Util::File::read("README.md").empty());
}

TEST_CASE("Util::File::read returns empty string for a missing file") {
    REQUIRE(Util::File::read("does_not_exist.txt").empty());
}