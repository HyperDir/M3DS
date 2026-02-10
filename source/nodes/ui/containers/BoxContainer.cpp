#include <m3ds/nodes/ui/containers/BoxContainer.hpp>

namespace M3DS {
    BoxContainer::BoxContainer(const bool vertical) noexcept
        : mVertical(vertical)
    {}

    void BoxContainer::setAlign(const Align to) noexcept {
        if (mAlign != to) {
            mAlign = to;
            queueResize();
        }
    }

    Align BoxContainer::getAlign() const noexcept {
        return mAlign;
    }

    void BoxContainer::setSeparation(const float to) noexcept {
        if (mSeparation != to) {
            mSeparation = to;
            queueResize();
        }
    }

    float BoxContainer::getSeparation() const noexcept {
        return mSeparation;
    }

    Failure BoxContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (
            !file.write(mAlign) ||
            !file.write(mSeparation) ||
            !file.write(mVertical)
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure BoxContainer::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        if (
            !file.read(mAlign) ||
            !file.read(mSeparation) ||
            !file.read(mVertical)
        )
            return Failure{ ErrorCode::file_read_fail };

        return Success;

    }

    void BoxContainer::updateMinSize() noexcept {
        Container::updateMinSize();
        Vector2 minSize = {};

        if (mVertical) {
            for (const auto* child : getChildrenOfType<UINode>()) {
                minSize.x = std::max(minSize.x, child->getMinSize().x);
                minSize.y += child->getMinSize().y + mSeparation;
            }
            minSize.y -= mSeparation;
        } else {
            for (const auto* child : getChildrenOfType<UINode>()) {
                minSize.y = std::max(minSize.y, child->getMinSize().y);
                minSize.x += child->getMinSize().x + mSeparation;
            }
            minSize.x -= mSeparation;
        }

        mInternalMinSize = max(mInternalMinSize, minSize);
    }

    void BoxContainer::resize() noexcept {
        Container::resize();

        float totalLength {};
        std::size_t count {};
        std::size_t fillSpaceCount {};

        if (mVertical) {
            for (const auto* child : getChildrenOfType<UINode>()) {
                ++count;
                totalLength += child->getMinSize().y;
                if (child->getFillSpace()) ++fillSpaceCount;
            }
        } else {
            for (const auto* child : getChildrenOfType<UINode>()) {
                ++count;
                totalLength += child->getMinSize().x;
                if (child->getFillSpace()) ++fillSpaceCount;
            }
        }

        if (count == 0)
            return;

        totalLength += mSeparation * static_cast<float>(count - 1);

        const Vector2& size = getSize();

        const float leftoverLength = (mVertical ? size.y : size.x) - totalLength;
        const float fillSpaceLength = fillSpaceCount ? leftoverLength / static_cast<float>(fillSpaceCount) : 0;

        float pos {};
        if (mAlign == Align::begin) {
            pos = 0;
        } else if (mAlign == Align::centre) {
            pos = ((mVertical ? size.y : size.x) - totalLength) / 2.f;
        } else if (mAlign == Align::end) {
            pos = (mVertical ? size.y : size.x) - totalLength;
        } else {
            Debug::err("Invalid alignment: {}", std::to_underlying(mAlign));
            return;
        }
        if (mVertical) {
            for (auto* child : getChildrenOfType<UINode>()) {
                child->setTranslation({0, pos});
                if (child->getFillSpace()) {
                    child->setSize({size.x, child->getMinSize().y + fillSpaceLength});
                } else {
                    child->setSize({size.x, child->getMinSize().y});
                }
                pos += child->getSize().y + mSeparation;
            }
        } else {
            for (auto* child : getChildrenOfType<UINode>()) {
                child->setTranslation({pos, 0});
                if (child->getFillSpace()) {
                    child->setSize({child->getMinSize().x + fillSpaceLength, size.y});
                } else {
                    child->setSize({child->getMinSize().x, size.y});
                }
                pos += child->getSize().x + mSeparation;
            }
        }
    }

    REGISTER_METHODS(
        BoxContainer,

        MUTABLE_METHOD(setSeparation),
        CONST_METHOD(getSeparation)
    );

    REGISTER_MEMBERS(
        BoxContainer,

        PRIVATE_MEMBER(separation, getSeparation, setSeparation)
    );
}
