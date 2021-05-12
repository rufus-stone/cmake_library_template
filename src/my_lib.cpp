#include "mylib/my_lib.hpp"

#include "mylib/utils.hpp"

namespace mylib
{

auto foo() -> int
{
  return 40 + bar();
}

} // namespace mylib
