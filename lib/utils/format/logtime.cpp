#include "logtime.hpp"

#include <chrono>
#include <iostream>
#include <print>
#include <string_view>

#include "format/ansi.hpp"

namespace utils {
LogTime::LogTime(std::string_view message) : _message(message), _created(std::chrono::steady_clock::now()) {}

LogTime::~LogTime() noexcept
{
  try {
    static constexpr auto info = ansi<{.bold = true, .negative = true, .color = COLOR::BLACK, .background = BACKGROUND::BRIGHT_BLUE}>();
    static constexpr auto time = ansi<{.bold = true, .negative = true, .color = COLOR::DEFAULT, .background = BACKGROUND::DEFAULT}>();
    static constexpr auto end = ansi();

    auto delta = std::chrono::steady_clock::now() - _created;
    std::println("{} {:%S}: {} {} {}", time, delta, info, _message, end);
  }
  catch (...) {
    std::cerr << ("Log time allocation error\n");
  }
}

}  // namespace utils