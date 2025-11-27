#include <m3ds/nodes/ui/containers/ScrollContainer.hpp>

namespace M3DS {
    ScrollContainer::ScrollContainer() noexcept {
        setClipContent(true);
    }

    Error ScrollContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Container::serialise(file); error != Error::none)
            return error;

        if (!file.write(mScrollMode) || !file.write(mScrollPosition))
            return Error::file_write_fail;

        return Error::none;
    }

    Error ScrollContainer::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = Container::deserialise(file); error != Error::none)
            return error;

        if (!file.read(mScrollMode) || !file.read(mScrollPosition))
            return Error::file_read_fail;

        return Error::none;
    }

    void ScrollContainer::input(Input::InputFrame& inputFrame) {
        Container::input(inputFrame);

        if (mScrollMode & ScrollMode::vertical) {
            mScrollPosition.y += inputFrame.getLeftJoy().y;
        }
        if (mScrollMode & ScrollMode::horizontal) {
            mScrollPosition.x += inputFrame.getLeftJoy().x;
        }

        mScrollPosition = mScrollPosition.clamp({}, getChildrenMaxSize() - getSize());

        for (auto* child : getChildrenOfType<UINode>()) {
            child->setTranslation(-mScrollPosition);
        }
    }

    void ScrollContainer::resize() noexcept {
        Container::resize();

        for (auto* child : getChildrenOfType<UINode>()) {
            child->setTranslation(-mScrollPosition);
        }
    }

    void ScrollContainer::setScrollMode(const ScrollMode to) noexcept {
        mScrollMode = to;
        queueResize();
    }

    void ScrollContainer::updateMinSize() noexcept {
        Container::updateMinSize();

        if (!(mScrollMode & ScrollMode::horizontal))
            mInternalMinSize.x = std::max(mInternalMinSize.x, getChildrenMaxSize().x);

        if (!(mScrollMode & ScrollMode::vertical))
            mInternalMinSize.y = std::max(mInternalMinSize.y, getChildrenMaxSize().y);
    }

    REGISTER_NO_METHODS(ScrollContainer);
    REGISTER_NO_MEMBERS(ScrollContainer);
}
