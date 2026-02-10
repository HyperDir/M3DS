#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    Vector2 Container::getChildrenMaxSize() noexcept {
        Vector2 maxSize {};
        for (const auto* child : getChildrenOfType<UINode>()) {
            maxSize = max(maxSize, child->getMinSize());
        }
        return maxSize;
    }

    Failure Container::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Failure Container::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }



    REGISTER_NO_METHODS(Container);
    REGISTER_NO_MEMBERS(Container);
}
