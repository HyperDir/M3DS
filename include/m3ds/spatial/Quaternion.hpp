#pragma once

#include <m3ds/lib/SPhys/Spatial/Quaternion.hpp>

#include <m3ds/spatial/Vector3.hpp>

namespace M3DS {
    template <std::floating_point T> using Quat = SPhys::Quat<T>;

    using Quaternion = Quat<float>;
}
