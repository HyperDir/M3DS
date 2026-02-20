#include <m3ds/nodes/ui/Panel.hpp>

namespace M3DS {
    Panel::Panel(Style style) noexcept
        : mStyle(std::move(style))
    {}

    void Panel::setStyle(Style to) noexcept {
        mStyle = std::move(to);
        queueResize();
    }

    const Style& Panel::getStyle() const noexcept {
        return mStyle;
    }

    Failure Panel::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        return M3DS::serialise(mStyle, serialiser);
    }

    Failure Panel::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        return M3DS::deserialise(mStyle, deserialiser);
    }

    void Panel::draw(RenderTarget2D& target) {
        target.draw(mStyle, getGlobalTransform(), getSize());

        UINode::draw(target);
    }

    void Panel::updateMinSize() noexcept {
        UINode::updateMinSize();

        mInternalMinSize = max(mInternalMinSize, M3DS::getMinSize(mStyle));
    }

    REGISTER_NO_METHODS(Panel);
    REGISTER_NO_MEMBERS(Panel);
}
