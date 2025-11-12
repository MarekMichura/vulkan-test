#ifndef LIB_UTILS_AVAILABLE_AVAILABLE
#define LIB_UTILS_AVAILABLE_AVAILABLE

#include <algorithm>
#include <concepts>
#include <functional>

namespace utils {
template <typename T, typename Range, typename Comp = std::equal_to<>>
  requires std::ranges::range<Range> &&  //
           std::invocable<Comp, const T&, const std::ranges::range_value_t<Range>&>
bool checkPresent(const T& find, const Range& elements, Comp comp = {})
{
  return std::ranges::find_if(elements, [&](const auto& ele) { return comp(find, ele); }) != elements.end();
}

template <std::ranges::input_range Range1, std::ranges::input_range Range2, typename Comp = std::equal_to<>>
  requires std::ranges::range<Range1> &&  //
           std::ranges::range<Range2> &&  //
           std::invocable<Comp, const std::ranges::range_value_t<Range1>&, const std::ranges::range_value_t<Range2>&>
bool checkPresent(const Range1& findList, const Range2& elements, Comp comp = {})
{
  return std::ranges::all_of(findList, [&](const auto& find) {
    return std::ranges::find_if(elements, [&](const auto& ele) { return comp(find, ele); }) != elements.end();
  });
}
}  // namespace utils

#endif /* LIB_UTILS_AVAILABLE_AVAILABLE */
