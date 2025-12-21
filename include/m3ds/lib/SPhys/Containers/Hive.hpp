#pragma once

#include <vector>
#include <cstdint>
#include <array>
#include <memory>
#include <iterator>
#include <bitset>
#include <type_traits>
#include <cassert>
#include <limits>

namespace SPhys {
    constexpr std::size_t HiveBucketSize = sizeof(std::size_t) * std::numeric_limits<unsigned char>::digits;

    template <typename T>
    requires (!std::is_reference_v<T>)
    union HiveStorage {
        constexpr HiveStorage() noexcept requires (std::is_trivially_constructible_v<T>) = default;
        constexpr HiveStorage() noexcept requires (!std::is_trivially_constructible_v<T>) {}

        HiveStorage(const HiveStorage&) = delete;
        HiveStorage& operator=(const HiveStorage&) = delete;

        HiveStorage(HiveStorage&&) = delete;
        HiveStorage& operator=(HiveStorage&&) = delete;

        constexpr ~HiveStorage() noexcept requires (std::is_trivially_destructible_v<T>) = default;
        constexpr ~HiveStorage() noexcept requires (!std::is_trivially_destructible_v<T>) {}

        T object;
        std::uint8_t skip {};

        static_assert(HiveBucketSize * 2 <= std::numeric_limits<decltype(skip)>::max());
    };

    template <typename ElementType, bool IsConst>
    class HiveIterator;

    template <typename ElementType, bool IsConst = false>
    class ReverseHiveIterator;

    template <typename ElementType>
    class Hive;

    template <typename ElementType>
    class HiveBucket {
        friend HiveIterator<ElementType, true>;
        friend HiveIterator<ElementType, false>;
        friend Hive<ElementType>;

        std::array<HiveStorage<ElementType>, HiveBucketSize> block {};
        std::bitset<HiveBucketSize> mValidElements {}; 

        std::unique_ptr<HiveBucket> next {};
        HiveBucket* prev {};
    public:
        ~HiveBucket() noexcept {
            for (std::size_t i{}; i < HiveBucketSize; ++i) {
                if (mValidElements[i])
                    std::destroy_at(&block[i].object);
            }
        }
    };

    template <typename ElementType, bool IsConst = false>
    class HiveIterator {
        friend class Hive<ElementType>;
        friend class ReverseHiveIterator<ElementType>;

        HiveBucket<ElementType>* mBlock {};
        std::ptrdiff_t mElement {};

        HiveIterator(HiveBucket<ElementType>* block, std::ptrdiff_t element)
            : mBlock(block)
            , mElement(element)
        {}

        void settle() {
            if (mBlock && !mBlock->mValidElements[static_cast<std::size_t>(mElement)]) {
                mElement += mBlock->block[static_cast<std::size_t>(mElement)].skip;
                if (mBlock && mElement >= static_cast<std::ptrdiff_t>(HiveBucketSize)) {
                    mBlock = mBlock->next.get();
                    mElement -= HiveBucketSize;
                }
            }
        }

        void settleReverse() {
            if (mBlock && !mBlock->mValidElements[static_cast<std::size_t>(mElement)]) {
                mElement -= mBlock->block[static_cast<std::size_t>(mElement)].skip;
                if (mBlock && mElement < 0) {
                    mBlock = mBlock->prev;
                    mElement += HiveBucketSize;
                }
            }
        }
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = ElementType;
        using difference_type   = std::ptrdiff_t;
        using pointer           = std::conditional_t<IsConst, const value_type, value_type>*;
        using reference         = std::conditional_t<IsConst, const value_type, value_type>&;

        constexpr HiveIterator() noexcept = default;

        constexpr reference operator*() const noexcept {
            return mBlock->block[static_cast<std::size_t>(mElement)].object;
        }

        constexpr pointer operator->() const noexcept {
            return &mBlock->block[mElement].object;
        }

        constexpr HiveIterator& operator++() noexcept {
            if (++mElement == HiveBucketSize) {
                mElement = 0;
                mBlock = mBlock->next.get();
            }

            settle();

            return *this;
        }

        constexpr HiveIterator& operator--() noexcept {
            if (mElement-- == 0) {
                mElement = HiveBucketSize - 1;
                mBlock = mBlock->prev;
            }

            settleReverse();

            return *this;
        }

        constexpr HiveIterator operator++(int) noexcept {
            HiveIterator tmp { *this };
            ++(*this);
            return tmp;
        }

        constexpr HiveIterator operator--(int) noexcept {
            HiveIterator tmp { *this };
            --(*this);
            return tmp;
        }

        [[nodiscard]] constexpr bool operator==(const HiveIterator& other) const noexcept = default;
    };

    template <typename ElementType, bool IsConst>
    class ReverseHiveIterator {
        friend class Hive<ElementType>;

        HiveIterator<ElementType, IsConst> base {};

        ReverseHiveIterator(HiveBucket<ElementType>* block, std::size_t element) : base(block, element) {}
    public:
        constexpr ReverseHiveIterator() noexcept = default;

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = ElementType;
        using difference_type   = std::ptrdiff_t;
        using pointer           = HiveIterator<ElementType, IsConst>::pointer;
        using reference         = HiveIterator<ElementType, IsConst>::reference;

        constexpr reference operator*() const noexcept {
            return *base;
        }

        constexpr pointer operator->() const noexcept {
            return &*base;
        }

        constexpr ReverseHiveIterator& operator++() noexcept {
            --base;
            return *this;
        }

        constexpr ReverseHiveIterator& operator--() noexcept {
            ++base;
            return *this;
        }

        constexpr ReverseHiveIterator operator++(int) noexcept {
            ReverseHiveIterator tmp { *this };
            ++(*this);
            return tmp;
        }

        constexpr ReverseHiveIterator operator--(int) noexcept {
            ReverseHiveIterator tmp { *this };
            --(*this);
            return tmp;
        }

        [[nodiscard]] constexpr bool operator==(const ReverseHiveIterator& other) const noexcept = default;
    };


    template <typename ElementType>
    class Hive {
        std::unique_ptr<HiveBucket<ElementType>> mHead {};
        HiveBucket<ElementType>* mTail {};

        std::ptrdiff_t mEndIndex {};
    public:
        [[nodiscard]] constexpr HiveIterator<ElementType> begin() noexcept {
            HiveIterator<ElementType> it { mHead.get(), 0 };
            it.settle();
            return it;
        }

        [[nodiscard]] constexpr HiveIterator<ElementType> end() noexcept {
            return { mEndIndex ? mTail : nullptr, mEndIndex };
        }

        [[nodiscard]] constexpr ReverseHiveIterator<ElementType> rbegin() noexcept {
            ReverseHiveIterator<ElementType> it { mTail, mEndIndex ? mEndIndex - 1 : HiveBucketSize - 1 };
            it.base.settleReverse();
            return it;
        }

        [[nodiscard]] constexpr ReverseHiveIterator<ElementType> rend() noexcept {
            return { nullptr, HiveBucketSize - 1 };
        }

        [[nodiscard]] constexpr HiveIterator<ElementType, true> begin() const noexcept {
            HiveIterator<ElementType, true> it { mHead.get(), 0 };
            it.settle();
            return it;
        }

        [[nodiscard]] constexpr HiveIterator<ElementType, true> end() const noexcept {
            return { mEndIndex ? mTail : nullptr, mEndIndex };
        }

        [[nodiscard]] constexpr ReverseHiveIterator<ElementType, true> rbegin() const noexcept {
            ReverseHiveIterator<ElementType, true> it { mTail, mEndIndex ? mEndIndex - 1 : HiveBucketSize - 1 };
            it.base.settleReverse();
            return it;
        }

        [[nodiscard]] constexpr ReverseHiveIterator<ElementType, true> rend() const noexcept {
            return { nullptr, HiveBucketSize - 1 };
        }

        template <typename... Args> 
        constexpr HiveIterator<ElementType> insert(Args&&... args) {
            for (HiveBucket<ElementType>* curr = mHead.get(); curr != nullptr; curr = curr->next.get()) {
                if (curr->mValidElements.all())
                    continue;
                
                const std::size_t i = static_cast<std::size_t>(std::countr_one(std::bit_cast<std::size_t>(curr->mValidElements)));
                if (!curr->mValidElements[i]) {
                    if (curr == mTail && static_cast<std::ptrdiff_t>(i) == mEndIndex) {
                        if (++mEndIndex == HiveBucketSize)
                            mEndIndex = 0;
                    } else {
                        // Clean up skip count
                        const std::size_t skip = curr->block[i].skip;
                        if (skip > 1) {
                            // Handle near corner
                            HiveBucket<ElementType>* tmp = curr;
                            if (i == HiveBucketSize - 1) {
                                tmp = tmp->next.get();
                                assert(!tmp->mValidElements[0]);
                                tmp->block[0].skip = static_cast<std::uint8_t>(skip - 1);
                            } else {
                                assert(!tmp->mValidElements[i + 1]);
                                tmp->block[i + 1].skip = static_cast<std::uint8_t>(skip - 1);
                            }

                            // Handle far corner
                            if (skip > 2) {
                                std::ptrdiff_t idx = static_cast<std::ptrdiff_t>(i + skip) - 1;
                                if (idx >= static_cast<std::ptrdiff_t>(HiveBucketSize)) {
                                    tmp = tmp->next.get();
                                    idx -= HiveBucketSize;
                                    assert(tmp);
                                }

                                tmp->block[static_cast<std::size_t>(idx)].skip = static_cast<std::uint8_t>(skip - 1);
                            }
                        }
                    }

                    curr->mValidElements.set(i);
                    std::construct_at<ElementType>(&curr->block[i].object, std::forward<Args>(args)...);

                    return HiveIterator<ElementType>{ curr, static_cast<std::ptrdiff_t>(i) };
                }
            }

            assert(mEndIndex == 0);
            mEndIndex = 1;

            if (mTail) {
                auto* newTail = (mTail->next = std::make_unique<HiveBucket<ElementType>>()).get();
                newTail->prev = mTail;
                mTail = newTail;
            } else {
                mTail = (mHead = std::make_unique<HiveBucket<ElementType>>()).get();
            }
            mTail->mValidElements.set(0);

            std::construct_at<ElementType>(&mTail->block[0].object, std::forward<Args>(args)...);

            return HiveIterator<ElementType>{ mTail, 0 };
        }

        template <typename... Args>
        constexpr ElementType& emplace(Args&&... args) {
            return *insert(std::forward<Args>(args)...);
        }

        constexpr void erase(const HiveIterator<ElementType>& it) noexcept {
            assert(it.mBlock);

            it.mBlock->mValidElements.reset(static_cast<std::size_t>(it.mElement));

            HiveBucket<ElementType>* block = it.mBlock;

            std::uint8_t leftSkip {};
            std::uint8_t* leftCorner {};

            // Count skip value to left of element
            {
                if (it.mElement == 0) {
                    HiveBucket<ElementType>* curr = block->prev;
                    if (curr == nullptr || curr->mValidElements[HiveBucketSize - 1]) {
                        leftSkip = 0;
                    } else {
                        leftSkip = curr->block[HiveBucketSize - 1].skip;
                    }
                } else {
                    const std::size_t leftIndex = static_cast<std::size_t>(it.mElement) - 1;
                    if (block->mValidElements[leftIndex]) {
                        leftSkip = 0;
                    } else {
                        leftSkip = block->block[leftIndex].skip;
                    }
                }

                // Find left corner
                if (leftSkip == 0) {
                    leftCorner = &block->block[static_cast<std::size_t>(it.mElement)].skip;
                } else {
                    std::ptrdiff_t idx = it.mElement - static_cast<std::ptrdiff_t>(leftSkip);
                    HiveBucket<ElementType>* curr = block;
                    if (idx < 0) {
                        curr = curr->prev;
                        idx += HiveBucketSize;
                        assert(curr != nullptr);
                    }

                    leftCorner = &curr->block[static_cast<std::size_t>(idx)].skip;
                }
            }

            std::uint8_t rightSkip {};
            std::uint8_t* rightCorner {};

            // Count skip value to right of element
            {
                if (it.mElement == HiveBucketSize - 1) {
                    HiveBucket<ElementType>* curr = block->next.get();
                    if (curr == nullptr || curr->mValidElements[0]) {
                        rightSkip = 0;
                    } else {
                        rightSkip = curr->block[0].skip;
                    }
                } else {
                    const std::size_t rightIndex = static_cast<std::size_t>(it.mElement) + 1;
                    if (block->mValidElements[rightIndex]) {
                        rightSkip = 0;
                    } else {
                        rightSkip = block->block[rightIndex].skip;
                    }
                }

                // Find right corner
                if (rightSkip == 0) {
                    rightCorner = &block->block[static_cast<std::size_t>(it.mElement)].skip;
                } else {
                    std::ptrdiff_t idx = it.mElement + static_cast<std::ptrdiff_t>(rightSkip);
                    HiveBucket<ElementType>* curr = block;
                    if (idx >= static_cast<std::ptrdiff_t>(HiveBucketSize)) {
                        curr = curr->next.get();
                        idx -= HiveBucketSize;
                        assert(curr != nullptr);
                    }

                    rightCorner = &curr->block[static_cast<std::size_t>(idx)].skip;
                }
            }

            std::destroy_at(&block->block[static_cast<std::size_t>(it.mElement)].object);
            std::uint8_t totalSkip = static_cast<std::uint8_t>(leftSkip + rightSkip + 1);

            if (block->mValidElements.none()) {
                totalSkip -= static_cast<std::uint8_t>(HiveBucketSize);

                std::unique_ptr<HiveBucket<ElementType>> next = std::move(block->next);
                HiveBucket<ElementType>* prev = block->prev;

                if (next)
                    next->prev = prev;

                if (prev)
                    std::swap(prev->next, next);
                else
                    std::swap(mHead, next);
            }
        
            *leftCorner = totalSkip;
            *rightCorner = totalSkip;
        }
    };
}
