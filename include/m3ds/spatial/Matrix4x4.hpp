#pragma once

#include <utility>

#include <m3ds/utils/Units.hpp>
#include <m3ds/spatial/Vector3.hpp>
#include <m3ds/spatial/Quaternion.hpp>

// Derived from https://github.com/devkitPro/citro3d/tree/master/source/maths

namespace M3DS {
    class Matrix4x4 {
    public:
        static constexpr Matrix4x4 zeros() noexcept;
        static constexpr Matrix4x4 identity() noexcept;
        static constexpr Matrix4x4 diagonal(float x, float y, float z, float w) noexcept;

        static constexpr Matrix4x4 fromTranslation(const Vector3& vec) noexcept;
        static constexpr Matrix4x4 fromScale(const Vector3& vec) noexcept;

        static constexpr Matrix4x4 fromTRS(const Vector3& translation, const Quaternion& rotation, const Vector3& scale) noexcept;

        template <bool RightHanded = true>
        static constexpr Matrix4x4 lookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector) noexcept;
        static constexpr Matrix4x4 fromQuat(const Quaternion& quat) noexcept;

        template <bool RightHanded = true>
        static constexpr Matrix4x4 ortho(float left, float right, float bottom, float top, float near, float far) noexcept;
        template <bool RightHanded = true>
        static constexpr Matrix4x4 orthoTilt(float left, float right, float bottom, float top, float near, float far) noexcept;

        template <bool RightHanded = true>
        static constexpr Matrix4x4 persp(float yFov, float aspect, float near, float far) noexcept;
        template <bool RightHanded = true>
        static constexpr Matrix4x4 perspTilt(float xFov, float invAspect, float near, float far) noexcept;

        template <bool RightHanded = true>
        static constexpr Matrix4x4 perspStereo(float yFov, float aspect, float near, float far, float iod, float screen) noexcept;
        template <bool RightHanded = true>
        static constexpr Matrix4x4 perspStereoTilt(float xFov, float invAspect, float near, float far, float iod, float screen) noexcept;

        friend constexpr bool operator==(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;

        [[nodiscard]] constexpr Vector3 getTranslation() const noexcept;
        [[nodiscard]] constexpr Vector3 getScale() const noexcept;
        [[nodiscard]] constexpr Quaternion getRotation() const noexcept;

        [[nodiscard]] constexpr std::pair<Matrix4x4, float> inverseAndDeterminant() const noexcept;
        [[nodiscard]] constexpr Matrix4x4 inverse() const noexcept;
        [[nodiscard]] constexpr float determinant() const noexcept;

        friend constexpr Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;
        friend constexpr Matrix4x4& operator*=(Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;

        friend constexpr Matrix4x4 operator+(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;
        friend constexpr Matrix4x4& operator+=(Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;

        friend constexpr Matrix4x4 operator-(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;
        friend constexpr Matrix4x4& operator-=(Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;

        friend constexpr Vector3 operator*(const Matrix4x4& lhs, const Vector3& rhs) noexcept;
        friend constexpr Quaternion operator*(const Matrix4x4& lhs, const Quaternion& rhs) noexcept;

        template <bool RightSide = true> constexpr Matrix4x4& rotate(Vector3 axis, Radians<float> angle);
        template <bool RightSide = true> constexpr Matrix4x4& rotateX(Radians<float> angle);
        template <bool RightSide = true> constexpr Matrix4x4& rotateY(Radians<float> angle);
        template <bool RightSide = true> constexpr Matrix4x4& rotateZ(Radians<float> angle);

        constexpr Matrix4x4& scale(float x, float y, float z) noexcept;
        constexpr Matrix4x4& scale(const Vector3& s) noexcept;

        template <bool RightSide = true> constexpr Matrix4x4& translate(float x, float y, float z) noexcept;
        template <bool RightSide = true> constexpr Matrix4x4& translate(const Vector3& t) noexcept;

        constexpr void setTranslation(const Vector3& to) noexcept;
        constexpr void setRotation(const Quaternion& to) noexcept;

        constexpr Matrix4x4& transpose() noexcept;

        constexpr auto& w(this auto&& self) noexcept { return self[0]; }
        constexpr auto& z(this auto&& self) noexcept { return self[1]; }
        constexpr auto& y(this auto&& self) noexcept { return self[2]; }
        constexpr auto& x(this auto&& self) noexcept { return self[3]; }

        constexpr float& operator[](const unsigned int i, const unsigned int j) noexcept {
#ifdef __3DS__
            return mData[i][3 - j];
#else
            return mData[i][j];
#endif
        }
        constexpr const float& operator[](const unsigned int i, const unsigned int j) const noexcept {
#ifdef __3DS__
            return mData[i][3 - j];
#else
            return mData[i][j];
#endif
        }
        constexpr float& get(const unsigned int i, const unsigned int j) noexcept {
            return (*this)[i, j];
        }
        constexpr const float& get(const unsigned int i, const unsigned int j) const noexcept {
            return (*this)[i, j];
        }

        constexpr void setScale(const Vector3& to) noexcept;
    private:
        std::array<std::array<float, 4>, 4> mData {};

        [[nodiscard]] constexpr Matrix4x4() noexcept = default;
    };

    template <bool RightHanded>
    constexpr Matrix4x4 Matrix4x4::lookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector) noexcept {
        // Left-handed and Right-handed Look-At matrix functions are DirectX implementations.
        // Left-handed: https://msdn.microsoft.com/en-us/library/windows/desktop/bb281710
        // Right-handed: https://msdn.microsoft.com/en-us/library/windows/desktop/bb281711
        const Vector3 zAxis = [&]{
            // Order of operations is crucial.
            if constexpr (RightHanded)
                return (cameraPosition - cameraTarget).normalise();
            else
                return (cameraTarget - cameraPosition).normalise();
        }();

        const Vector3 xAxis = cameraUpVector.cross(zAxis).normalise();
        const Vector3 yAxis = zAxis.cross(xAxis);

        Matrix4x4 mtx {};

        mtx[0, 0] = xAxis.x;
        mtx[0, 1] = xAxis.y;
        mtx[0, 2] = xAxis.z;
        mtx[0, 3] = -xAxis.dot(cameraPosition);

        mtx[1, 0] = yAxis.x;
        mtx[1, 1] = yAxis.y;
        mtx[1, 2] = yAxis.z;
        mtx[1, 3] = -yAxis.dot(cameraPosition);

        mtx[2, 0] = zAxis.x;
        mtx[2, 1] = zAxis.y;
        mtx[2, 2] = zAxis.z;
        mtx[2, 3] = -zAxis.dot(cameraPosition);

        mtx[3, 0] = 0.0f;
        mtx[3, 1] = 0.0f;
        mtx[3, 2] = 0.0f;
        mtx[3, 3] = 1.0f;

        return mtx;
    }

    template <bool RightHanded>
    constexpr Matrix4x4 Matrix4x4::ortho(float left, float right, float bottom, float top, float near, float far) noexcept {
        auto mtx = Matrix4x4::zeros();

        // Standard orthogonal projection matrix, with a fixed depth range of [-1,0] (required by PICA)
        // http://www.wolframalpha.com/input/?i={{1,0,0,0},{0,1,0,0},{0,0,0.5,-0.5},{0,0,0,1}}{{2/(r-l),0,0,(l%2Br)/(l-r)},{0,2/(t-b),0,(b%2Bt)/(b-t)},{0,0,2/(n-f),(n%2Bf)/(n-f)},{0,0,0,1}}

        mtx[0, 0] = 2.0f / (right - left);
        mtx[0, 3] = (left + right) / (left - right);
        mtx[1, 1] = 2.0f / (top - bottom);
        mtx[1, 3] = (bottom + top) / (bottom - top);
        if constexpr (RightHanded) {
            mtx[2, 2] = 1.0f / static_cast<float>(near - far);
        } else {
            mtx[2, 2] = 1.0f / static_cast<float>(far - near);
        }
        mtx[2, 3] = 0.5f * (near + far) / (near - far) - 0.5f;
        mtx[3, 3] = 1.0;

        return mtx;
    }

    template <bool RightHanded>
    constexpr Matrix4x4 Matrix4x4::orthoTilt(float left, float right, float bottom, float top, float near, float far) noexcept {
        auto mtx = Matrix4x4::zeros();

        // Standard orthogonal projection matrix, with a fixed depth range of [-1,0] (required by PICA) and rotated τ/4 radians counterclockwise around the Z axis (due to 3DS screen orientation)
        // http://www.wolframalpha.com/input/?i={{0,1,0,0},{-1,0,0,0},{0,0,1,0},{0,0,0,1}}{{1,0,0,0},{0,1,0,0},{0,0,0.5,-0.5},{0,0,0,1}}
        // http://www.wolframalpha.com/input/?i={{0,1,0,0},{-1,0,0,0},{0,0,0.5,-0.5},{0,0,0,1}}{{2/(r-l),0,0,(l%2Br)/(l-r)},{0,2/(t-b),0,(b%2Bt)/(b-t)},{0,0,2/(n-f),(n%2Bf)/(n-f)},{0,0,0,1}}

        mtx[0, 1] = 2.0f / (top - bottom);
        mtx[0, 3] = (bottom + top) / (bottom - top);
        mtx[1, 0] = 2.0f / (left - right);
        mtx[1, 3] = (left + right) / (right - left);
        if constexpr (RightHanded) {
            mtx[2, 2] = 1.0f / (near - far);
        } else {
            mtx[2, 2] = 1.0f / (far - near);
        }
        mtx[2, 3] = 0.5f * (near + far) / (near - far) - 0.5f;
        mtx[3, 3] = 1.0f;

        return mtx;
    }

    template <bool RightHanded>
    constexpr Matrix4x4 Matrix4x4::persp(float yFov, float aspect, float near, float far) noexcept {
        float fovy_tan = std::tan(yFov / 2.0f);

        auto mtx = Matrix4x4::zeros();

        // Standard perspective projection matrix, with a fixed depth range of [-1,0] (required by PICA)
        // http://www.wolframalpha.com/input/?i=%7B%7B1,0,0,0%7D,%7B0,1,0,0%7D,%7B0,0,0.5,-0.5%7D,%7B0,0,0,1%7D%7D%7B%7B1%2F(a*tan(v)),0,0,0%7D,%7B0,1%2Ftan(v),0,0%7D,%7B0,0,(n%2Bf)%2F(n-f),(2fn)%2F(n-f)%7D,%7B0,0,-1,0%7D%7D

        mtx[0, 0] = 1.0f / (aspect * fovy_tan);
        mtx[1, 1] = 1.0f / fovy_tan;
        mtx[2, 3] = far * near / (near - far);
        if constexpr (RightHanded) {
            mtx[3, 2] = -1.0f;
        } else {
            mtx[3, 2] = 1.0f;
        }
        mtx[2, 2] = -mtx[3, 2] * near / (near - far);

        return mtx;
    }

    template <bool RightHanded>
    constexpr Matrix4x4 Matrix4x4::perspTilt(float xFov, float invAspect, float near, float far) noexcept {
        float fovx_tan = std::tan(xFov / 2.0f);

        auto mtx = Matrix4x4::zeros();

        mtx[0, 1] = 1.0f / fovx_tan;
        mtx[1, 0] = -1.0f / (fovx_tan * invAspect);
        mtx[2, 3] = far*near / (near - far);
        if constexpr (RightHanded) {
            mtx[3, 2] = -1.0f;
        } else {
            mtx[3, 2] = 1.0f;
        }
        mtx[2, 2] = -mtx[3, 2] * near / (near - far);

        return mtx;
    }

    template <bool RightHanded>
    constexpr Matrix4x4 Matrix4x4::perspStereo(float yFov, float aspect, float near, float far, float iod, float screen) noexcept {
        float fovy_tan = std::tan(yFov / 2.0f);
        float fovy_tan_aspect = fovy_tan * aspect;
        float shift = iod / (2.0f * screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

        auto mtx = Matrix4x4::zeros();

        mtx[0, 0] = 1.0f / fovy_tan_aspect;
        mtx[0, 3] = -iod / 2.0f;
        mtx[1, 1] = 1.0f / fovy_tan;
        mtx[2, 3] = near * far / (near - far);
        if constexpr (RightHanded) {
            mtx[3, 2] = -1.0f;
        } else {
            mtx[3, 2] = 1.0f;
        }
        mtx[0, 2] = mtx[3, 2] * shift / fovy_tan_aspect;
        mtx[2, 2] = -mtx[3, 2] * near / (near - far);

        return mtx;
    }

    template <bool RightHanded>
    constexpr Matrix4x4 Matrix4x4::perspStereoTilt(float xFov, float invAspect, float near, float far, float iod, float screen) noexcept {
        // Notes:
        // Once again, we are passed "fovy" and the "aspect ratio"; however the 3DS screens are sideways,
        // and the formula had to be tweaked. With stereo, left/right separation becomes top/bottom separation.
        // The detailed mathematical explanation is in mtx_persptilt.c.

        float fovx_tan = std::tan(xFov / 2.0f);
        float fovx_tan_invaspect = fovx_tan * invAspect;
        float shift = iod / (2.0f * screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

        auto mtx = Matrix4x4::zeros();

        mtx[0, 1] = 1.0f / fovx_tan;
        mtx[1, 0] = -1.0f / fovx_tan_invaspect;
        mtx[1, 3] = iod / 2.0f;
        mtx[2, 3] = near * far / (near - far);
        if constexpr (RightHanded) {
            mtx[3, 2] = -1.0f;
        } else {
            mtx[3, 2] = 1.0f;
        }
        mtx[1, 2] = -mtx[3, 2] * shift / fovx_tan_invaspect;
        mtx[2, 2] = -mtx[3, 2] * near / (near - far);

        return mtx;
    }

    constexpr Matrix4x4 Matrix4x4::fromQuat(const Quaternion& quat) noexcept {
        Matrix4x4 mtx {};

        const float ii = quat.x * quat.x;
        const float ij = quat.x * quat.y;
        const float ik = quat.x * quat.z;
        const float jj = quat.y * quat.y;
        const float jk = quat.y * quat.z;
        const float kk = quat.z * quat.z;
        const float ri = quat.w * quat.x;
        const float rj = quat.w * quat.y;
        const float rk = quat.w * quat.z;

        mtx[0, 0] = 1.0f - (2.0f * (jj + kk));
        mtx[1, 0] = 2.0f * (ij + rk);
        mtx[2, 0] = 2.0f * (ik - rj);
        mtx[3, 0] = 0.0f;

        mtx[0, 1] = 2.0f * (ij - rk);
        mtx[1, 1] = 1.0f - (2.0f * (ii + kk));
        mtx[2, 1] = 2.0f * (jk + ri);
        mtx[3, 1] = 0.0f;

        mtx[0, 2] = 2.0f * (ik + rj);
        mtx[1, 2] = 2.0f * (jk - ri);
        mtx[2, 2] = 1.0f - (2.0f * (ii + jj));
        mtx[3, 2] = 0.0f;

        mtx[0, 3] = 0.0f;
        mtx[1, 3] = 0.0f;
        mtx[2, 3] = 0.0f;
        mtx[3, 3] = 1.0f;

        return mtx;
    }

    constexpr Matrix4x4 Matrix4x4::zeros() noexcept {
        return {};
    }

    constexpr Matrix4x4 Matrix4x4::identity() noexcept {
        return diagonal(1.f, 1.f, 1.f, 1.f);
    }

    constexpr Matrix4x4 Matrix4x4::diagonal(const float x, const float y, const float z, const float w) noexcept {
        Matrix4x4 mtx {};
        mtx[0, 0] = x;
        mtx[1, 1] = y;
        mtx[2, 2] = z;
        mtx[3, 3] = w;
        return mtx;
    }

    constexpr Matrix4x4 Matrix4x4::fromTranslation(const Vector3& vec) noexcept {
        return identity().translate(vec);
    }

    constexpr Matrix4x4 Matrix4x4::fromScale(const Vector3& vec) noexcept {
        return identity().scale(vec);
    }

    constexpr Matrix4x4 Matrix4x4::fromTRS(
        const Vector3& translation,
        const Quaternion& rotation,
        const Vector3& scale
    ) noexcept {
        return fromTranslation(translation) * fromQuat(rotation) * fromScale(scale);
    }

    constexpr Vector3 Matrix4x4::getTranslation() const noexcept {
        return { get(0, 3), get(1, 3), get(2, 3) };
    }

    constexpr Vector3 Matrix4x4::getScale() const noexcept {
        return {
            std::sqrt(get(0, 0) * get(0, 0) + get(1, 0) * get(1, 0) + get(2, 0) * get(2, 0)),
            std::sqrt(get(0, 1) * get(0, 1) + get(1, 1) * get(1, 1) + get(2, 1) * get(2, 1)),
            std::sqrt(get(0, 2) * get(0, 2) + get(1, 2) * get(1, 2) + get(2, 2) * get(2, 2))
        };
    }

    constexpr Quaternion Matrix4x4::getRotation() const noexcept {
        const Matrix4x4& m = *this;
        const float trace = m[0, 0] + m[1, 1] + m[2, 2];
        std::array<float, 4> tmp {};

        if (trace > 0) {
            float s = std::sqrt(trace + 1.f);
            tmp[3] = 0.5f * s;
            s = .5f / s;

            tmp[0] = (m[2, 1] - m[1, 2]) * s;
            tmp[1] = (m[0, 2] - m[2, 0]) * s;
            tmp[2] = (m[1, 0] - m[0, 1]) * s;
        } else {
            const unsigned int i = m[0, 0] < m[1, 1]
                ? (m[1, 1] < m[2, 2] ? 2 : 1)
                : (m[0, 0] < m[2, 2] ? 2 : 0);
            const unsigned int j = (i + 1) % 3;
            const unsigned int k = (i + 2) % 3;

            float s = std::sqrt(m[i, i] - m[j, j] - m[k, k] + 1.f);
            tmp[i] = 0.5f * s;
            s = .5f / s;

            tmp[3] = m[k, j] - m[j, k] * s;
            tmp[j] = m[j, i] + m[i, j] * s;
            tmp[k] = m[k, i] + m[i, k] * s;
        }

        return { tmp[0], tmp[1], tmp[2], tmp[3] };
    }

    constexpr bool operator==(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        return (
            lhs[0, 0] == rhs[0, 0] &&
            lhs[0, 1] == rhs[0, 1] &&
            lhs[0, 2] == rhs[0, 2] &&
            lhs[0, 3] == rhs[0, 3] &&

            lhs[1, 0] == rhs[1, 0] &&
            lhs[1, 1] == rhs[1, 1] &&
            lhs[1, 2] == rhs[1, 2] &&
            lhs[1, 3] == rhs[1, 3] &&

            lhs[2, 0] == rhs[2, 0] &&
            lhs[2, 1] == rhs[2, 1] &&
            lhs[2, 2] == rhs[2, 2] &&
            lhs[2, 3] == rhs[2, 3] &&

            lhs[3, 0] == rhs[3, 0] &&
            lhs[3, 1] == rhs[3, 1] &&
            lhs[3, 2] == rhs[3, 2] &&
            lhs[3, 3] == rhs[3, 3]
        );
    }

    constexpr std::pair<Matrix4x4, float> Matrix4x4::inverseAndDeterminant() const noexcept {
        std::pair<Matrix4x4, float> pair { identity(), 0.f };
        Matrix4x4& inv = pair.first;
        float& det = pair.second;

        inv[0, 0] =     get(1, 1) * get(2, 2) * get(3, 3) -
                        get(1, 1) * get(2, 3) * get(3, 2) -
                        get(2, 1) * get(1, 2) * get(3, 3) +
                        get(2, 1) * get(1, 3) * get(3, 2) +
                        get(3, 1) * get(1, 2) * get(2, 3) -
                        get(3, 1) * get(1, 3) * get(2, 2);

        inv[1, 0] =    -get(1, 0) * get(2, 2) * get(3, 3) +
                        get(1, 0) * get(2, 3) * get(3, 2) +
                        get(2, 0) * get(1, 2) * get(3, 3) -
                        get(2, 0) * get(1, 3) * get(3, 2) -
                        get(3, 0) * get(1, 2) * get(2, 3) +
                        get(3, 0) * get(1, 3) * get(2, 2);

        inv[2, 0] =     get(1, 0) * get(2, 1) * get(3, 3) -
                        get(1, 0) * get(2, 3) * get(3, 1) -
                        get(2, 0) * get(1, 1) * get(3, 3) +
                        get(2, 0) * get(1, 3) * get(3, 1) +
                        get(3, 0) * get(1, 1) * get(2, 3) -
                        get(3, 0) * get(1, 3) * get(2, 1);

        inv[3, 0] =    -get(1, 0) * get(2, 1) * get(3, 2) +
                        get(1, 0) * get(2, 2) * get(3, 1) +
                        get(2, 0) * get(1, 1) * get(3, 2) -
                        get(2, 0) * get(1, 2) * get(3, 1) -
                        get(3, 0) * get(1, 1) * get(2, 2) +
                        get(3, 0) * get(1, 2) * get(2, 1);

        det = get(0, 0) * inv[0, 0] + get(0, 1) * inv[1, 0] + get(0, 2) * inv[2, 0] + get(0, 3) * inv[3, 0];

        if (std::abs(det) < 1e-6f) {
            det = 0;
            inv = {};
            return pair;
        }

        inv[0, 1] =    -get(0, 1) * get(2, 2) * get(3, 3) +
                        get(0, 1) * get(2, 3) * get(3, 2) +
                        get(2, 1) * get(0, 2) * get(3, 3) -
                        get(2, 1) * get(0, 3) * get(3, 2) -
                        get(3, 1) * get(0, 2) * get(2, 3) +
                        get(3, 1) * get(0, 3) * get(2, 2);

        inv[1, 1] =     get(0, 0) * get(2, 2) * get(3, 3) -
                        get(0, 0) * get(2, 3) * get(3, 2) -
                        get(2, 0) * get(0, 2) * get(3, 3) +
                        get(2, 0) * get(0, 3) * get(3, 2) +
                        get(3, 0) * get(0, 2) * get(2, 3) -
                        get(3, 0) * get(0, 3) * get(2, 2);

        inv[2, 1] =    -get(0, 0) * get(2, 1) * get(3, 3) +
                        get(0, 0) * get(2, 3) * get(3, 1) +
                        get(2, 0) * get(0, 1) * get(3, 3) -
                        get(2, 0) * get(0, 3) * get(3, 1) -
                        get(3, 0) * get(0, 1) * get(2, 3) +
                        get(3, 0) * get(0, 3) * get(2, 1);

        inv[3, 1] =     get(0, 0) * get(2, 1) * get(3, 2) -
                        get(0, 0) * get(2, 2) * get(3, 1) -
                        get(2, 0) * get(0, 1) * get(3, 2) +
                        get(2, 0) * get(0, 2) * get(3, 1) +
                        get(3, 0) * get(0, 1) * get(2, 2) -
                        get(3, 0) * get(0, 2) * get(2, 1);

        inv[0, 2] =     get(0, 1) * get(1, 2) * get(3, 3) -
                        get(0, 1) * get(1, 3) * get(3, 2) -
                        get(1, 1) * get(0, 2) * get(3, 3) +
                        get(1, 1) * get(0, 3) * get(3, 2) +
                        get(3, 1) * get(0, 2) * get(1, 3) -
                        get(3, 1) * get(0, 3) * get(1, 2);

        inv[1, 2] =    -get(0, 0) * get(1, 2) * get(3, 3) +
                        get(0, 0) * get(1, 3) * get(3, 2) +
                        get(1, 0) * get(0, 2) * get(3, 3) -
                        get(1, 0) * get(0, 3) * get(3, 2) -
                        get(3, 0) * get(0, 2) * get(1, 3) +
                        get(3, 0) * get(0, 3) * get(1, 2);

        inv[2, 2] =     get(0, 0) * get(1, 1) * get(3, 3) -
                        get(0, 0) * get(1, 3) * get(3, 1) -
                        get(1, 0) * get(0, 1) * get(3, 3) +
                        get(1, 0) * get(0, 3) * get(3, 1) +
                        get(3, 0) * get(0, 1) * get(1, 3) -
                        get(3, 0) * get(0, 3) * get(1, 1);

        inv[3, 2] =    -get(0, 0) * get(1, 1) * get(3, 2) +
                        get(0, 0) * get(1, 2) * get(3, 1) +
                        get(1, 0) * get(0, 1) * get(3, 2) -
                        get(1, 0) * get(0, 2) * get(3, 1) -
                        get(3, 0) * get(0, 1) * get(1, 2) +
                        get(3, 0) * get(0, 2) * get(1, 1);

        inv[0, 3] =    -get(0, 1) * get(1, 2) * get(2, 3) +
                        get(0, 1) * get(1, 3) * get(2, 2) +
                        get(1, 1) * get(0, 2) * get(2, 3) -
                        get(1, 1) * get(0, 3) * get(2, 2) -
                        get(2, 1) * get(0, 2) * get(1, 3) +
                        get(2, 1) * get(0, 3) * get(1, 2);

        inv[1, 3] =     get(0, 0) * get(1, 2) * get(2, 3) -
                        get(0, 0) * get(1, 3) * get(2, 2) -
                        get(1, 0) * get(0, 2) * get(2, 3) +
                        get(1, 0) * get(0, 3) * get(2, 2) +
                        get(2, 0) * get(0, 2) * get(1, 3) -
                        get(2, 0) * get(0, 3) * get(1, 2);

        inv[2, 3] =    -get(0, 0) * get(1, 1) * get(2, 3) +
                        get(0, 0) * get(1, 3) * get(2, 1) +
                        get(1, 0) * get(0, 1) * get(2, 3) -
                        get(1, 0) * get(0, 3) * get(2, 1) -
                        get(2, 0) * get(0, 1) * get(1, 3) +
                        get(2, 0) * get(0, 3) * get(1, 1);

        inv[3, 3] =     get(0, 0) * get(1, 1) * get(2, 2) -
                        get(0, 0) * get(1, 2) * get(2, 1) -
                        get(1, 0) * get(0, 1) * get(2, 2) +
                        get(1, 0) * get(0, 2) * get(2, 1) +
                        get(2, 0) * get(0, 1) * get(1, 2) -
                        get(2, 0) * get(0, 2) * get(1, 1);

        for (unsigned int i {}; i < 4; ++i)
            for (unsigned int j {}; j < 4; ++j)
                inv[i, j] /= det;

        return pair;
    }

    constexpr Matrix4x4 Matrix4x4::inverse() const noexcept {
        return inverseAndDeterminant().first;
    }

    constexpr float Matrix4x4::determinant() const noexcept {
        return inverseAndDeterminant().second;
    }

    constexpr Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        Matrix4x4 out;

        // http://www.wolframalpha.com/input/?i={{a,b,c,d},{e,f,g,h},{i,j,k,l},{m,n,o,p}}{{α,β,γ,δ},{ε,θ,ι,κ},{λ,μ,ν,ξ},{ο,π,ρ,σ}}
        for (unsigned int j {}; j < 4; ++j)
            for (unsigned int i {}; i < 4; ++i)
                out[j, i] = lhs[j, 0]*rhs[0, i] + lhs[j, 1]*rhs[1, i] + lhs[j, 2]*rhs[2, i] + lhs[j, 3]*rhs[3, i];

        return out;
    }


    constexpr Matrix4x4& operator*=(Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    constexpr Matrix4x4 operator+(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        Matrix4x4 mtx {};
        for (unsigned int i {}; i < 4; ++i)
            for (unsigned int j {}; j < 4; ++j)
                mtx[i, j] = lhs[i, j] + rhs[i, j];

        return mtx;
    }

    constexpr Matrix4x4& operator+=(Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    constexpr Matrix4x4 operator-(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        Matrix4x4 mtx {};

        for (unsigned int i {}; i < 4; ++i)
            for (unsigned int j {}; j < 4; ++j)
                mtx[i, j] = lhs[i, j] - rhs[i, j];

        return mtx;
    }

    constexpr Matrix4x4& operator-=(Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        lhs = lhs - rhs;
        return lhs;
    }

    constexpr Vector3 operator*(const Matrix4x4& lhs, const Vector3& rhs) noexcept {
        return {
            lhs[0, 0] * rhs.x + lhs[0, 1] * rhs.y + lhs[0, 2] * rhs.z,
            lhs[1, 0] * rhs.x + lhs[1, 1] * rhs.y + lhs[1, 2] * rhs.z,
            lhs[2, 0] * rhs.x + lhs[2, 1] * rhs.y + lhs[2, 2] * rhs.z
        };
    }

    constexpr Quaternion operator*(const Matrix4x4& lhs, const Quaternion& rhs) noexcept {
        return {
            lhs[0, 0] * rhs.x + lhs[0, 1] * rhs.y + lhs[0, 2] * rhs.z + lhs[0, 3] * rhs.w,
            lhs[1, 0] * rhs.x + lhs[1, 1] * rhs.y + lhs[1, 2] * rhs.z + lhs[1, 3] * rhs.w,
            lhs[2, 0] * rhs.x + lhs[2, 1] * rhs.y + lhs[2, 2] * rhs.z + lhs[2, 3] * rhs.w,
            lhs[3, 0] * rhs.x + lhs[3, 1] * rhs.y + lhs[3, 2] * rhs.z + lhs[3, 3] * rhs.w,
        };
    }

    template <bool RightSide>
    constexpr Matrix4x4& Matrix4x4::rotate(Vector3 axis, Radians<float> angle) {
        Matrix4x4 om;

        float s = std::sin(angle);
        float c = std::cos(angle);
        float t = 1.0f - c;

        axis = axis.normalise();

        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        om[0, 1] = t * x * x + c;
        om[1, 1] = t * x * y + s * z;
        om[2, 1] = t * x * z - s * y;
        //om.r[3, 1] = 0.0f ; //optimized out

        om[0, 2] = t * y * x - s * z;
        om[1, 2] = t * y * y + c;
        om[2, 2] = t * y * z + s * x;
        //om.r[3, 2] = 0.0f ; //optimized out

        om[0, 3] = t * z * x + s * y;
        om[1, 3] = t * z * y - s * x;
        om[2, 3] = t * z * z + c;
        //om.r[3, 3] = 0.0f; //optimized out

        /* optimized out
        om.r[0, 0] = 0.0f;
        om.r[1, 0] = 0.0f;
        om.r[2, 0] = 0.0f;
        om.r[3, 0] = 1.0f;
        */

        if constexpr (RightSide) {
            for (unsigned int i {}; i < 4; ++i) {
                x = get(i, 1) * om[0, 1] + get(i, 2) * om[1, 1] + get(i, 3) * om[2, 1];
                y = get(i, 1) * om[0, 2] + get(i, 2) * om[1, 2] + get(i, 3) * om[2, 2];
                z = get(i, 1) * om[0, 3] + get(i, 2) * om[1, 3] + get(i, 3) * om[2, 3];

                get(i, 1) = x;
                get(i, 2) = y;
                get(i, 3) = z;
            }
        } else {
            for (unsigned int i {}; i < 3; ++i) {
                x = get(0, 1) * om[i, 1] + get(1, 1) * om[i, 2] + get(2, 1) * om[i, 3];
                y = get(0, 2) * om[i, 1] + get(1, 2) * om[i, 2] + get(2, 2) * om[i, 3];
                z = get(0, 3) * om[i, 1] + get(1, 3) * om[i, 2] + get(2, 3) * om[i, 3];
                const float w = get(0, 0) * om[i, 1] + get(1, 0) * om[i, 2] + get(2, 0) * om[i, 3];

                om[i, 1] = x;
                om[i, 2] = y;
                om[i, 3] = z;
                om[i, 0] = w;
            }

            // TODO: Is this wrong?
            // for (int i = 0; i < 3; ++i)
            //     mtx[i] = { om[i, 0], om[i, 1], om[i, 2], om[i, 3] };
            for (unsigned int i = 0; i < 4; ++i)
                for (unsigned int j {}; j < 4; ++j)
                    get(i, j) = om[i, j];
        }
        return *this;
    }

    template <bool RightSide>
    constexpr Matrix4x4& Matrix4x4::rotateX(const Radians<float> angle) {
        const float cosAngle = std::cos(angle);
        const float sinAngle = std::sin(angle);

        if constexpr (RightSide) {
            for (unsigned int i = 0; i < 4; ++i) {
                const float a = get(i, 1) * cosAngle + get(i, 2) * sinAngle;
                const float b = get(i, 2) * cosAngle - get(i, 1) * sinAngle;
                get(i, 1) = a;
                get(i, 2) = b;
            }
        } else {
            for (unsigned int i = 0; i < 4; ++i) {
                const float a = get(1, i) * cosAngle - get(2, i) * sinAngle;
                const float b = get(2, i) * cosAngle + get(1, i) * sinAngle;
                get(1, i) = a;
                get(2, i) = b;
            }
        }
        return *this;
    }

    template <bool RightSide>
    constexpr Matrix4x4& Matrix4x4::rotateY(Radians<float> angle) {
        const float cosAngle = cosf(angle);
        const float sinAngle = sinf(angle);

        if constexpr (RightSide) {
            for (unsigned int i = 0; i < 4; ++i) {
                const float a = get(i, 0) * cosAngle - get(i, 2) * sinAngle;
                const float b = get(i, 2) * cosAngle + get(i, 0) * sinAngle;
                get(i, 0) = a;
                get(i, 2) = b;
            }
        } else {
            for (unsigned int i = 0; i < 4; ++i) {
                const float a = get(0, i) * cosAngle + get(2, i) * sinAngle;
                const float b = get(2, i) * cosAngle - get(0, i) * sinAngle;
                get(0, i) = a;
                get(2, i) = b;
            }
        }
        return *this;
    }

    template <bool RightSide>
    constexpr Matrix4x4& Matrix4x4::rotateZ(Radians<float> angle) {
        const float cosAngle = std::cos(angle);
        const float sinAngle = std::sin(angle);

        if constexpr (RightSide) {
            for (unsigned int i = 0; i < 4; ++i) {
                const float a = get(i, 0) * cosAngle + get(i, 1) * sinAngle;
                const float b = get(i, 1) * cosAngle - get(i, 0) * sinAngle;
                get(i, 0) = a;
                get(i, 1) = b;
            }
        } else {
            for (unsigned int i = 0; i < 4; ++i) {
                const float a = get(0, i) * cosAngle - get(1, i) * sinAngle;
                const float b = get(1, i) * cosAngle + get(0, i) * sinAngle;
                get(0, i) = a;
                get(1, i) = b;
            }
        }
        return *this;
    }

    constexpr Matrix4x4& Matrix4x4::scale(const float x, const float y, const float z) noexcept {
        for (unsigned int i{}; i < 4; ++i) {
            get(i, 0) *= x;
            get(i, 1) *= y;
            get(i, 2) *= z;
        }
        return *this;
    }

    constexpr Matrix4x4& Matrix4x4::scale(const Vector3& s) noexcept {
        return scale(s.x, s.y, s.z);
    }

    template <bool RightSide>
    constexpr Matrix4x4& Matrix4x4::translate(const float x, const float y, const float z) noexcept {
        const std::array<float, 4> v { x, y, z, 1 };

        if constexpr (RightSide) {
            for (unsigned int i = 0; i < 4; ++i)
                get(i, 3) = get(i, 0) * v[0] + get(i, 1) * v[1] + get(i, 2) * v[2] + get(i, 3) * v[3];
        } else {
            for (unsigned int j = 0; j < 3; ++j)
                for (unsigned int i = 0; i < 4; ++i)
                    get(j, i) += get(3, i) * v[j];
        }

        return *this;
    }

    template <bool RightSide>
    constexpr Matrix4x4& Matrix4x4::translate(const Vector3& t) noexcept {
        return translate<RightSide>(t.x, t.y, t.z);
    }


    constexpr void Matrix4x4::setTranslation(const Vector3& to) noexcept {
        get(0, 3) = to.x;
        get(1, 3) = to.y;
        get(2, 3) = to.z;
    }

    constexpr void Matrix4x4::setRotation([[maybe_unused]] const Quaternion& to) noexcept {
        *this = fromTRS(getTranslation(), to, getScale());
    }

    constexpr Matrix4x4& Matrix4x4::transpose() noexcept {
        for (unsigned int i{}; i < 3; ++i)
            for (unsigned int j{}; j < 3 - i; ++j)
                std::swap(get(i, j), get(3 - j, 3 - i));
        return *this;
    }

    constexpr void Matrix4x4::setScale(const Vector3& to) noexcept {
        scale(to / getScale());
    }
}

template <> struct std::formatter<M3DS::Matrix4x4> : std::formatter<std::string> {
    static auto format(M3DS::Matrix4x4 matrix, format_context& ctx) {
        return std::format_to(
            ctx.out(),
            "{{"
                "\n{: >8.3f},{: >8.3f},{: >8.3f},{: >8.3f},"
                "\n{: >8.3f},{: >8.3f},{: >8.3f},{: >8.3f},"
                "\n{: >8.3f},{: >8.3f},{: >8.3f},{: >8.3f},"
                "\n{: >8.3f},{: >8.3f},{: >8.3f},{: >8.3f}"
            "\n}}",
            matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
            matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
            matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3],
            matrix[3, 0], matrix[3, 1], matrix[3, 2], matrix[3, 3]
        );
    }
};

inline std::ostream& operator<<(std::ostream& os, const M3DS::Matrix4x4& matrix) {
	std::format_to(std::ostreambuf_iterator{ os }, "{}", matrix);
    return os;
}