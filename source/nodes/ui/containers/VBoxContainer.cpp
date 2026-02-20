#include <m3ds/nodes/ui/containers/VBoxContainer.hpp>

namespace M3DS {
    VBoxContainer::VBoxContainer() noexcept
        : BoxContainer(true)
    {}

    Failure VBoxContainer::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure VBoxContainer::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }


    REGISTER_NO_METHODS(VBoxContainer);
    REGISTER_NO_MEMBERS(VBoxContainer);
}
