#include <m3ds/nodes/ui/ProgressBar.hpp>

namespace M3DS {
    Failure ProgressBar::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (
            !file.write(fillMode) ||
            !file.write(value) ||
            !file.write(minValue) ||
            !file.write(maxValue) ||
            !file.write(backgroundColour) ||
            !file.write(fillColour)
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure ProgressBar::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        if (
            !file.read(fillMode) ||
            !file.read(value) ||
            !file.read(minValue) ||
            !file.read(maxValue) ||
            !file.read(backgroundColour) ||
            !file.read(fillColour)
        )
            return Failure{ ErrorCode::file_read_fail };

        return Success;
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
