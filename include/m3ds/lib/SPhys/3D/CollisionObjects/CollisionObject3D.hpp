#pragma once

#include "../Shape3D.hpp"
#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector3.hpp"

#include "../PhysicsEnvironment3D.hpp"

#include "../../Utils/DirtyCheck.hpp"

namespace SPhys {
    enum class ObjectType3D : std::uint8_t {
        object,
        static_body,
        kinematic_body,
        rigid_body,
        area
    };

    class CollisionObject3D {
        template <PhysicsEnvironment3D> friend class PhysicsServer3D;
    public:
        void* userData {};

        explicit constexpr CollisionObject3D(
            const Metres<Vector3>& translation,
            const Quaternion& rotation,
            const Shape3D& shape
        ) noexcept;

        constexpr void setTranslation(const Metres<Vector3>& to) noexcept;
        constexpr void setRotation(const Quaternion& to) noexcept;

        constexpr void addTranslation(const Metres<Vector3>& amount) noexcept;
        constexpr void addRotation(const Quaternion& amount) noexcept;

        constexpr void setLocalShape(const Shape3D& to) noexcept;
        constexpr void updateGlobalShape() const noexcept;

        constexpr void setLayer(std::uint32_t to) noexcept;
        constexpr void setMask(std::uint32_t to) noexcept;

        constexpr void enable() noexcept;
        constexpr void disable() noexcept;

        [[nodiscard]] constexpr const Metres<Vector3>& getTranslation() const noexcept;
        [[nodiscard]] constexpr const Quaternion& getRotation() const noexcept;

        [[nodiscard]] constexpr const Shape3D& getLocalShape() const noexcept;

        template <DirtyCheck dirtyCheck = DirtyCheck::perform>
        [[nodiscard]] constexpr const Shape3D& getGlobalShape() const noexcept;
        template <DirtyCheck dirtyCheck = DirtyCheck::perform>
        [[nodiscard]] constexpr const BoundingBox3D& getBoundingBox() const noexcept;

        [[nodiscard]] constexpr std::uint32_t getLayer() const noexcept;
        [[nodiscard]] constexpr std::uint32_t getMask() const noexcept;

        [[nodiscard]] constexpr bool isDisabled() const noexcept;

        [[nodiscard]] constexpr ObjectType3D getObjectType() const noexcept;
    protected:
        ObjectType3D mObjectType = ObjectType3D::object;
    private:
        Metres<Vector3> mTranslation {};
        Quaternion mRotation {};

        Shape3D mLocalShape {};
        mutable Shape3D mGlobalShape {};
        mutable BoundingBox3D mBoundingBox {};

        std::uint32_t mLayer = 0b1;
        std::uint32_t mMask = 0b1;

        bool mDisabled = false;
        mutable bool mDirty = true;
    };

    constexpr CollisionObject3D::CollisionObject3D(
        const Metres<Vector3>& translation,
        const Quaternion& rotation,
        const Shape3D& shape
    ) noexcept
        : mTranslation(translation)
        , mRotation(rotation)
        , mLocalShape(shape)
    {}

    constexpr void CollisionObject3D::setTranslation(const Metres<Vector3>& to) noexcept {
        if (to != mTranslation) {
            mTranslation = to;
            if consteval {
                updateGlobalShape();
            } else {
                mDirty = true;
            }
        }
    }

    constexpr void CollisionObject3D::setRotation(const Quaternion& to) noexcept {
        if (to != mRotation) {
            mRotation = to;
            if consteval {
                updateGlobalShape();
            } else {
                mDirty = true;
            }
        }
    }

    constexpr const Quaternion& CollisionObject3D::getRotation() const noexcept {
        return mRotation;
    }

    constexpr const Metres<Vector3>& CollisionObject3D::getTranslation() const noexcept {
        return mTranslation;
    }

    constexpr void CollisionObject3D::addTranslation(const Metres<Vector3>& amount) noexcept {
        setTranslation(getTranslation() + amount);
    }

    constexpr void CollisionObject3D::addRotation(const Quaternion& amount) noexcept {
        setRotation(amount * mRotation);
    }

    constexpr void CollisionObject3D::setLocalShape(const Shape3D& to) noexcept {
        mLocalShape = to;
        if consteval {
            updateGlobalShape();
        } else {
            mDirty = true;
        }
    }

    constexpr void CollisionObject3D::updateGlobalShape() const noexcept {
        if (mDirty || std::is_constant_evaluated()) {
            mLocalShape.visit(
                [&]<typename T>(const T& shape) {
                    mGlobalShape = shape;
                    T& newShape = std::get<T>(mGlobalShape);
                    newShape.setTranslation(newShape.getTranslation().rotated(mRotation) + mTranslation);
                    if constexpr (RotatingShapeType3D<T>)
                        newShape.rotate(mRotation);
                    mBoundingBox = newShape.getBoundingBox();
                }
            );
            if !consteval {
                mDirty = false;
            }
        }
    }

    constexpr const Shape3D& CollisionObject3D::getLocalShape() const noexcept {
        return mLocalShape;
    }

    template <DirtyCheck dirtyCheck>
    constexpr const Shape3D& CollisionObject3D::getGlobalShape() const noexcept {
        if !consteval {
            if constexpr (dirtyCheck == DirtyCheck::perform)
                updateGlobalShape();
        }
        return mGlobalShape;
    }

    template <DirtyCheck dirtyCheck>
    constexpr const BoundingBox3D& CollisionObject3D::getBoundingBox() const noexcept {
        if !consteval {
            if constexpr (dirtyCheck == DirtyCheck::perform)
                updateGlobalShape();
        }
        return mBoundingBox;
    }

    constexpr void CollisionObject3D::setLayer(const std::uint32_t to) noexcept {
        mLayer = to;
    }

    constexpr std::uint32_t CollisionObject3D::getLayer() const noexcept {
        return mLayer;
    }

    constexpr void CollisionObject3D::setMask(const std::uint32_t to) noexcept {
        mMask = to;
    }

    constexpr std::uint32_t CollisionObject3D::getMask() const noexcept {
        return mMask;
    }

    constexpr void CollisionObject3D::enable() noexcept {
        mDisabled = false;
    }

    constexpr void CollisionObject3D::disable() noexcept {
        mDisabled = true;
    }

    constexpr bool CollisionObject3D::isDisabled() const noexcept {
        return mDisabled;
    }

    constexpr ObjectType3D CollisionObject3D::getObjectType() const noexcept {
        return mObjectType;
    }
}
