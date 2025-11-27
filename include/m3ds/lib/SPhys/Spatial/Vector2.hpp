#pragma once

#include "Arithmetic.hpp"
#include "Units.hpp"

#include <iostream>
#include <cmath>

namespace SPhys {
    template <Arithmetic T>
    struct Vec2 {
        T x{}, y{};

        constexpr Vec2() noexcept = default;

        explicit constexpr Vec2(T size) noexcept;
        constexpr Vec2(T x, T y) noexcept;

        template <Arithmetic U>
        explicit constexpr Vec2(const Vec2<U>& other) noexcept;

        [[nodiscard]] static constexpr Vec2 fromAngle(Radians<float> angle) noexcept;

        [[nodiscard]] constexpr T lengthSquared() const noexcept;
        [[nodiscard]] constexpr auto length() const noexcept;

        [[nodiscard]] constexpr Vec2 abs() const noexcept;

        [[nodiscard]] constexpr Vec2 normalise() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec2 floor() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec2 round() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec2 ceil() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec2 clampToRadius(T radius) const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec2 clamp(const Vec2& lo, const Vec2& hi) const noexcept;

        [[nodiscard]] constexpr T distanceSquaredTo(const Vec2& other) const noexcept;
        [[nodiscard]] constexpr auto distanceTo(const Vec2& other) const noexcept;

        [[nodiscard]] constexpr T dot(const Vec2& other) const noexcept;

        [[nodiscard]] constexpr bool operator==(const Vec2& other) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const Vec2& other) const noexcept = default;
        [[nodiscard]] constexpr auto operator<=>(const Vec2& other) const noexcept = default;

        constexpr Vec2& operator+=(const Vec2& other) noexcept;
        constexpr Vec2& operator-=(const Vec2& other) noexcept;
        constexpr Vec2& operator*=(const Vec2& other) noexcept;
        constexpr Vec2& operator/=(const Vec2& other) noexcept;

        constexpr Vec2& operator*=(T other) noexcept;
        constexpr Vec2& operator/=(T other) noexcept;

        [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const noexcept;
        [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const noexcept;
        [[nodiscard]] constexpr Vec2 operator*(const Vec2& other) const noexcept;
        [[nodiscard]] constexpr Vec2 operator/(const Vec2& other) const noexcept;
        
        [[nodiscard]] constexpr Vec2 operator-() const noexcept;

        [[nodiscard]] constexpr Vec2 rotated(T angle) const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec2 rotated(T sinAngle, T cosAngle) const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr T angle() const noexcept requires std::floating_point<T>;

        [[nodiscard]] constexpr T& operator[](int idx) noexcept;
        [[nodiscard]] constexpr const T& operator[](int idx) const noexcept;

        [[nodiscard]] constexpr explicit operator bool() const noexcept;
    };

    using Vector2 = Vec2<float>;
    using Vector2i = Vec2<int>;
    

    template <Arithmetic T>
    constexpr Vec2<T>::Vec2(T size) noexcept : x(size), y(size) {}

    template <Arithmetic T>
    constexpr Vec2<T>::Vec2(T x, T y) noexcept : x(x), y(y) {}

    template <Arithmetic T>
    template <Arithmetic U>
    constexpr Vec2<T>::Vec2(const Vec2<U>& other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::fromAngle(const Radians<float> angle) noexcept {
        return { std::cos(angle), std::sin(angle) };
    }

    template <Arithmetic T>
    constexpr T Vec2<T>::lengthSquared() const noexcept {
        return x * x + y * y;
    }

    template <Arithmetic T>
    constexpr auto Vec2<T>::length() const noexcept {
        return std::sqrt(lengthSquared());
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::abs() const noexcept {
        return {
            std::abs(x),
            std::abs(y)
        };
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::normalise() const noexcept requires std::floating_point<T> {
        return *this / length();
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::floor() const noexcept requires std::floating_point<T> {
        return {
            std::floor(x),
            std::floor(y)
        };
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::round() const noexcept requires std::floating_point<T> {
        return {
            std::round(x),
            std::round(y)
        };
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::ceil() const noexcept requires std::floating_point<T> {
        return {
            std::ceil(x),
            std::ceil(y)
        };
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::clampToRadius(const T radius) const noexcept requires std::floating_point<T> {
        const auto l = lengthSquared();
        if (l > radius * radius) {
            return *this / std::sqrt(l) * radius;
        }
        return *this;
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::clamp(const Vec2& lo, const Vec2& hi) const noexcept {
        return {
            std::clamp(x, lo.x, hi.x),
            std::clamp(y, lo.y, hi.y)
        };
    }

    template <Arithmetic T>
    constexpr T Vec2<T>::distanceSquaredTo(const Vec2& other) const noexcept {
        return (*this - other).lengthSquared();
    }

    template <Arithmetic T>
    constexpr auto Vec2<T>::distanceTo(const Vec2& other) const noexcept {
        return std::sqrt(distanceSquaredTo(other));
    }

    template <Arithmetic T>
    constexpr T Vec2<T>::dot(const Vec2& other) const noexcept {
        return x * other.x + y * other.y;
    }

    template <Arithmetic T>
    constexpr Vec2<T>& Vec2<T>::operator+=(const Vec2& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <Arithmetic T>
    constexpr Vec2<T>& Vec2<T>::operator-=(const Vec2& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template <Arithmetic T>
    constexpr Vec2<T>& Vec2<T>::operator*=(const Vec2& other) noexcept {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    template <Arithmetic T>
    constexpr Vec2<T>& Vec2<T>::operator/=(const Vec2& other) noexcept {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    template <Arithmetic T>
    constexpr Vec2<T>& Vec2<T>::operator*=(T other) noexcept {
        x *= other;
        y *= other;
        return *this;
    }

    template <Arithmetic T>
    constexpr Vec2<T>& Vec2<T>::operator/=(T other) noexcept {
        x /= other;
        y /= other;
        return *this;
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::operator+(const Vec2& other) const noexcept {
        return Vec2{*this} += other;
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::operator-(const Vec2& other) const noexcept {
        return Vec2{*this} -= other;
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::operator*(const Vec2& other) const noexcept {
        return Vec2{*this} *= other;
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::operator/(const Vec2& other) const noexcept {
        return Vec2{*this} /= other;
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::operator-() const noexcept {
        return {-x, -y};
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::rotated(T angle) const noexcept requires std::floating_point<T> {
        return rotated(std::sin(angle), std::cos(angle));
    }

    template <Arithmetic T>
    constexpr Vec2<T> Vec2<T>::rotated(T sinAngle, T cosAngle) const noexcept requires std::floating_point<T> {
        return {
            x * cosAngle - y * sinAngle,
            x * sinAngle + y * cosAngle
        };
    }

    template <Arithmetic T>
    constexpr T Vec2<T>::angle() const noexcept requires std::floating_point<T> {
        return std::atan2(y, x);
    }

    template <Arithmetic T>
    constexpr T& Vec2<T>::operator[](const int idx) noexcept {
        if consteval {
            return idx == 0 ? x : y;
        }
        return &x[idx];
    }

    template <Arithmetic T>
    constexpr const T& Vec2<T>::operator[](const int idx) const noexcept {
        if consteval {
            return idx == 0 ? x : y;
        }
        return &x[idx];
    }

    template <Arithmetic T>
    constexpr Vec2<T>::operator bool() const noexcept {
        return x || y;
    }


    template <Arithmetic T>
    constexpr Vec2<T> operator*(const Vec2<T>& lhs, const T rhs) {
        return Vec2{lhs} *= rhs;
    }

    template <Arithmetic T>
    constexpr Vec2<T> operator/(const Vec2<T>& lhs, const T rhs) {
        return Vec2{lhs} /= rhs;
    }

    template <Arithmetic T>
    constexpr Vec2<T> operator*(const T lhs, const Vec2<T>& rhs) {
        return Vec2{rhs} *= lhs;
    }

    template <Arithmetic T>
    constexpr Vec2<T> operator/(const T lhs, const Vec2<T>& rhs) {
        return {
            lhs / rhs.x,
            lhs / rhs.y
        };
    }


    template <typename T>
    constexpr Vec2<T> max(const Vec2<T>& left, const Vec2<T>& right) {
        return {
            std::max(left.x, right.x),
            std::max(left.y, right.y)
        };
    }
    template <typename T>
    constexpr Vec2<T> min(const Vec2<T>& left, const Vec2<T>& right) {
        return {
            std::min(left.x, right.x),
            std::min(left.y, right.y)
        };
    }
    template <typename T>
    constexpr Vec2<T> clamp(const Vec2<T>& vec, const Vec2<T>& lo, const Vec2<T>& hi) {
        return vec.clamp(lo, hi);
    }

    template <typename T>
    constexpr Vec2<T> floor(const Vec2<T>& vec) {
        return vec.floor();
    }

    template <typename T>
    constexpr Vec2<T> round(const Vec2<T>& vec) {
        return vec.round();
    }

    template <typename T>
    constexpr Vec2<T> ceil(const Vec2<T>& vec) {
        return vec.ceil();
    }

    template <Arithmetic T>
    constexpr Vec2<T> lerp(const Vec2<T>& from, const Vec2<T>& to, const float progress) {
        return from + (to - from) * progress;
    }
}

template <typename T>
struct std::hash<SPhys::Vec2<T>> {
    constexpr size_t operator()(const SPhys::Vec2<T>& v) const noexcept {
        const size_t hx = std::hash<T>{}(v.x);
        const size_t hy = std::hash<T>{}(v.y);
        return hx ^ (hy << 1);
    }
};

template <typename T>
struct std::formatter<SPhys::Vec2<T>> : std::formatter<T> {
    auto format(const SPhys::Vec2<T>& vec, format_context& ctx) const {
        auto&& out = ctx.out();

        std::format_to(out, "(");

        std::formatter<T>::format(vec.x, ctx);
        std::format_to(out, ", ");
        std::formatter<T>::format(vec.y, ctx);

        return std::format_to(out, ")");
    }
};

template <typename T>
constexpr std::ostream& operator<<(std::ostream& stream, const SPhys::Vec2<T>& vec) {
    std::format_to(std::ostreambuf_iterator{stream}, "{}", vec);
    return stream;
}
