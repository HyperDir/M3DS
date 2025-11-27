#include <m3ds/nodes/3d/StaticBody3D.hpp>

namespace M3DS {
    StaticBody3D::StaticBody3D()
        : PhysicsBody3D(&mSpecialisedObject)
    {
        mSpecialisedObject.userData = this;
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
