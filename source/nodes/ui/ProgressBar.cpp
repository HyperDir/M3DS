#include <m3ds/nodes/ui/ProgressBar.hpp>

namespace M3DS {
    Error ProgressBar::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = UINode::serialise(file); error != Error::none)
            return error;

        if (
            !file.write(fillMode) ||
            !file.write(value) ||
            !file.write(minValue) ||
            !file.write(maxValue) ||
            !file.write(backgroundColour) ||
            !file.write(fillColour)
        )
            return Error::file_write_fail;

        return Error::none;
    }

    Error ProgressBar::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = UINode::deserialise(file); error != Error::none)
            return error;

        if (
            !file.read(fillMode) ||
            !file.read(value) ||
            !file.read(minValue) ||
            !file.read(maxValue) ||
            !file.read(backgroundColour) ||
            !file.read(fillColour)
        )
            return Error::file_read_fail;

        return Error::none;
    }

    void ProgressBar::updateMinSize() noexcept {
        UINode::updateMinSize();

        mInternalMinSize = max(mInternalMinSize, {1, 1});
    }

    void ProgressBar::draw(RenderTarget2D& target) {
        const Transform2D& xform = getGlobalTransform();
        const float fillSizeX = getSize().x * value / (maxValue - minValue);

        target.drawRectSolid(xform, getSize(), backgroundColour);
        if (fillMode == FillMode::left_to_right) {
            target.drawRectSolid(xform, {fillSizeX, getSize().y}, fillColour);
        } else if (fillMode == FillMode::right_to_left) {
            target.drawRectSolid(
                xform.offset(getSize() - Vector2{fillSizeX, 0}),
                {fillSizeX, getSize().y},
                fillColour
            );
        } else {
            Debug::err("Unknown fill mode: {}", std::to_underlying(fillMode));
        }

        UINode::draw(target);
    }

    REGISTER_NO_METHODS(ProgressBar);

    REGISTER_MEMBERS(
        ProgressBar,

        MEMBER(value),
        MEMBER(minValue),
        MEMBER(maxValue)
    );
}
