#pragma once

#include "Shapes/Sphere.hpp"
#include "Shapes/AABB.hpp"
#include "Shapes/OBB.hpp"

#include "../Concepts/ContiguousRange.hpp"

#include <variant>

namespace SPhys {
    template <typename T>
    concept ShapeType3D = requires(T a, Vector3 vec) {
        { a.project(vec) } -> std::same_as<Projection3D>;
        { a.setTranslation(vec) } -> std::same_as<void>;
        { a.getTranslation() } -> std::convertible_to<Vector3>;
        { a.getBoundingBox() } -> std::same_as<BoundingBox3D>;
    };

    template <typename T>
    concept RotatingShapeType3D = ShapeType3D<T> && requires(T a, Quaternion quat) {
        { a.rotate(quat) } -> std::same_as<void>;
        { a.setRotation(quat) } -> std::same_as<void>;
        { a.getRotation() } -> std::convertible_to<Quaternion>;
    };

    template <typename T>
    concept SeparatingAxisShapeType3D = ShapeType3D<T> && requires(T a, Vector3 vec) {
        { a.getSeparationAxes() } -> ContiguousRangeOf<Vector3>;
        { a.getSeparationEdges() } -> ContiguousRangeOf<Vector3>;
        { a.getClosestPoint(vec) } -> std::same_as<Vector3>;
    };

    using Shape3D = std::variant<Sphere, AABB, OBB>;
}
