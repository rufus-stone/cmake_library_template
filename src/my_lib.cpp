#include "mylib/my_lib.hpp"

#include <spdlog/spdlog.h>

#include "mylib/utils.hpp"

namespace mylib
{

auto foo() -> int
{
  spdlog::info("foo()");
  return 40 + bar();
}

} // namespace mylib
