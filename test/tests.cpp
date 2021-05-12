#define CATCH_CONFIG_MAIN // This tells the Catch2 header to generate a main

#include "catch.hpp"

#include <mylib/my_lib.hpp>

////////////////////////////////////////////////////////////////
TEST_CASE("Test1", "[test][tags]")
{
  REQUIRE(mylib::foo() == 42);
}
