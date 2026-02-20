#include <m3ds/nodes/2d/StaticBody2D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void StaticBody2D::afterTreeEnter() {
        PhysicsBody2D::afterTreeEnter();

        mAccessor = getViewport()->getPhysicsServer2d().emplaceStaticBody();
        mAccessor->userData = this;

        updateCollisionObject(mAccessor.get());
        externaliseState();
    }

    void StaticBody2D::beforeTreeExit() {
        PhysicsBody2D::beforeTreeExit();

        internaliseState();

        getViewport()->getPhysicsServer2d().eraseStaticBody(mAccessor);
        mAccessor = {};
        updateCollisionObject(nullptr);
    }

    Failure StaticBody2D::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure StaticBody2D::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }

    REGISTER_NO_METHODS(StaticBody2D);
    REGISTER_NO_MEMBERS(StaticBody2D);
}
