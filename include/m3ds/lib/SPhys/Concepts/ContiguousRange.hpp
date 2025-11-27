#pragma once

#include <ranges>

namespace SPhys {
    template <typename Range, typename Of>
    concept ContiguousRangeOf =
        std::ranges::contiguous_range<Range> &&
        std::convertible_to<std::remove_reference_t<std::ranges::range_reference_t<Range>>, Of>;
}
