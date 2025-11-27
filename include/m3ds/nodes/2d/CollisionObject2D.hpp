#pragma once

#include <m3ds/nodes/2d/Node2D.hpp>

#include <m3ds/lib/SPhys/2d/Shape2D.hpp>
#include <m3ds/lib/SPhys/2d/CollisionObjects/CollisionObject2D.hpp>

namespace M3DS {
    class CollisionObject2D : public Node2D {
        M_CLASS(CollisionObject2D, Node2D)
        friend class Viewport;
    public:
        void setLayer(std::uint32_t layer) noexcept;
        void setMask(std::uint32_t mask) noexcept;

        void setShape(SPhys::Shape2D shape) noexcept;

        void enableCollision() noexcept;
        void disableCollision() noexcept;

        [[nodiscard]] std::uint32_t getLayer() const noexcept;
        [[nodiscard]] std::uint32_t getMask() const noexcept;

        [[nodiscard]] const SPhys::Shape2D& getShape() const noexcept;

        [[nodiscard]] bool isCollisionDisabled() const noexcept;

        [[nodiscard]] SPhys::CollisionObject2D* getCollisionObject() noexcept;
        [[nodiscard]] const SPhys::CollisionObject2D* getCollisionObject() const noexcept;
    protected:
        void update(Seconds<float> delta) override;

        explicit CollisionObject2D(SPhys::CollisionObject2D* collisionObject) noexcept;

        void afterTreeEnter() override;
        void beforeTreeExit() override;

        void readback() noexcept;
    private:
        SPhys::CollisionObject2D* mCollisionObject {};
    };

    [[nodiscard]] Error serialiseCollisionShape(const SPhys::Shape2D& shape, BinaryOutFileAccessor file) noexcept;
    [[nodiscard]] Error deserialiseCollisionShape(SPhys::Shape2D& shape, BinaryInFileAccessor file) noexcept;
}
