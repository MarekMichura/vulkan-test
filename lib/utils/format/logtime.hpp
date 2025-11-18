#ifndef LIB_UTILS_FORMAT_LOGTIME
#define LIB_UTILS_FORMAT_LOGTIME

#include <chrono>
#include <string>
#include <string_view>

namespace utils {
class LogTime {
public:
  LogTime(const LogTime&) = delete;
  LogTime(LogTime&&) = delete;
  LogTime& operator=(const LogTime&) = delete;
  LogTime& operator=(LogTime&&) = delete;

  explicit LogTime(std::string_view message);
  ~LogTime() noexcept;

private:
  std::string _message;
  std::chrono::time_point<std::chrono::steady_clock> _created;
};
}  // namespace utils

#endif /* LIB_UTILS_FORMAT_LOGTIME */
