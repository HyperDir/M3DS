#include <m3ds/nodes/Timer.hpp>

namespace M3DS {
    void Timer::start() noexcept {
        mElapsed = 0;
        mActive = true;
    }

    void Timer::restart() noexcept {
        mElapsed = 0;
    }

    void Timer::pause() noexcept {
        mActive = false;
    }

    void Timer::stop() noexcept {
        mActive = false;
        mElapsed = 0;
    }

    void Timer::resume() noexcept {
        mActive = true;
    }

    bool Timer::isActive() const noexcept {
        return mActive;
    }

    Seconds<float> Timer::getElapsed() const noexcept {
        return mElapsed;
    }

    Seconds<float> Timer::getTimeLeft() const noexcept {
        return duration - mElapsed;
    }

    Failure Timer::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (
            !serialiser.write(duration) ||
            !serialiser.write(oneShot) ||
            !serialiser.write(mActive) ||
            !serialiser.write(mElapsed)
        )
            return Failure{ ErrorCode::file_write_fail };

        return timeout.serialise(this, serialiser);
    }

    Failure Timer::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        if (
            !deserialiser.read(duration) ||
            !deserialiser.read(oneShot) ||
            !deserialiser.read(mActive) ||
            !deserialiser.read(mElapsed)
        )
            return Failure{ ErrorCode::file_write_fail };

        return timeout.deserialise(this, deserialiser);
    }

    void Timer::update(const Seconds<float> delta) {
        Node::update(delta);

        if (mActive) {
            mElapsed += delta;
            if (mElapsed >= duration) {
                if (oneShot)
                    stop();
                else
                    mElapsed -= duration;
                timeout.emit();
            }
        }
    }

    REGISTER_METHODS(
        Timer,

        MUTABLE_METHOD(start),
        MUTABLE_METHOD(restart),
        MUTABLE_METHOD(pause),
        MUTABLE_METHOD(stop),
        MUTABLE_METHOD(resume),
        CONST_METHOD(isActive),
        CONST_METHOD(getElapsed),
        CONST_METHOD(getTimeLeft)
    );

    REGISTER_MEMBERS(
        Timer,

        MEMBER(duration),
        MEMBER(oneShot)
    );
}
