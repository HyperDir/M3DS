#pragma once

#include <m3ds/lib/SPhys/Spatial/Vector3.hpp>

#include <m3ds/spatial/Quaternion.hpp>

namespace M3DS {
    template <typename T> using Vec3 = SPhys::Vec3<T>;

    using Vector3 = Vec3<float>;
    using Vector3i = Vec3<int>;
}

namespace SPhys {
    template <typename T>
    constexpr std::ostream& operator<<(std::ostream& stream, const Vec3<T>& vec) {
        return stream << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
    }
}