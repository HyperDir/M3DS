#include <m3ds/nodes/ui/containers/Container.hpp>

namespace M3DS {
    Vector2 Container::getChildrenMaxSize() noexcept {
        Vector2 maxSize {};
        for (const auto* child : getChildrenOfType<UINode>()) {
            maxSize = max(maxSize, child->getMinSize());
        }
        return maxSize;
    }

    Failure Container::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure Container::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }



    REGISTER_NO_METHODS(Container);
    REGISTER_NO_MEMBERS(Container);
}
