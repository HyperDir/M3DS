#pragma once

#include <cmath>

#include "../Spatial/Units.hpp"

namespace SPhys {
    struct Projection2D {
        Pixels<float> from {};
        Pixels<float> to {};

        [[nodiscard]] constexpr Pixels<float> getOverlap(const Projection2D& with) const noexcept;
        [[nodiscard]] constexpr bool contains(const Projection2D& other) const noexcept;
    };

    constexpr Pixels<float> Projection2D::getOverlap(const Projection2D& with) const noexcept {
        return std::min(to, with.to) - std::max(from, with.from);
    }

    constexpr bool Projection2D::contains(const Projection2D& other) const noexcept {
        return from <= other.from && to >= other.to;
    }
}
