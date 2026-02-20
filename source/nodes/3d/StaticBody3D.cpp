#include <m3ds/nodes/3d/StaticBody3D.hpp>

#include <m3ds/nodes/Viewport.hpp>

namespace M3DS {
    void StaticBody3D::afterTreeEnter() {
        PhysicsBody3D::afterTreeEnter();

        mAccessor = getViewport()->getPhysicsServer3d().emplaceStaticBody();
        mAccessor->userData = this;

        updateCollisionObject(mAccessor.get());
        externaliseState();
    }

    void StaticBody3D::beforeTreeExit() {
        PhysicsBody3D::beforeTreeExit();

        internaliseState();

        getViewport()->getPhysicsServer3d().eraseStaticBody(mAccessor);
        mAccessor = {};
        updateCollisionObject(nullptr);
    }

    Failure StaticBody3D::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure StaticBody3D::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }

    REGISTER_NO_METHODS(StaticBody3D);
    REGISTER_NO_MEMBERS(StaticBody3D);
}
