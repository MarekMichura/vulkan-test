#ifndef LIB_UTILS_FORMAT_ANSI
#define LIB_UTILS_FORMAT_ANSI

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace utils {
size_t strLen(std::string_view str);

enum class COLOR : std::int8_t {
  NONE = -1,
  DEFAULT = 39,
  BLACK = 30,
  RED = 31,
  GREEN = 32,
  YELLOW = 33,
  BLUE = 34,
  MAGENTA = 35,
  CYAN = 36,
  WHITE = 37,
  BRIGHT_BLACK = 90,
  BRIGHT_RED = 91,
  BRIGHT_GREEN = 92,
  BRIGHT_YELLOW = 93,
  BRIGHT_BLUE = 94,
  BRIGHT_MAGENTA = 95,
  BRIGHT_CYAN = 96,
  BRIGHT_WHITE = 97,
};
enum class BACKGROUND : std::int8_t {
  NONE = -1,
  DEFAULT = 49,
  BLACK = 40,
  RED = 41,
  GREEN = 42,
  YELLOW = 43,
  BLUE = 44,
  MAGENTA = 45,
  CYAN = 46,
  WHITE = 47,
  BRIGHT_BLACK = 100,
  BRIGHT_RED = 101,
  BRIGHT_GREEN = 102,
  BRIGHT_YELLOW = 103,
  BRIGHT_BLUE = 104,
  BRIGHT_MAGENTA = 105,
  BRIGHT_CYAN = 106,
  BRIGHT_WHITE = 107,
};

struct AnsiData {
  bool bold = false;
  bool dark = false;
  bool italic = false;
  bool underline = false;
  bool blinking_slow = false;
  bool blinking_fast = false;
  bool negative = false;
  bool hidden = false;
  bool strike = false;

  COLOR color = COLOR::NONE;
  BACKGROUND background = BACKGROUND::NONE;
};

template <typename T>
static consteval std::string_view code(T data)
{
  if constexpr (std::is_same_v<T, COLOR>) {
    // clang-format off
    switch (data) {
      case COLOR::DEFAULT: return "39";
      case COLOR::BLACK: return "30";
      case COLOR::RED: return "31";
      case COLOR::GREEN: return "32";
      case COLOR::YELLOW: return "33";
      case COLOR::BLUE: return "34";
      case COLOR::MAGENTA: return "35";
      case COLOR::CYAN: return "36";
      case COLOR::WHITE: return "37";
      case COLOR::BRIGHT_BLACK: return "90";
      case COLOR::BRIGHT_RED: return "91";
      case COLOR::BRIGHT_GREEN: return "92";
      case COLOR::BRIGHT_YELLOW: return "93";
      case COLOR::BRIGHT_BLUE: return "94";
      case COLOR::BRIGHT_MAGENTA: return "95";
      case COLOR::BRIGHT_CYAN: return "96";
      case COLOR::BRIGHT_WHITE: return "97";
      default: return "";
    }
    // clang-format on
  }
  else if constexpr (std::is_same_v<T, BACKGROUND>) {
    // clang-format off
    switch (data) {
      case BACKGROUND::DEFAULT: return "49";
      case BACKGROUND::BLACK: return "40";
      case BACKGROUND::RED: return "41";
      case BACKGROUND::GREEN: return "42";
      case BACKGROUND::YELLOW: return "43";
      case BACKGROUND::BLUE: return "44";
      case BACKGROUND::MAGENTA: return "45";
      case BACKGROUND::CYAN: return "46";
      case BACKGROUND::WHITE: return "47";
      case BACKGROUND::BRIGHT_BLACK: return "100";
      case BACKGROUND::BRIGHT_RED: return "101";
      case BACKGROUND::BRIGHT_GREEN: return "102";
      case BACKGROUND::BRIGHT_YELLOW: return "103";
      case BACKGROUND::BRIGHT_BLUE: return "104";
      case BACKGROUND::BRIGHT_MAGENTA: return "105";
      case BACKGROUND::BRIGHT_CYAN: return "106";
      case BACKGROUND::BRIGHT_WHITE: return "107";
      default: return "";
    }
    // clang-format on
  }
}

template <AnsiData data = {}>
consteval std::string_view ansi()  // NOLINT(readability-function-cognitive-complexity)
{
  constexpr std::string_view color = code(data.color);
  constexpr std::string_view background = code(data.background);
  constexpr std::string_view bold = data.bold ? "1" : "";
  constexpr std::string_view dark = data.dark ? "2" : "";
  constexpr std::string_view italic = data.italic ? "3" : "";
  constexpr std::string_view underline = data.underline ? "4" : "";
  constexpr std::string_view blinking_slow = data.blinking_slow ? "5" : "";
  constexpr std::string_view blinking_fast = data.blinking_fast ? "6" : "";
  constexpr std::string_view negative = data.negative ? "7" : "";
  constexpr std::string_view hidden = data.hidden ? "8" : "";
  constexpr std::string_view strike = data.strike ? "9" : "";

  constexpr size_t elements = (color.empty() ? 0U : 1U) +          //
                              (background.empty() ? 0U : 1U) +     //
                              (bold.empty() ? 0U : 1U) +           //
                              (dark.empty() ? 0U : 1U) +           //
                              (italic.empty() ? 0U : 1U) +         //
                              (underline.empty() ? 0U : 1U) +      //
                              (blinking_slow.empty() ? 0U : 1U) +  //
                              (blinking_fast.empty() ? 0U : 1U) +  //
                              (negative.empty() ? 0U : 1U) +       //
                              (hidden.empty() ? 0U : 1U) +         //
                              (strike.empty() ? 0U : 1U);          //
  constexpr size_t length = color.length() + background.length() + bold.length() + italic.length() + underline.length() +
                            blinking_slow.length() + blinking_fast.length() + negative.length() + hidden.length() + strike.length() +
                            elements + 3;

  if constexpr (elements == 0) {
    return {"\033[0m"};
  }

  static constexpr auto buf = [&]() {  // NOLINT(readability-function-cognitive-complexity)
    size_t pos = 0;
    std::array<char, length> result{};
    result.at(pos++) = '\033';
    result.at(pos++) = '[';

    if constexpr (!color.empty()) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : color) {
        result.at(pos++) = token;
      }
    }
    if constexpr (!background.empty()) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : background) {
        result.at(pos++) = token;
      }
    }

    if constexpr (data.bold) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : bold) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.dark) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : dark) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.italic) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : italic) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.underline) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : underline) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.blinking_slow) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : blinking_slow) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.blinking_fast) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : blinking_fast) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.negative) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : negative) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.hidden) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : hidden) {
        result.at(pos++) = token;
      }
    }
    if constexpr (data.strike) {
      if (pos > 2) {
        result.at(pos++) = ';';
      }
      for (auto token : strike) {
        result.at(pos++) = token;
      }
    }

    result.at(pos++) = 'm';
    return result;
  }();

  return std::string_view(buf.data(), length);
}

template <AnsiData data, size_t N>
constexpr auto ansi(const std::array<char, N> str)
{
  constexpr auto open = ansi<data>();
  constexpr auto close = ansi();

  std::array<char, open.size() + close.size() + N - 1> result{};
  size_t pos = 0;

  for (size_t j = 0; j < open.size() - 1; j++) {
    result.at(pos++) = open.at(j);
  }
  for (size_t j = 0; j < N - 1; ++j) {
    result.at(pos++) = str.at(j);
  }
  for (const char token : close) {
    result.at(pos++) = token;
  }

  return result;
}

template <AnsiData data, size_t N>
constexpr auto ansi(const char (&str)[N])  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
{
  return ansi<data>(std::to_array(str));
}

}  // namespace utils

#endif /* LIB_UTILS_FORMAT_ANSI */
