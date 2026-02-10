#include <m3ds/nodes/ui/containers/PanelContainer.hpp>

namespace M3DS {
    void PanelContainer::setStyle(Style to) noexcept {
        if (mStyle != to) {
            mStyle = std::move(to);
            queueResize();
        }
    }

    const Style& PanelContainer::getStyle() const noexcept {
        return mStyle;
    }

    Failure PanelContainer::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        return M3DS::serialise(mStyle, file);
    }

    Failure PanelContainer::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        return M3DS::deserialise(mStyle, file);
    }

    void PanelContainer::draw(RenderTarget2D& target) {
        target.draw(mStyle, getGlobalTransform(), getSize());

        Container::draw(target);
    }

    void PanelContainer::updateMinSize() noexcept {
        Container::updateMinSize();

        mInternalMinSize = max(
            mInternalMinSize,
            max(
                M3DS::getMinSize(mStyle),
                getChildrenMaxSize() + getContentMargin(mStyle).getSize()
            )
        );
    }

    void PanelContainer::resize() noexcept {
        Container::resize();

        const Margin& margin = getContentMargin(mStyle);
        const Vector2 pos = margin.getPosition();
        const Vector2 size = getSize() - margin.getSize();

        for (auto* child : getChildrenOfType<UINode>()) {
            child->setTranslation(pos);
            child->setSize(size);
        }
    }

    REGISTER_NO_METHODS(PanelContainer);
    REGISTER_NO_MEMBERS(PanelContainer);
}
