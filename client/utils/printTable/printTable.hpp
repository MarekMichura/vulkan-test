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
#include <string_view>
#include <vector>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

namespace vul {
class printTable {
private:
  static std::string stripAnsi(const std::string&);
  static size_t visibleLength(const std::string&);

public:
  enum class Align : std::uint8_t { left, right, center };

  template <typename T>
  struct TableColumn {
    std::string_view header;
    std::function<std::string(const T&)> toString;
    std::optional<size_t> width = std::nullopt;
    std::optional<Align> align = std::nullopt;
  };

private:
  template <typename T>
  struct PreparedColumnT {
    const TableColumn<T>& column;
    size_t columnWidth;
    std::vector<std::string> row;
    std::vector<size_t> rowSize;
  };

public:
  template <typename T>
  static void print(const std::string_view, const std::vector<T>&, const std::vector<TableColumn<T>>&);

  static std::string version(uint32_t);
  static std::string boolean(bool);
  template <Number T>
  static std::string number(T, char = '_');
  template <typename T>
  static std::string table(std::span<T>, char = '_');
  static std::string vendor(uint32_t);
};

template <Number T>
std::string printTable::number(T ele, char floor)
{
  std::string str;
  try {
    str = std::format(std::locale("en_US.UTF-8"), "{:L}", ele);
  }
  catch (...) {
    str = std::to_string(ele);
  }
  std::ranges::replace(str, ',', floor);
  return str;
}

template <typename T>
std::string printTable::table(std::span<T> ele, char floor)
{
  assert(ele.size() > 0);
  std::string str = "\033[1;32m[\033[0m" + number(ele[0], floor);
  for (size_t i = 1; i < ele.size(); i++) {
    str += "\033[1;31m, \033[0m" + number(ele[i], floor);
  }
  str += "\033[1;32m]\033[0m";
  return str;
}

template <typename T>
void printTable::print(const std::string_view tableName, const std::vector<T>& rows, const std::vector<TableColumn<T>>& columns)
{
  assert(!columns.empty());
  assert(!rows.empty());

  std::vector<PreparedColumnT<T>> preparedColumns;
  preparedColumns.reserve(columns.size());

  for (const TableColumn<T>& column : columns) {
    PreparedColumnT t{.column = column, .row = {}, .rowSize = {}};
    t.row.reserve(rows.size());

    for (const T& row : rows) {
      std::string rowString = column.toString(row);
      t.row.push_back(rowString);
      t.rowSize.push_back(visibleLength(rowString));
    }

    size_t tmp = std::ranges::max(t.rowSize);
    t.columnWidth = std::max({tmp, column.width.value_or(0), column.header.size()});
    preparedColumns.push_back(t);
  }
  size_t fullWidth = std::ranges::fold_left(  //
      preparedColumns, (preparedColumns.size() * 3) + 1,
      [](size_t sum, const PreparedColumnT<T>& column) { return sum + column.columnWidth; });

  std::println("\n{:=^{}}", std::format(" {} ", tableName), fullWidth);
  for (const PreparedColumnT<T>& column : preparedColumns) {
    std::print("| {:^{}} ", column.column.header, column.columnWidth);
  }
  std::println("|\n{:=^{}}", "", fullWidth);
  for (size_t i = 0; i < rows.size(); i++) {
    for (const PreparedColumnT<T>& column : preparedColumns) {
      std::string_view string = column.row.at(i);

      switch (column.column.align.value_or(Align::center)) {
      case Align::left:
        std::print("| {:<{}} ", string, column.columnWidth + (string.size() - column.rowSize.at(i)));
        break;
      case Align::right:
        std::print("| {:>{}} ", string, column.columnWidth + (string.size() - column.rowSize.at(i)));
        break;
      case Align::center:
        std::print("| {:^{}} ", string, column.columnWidth + (string.size() - column.rowSize.at(i)));
        break;
      }
    }
    std::println("|");
  }
  std::println("{:=^{}}", "", fullWidth);
}
}  // namespace vul

#endif