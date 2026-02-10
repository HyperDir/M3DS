#include <m3ds/nodes/2d/PhysicsBody2D.hpp>

namespace M3DS {
    Failure PhysicsBody2D::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Failure PhysicsBody2D::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }

    REGISTER_NO_METHODS(PhysicsBody2D);
    REGISTER_NO_MEMBERS(PhysicsBody2D);
}
