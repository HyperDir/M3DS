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

    Error CentreContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Error CentreContainer::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }


    REGISTER_NO_METHODS(CentreContainer);
    REGISTER_NO_MEMBERS(CentreContainer);
}
