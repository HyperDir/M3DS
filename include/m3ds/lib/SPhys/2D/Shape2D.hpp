#pragma once

#include "Shapes/Circle2D.hpp"
#include "Shapes/AARect2D.hpp"
#include "Shapes/Rect2D.hpp"
// #include "Shapes/Triangle2D.hpp"

#include "../Concepts/ContiguousRange.hpp"

#include <variant>

namespace SPhys {
    template <typename T>
    concept ShapeType2D = requires(T a, Vector2 vec) {
        { a.project(vec) } -> std::same_as<Projection2D>;
        { a.setTranslation(vec) } -> std::same_as<void>;
        { a.getTranslation() } -> std::convertible_to<Vector2>;
        { a.getBoundingBox() } -> std::same_as<BoundingBox2D>;
    };

    template <typename T>
    concept RotatingShapeType2D = ShapeType2D<T> && requires(T a, float f) {
        { a.rotate(f) } -> std::same_as<void>;
        { a.setRotation(f) } -> std::same_as<void>;
        { a.getRotation() } -> std::convertible_to<float>;
    };

    template <typename T>
    concept SeparatingAxisShapeType2D = ShapeType2D<T> && requires(T a, Vector2 vec) {
        { a.getSeparationAxes() } -> ContiguousRangeOf<Vector2>;
        { a.getClosestPoint(vec) } -> std::same_as<Vector2>;
    };

    using Shape2D = std::variant<Circle2D, AARect2D, Rect2D>;
}
