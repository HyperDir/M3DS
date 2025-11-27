#pragma once

#include <m3ds/nodes/Node.hpp>
#include <m3ds/types/Signal.hpp>
#include <m3ds/utils/Interpolate.hpp>

namespace M3DS {
    // TODO: serialisation

    class Tween : public Node {
        M_CLASS(Tween, Node)
    public:
        Signal tweenComplete {};

        template <typename O, typename T>
        void tween(
            O* object,
            const Member<T>* member,
            T tweenTo,
            float duration,
            InterpolationMethod method = InterpolationMethod::linear,
            Easing easing = Easing::in_out
        ) noexcept;

        // void tween(
        //     Object* object,
        //     const GenericMember* member,
        //     const BindableTypesVariant& tweenTo,
        //     float duration,
        //     Method method = Method::linear,
        //     Easing easing = Easing::in_out
        // ) noexcept;

        [[nodiscard]] bool isActive() const noexcept;
        void stop() noexcept;
    protected:
        void update(Seconds<float> delta) override;
    private:
        float mDuration {};
        float mElapsed {};
        bool mActive {};

        InterpolationMethod mMethod {};
        Easing mEasing {};

        Object* mTweenObject {};

        template <typename T>
        struct TweenData {
            const Member<T>* member {};
            T from {};
            T to {};
        };

        using TweenVariant = BindableTypes::transform<TweenData>::apply<std::variant>;

        TweenVariant mTweenData { TweenData<bool>{} };
    };

    template <typename O, typename T>
    void Tween::tween(
        O* object,
        const Member<T>* member,
        const T tweenTo,
        const float duration,
        const InterpolationMethod method,
        const Easing easing
    ) noexcept {
        mTweenObject = object;

        mTweenData = TweenData<T>{ member, member->get(object), tweenTo };

        mDuration = duration;

        mMethod = method;
        mEasing = easing;

        mElapsed = 0.f;
        mActive = true;
    }
}
