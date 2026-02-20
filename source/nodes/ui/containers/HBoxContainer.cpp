#include <m3ds/nodes/ui/containers/HBoxContainer.hpp>

namespace M3DS {
    HBoxContainer::HBoxContainer() noexcept
        : BoxContainer(false)
    {}

    Failure HBoxContainer::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure HBoxContainer::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }



    REGISTER_NO_METHODS(HBoxContainer);
    REGISTER_NO_MEMBERS(HBoxContainer);
}
