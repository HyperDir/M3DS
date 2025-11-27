#pragma once

#include <m3ds/nodes/Node.hpp>
#include <m3ds/spatial/Matrix4x4.hpp>

namespace M3DS {
    class Node3D : public Node {
        M_CLASS(Node3D, Node)
    public:
        void setTransform(const Matrix4x4& to) noexcept;

        void setTranslation(const Vector3& to) noexcept;
        void setRotation(const Quaternion& to) noexcept;
        void setScale(const Vector3& to) noexcept;

        [[nodiscard]] const Matrix4x4& getTransform() const noexcept;
        [[nodiscard]] const Matrix4x4& getGlobalTransform() const noexcept;
        [[nodiscard]] Vector3 getGlobalTranslation() const noexcept;

        [[nodiscard]] Vector3 getTranslation() const noexcept;
        [[nodiscard]] Quaternion getRotation() const noexcept;
        [[nodiscard]] Vector3 getScale() const noexcept;

        void setGlobalTransform(const Matrix4x4& to) noexcept;
        void setGlobalTranslation(const Metres<Vector3>& to) noexcept;
        void setGlobalRotation(const Quaternion& to);
    private:
        Matrix4x4 mTransform = Matrix4x4::identity();
        mutable Matrix4x4 mGlobalTransform = Matrix4x4::identity();
        mutable bool mGlobalTransformDirty {};

        void setGlobalTransformDirty() noexcept;
    };
}
