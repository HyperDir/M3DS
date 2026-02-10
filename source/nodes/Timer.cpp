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

    Failure Timer::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (
            !file.write(duration) ||
            !file.write(oneShot) ||
            !file.write(mActive) ||
            !file.write(mElapsed)
        )
            return Failure{ ErrorCode::file_write_fail };

        return timeout.serialise(file);
    }

    Failure Timer::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        if (
            !file.read(duration) ||
            !file.read(oneShot) ||
            !file.read(mActive) ||
            !file.read(mElapsed)
        )
            return Failure{ ErrorCode::file_write_fail };

        return timeout.deserialise(file);
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
                timeout.emit(this);
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
