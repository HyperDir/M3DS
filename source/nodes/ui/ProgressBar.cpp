#include <m3ds/nodes/ui/ProgressBar.hpp>

namespace M3DS {
    Failure ProgressBar::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (
            !serialiser.write(fillMode) ||
            !serialiser.write(value) ||
            !serialiser.write(minValue) ||
            !serialiser.write(maxValue) ||
            !serialiser.write(backgroundColour) ||
            !serialiser.write(fillColour)
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure ProgressBar::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (
            !deserialiser.read(fillMode) ||
            !deserialiser.read(value) ||
            !deserialiser.read(minValue) ||
            !deserialiser.read(maxValue) ||
            !deserialiser.read(backgroundColour) ||
            !deserialiser.read(fillColour)
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
