#pragma once
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
#include <vector>

namespace vul {
class printTable {
private:
  template <typename T>
  struct TableColumn {
    std::string header;
    std::function<std::string(const T&)> toString;
    std::optional<size_t> width = std::nullopt;
  };

  static std::string stripAnsi(const std::string& s);
  static size_t visibleLength(const std::string& s);

public:
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
  std::string s;
  try {
    s = std::format(std::locale("en_US.UTF-8"), "{:L}", ele);
  }
  catch (...) {
    s = std::to_string(ele);
  }
  std::ranges::replace(s, ',', '_');
  return s;
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
      std::print("| {:^{}} ", strings[i], width + (strings[i].size() - stripAnsi(strings[i]).size()));
    }
    std::println("|");
  }
  std::println("{:=^{}}", "", fullWidth);
}
}  // namespace vul