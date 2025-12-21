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

    Error StaticBody3D::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Error StaticBody3D::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }

    REGISTER_NO_METHODS(StaticBody3D);
    REGISTER_NO_MEMBERS(StaticBody3D);
}
