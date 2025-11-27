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

    Error MarginContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Container::serialise(file); error != Error::none)
            return error;

        if (!file.write(mMargin))
            return Error::file_write_fail;

        return Error::none;
    }

    Error MarginContainer::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = Container::deserialise(file); error != Error::none)
            return error;

        if (!file.read(mMargin))
            return Error::file_read_fail;

        return Error::none;
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
