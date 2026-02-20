#include <m3ds/nodes/2d/Node2D.hpp>

namespace M3DS {
    Failure Node2D::serialise(Serialiser& serialiser) const noexcept {
        return CanvasItem::serialise(serialiser);
    }

    Failure Node2D::deserialise(Deserialiser& deserialiser) noexcept {
        return CanvasItem::deserialise(deserialiser);
    }


    REGISTER_NO_METHODS(Node2D);
    REGISTER_NO_MEMBERS(Node2D);
}
