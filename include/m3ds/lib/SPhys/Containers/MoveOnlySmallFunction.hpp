#pragma once

#include <memory>

namespace SPhys {
    template <typename T>
    class MoveOnlySmallFunction;

    template <typename ReturnType, typename... Args>
    class MoveOnlySmallFunction<ReturnType(Args...)> {
        struct AbstractControlBlock {
            virtual ~AbstractControlBlock() = default;

            virtual ReturnType call(Args... args) = 0;
        };

        template <typename T>
        struct ControlBlock final : AbstractControlBlock {
            T callable {};

            explicit constexpr ControlBlock(T&& functor) : callable(std::move(functor)) {}

            ReturnType call(Args... args) override {
                return callable(std::forward<Args>(args)...);
            }
        };

        std::unique_ptr<AbstractControlBlock> mControlBlock {};
    public:
        [[nodiscard]] constexpr MoveOnlySmallFunction() noexcept = default;

        template <typename T>
        [[nodiscard]] constexpr MoveOnlySmallFunction(T&& functor)
            : mControlBlock(std::make_unique<ControlBlock<T>>(std::move(functor)))
        {}

        template <typename T>
        constexpr MoveOnlySmallFunction& operator=(T&& functor) {
            mControlBlock = std::make_unique<ControlBlock<T>>(std::move(functor));
            return *this;
        }

        [[nodiscard]] constexpr ReturnType operator()(Args... args) {
            return mControlBlock->call(std::forward<Args>(args)...);
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return static_cast<bool>(mControlBlock);
        }
    };
}
