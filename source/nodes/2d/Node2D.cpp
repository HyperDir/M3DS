#include <m3ds/nodes/2d/Node2D.hpp>

namespace M3DS {
    Failure Node2D::serialise(BinaryOutFileAccessor file) const noexcept {
        return CanvasItem::serialise(file);
    }

    Failure Node2D::deserialise(BinaryInFileAccessor file) noexcept {
        return CanvasItem::deserialise(file);
    }


    REGISTER_NO_METHODS(Node2D);
    REGISTER_NO_MEMBERS(Node2D);
}
