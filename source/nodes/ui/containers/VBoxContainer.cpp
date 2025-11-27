#include <m3ds/nodes/ui/containers/VBoxContainer.hpp>

namespace M3DS {
    VBoxContainer::VBoxContainer() noexcept
        : BoxContainer(true)
    {}

    Error VBoxContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Error VBoxContainer::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }


    REGISTER_NO_METHODS(VBoxContainer);
    REGISTER_NO_MEMBERS(VBoxContainer);
}
