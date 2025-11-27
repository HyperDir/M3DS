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

    Error Panel::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = UINode::serialise(file); error != Error::none)
            return error;

        return M3DS::serialise(mStyle, file);
    }

    Error Panel::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = UINode::deserialise(file); error != Error::none)
            return error;

        return M3DS::deserialise(mStyle, file);
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
