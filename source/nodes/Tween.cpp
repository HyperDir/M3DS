#include <m3ds/nodes/Tween.hpp>

namespace M3DS {
    bool Tween::isActive() const noexcept {
        return mActive;
    }

    void Tween::stop() noexcept {
        mActive = false;
    }

    void Tween::update(const Seconds<float> delta) {
        if (mActive) {
            mElapsed += delta;

            mTweenData.visit(
                [&]<typename T>(const T& arg) {
                    if (mElapsed >= mDuration) {
                        arg.member->set(mTweenObject, arg.to);
                        mActive = false;
                        tweenComplete.emit(this);
                    } else if constexpr (CanInterpolate<T>) {
                        arg.member->set(
                            mTweenObject,
                            interpolate(arg.from, arg.to, mElapsed / mDuration, mMethod, mEasing)
                        );
                    }
                }
            );
        }

        Node::update(delta);
    }

    Error Tween::serialise([[maybe_unused]] BinaryOutFileAccessor file) const noexcept {
        Debug::err("Tween serialisation not implemented!");
        return Error::not_implemented;
    }

    Error Tween::deserialise([[maybe_unused]] BinaryInFileAccessor file) noexcept {
        Debug::err("Tween serialisation not implemented!");
        return Error::not_implemented;
    }



    REGISTER_NO_METHODS(Tween);
    REGISTER_NO_MEMBERS(Tween);
}
