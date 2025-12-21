#pragma once

#include "../Shape2D.hpp"
#include "../PhysicsEnvironment2D.hpp"

#include "../../Utils/DirtyCheck.hpp"
#include "../../Spatial/Units.hpp"
#include "../../Spatial/Vector2.hpp"

namespace SPhys {
    enum class ObjectType2D : std::uint8_t {
        object,
        static_body,
        kinematic_body,
        rigid_body,
        area
    };

    class CollisionObject2D {
        template <PhysicsEnvironment2D> friend class PhysicsServer2D;
    public:
        void* userData {};

        explicit constexpr CollisionObject2D(ObjectType2D objectType) noexcept;

        constexpr void setTranslation(const Pixels<Vector2>& to) noexcept;
        constexpr void setRotation(float to) noexcept;

        constexpr void addTranslation(const Pixels<Vector2>& amount) noexcept;
        constexpr void addRotation(float amount) noexcept;

        constexpr void setLocalShape(const Shape2D& to) noexcept;
        constexpr void updateGlobalShape() const noexcept;

        constexpr void setLayer(std::uint32_t to) noexcept;
        constexpr void setMask(std::uint32_t to) noexcept;

        constexpr void enable() noexcept;
        constexpr void disable() noexcept;

        [[nodiscard]] constexpr const Pixels<Vector2>& getTranslation() const noexcept;
        [[nodiscard]] constexpr float getRotation() const noexcept;

        [[nodiscard]] constexpr const Shape2D& getLocalShape() const noexcept;

        template <DirtyCheck dirtyCheck = DirtyCheck::perform>
        [[nodiscard]] constexpr const Shape2D& getGlobalShape() const noexcept;
        template <DirtyCheck dirtyCheck = DirtyCheck::perform>
        [[nodiscard]] constexpr const BoundingBox2D& getBoundingBox() const noexcept;

        [[nodiscard]] constexpr std::uint32_t getLayer() const noexcept;
        [[nodiscard]] constexpr std::uint32_t getMask() const noexcept;

        [[nodiscard]] constexpr bool isDisabled() const noexcept;

        [[nodiscard]] constexpr ObjectType2D getObjectType() const noexcept;
    private:
        ObjectType2D mObjectType = ObjectType2D::object;

        bool mDisabled {};
        mutable bool mIsClean {};

        /* 1 byte padding */

        Pixels<Vector2> mTranslation {};
        float mRotation {};

        Shape2D mLocalShape {};
        mutable Shape2D mGlobalShape {};
        mutable BoundingBox2D mBoundingBox {};

        std::uint32_t mLayer = 0b1;
        std::uint32_t mMask = 0b1;
    };
}



/* Implementation */
namespace SPhys {
    constexpr CollisionObject2D::CollisionObject2D(const ObjectType2D objectType) noexcept : mObjectType(objectType) {}

    constexpr void CollisionObject2D::setTranslation(const Pixels<Vector2>& to) noexcept {
        if (to != mTranslation) {
            mTranslation = to;
            if consteval {
                updateGlobalShape();
            } else {
                mIsClean = false;
            }
        }
    }

    constexpr void CollisionObject2D::setRotation(const float to) noexcept {
        if (to != mRotation) {
            mRotation = to;
            if consteval {
                updateGlobalShape();
            } else {
                mIsClean = false;
            }
        }
    }

    constexpr float CollisionObject2D::getRotation() const noexcept {
        return mRotation;
    }

    constexpr const Pixels<Vector2>& CollisionObject2D::getTranslation() const noexcept {
        return mTranslation;
    }

    constexpr void CollisionObject2D::addTranslation(const Pixels<Vector2>& amount) noexcept {
        setTranslation(getTranslation() + amount);
    }

    constexpr void CollisionObject2D::addRotation(const float amount) noexcept {
        setRotation(mRotation + amount);
    }

    constexpr void CollisionObject2D::setLocalShape(const Shape2D& to) noexcept {
        mLocalShape = to;
        if consteval {
            updateGlobalShape();
        } else {
            mIsClean = false;
        }
    }

    constexpr void CollisionObject2D::updateGlobalShape() const noexcept {
        if (!mIsClean || std::is_constant_evaluated()) {
            mLocalShape.visit(
                [&]<typename T>(const T& shape) {
                    T& newShape = mGlobalShape.emplace<T>(shape);
                    newShape.setTranslation(newShape.getTranslation().rotated(mRotation) + mTranslation);
                    if constexpr (RotatingShapeType2D<T>)
                        newShape.rotate(mRotation);
                    mBoundingBox = newShape.getBoundingBox();
                }
            );
            if !consteval {
                mIsClean = true;
            }
        }
    }

    constexpr const Shape2D& CollisionObject2D::getLocalShape() const noexcept {
        return mLocalShape;
    }

    template <DirtyCheck dirtyCheck>
    constexpr const Shape2D& CollisionObject2D::getGlobalShape() const noexcept {
        if !consteval {
            if constexpr (dirtyCheck == DirtyCheck::perform)
                updateGlobalShape();
        }
        return mGlobalShape;
    }

    template <DirtyCheck dirtyCheck>
    constexpr const BoundingBox2D& CollisionObject2D::getBoundingBox() const noexcept {
        if !consteval {
            if constexpr (dirtyCheck == DirtyCheck::perform)
                updateGlobalShape();
        }
        return mBoundingBox;
    }

    constexpr void CollisionObject2D::setLayer(const std::uint32_t to) noexcept {
        mLayer = to;
    }

    constexpr std::uint32_t CollisionObject2D::getLayer() const noexcept {
        return mLayer;
    }

    constexpr void CollisionObject2D::setMask(const std::uint32_t to) noexcept {
        mMask = to;
    }

    constexpr std::uint32_t CollisionObject2D::getMask() const noexcept {
        return mMask;
    }

    constexpr void CollisionObject2D::enable() noexcept {
        mDisabled = false;
    }

    constexpr void CollisionObject2D::disable() noexcept {
        mDisabled = true;
    }

    constexpr bool CollisionObject2D::isDisabled() const noexcept {
        return mDisabled;
    }

    constexpr ObjectType2D CollisionObject2D::getObjectType() const noexcept {
        return mObjectType;
    }
}
