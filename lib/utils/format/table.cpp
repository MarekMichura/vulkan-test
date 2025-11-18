#include "table.hpp"

namespace utils {
void print(std::string str)
{
  static std::mutex print_mutex;
  std::scoped_lock lock(print_mutex);

  if constexpr (PrintTable) {
    std::cout << str << "\n";
  }
  else {
    static std::ofstream file("table.log", std::ios::trunc);
    file << str << "\n";
    file.flush();
  }
}
}  // namespace utils
