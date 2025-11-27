#pragma once

#ifdef __3DS__
#include <concepts>

#include <3ds/allocator/linear.h>
#include <m3ds/utils/Debug.hpp>
#include <m3ds/containers/HeapArray.hpp>

namespace M3DS {
	template <typename T>
	struct LinearAllocator {
		static constexpr bool logAllocations = false;

		using allocator_type = LinearAllocator;
		using value_type = T;

		static inline std::size_t allocation {};

		static T* allocate(const std::size_t n) {
			const auto bytes = n * sizeof(T);
			T* ptr = static_cast<T*>(linearAlloc(bytes));
			if constexpr (logAllocations) {
				// ReSharper disable once CppDFAUnreachableCode
				if (ptr) {
					allocation += bytes;
					Debug::log( "LinAlloc {}B ({}B used)", bytes, allocation);
				}
			}
			if (!ptr) {
	#if __cpp_exceptions
				throw std::bad_alloc();
	#else
				Debug::terminate("LinearAllocator failed to allocate {} bytes!", bytes);
	#endif
			}
			return ptr;
		}

		static void deallocate(T* const ptr, const std::size_t n) noexcept {
			if constexpr (logAllocations) {
				// ReSharper disable once CppDFAUnreachableCode
				if (ptr) {
					const auto bytes = n * sizeof(T);
					allocation -= bytes;
					Debug::log( "LinDealloc {}B ({}B used)", bytes, allocation);
				}
			}
			linearFree(ptr);
		}
	};

	template <typename T, std::unsigned_integral S = std::size_t>
	using LinearHeapArray = HeapArray<T, S, LinearAllocator<T>>;
}
#endif
