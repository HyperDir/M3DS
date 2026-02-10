#include <m3ds/nodes/ui/containers/ScrollContainer.hpp>

namespace M3DS {
    ScrollContainer::ScrollContainer() noexcept {
        setClipContent(true);
    }

    Failure ScrollContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (!file.write(mScrollMode) || !file.write(mScrollPosition))
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure ScrollContainer::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        if (!file.read(mScrollMode) || !file.read(mScrollPosition))
            return Failure{ ErrorCode::file_read_fail };

        return Success;
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
