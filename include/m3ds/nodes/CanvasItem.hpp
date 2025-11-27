#pragma once

#include <m3ds/nodes/Node.hpp>
#include <m3ds/spatial/Transform2D.hpp>

namespace M3DS {
    class CanvasItem : public Node {
        M_CLASS(CanvasItem, Node)
    public:
        void setTranslation(const Vector2& to) noexcept;
        void setGlobalTranslation(const Vector2& to) noexcept;
        void setRotation(Radians<float> to) noexcept;
        void setGlobalRotation(Radians<float> to) noexcept;
        void setScale(const Vector2& to) noexcept;

        [[nodiscard]] const Transform2D& getTransform() const noexcept;
        [[nodiscard]] const Transform2D& getGlobalTransform() const noexcept;

        [[nodiscard]] const Vector2& getTranslation() const noexcept;
        [[nodiscard]] Radians<float> getRotation() const noexcept;
        [[nodiscard]] const Vector2& getScale() const noexcept;

        [[nodiscard]] Vector2 getScreenPosition() const noexcept;
    protected:
        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        Transform2D mTransform {};

        mutable Transform2D mGlobalTransform {};
        mutable bool mGlobalTransformDirty {};

        void setGlobalTransformDirty() noexcept;
    };
}
