#pragma once

#include <iostream>
#include <cmath>

#include "Arithmetic.hpp"

namespace SPhys {
    template <typename T>
    struct Vec3;
}

#include "Quaternion.hpp"

namespace SPhys {
    template <typename T>
    struct Vec3 {
        T x{}, y{}, z{};

        constexpr Vec3() noexcept = default;

        explicit constexpr Vec3(T size) noexcept;
        constexpr Vec3(T x, T y, T z) noexcept;

        template <typename U>
        [[nodiscard]] explicit constexpr operator Vec3<U>() const noexcept;

        [[nodiscard]] constexpr T lengthSquared() const noexcept;
        [[nodiscard]] constexpr auto length() const noexcept;

        [[nodiscard]] constexpr double manhattanLength() const noexcept;

        [[nodiscard]] constexpr Vec3 abs() const noexcept;
        
        [[nodiscard]] constexpr Vec3 normalise() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec3 floor() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec3 round() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec3 ceil() const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec3 clampToRadius(T radius) const noexcept requires std::floating_point<T>;
        [[nodiscard]] constexpr Vec3 clamp(const Vec3& lo, const Vec3& hi) const noexcept;

        [[nodiscard]] constexpr T distanceSquaredTo(const Vec3& other) const noexcept;
        [[nodiscard]] constexpr auto distanceTo(const Vec3& other) const noexcept;

        [[nodiscard]] constexpr T dot(const Vec3& other) const noexcept;
        [[nodiscard]] constexpr Vec3 cross(const Vec3& other) const noexcept;

        [[nodiscard]] constexpr bool operator==(const Vec3& other) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const Vec3& other) const noexcept = default;
        [[nodiscard]] constexpr auto operator<=>(const Vec3& other) const noexcept = default;

        constexpr Vec3& operator+=(const Vec3& other) noexcept;
        constexpr Vec3& operator-=(const Vec3& other) noexcept;
        constexpr Vec3& operator*=(const Vec3& other) noexcept;
        constexpr Vec3& operator/=(const Vec3& other) noexcept;

        constexpr Vec3& operator*=(T other) noexcept;
        constexpr Vec3& operator/=(T other) noexcept;

        constexpr Vec3 operator+(const Vec3& other) const noexcept;
        constexpr Vec3 operator-(const Vec3& other) const noexcept;
        constexpr Vec3 operator*(const Vec3& other) const noexcept;
        constexpr Vec3 operator/(const Vec3& other) const noexcept;

        [[nodiscard]] constexpr Vec3 operator-() const noexcept;

        [[nodiscard]] constexpr Vec3 rotated(const Vec3& axis, float angle) const noexcept;
        [[nodiscard]] constexpr Vec3 rotated(const Quat<T>& quat) const noexcept requires std::floating_point<T>;

        [[nodiscard]] constexpr T& operator[](int idx) noexcept;
        [[nodiscard]] constexpr const T& operator[](int idx) const noexcept;

        [[nodiscard]] constexpr explicit operator bool() const noexcept;

        [[nodiscard]] constexpr bool isEqualApprox(const Vec3& other) const noexcept requires std::floating_point<T>;
    };

    using Vector3 = Vec3<float>;
    using Vector3i = Vec3<int>;
    

    template <typename T>
    constexpr Vec3<T>::Vec3(T size) noexcept : x(size), y(size), z(size) {}

    template <typename T>
    constexpr Vec3<T>::Vec3(T x, T y, T z) noexcept : x(x), y(y), z(z) {}

    template <typename T>
    template <typename U>
    constexpr Vec3<T>::operator Vec3<U>() const noexcept {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(z)
        };
    }

    template <typename T>
    constexpr T Vec3<T>::lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }

    template <typename T>
    constexpr auto Vec3<T>::length() const noexcept {
        return std::sqrt(lengthSquared());
    }

    template <typename T>
    constexpr double Vec3<T>::manhattanLength() const noexcept {
        return std::abs(x) + std::abs(y) + std::abs(z);
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::abs() const noexcept {
        return {
            std::abs(x),
            std::abs(y),
            std::abs(z)
        };
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::normalise() const noexcept requires std::floating_point<T> {
        return *this / length();
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::floor() const noexcept requires std::floating_point<T> {
        return {
            std::floor(x),
            std::floor(y),
            std::floor(z)
        };
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::round() const noexcept requires std::floating_point<T> {
        return {
            std::round(x),
            std::round(y),
            std::round(z)
        };
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::ceil() const noexcept requires std::floating_point<T> {
        return {
            std::ceil(x),
            std::ceil(y),
            std::ceil(z)
        };
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::clampToRadius(const T radius) const noexcept requires std::floating_point<T> {
        const auto l = lengthSquared();
        if (l > radius * radius) {
            return *this / std::sqrt(l) * radius;
        }
        return *this;
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::clamp(const Vec3& lo, const Vec3& hi) const noexcept {
        return {
            std::clamp(x, lo.x, hi.x),
            std::clamp(y, lo.y, hi.y),
            std::clamp(z, lo.z, hi.z)
        };
    }
    

    template <typename T>
    constexpr T Vec3<T>::distanceSquaredTo(const Vec3& other) const noexcept {
        return (*this - other).lengthSquared();
    }

    template <typename T>
    constexpr auto Vec3<T>::distanceTo(const Vec3& other) const noexcept {
        return std::sqrt(distanceSquaredTo(other));
    }

    template <typename T>
    constexpr T Vec3<T>::dot(const Vec3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::cross(const Vec3& other) const noexcept {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    template <typename T>
    constexpr Vec3<T>& Vec3<T>::operator+=(const Vec3& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    template <typename T>
    constexpr Vec3<T>& Vec3<T>::operator-=(const Vec3& other) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    template <typename T>
    constexpr Vec3<T>& Vec3<T>::operator*=(const Vec3& other) noexcept {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    template <typename T>
    constexpr Vec3<T>& Vec3<T>::operator/=(const Vec3& other) noexcept {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    template <typename T>
    constexpr Vec3<T>& Vec3<T>::operator*=(const T other) noexcept {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    template <typename T>
    constexpr Vec3<T>& Vec3<T>::operator/=(const T other) noexcept {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::operator+(const Vec3& other) const noexcept {
        return Vec3{*this} += other;
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::operator-(const Vec3& other) const noexcept {
        return Vec3{*this} -= other;
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::operator*(const Vec3& other) const noexcept {
        return Vec3{*this} *= other;
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::operator/(const Vec3& other) const noexcept {
        return Vec3{*this} /= other;
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::operator-() const noexcept {
        return { -x, -y, -z };
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::rotated(const Vec3& axis, const float angle) const noexcept {
        return *this * std::cos(angle) + axis.cross(*this) * std::sin(angle) + axis * axis.dot(*this) * (1 - std::cos(angle));
    }

    template <typename T>
    constexpr Vec3<T> Vec3<T>::rotated(const Quat<T>& quat) const noexcept requires std::floating_point<T> {
        return quat.rotate(*this);
    }

    template <typename T>
    constexpr T& Vec3<T>::operator[](int idx) noexcept {
        if consteval {
            return idx == 0 ? x : (idx == 1 ? y : z);
        }
        return (&x)[idx];
    }

    template <typename T>
    constexpr const T& Vec3<T>::operator[](int idx) const noexcept {
        if consteval {
            return idx == 0 ? x : (idx == 1 ? y : z);
        }
        return (&x)[idx];
    }

    template <typename T>
    constexpr Vec3<T>::operator bool() const noexcept {
        return x || y || z;
    }

    template <typename T>
    constexpr bool Vec3<T>::isEqualApprox(const Vec3& other) const noexcept requires std::floating_point<T> {
        return SPhys::isEqualApprox(x, other.x) && SPhys::isEqualApprox(y, other.y) && SPhys::isEqualApprox(z, other.z);
    }

    template <typename T>
    constexpr Vec3<T> operator*(const Vec3<T>& lhs, const T rhs) {
        return Vec3{lhs} *= rhs;
    }

    template <typename T>
    constexpr Vec3<T> operator/(const Vec3<T>& lhs, const T rhs) {
        return Vec3{lhs} /= rhs;
    }

    template <typename T>
    constexpr Vec3<T> operator*(const T lhs, const Vec3<T>& rhs) {
        return Vec3{rhs} *= lhs;
    }

    template <typename T>
    constexpr Vec3<T> operator/(const T lhs, const Vec3<T>& rhs) {
        return {
            lhs / rhs.x,
            lhs / rhs.y,
            lhs / rhs.z
        };
    }


    


    template <typename T>
    constexpr Vec3<T> max(const Vec3<T>& left, const Vec3<T>& right) {
        return {
            std::max(left.x, right.x),
            std::max(left.y, right.y),
            std::max(left.z, right.z)
        };
    }
    template <typename T>
    constexpr Vec3<T> min(const Vec3<T>& left, const Vec3<T>& right) {
        return {
            std::min(left.x, right.x),
            std::min(left.y, right.y),
            std::min(left.z, right.z)
        };
    }
    template <typename T>
    constexpr Vec3<T> clamp(const Vec3<T>& vec, const Vec3<T>& lo, const Vec3<T>& hi) {
        return vec.clamp(lo, hi);
    }

    template <typename T>
    constexpr Vec3<T> floor(const Vec3<T>& vec) {
        return vec.floor();
    }
    template <typename T>
    constexpr Vec3<T> round(const Vec3<T>& vec) {
        return vec.round();
    }
    template <typename T>
    constexpr Vec3<T> ceil(const Vec3<T>& vec) {
        return vec.ceil();
    }

    template <typename T>
    constexpr Vec3<T> lerp(const Vec3<T>& from, const Vec3<T>& to, const float progress) {
        return from + (to - from) * progress;
    }
}

template <typename T>
struct std::hash<SPhys::Vec3<T>> {
    constexpr size_t operator()(const SPhys::Vec3<T>& v) const noexcept {
        const size_t hx = std::hash<T>{}(v.x);
        const size_t hy = std::hash<T>{}(v.y);
        const size_t hz = std::hash<T>{}(v.z);
        return hx ^ (hy << 1) ^ (hz << 2);
    }
};

template <typename T>
struct std::formatter<SPhys::Vec3<T>> : std::formatter<T> {
    template <typename FormatContext>
    auto format(const SPhys::Vec3<T>& vec, FormatContext& ctx) const {
        auto&& out = ctx.out();

        std::format_to(out, "(");

        std::formatter<T>::format(vec.x, ctx);
        std::format_to(out, ", ");
        std::formatter<T>::format(vec.y, ctx);
        std::format_to(out, ", ");
        std::formatter<T>::format(vec.z, ctx);

        return std::format_to(out, ")");
    }
};

template <typename T>
constexpr std::ostream& operator<<(std::ostream& stream, const SPhys::Vec3<T>& vec) {
    std::format_to(std::ostreambuf_iterator{stream}, "{}", vec);
    return stream;
}
