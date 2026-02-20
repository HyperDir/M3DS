#include <m3ds/nodes/ui/containers/FillContainer.hpp>

namespace M3DS {
    void FillContainer::updateMinSize() noexcept {
        Container::updateMinSize();

        mInternalMinSize = max(
            mInternalMinSize,
            getChildrenMaxSize()
        );
    }

    void FillContainer::resize() noexcept {
        Container::resize();

        for (auto* child : getChildrenOfType<UINode>()) {
            child->setTranslation({});
            child->setSize(getSize());
        }
    }

    Failure FillContainer::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure FillContainer::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }



    REGISTER_NO_METHODS(FillContainer);
    REGISTER_NO_MEMBERS(FillContainer);
}
