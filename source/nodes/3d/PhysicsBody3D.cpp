#include <m3ds/nodes/3d/PhysicsBody3D.hpp>

namespace M3DS {
    Failure PhysicsBody3D::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure PhysicsBody3D::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }

    REGISTER_NO_METHODS(PhysicsBody3D);
    REGISTER_NO_MEMBERS(PhysicsBody3D);
}
