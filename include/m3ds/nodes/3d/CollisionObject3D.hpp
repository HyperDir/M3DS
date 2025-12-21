#pragma once

#include <m3ds/nodes/3d/Node3D.hpp>

#include "m3ds/lib/SPhys/3D/PhysicsServer3D.hpp"

namespace M3DS {
    class CollisionObject3D : public Node3D {
        M_CLASS(CollisionObject3D, Node3D)
        friend class Viewport;
    public:
        void setLayer(std::uint32_t layer) noexcept;
        void setMask(std::uint32_t mask) noexcept;

        void setShape(const SPhys::Shape3D& shape) noexcept;

        void enableCollision() noexcept;
        void disableCollision() noexcept;

        [[nodiscard]] std::uint32_t getLayer() const noexcept;
        [[nodiscard]] std::uint32_t getMask() const noexcept;

        [[nodiscard]] const SPhys::Shape3D& getShape() const noexcept;

        [[nodiscard]] bool isCollisionDisabled() const noexcept;

        [[nodiscard]] SPhys::CollisionObject3D* getCollisionObject() noexcept;
        [[nodiscard]] const SPhys::CollisionObject3D* getCollisionObject() const noexcept;

        void readback() noexcept;
    protected:
        void update(Seconds<float> delta) override;

        void updateCollisionObject(SPhys::CollisionObject3D* obj) noexcept {
            mCollisionObject = obj;
        }

        virtual void internaliseState() noexcept;
        virtual void externaliseState() noexcept;
    private:
        SPhys::CollisionObject3D* mCollisionObject {};

        std::uint32_t mLayer = 0b1;
        std::uint32_t mMask = 0b1;

        SPhys::Shape3D mShape {};
        bool mCollisionDisabled {};
    };
}
