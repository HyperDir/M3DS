#pragma once

#include "BoundingBox3D.hpp"
#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector3.hpp"
#include "Projection3D.hpp"

namespace SPhys {
    struct Sphere {
        Metres<Vector3> origin {};
        Metres<float> radius {};

        constexpr void setTranslation(const Vector3& to) noexcept;
        [[nodiscard]] constexpr const Vector3& getTranslation() const noexcept;

        [[nodiscard]] constexpr Projection3D project(const Vector3& axis) const noexcept;

        [[nodiscard]] constexpr BoundingBox3D getBoundingBox() const noexcept;
    };

    constexpr void Sphere::setTranslation(const Vector3& to) noexcept {
        origin = to;
    }

    [[nodiscard]] constexpr const Vector3& Sphere::getTranslation() const noexcept {
        return origin;
    }

    constexpr Projection3D Sphere::project(const Vector3& axis) const noexcept {
        const float projectedOrigin = origin.dot(axis);

        return { projectedOrigin - radius, projectedOrigin + radius };
    }

    constexpr BoundingBox3D Sphere::getBoundingBox() const noexcept {
        return { origin - Vector3{radius}, origin + Vector3{radius} };
    }
}
