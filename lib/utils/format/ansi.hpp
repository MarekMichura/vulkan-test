#ifndef LIB_UTILS_FORMAT_ANSI
#define LIB_UTILS_FORMAT_ANSI

#include <array>
#include <cstddef>
#include <string_view>

namespace utils {
size_t strLen(const std::string_view str);

enum class COLOR {
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
enum class BACKGROUND {
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

namespace {
template <typename T>
consteval std::string_view code(T data)
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
}  // namespace

template <AnsiData data>
consteval std::string_view ansi()
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

  constexpr size_t elements = (color.empty() ? 0u : 1u) +          //
                              (background.empty() ? 0u : 1u) +     //
                              (bold.empty() ? 0u : 1u) +           //
                              (dark.empty() ? 0u : 1u) +           //
                              (italic.empty() ? 0u : 1u) +         //
                              (underline.empty() ? 0u : 1u) +      //
                              (blinking_slow.empty() ? 0u : 1u) +  //
                              (blinking_fast.empty() ? 0u : 1u) +  //
                              (negative.empty() ? 0u : 1u) +       //
                              (hidden.empty() ? 0u : 1u) +         //
                              (strike.empty() ? 0u : 1u);          //
  constexpr size_t length = color.length() + background.length() + bold.length() + italic.length() + underline.length() +
                            blinking_slow.length() + blinking_fast.length() + negative.length() + hidden.length() + strike.length() +
                            elements + 3;

  if constexpr (elements == 0) {
    return std::string_view("\033[0m");
  }

  static constexpr auto buf = [&]() {
    size_t pos = 0;
    std::array<char, length> result{};
    result[pos++] = '\033';
    result[pos++] = '[';

    if constexpr (!color.empty()) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : color) {
        result[pos++] = c;
      }
    }
    if constexpr (!background.empty()) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : background) {
        result[pos++] = c;
      }
    }

    if constexpr (data.bold) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : bold) {
        result[pos++] = c;
      }
    }
    if constexpr (data.dark) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : dark) {
        result[pos++] = c;
      }
    }
    if constexpr (data.italic) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : italic) {
        result[pos++] = c;
      }
    }
    if constexpr (data.underline) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : underline) {
        result[pos++] = c;
      }
    }
    if constexpr (data.blinking_slow) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : blinking_slow) {
        result[pos++] = c;
      }
    }
    if constexpr (data.blinking_fast) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : blinking_fast) {
        result[pos++] = c;
      }
    }
    if constexpr (data.negative) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : negative) {
        result[pos++] = c;
      }
    }
    if constexpr (data.hidden) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : hidden) {
        result[pos++] = c;
      }
    }
    if constexpr (data.strike) {
      if (pos > 2) {
        result[pos++] = ';';
      }
      for (auto c : strike) {
        result[pos++] = c;
      }
    }

    result[pos++] = 'm';
    return result;
  }();

  return std::string_view(buf.data(), length);
}

template <AnsiData data, size_t N>
constexpr auto ansi(const char (&str)[N])
{
  constexpr auto open = ansi<data>();
  constexpr auto close = ansi<{}>();

  std::array<char, open.size() + close.size() + N - 1> result{};
  size_t i = 0;

  for (size_t j = 0; j < open.size() - 1; j++) {
    result[i++] = open[j];
  }
  for (size_t j = 0; j < N - 1; ++j) {
    result[i++] = str[j];
  }
  for (char c : close)
    result[i++] = c;

  return result;
}

}  // namespace utils

#endif /* LIB_UTILS_FORMAT_ANSI */
