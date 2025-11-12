#ifndef LIB_UTILS_FORMAT_TABLE
#define LIB_UTILS_FORMAT_TABLE

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <execution>
#include <functional>
#include <numeric>
#include <optional>
#include <print>
#include <string_view>
#include <vector>

#include "format/ansi.hpp"

namespace utils {
enum class Align : std::uint8_t { left, right, center };

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
void table(std::string_view tableName, Rows&& rows, Columns&& columns)
{
  assert(!rows.empty());
  assert(columns.size() != 0);
  static constexpr auto boldText = ansi<{.bold = true}>();
  static constexpr auto normalText = ansi<{}>();
  struct PreparedColumnT {
    std::string_view title;
    size_t columnWidth;
    Align align;
    std::vector<std::pair<std::string, size_t>> data{};
  };

  std::vector<PreparedColumnT> preparedColumns;
  preparedColumns.reserve(columns.size());
  for (const TableColumn<T>& column : columns) {
    std::vector<std::pair<std::string, size_t>> data;
    data.reserve(rows.size());
    size_t columnWidth = std::max(column.minWidth.value_or(0), column.title.length());

    for (const T& row : rows) {
      std::string value = column.toString(row);
      size_t length = utils::strLen(value);
      columnWidth = std::max(columnWidth, length);
      data.push_back({std::move(value), length});
    }

    preparedColumns.emplace_back(column.title, columnWidth, column.align.value_or(Align::center), std::move(data));
  }
  size_t width = std::transform_reduce(  //
      std::execution::seq, preparedColumns.begin(), preparedColumns.end(), size_t(columns.size() * 3 + 1), std::plus<>(),
      [](const PreparedColumnT& ele) -> size_t { return ele.columnWidth; });

  std::println("\n{}{:=^{}}", boldText, std::format(" {} ", tableName), width);
  for (const PreparedColumnT& column : preparedColumns) {
    std::print("| {:^{}} ", column.title, column.columnWidth);
  }
  std::println("|\n{:=^{}}{}", "", width, normalText);
  for (size_t i = 0; i < rows.size(); ++i) {
    for (const PreparedColumnT& column : preparedColumns) {
      auto& [data, length] = column.data.at(i);
      size_t width = column.columnWidth + (data.size() - length);

      switch (column.align) {
      case Align::left:
        std::print("{}|{} {:<{}} ", boldText, normalText, data, width);
        break;
      case Align::right:
        std::print("{}|{} {:>{}} ", boldText, normalText, data, width);
        break;
      case Align::center:
        std::print("{}|{} {:^{}} ", boldText, normalText, data, width);
        break;
      }
    }
    std::println("{}|{}", boldText, normalText);
  }
  std::println("{}{:=^{}}{}\n", boldText, "", width, normalText);
}
}  // namespace utils

#endif /* LIB_UTILS_FORMAT_TABLE */
