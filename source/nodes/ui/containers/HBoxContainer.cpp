#include <m3ds/nodes/ui/containers/HBoxContainer.hpp>

namespace M3DS {
    HBoxContainer::HBoxContainer() noexcept
        : BoxContainer(false)
    {}

    Failure HBoxContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Failure HBoxContainer::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }



    REGISTER_NO_METHODS(HBoxContainer);
    REGISTER_NO_MEMBERS(HBoxContainer);
}
