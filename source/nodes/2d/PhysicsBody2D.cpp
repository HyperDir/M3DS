#include <m3ds/nodes/2d/PhysicsBody2D.hpp>

namespace M3DS {
    Failure PhysicsBody2D::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure PhysicsBody2D::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }

    REGISTER_NO_METHODS(PhysicsBody2D);
    REGISTER_NO_MEMBERS(PhysicsBody2D);
}
