#include <m3ds/nodes/2d/StaticBody2D.hpp>

namespace M3DS {
    StaticBody2D::StaticBody2D()
        : PhysicsBody2D(&mSpecialisedObject)
    {
        mSpecialisedObject.userData = this;
    }

    Error StaticBody2D::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }


    Error StaticBody2D::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }

    REGISTER_NO_METHODS(StaticBody2D);
    REGISTER_NO_MEMBERS(StaticBody2D);
}
