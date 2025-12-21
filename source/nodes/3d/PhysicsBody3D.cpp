#include <m3ds/nodes/3d/PhysicsBody3D.hpp>

namespace M3DS {
    Error PhysicsBody3D::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Error PhysicsBody3D::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }

    REGISTER_NO_METHODS(PhysicsBody3D);
    REGISTER_NO_MEMBERS(PhysicsBody3D);
}
