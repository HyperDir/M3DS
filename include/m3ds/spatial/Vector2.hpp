#pragma once

#include <m3ds/lib/SPhys/Spatial/Vector2.hpp>
#include <m3ds/spatial/Arithmetic.hpp>

namespace M3DS {
    template <Arithmetic T> using Vec2 = SPhys::Vec2<T>;

    using Vector2 = Vec2<float>;
    using Vector2i = Vec2<int>;
}

namespace SPhys {
    template <Arithmetic T>
    constexpr std::ostream& operator<<(std::ostream& stream, const Vec2<T>& vec) {
        return stream << '(' << vec.x << ", " << vec.y << ')';
    }
}

namespace M3DS {
    struct Rect2 {
        Vector2 position {};
        Vector2 size {16.f};

        Rect2() = default;
        Rect2(float x, float y, float w, float h) : position(x, y), size(w, h) {}
        template <typename T> Rect2(const Vector2& position, const Vector2& size) : position(position), size(size) {}
    };

    inline std::ostream& operator<<(std::ostream& stream, const Rect2& rect) {
        return stream << "<" << rect.position.x << ", " << rect.position.y << ", " << rect.size.x << ", " << rect.size.y << ">";
    }
}