#ifndef PRINT_TABLE_HPP
#define PRINT_TABLE_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <locale>
#include <optional>
#include <print>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace vul {
class printTable {
private:
  static std::string stripAnsi(const std::string& s);
  static size_t visibleLength(const std::string& s);

public:
  enum class Align : std::uint8_t { left, right, center };

  template <typename T>
  struct TableColumn {
    std::string header;
    std::function<std::string(const T&)> toString;
    std::optional<size_t> width = std::nullopt;
    std::optional<Align> align = std::nullopt;
  };

  template <typename T>
  static void print(const std::string& tableName,
                    const std::vector<T>& rows,
                    const std::vector<TableColumn<T>>& columns);

  static std::string version(uint32_t ver);
  static std::string boolean(bool ele);
  static std::string number(auto ele);
};

std::string printTable::number(auto ele)
{
  using T = std::remove_cvref_t<decltype(ele)>;

  if constexpr (std::is_array_v<T>) {
    std::string str = "[";
    for (size_t i = 0; i < std::size(ele); ++i) {
      if (i > 0) {
        str += ", ";
      }
      try {
        str += std::format(std::locale("en_US.UTF-8"), "{:L}", ele[i]);
      }
      catch (...) {
        str += std::to_string(ele[i]);
      }
    }
    str += "]";
    std::ranges::replace(str, ',', '_');
    return str;
  }
  else if constexpr (std::is_pointer_v<T>) {
    std::string str = "[";
    for (int i = 0; i < 2; ++i) {  // zakładamy rozmiar 2, np. Vulkan ranges
      if (i) {
        str += ", ";
      }
      str += printTable::number(ele[i]);
    }
    str += "]";
    return str;
  }
  else {
    std::string str;
    try {
      str = std::format(std::locale("en_US.UTF-8"), "{:L}", ele);
    }
    catch (...) {
      str = std::to_string(ele);
    }
    std::ranges::replace(str, ',', '_');
    return str;
  }
}

template <typename T>
void printTable::print(const std::string& tableName,
                       const std::vector<T>& rows,
                       const std::vector<TableColumn<T>>& columns)
{
  assert(!columns.empty());
  assert(!rows.empty());

  std::vector<std::tuple<const TableColumn<T>&, size_t, std::vector<std::string>>> preparedColumns;
  preparedColumns.reserve(columns.size());

  for (const auto& col : columns) {
    std::vector<std::string> strings;
    strings.reserve(rows.size());
    for (const auto& row : rows) {
      strings.push_back(col.toString(row));
    }

    size_t maxWidthString =
        std::ranges::max_element(strings, {}, [](const std::string& str) { return visibleLength(str); })->size();
    auto width = std::max({maxWidthString, col.header.size(), col.width.value_or(0)});

    preparedColumns.emplace_back(col, width, std::move(strings));
  }

  auto fullWidth = std::ranges::fold_left(preparedColumns, (columns.size() * 3) + 1,
                                          [](auto size, const auto& col) { return size + std::get<1>(col); });

  std::println("\n{:=^{}}", std::format(" {} ", tableName), fullWidth);
  for (const auto& [col, width, strings] : preparedColumns) {
    std::print("| {:^{}} ", col.header, width);
  }
  std::println("|\n{:=^{}}", "", fullWidth);

  for (size_t i = 0; i < rows.size(); i++) {
    for (const auto& [col, width, strings] : preparedColumns) {
      auto str = strings[i];
      auto myWidth = width + (strings[i].size() - stripAnsi(strings[i]).size());

      switch (col.align.value_or(Align::center)) {
      case Align::left:
        std::print("| {:<{}} ", str, myWidth);
        break;
      case Align::right:
        std::print("| {:>{}} ", str, myWidth);
        break;
      default:
      case Align::center:
        std::print("| {:^{}} ", str, myWidth);
      }
    }
    std::println("|");
  }
  std::println("{:=^{}}", "", fullWidth);
}
}  // namespace vul

#endif