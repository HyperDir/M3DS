#include <m3ds/nodes/ui/containers/CentreContainer.hpp>

namespace M3DS {
    void CentreContainer::resize() noexcept {
        Container::resize();

        const Vector2 centre = getSize() * 0.5f;
        for (auto* child : getChildrenOfType<UINode>()) {
            const Vector2 childSize = child->getMinSize();
            child->setSize(childSize);
            child->setTranslation(centre - childSize * 0.5f);
        }
    }

    void CentreContainer::updateMinSize() noexcept {
        Container::updateMinSize();

        mInternalMinSize = max(
            mInternalMinSize,
            getChildrenMaxSize()
        );
    }

    Failure CentreContainer::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure CentreContainer::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }


    REGISTER_NO_METHODS(CentreContainer);
    REGISTER_NO_MEMBERS(CentreContainer);
}
