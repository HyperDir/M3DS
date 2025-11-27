#pragma once

#include <m3ds/nodes/3d/Node3D.hpp>

#include <m3ds/lib/SPhys/3D/Shape3D.hpp>
#include <m3ds/lib/SPhys/3D/CollisionObjects/CollisionObject3D.hpp>

namespace M3DS {
    class CollisionObject3D : public Node3D {
        M_CLASS(CollisionObject3D, Node3D)
        friend class Viewport;
    public:
        void setLayer(std::uint32_t layer) noexcept;
        void setMask(std::uint32_t mask) noexcept;

        void setShape(SPhys::Shape3D shape) noexcept;

        void enableCollision() noexcept;
        void disableCollision() noexcept;

        [[nodiscard]] std::uint32_t getLayer() const noexcept;
        [[nodiscard]] std::uint32_t getMask() const noexcept;

        [[nodiscard]] const SPhys::Shape3D& getShape() const noexcept;

        [[nodiscard]] bool isCollisionDisabled() const noexcept;

        [[nodiscard]] SPhys::CollisionObject3D* getCollisionObject() noexcept;
        [[nodiscard]] const SPhys::CollisionObject3D* getCollisionObject() const noexcept;
    protected:
        void update(Seconds<float> delta) override;

        explicit CollisionObject3D(SPhys::CollisionObject3D* collisionObject) noexcept;

        void afterTreeEnter() override;
        void beforeTreeExit() override;

        void readback() noexcept;
    private:
        SPhys::CollisionObject3D* mCollisionObject {};
    };
}
