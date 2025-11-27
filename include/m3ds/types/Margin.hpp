#pragma once

#include <m3ds/spatial/Vector2.hpp>

namespace M3DS {
    struct Margin {
        float left {};
        float right {};
        float bottom {};
        float top {};

        constexpr bool operator==(const Margin&) const noexcept = default;

        [[nodiscard]] constexpr Vector2 getSize() const noexcept { return { left + right, top + bottom }; }
        [[nodiscard]] constexpr Vector2 getPosition() const noexcept { return { left, top }; }
    };
}
