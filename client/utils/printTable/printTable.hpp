#include <cassert>
#include <format>
#include <functional>
#include <optional>
#include <print>
#include <string>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <tuple>
#include <vector>
#include "types.h"

namespace vul {
template <typename T>
struct TableColumn {
  std::string header;
  std::function<std::string(const T&)> toString;

  std::optional<uint32> width = std::nullopt;
};

template <typename T>
void printTable(const std::string& tableName, const std::vector<T>& rows, const std::vector<TableColumn<T>>& columns)
{
  assert(!columns.empty());
  std::vector<std::tuple<TableColumn<T>, uint32>> trueColumns;
  trueColumns.reserve(columns.size());

  for (uint32 i = 0; i < columns.size(); i++) {
    auto col = columns[i];
    auto max =
        std::ranges::max(std::views::transform(rows, [&col](const auto& row) { return col.toString(row).size(); }));

    trueColumns.emplace_back(col, std::max({max, col.header.size(), static_cast<size_t>(col.width.value_or(0))}));
  }

  uint32 fullWidth =
      std::accumulate(trueColumns.begin(), trueColumns.end(), (static_cast<uint32>(trueColumns.size()) * 3) + 1,
                      [](size_t sum, const auto& col) { return sum + std::get<1>(col); });

  std::println("{:=^{}s}", tableName, fullWidth);
  for (const auto& [col, width] : trueColumns) {
    std::print("| {:^{}} ", col.header, width);
  }
  std::println("|\n{:=^{}s}", "", fullWidth);

  for (const auto& row : rows) {
    for (const auto& [col, width] : trueColumns) {
      std::print("| {:^{}} ", col.toString(row), width);
    }
    std::println("|");
  }
  std::println("{:=^{}s}", "", fullWidth);
}
}  // namespace vul