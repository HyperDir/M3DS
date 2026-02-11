#pragma once

#include <array>

#include <m3ds/containers/StaticHeapArray.hpp>
#include <m3ds/types/Failure.hpp>

namespace M3DS {
    template <typename Element, std::size_t N, typename Allocator>
    class ScratchBuffer {
    public:
        [[nodiscard]] constexpr std::span<Element> getCurrentSpan() noexcept {
            return { mHead, mCount };
        }

        constexpr void startNewSpan() noexcept {
            mHead += mCount;
            mCount = 0;
        }

        template <typename... Args>
        [[nodiscard]] constexpr Failure emplace(Args&&... args) noexcept {
            if (mHead + mCount + 1 >= getCurrentBuffer().end())
                return { ErrorCode::out_of_bounds };

            mHead[mCount++] = Element{ std::forward<Args>(args)... };
            return Success;
        }

        [[nodiscard]] constexpr Failure emplace(std::span<const Element> elements) noexcept {
            if (elements.size() + mHead + mCount >= getCurrentBuffer().end())
                return { ErrorCode::out_of_bounds };

            for (const auto& element : elements)
                mHead[mCount++] = element;
            return Success;
        }

        constexpr void swap() noexcept {
            mCurrentBuffer = !mCurrentBuffer;
            mHead = getCurrentBuffer().data();
            mCount = 0;
        }
    private:
        [[nodiscard]] constexpr StaticHeapArray<Element, N, Allocator>& getCurrentBuffer() noexcept {
            return mBuffers[mCurrentBuffer];
        }

        std::array<StaticHeapArray<Element, N, Allocator>, 2> mBuffers {};
        bool mCurrentBuffer {};
        Element* mHead = getCurrentBuffer().data();
        std::size_t mCount {};
    };
}
