#include <m3ds/nodes/ui/containers/MarginContainer.hpp>

namespace M3DS {
    const Margin& MarginContainer::getMargin() const noexcept {
        return mMargin;
    }

    void MarginContainer::setMargin(const Margin& to) noexcept {
        if (mMargin != to) {
            mMargin = to;
            queueResize();
        }
    }

    Failure MarginContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (!file.write(mMargin))
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure MarginContainer::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        if (!file.read(mMargin))
            return Failure{ ErrorCode::file_read_fail };

        return Success;
    }

    void MarginContainer::updateMinSize() noexcept {
        Container::updateMinSize();

        mInternalMinSize = max(mInternalMinSize, getChildrenMaxSize() + mMargin.getSize());
    }

    void MarginContainer::resize() noexcept {
        Container::resize();

        const Vector2 pos = mMargin.getPosition();
        const Vector2 size = getSize() - mMargin.getSize();
        for (auto* child : getChildrenOfType<UINode>()) {
            child->setTranslation(pos);
            child->setSize(size);
        }
    }

    REGISTER_NO_METHODS(MarginContainer);
    REGISTER_NO_MEMBERS(MarginContainer);
}
