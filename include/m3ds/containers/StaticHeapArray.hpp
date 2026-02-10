#pragma once

#include <memory>
#include <utility>

namespace M3DS {
	template <typename T, std::size_t N, typename Allocator = std::allocator<T>>
	class StaticHeapArray {
	public:
		using value_type = T;
		using allocator_type = Allocator;

		constexpr StaticHeapArray();;
		explicit constexpr StaticHeapArray(Allocator alloc);
		explicit constexpr StaticHeapArray(T fill, Allocator alloc = {});

		template <typename Iterator>
		constexpr StaticHeapArray(Iterator begin, Iterator end, Allocator alloc = {});

		constexpr StaticHeapArray(const StaticHeapArray& other);
		constexpr StaticHeapArray& operator=(const StaticHeapArray& other);

		constexpr StaticHeapArray(StaticHeapArray&& other) = delete("Cannot be noexcept");
	    constexpr StaticHeapArray& operator=(StaticHeapArray&& other) noexcept;

		constexpr ~StaticHeapArray() noexcept;

		[[nodiscard]] constexpr value_type* data();
		[[nodiscard]] constexpr const value_type* data() const;

		[[nodiscard]] constexpr std::size_t size() const;
		[[nodiscard]] constexpr bool empty() const;

		[[nodiscard]] constexpr auto* begin(this auto&& self);
		[[nodiscard]] constexpr auto* end(this auto&& self);

		[[nodiscard]] constexpr auto& back(this auto&& self);

		[[nodiscard]] constexpr auto& at(this auto&& self, std::size_t i);
		[[nodiscard]] constexpr auto& operator[](this auto&& self, std::size_t i);

		explicit operator std::span<T>() noexcept;
		explicit operator std::span<const T>() const noexcept;
	private:
		[[no_unique_address]] allocator_type mAllocator {};
		value_type* mAlloc {};
	};

	template <typename T, std::size_t N, typename Allocator>
	StaticHeapArray<T, N, Allocator>::operator std::span<T>() noexcept {
		return std::span<T>(mAlloc, N);
	}

	template <typename T, std::size_t N, typename Allocator>
	StaticHeapArray<T, N, Allocator>::operator std::span<const T>() const noexcept {
		return std::span<const T>(mAlloc, N);
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr StaticHeapArray<T, N, Allocator>::StaticHeapArray() {
		static_assert(N > 0);

		mAlloc = mAllocator.allocate(N);
		std::ranges::uninitialized_default_construct_n(mAlloc, N);
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr StaticHeapArray<T, N, Allocator>::StaticHeapArray(Allocator alloc)
		: mAllocator(std::move(alloc))
	{
		static_assert(N > 0);

		mAlloc = mAllocator.allocate(N);
		std::ranges::uninitialized_default_construct_n(mAlloc, N);
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr StaticHeapArray<T, N, Allocator>::StaticHeapArray(T fill, Allocator alloc)
		: mAllocator(std::move(alloc))
	{
		static_assert(N > 0);

		mAlloc = mAllocator.allocate(N);
		std::ranges::uninitialized_fill_n(mAlloc, N, fill);
	}

	template <typename T, std::size_t N, typename Allocator>
	template <typename Iterator>
	constexpr StaticHeapArray<T, N, Allocator>::StaticHeapArray(Iterator begin, Iterator end, Allocator alloc)
		: mAllocator(std::move(alloc))
	{
		if (begin == end)
			return;

		const auto size = std::distance(begin, end);
		allocate(size);
		std::uninitialized_copy(begin, end, this->begin());
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr StaticHeapArray<T, N, Allocator>::StaticHeapArray(const StaticHeapArray& other) {
		*this = other;
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr StaticHeapArray<T, N, Allocator>& StaticHeapArray<T, N, Allocator>::operator=(const StaticHeapArray& other) {
		if (this != &other) {
			allocate(other.size());
			std::uninitialized_copy(other.begin(), other.end(), begin());
		}
		return *this;
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr StaticHeapArray<T, N, Allocator>& StaticHeapArray<T, N, Allocator>::operator=(StaticHeapArray&& other) noexcept {
		if (this != &other) {
			std::swap(mAllocator, other.mAllocator);
			std::swap(mAlloc, other.mAlloc);
		}

		return *this;
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr StaticHeapArray<T, N, Allocator>::~StaticHeapArray() noexcept {
		std::ranges::destroy_n(mAlloc, N);
		mAllocator.deallocate(mAlloc, N);
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr T* StaticHeapArray<T, N, Allocator>::data() {
		return mAlloc;
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr const T* StaticHeapArray<T, N, Allocator>::data() const {
		return mAlloc;
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr std::size_t StaticHeapArray<T, N, Allocator>::size() const {
		return N;
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr bool StaticHeapArray<T, N, Allocator>::empty() const {
		return size() == 0;
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr auto* StaticHeapArray<T, N, Allocator>::begin(this auto&& self) {
		return self.data();
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr auto* StaticHeapArray<T, N, Allocator>::end(this auto&& self) {
		return self.begin() + self.size();
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr auto& StaticHeapArray<T, N, Allocator>::back(this auto&& self) {
		return *(self.end() - 1);
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr auto& StaticHeapArray<T, N, Allocator>::at(this auto&& self, std::size_t i) {
		if (i >= self.size()) {
	#if __cpp_exceptions
			throw std::out_of_range(std::format("StaticHeapArray::at() out of range {}/{}!", i, self.size()));
	#else
			std::terminate();
	#endif
		}
		return self.data()[i];
	}

	template <typename T, std::size_t N, typename Allocator>
	constexpr auto& StaticHeapArray<T, N, Allocator>::operator[](this auto&& self, std::size_t i) {
		return self.data()[i];
	}
}
