#pragma once

#include <m3ds/nodes/2d/Node2D.hpp>

#include "m3ds/lib/SPhys/2D/PhysicsServer2D.hpp"

namespace M3DS {
    class CollisionObject2D : public Node2D {
        M_CLASS(CollisionObject2D, Node2D)
        friend class Viewport;
    public:
        void setLayer(std::uint32_t layer) noexcept;
        void setMask(std::uint32_t mask) noexcept;

        void setShape(const SPhys::Shape2D& shape) noexcept;

        void enableCollision() noexcept;
        void disableCollision() noexcept;

        [[nodiscard]] std::uint32_t getLayer() const noexcept;
        [[nodiscard]] std::uint32_t getMask() const noexcept;

        [[nodiscard]] const SPhys::Shape2D& getShape() const noexcept;

        [[nodiscard]] bool isCollisionDisabled() const noexcept;

        [[nodiscard]] SPhys::CollisionObject2D* getCollisionObject() noexcept;
        [[nodiscard]] const SPhys::CollisionObject2D* getCollisionObject() const noexcept;

        void readback() noexcept;
    protected:
        void update(Seconds<float> delta) override;

        void updateCollisionObject(SPhys::CollisionObject2D* obj) noexcept {
            mCollisionObject = obj;
        }

        virtual void internaliseState() noexcept;
        virtual void externaliseState() noexcept;
    private:
        SPhys::CollisionObject2D* mCollisionObject {};

        std::uint32_t mLayer = 0b1;
        std::uint32_t mMask = 0b1;

        SPhys::Shape2D mShape {};
        bool mCollisionDisabled {};
    };
}
