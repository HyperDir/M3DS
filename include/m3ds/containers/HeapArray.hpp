#pragma once

#include <memory>
#include <utility>

namespace M3DS {
	template <typename T, std::unsigned_integral SizeType = std::size_t, typename Allocator = std::allocator<T>>
	class HeapArray {
	public:
		using value_type = T;
		using size_type = SizeType;
		using allocator_type = Allocator;

		explicit constexpr HeapArray(Allocator alloc = {});
		explicit constexpr HeapArray(size_type size, Allocator alloc = {});
		explicit constexpr HeapArray(size_type size, T fill, Allocator alloc = {});

		template <typename Iterator>
		constexpr HeapArray(Iterator begin, Iterator end, Allocator alloc = {});

		constexpr HeapArray(const HeapArray& other);
		constexpr HeapArray& operator=(const HeapArray& other);

		constexpr HeapArray(HeapArray&& other) noexcept;
	    constexpr HeapArray& operator=(HeapArray&& other) noexcept;

		constexpr ~HeapArray() noexcept;

		constexpr void resize(size_type newSize);
		constexpr void resize(size_type newSize, T fill);
		constexpr void clear() noexcept;

		[[nodiscard]] constexpr value_type* data();
		[[nodiscard]] constexpr const value_type* data() const;

		[[nodiscard]] constexpr size_type size() const;
		[[nodiscard]] constexpr bool empty() const;

		[[nodiscard]] constexpr auto* begin(this auto&& self);
		[[nodiscard]] constexpr auto* end(this auto&& self);

		[[nodiscard]] constexpr auto& back(this auto&& self);

		[[nodiscard]] constexpr auto& at(this auto&& self, size_type i);
		[[nodiscard]] constexpr auto& operator[](this auto&& self, size_type i);

		explicit operator std::span<T>() noexcept;
		explicit operator std::span<const T>() const noexcept;
	private:
		[[no_unique_address]] allocator_type mAllocator {};
		value_type* mAlloc {};
		size_type mSize {};

		constexpr void allocate(size_type size);
		constexpr void deallocate();
	};

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	HeapArray<T, SizeType, Allocator>::operator std::span<T>() noexcept {
		return std::span<T>(mAlloc, mSize);
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	HeapArray<T, SizeType, Allocator>::operator std::span<const T>() const noexcept {
		return std::span<const T>(mAlloc, mSize);
	}


	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr void HeapArray<T, SizeType, Allocator>::allocate(size_type size) {
		mAlloc = mAllocator.allocate(size);
		mSize = size;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr void HeapArray<T, SizeType, Allocator>::deallocate() {
		mAllocator.deallocate(mAlloc, mSize);
		mAlloc = nullptr;
		mSize = 0;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>::HeapArray(Allocator alloc)
		: mAllocator(std::move(alloc))
	{}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>::HeapArray(size_type size, Allocator alloc)
		: mAllocator(std::move(alloc))
	{
		if (size > 0) {
			allocate(size);
			std::ranges::uninitialized_default_construct_n(mAlloc, mSize);
		}
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>::HeapArray(size_type size, T fill, Allocator alloc)
		: mAllocator(std::move(alloc))
	{
		if (size > 0) {
			allocate(size);
			std::ranges::uninitialized_fill_n(mAlloc, mSize, fill);
		}
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	template <typename Iterator>
	constexpr HeapArray<T, SizeType, Allocator>::HeapArray(Iterator begin, Iterator end, Allocator alloc)
		: mAllocator(std::move(alloc))
	{
		if (begin == end)
			return;

		const auto size = std::distance(begin, end);
		allocate(size);
		std::uninitialized_copy(begin, end, this->begin());
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>::HeapArray(const HeapArray& other) {
		*this = other;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>& HeapArray<T, SizeType, Allocator>::operator=(const HeapArray& other) {
		if (this != &other) {
			clear();
			allocate(other.size());
			std::uninitialized_copy(other.begin(), other.end(), begin());
		}
		return *this;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>::HeapArray(HeapArray&& other) noexcept
		: mAlloc(std::exchange(other.mAlloc, {}))
		, mSize(std::exchange(other.mSize, {}))
	{}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>& HeapArray<T, SizeType, Allocator>::operator=(HeapArray&& other) noexcept {
		if (this != &other) {
			clear();

			mAlloc = std::exchange(other.mAlloc, {});
			mSize = std::exchange(other.mSize, {});
		}

		return *this;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr HeapArray<T, SizeType, Allocator>::~HeapArray() noexcept {
		clear();
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr void HeapArray<T, SizeType, Allocator>::resize(const size_type newSize) {
		HeapArray tmp = std::move(*this);

		allocate(newSize);
		const auto [first, second] = std::uninitialized_move_n(tmp.begin(), std::min(tmp.size(), size()), begin());
		std::ranges::uninitialized_default_construct(second, end());
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr void HeapArray<T, SizeType, Allocator>::resize(size_type newSize, T fill) {
		HeapArray tmp = std::move(*this);

		allocate(newSize);
		const auto [first, second] = std::uninitialized_move_n(tmp.begin(), std::min(tmp.size(), size()), begin());
		std::ranges::uninitialized_fill(second, end(), fill);
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr void HeapArray<T, SizeType, Allocator>::clear() noexcept {
		if (mAlloc) {
			std::ranges::destroy_n(mAlloc, mSize);
			deallocate();
		}
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr T* HeapArray<T, SizeType, Allocator>::data() {
		return mAlloc;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr const T* HeapArray<T, SizeType, Allocator>::data() const {
		return mAlloc;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr SizeType HeapArray<T, SizeType, Allocator>::size() const {
		return mSize;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr bool HeapArray<T, SizeType, Allocator>::empty() const {
		return size() == 0;
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr auto* HeapArray<T, SizeType, Allocator>::begin(this auto&& self) {
		return self.data();
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr auto* HeapArray<T, SizeType, Allocator>::end(this auto&& self) {
		return self.begin() + self.size();
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr auto& HeapArray<T, SizeType, Allocator>::back(this auto&& self) {
		return *(self.end() - 1);
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr auto& HeapArray<T, SizeType, Allocator>::at(this auto&& self, size_type i) {
		if (i >= self.size()) {
	#if __cpp_exceptions
			throw std::out_of_range(std::format("HeapArray::at() out of range {}/{}!", i, self.size()));
	#else
			std::terminate();
	#endif
		}
		return self.data()[i];
	}

	template <typename T, std::unsigned_integral SizeType, typename Allocator>
	constexpr auto& HeapArray<T, SizeType, Allocator>::operator[](this auto&& self, size_type i) {
		return self.data()[i];
	}
}
