#ifndef LIB_UTILS_FORMAT_TABLE
#define LIB_UTILS_FORMAT_TABLE

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <execution>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <ostream>
#include <print>
#include <sstream>
#include <string_view>
#include <vector>

#include "ansi.hpp"
#include "debug.hpp"
#include "format/ansi.hpp"

namespace utils {
enum class Align : std::uint8_t { left, right, center };

void print(std::string str);

template <typename T>
struct TableColumn {
  std::string_view title;
  std::optional<size_t> minWidth = std::nullopt;
  std::optional<Align> align = std::nullopt;
  std::function<std::string(const T&)> toString;
};

template <typename T, std::ranges::range Rows, std::ranges::range Columns>
  requires std::same_as<std::ranges::range_value_t<Rows>, T> &&  //
           std::same_as<std::ranges::range_value_t<Columns>, TableColumn<T>>
void table(std::string_view tableName, const Rows& rows, const Columns& columns)
{
  assert(!rows.empty());
  assert(!columns.empty());
  static constexpr auto boldText = ansi<{.bold = true}>();
  static constexpr auto normalText = ansi();
  struct PreparedColumnT {
    std::string_view title;
    size_t columnWidth = 0;
    Align align = Align::center;
    std::vector<std::pair<std::string, size_t>> data;
  };

  std::vector<PreparedColumnT> preparedColumns;
  preparedColumns.reserve(columns.size());
  for (const TableColumn<T>& column : columns) {
    std::vector<std::pair<std::string, size_t>> data;
    data.reserve(rows.size());
    size_t columnWidth = std::max(column.minWidth.value_or(0), column.title.length());

    for (const T& row : rows) {
      std::string value = column.toString(row);
      const size_t length = utils::strLen(value);
      columnWidth = std::max(columnWidth, length);
      data.emplace_back(std::move(value), length);
    }

    preparedColumns.emplace_back(column.title, columnWidth, column.align.value_or(Align::center), std::move(data));
  }
  size_t totalWidth = std::transform_reduce(  //
      std::execution::seq, preparedColumns.begin(), preparedColumns.end(), size_t((columns.size() * 3) + 1), std::plus<>(),
      [](const PreparedColumnT& ele) -> size_t { return ele.columnWidth; });

  std::ostringstream builder;
  builder << std::format("\n{}{:=^{}}\n", boldText, std::format(" {} ", tableName), totalWidth);
  for (const PreparedColumnT& column : preparedColumns) {
    builder << std::format("| {:^{}} ", column.title, column.columnWidth);
  }
  builder << std::format("|\n{:=^{}}{}\n", "", totalWidth, normalText);
  for (size_t i = 0; i < rows.size(); ++i) {
    for (const PreparedColumnT& column : preparedColumns) {
      auto& [data, length] = column.data.at(i);
      size_t width = column.columnWidth + (data.size() - length);

      switch (column.align) {
      case Align::left:
        builder << std::format("{}|{} {:<{}} ", boldText, normalText, data, width);
        break;
      case Align::right:
        builder << std::format("{}|{} {:>{}} ", boldText, normalText, data, width);
        break;
      case Align::center:
        builder << std::format("{}|{} {:^{}} ", boldText, normalText, data, width);
        break;
      }
    }
    builder << std::format("{}|{}\n", boldText, normalText);
  }
  builder << std::format("{}{:=^{}}{}\n", boldText, "", totalWidth, normalText);
  print(builder.str());
}
}  // namespace utils

#endif /* LIB_UTILS_FORMAT_TABLE */
