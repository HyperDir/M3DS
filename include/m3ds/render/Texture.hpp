#pragma once

#include <expected>
#include <filesystem>
#include <memory>

#include <citro3d.h>

#include <m3ds/types/Error.hpp>
#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/utils/Units.hpp>

#include <m3ds/utils/BinaryFile.hpp>

namespace M3DS {
	struct UVs {
		float left {};
		float right {};
		float top {};
		float bottom {};

		[[nodiscard]] constexpr bool operator==(const UVs& other) const noexcept = default;
	};

	using PathView = std::basic_string_view<std::filesystem::path::value_type>;

	struct TextureData;

	class Texture {
	public:
		[[nodiscard]] Texture() noexcept = default;

		[[nodiscard]] static std::expected<Texture, Failure> load(std::filesystem::path path, bool vram = false) noexcept;
		[[nodiscard]] static std::expected<Texture, Failure> load(std::span<const unsigned char> data, bool vram = false) noexcept;

		[[nodiscard]] bool operator==(const Texture& other) const noexcept = default;

		[[nodiscard]] PathView getPath() const noexcept;

		[[nodiscard]] explicit operator bool() const noexcept;

		[[nodiscard]] C3D_Tex* getNative() const noexcept;
		[[nodiscard]] const UVs& getUvs() const noexcept;
		[[nodiscard]] const Pixels<Vector2>& getSize() const noexcept;

		[[nodiscard]] Failure serialise(BinaryOutFileAccessor file) const noexcept;
		[[nodiscard]] Failure deserialise(BinaryInFileAccessor file) noexcept;
	private:
		[[nodiscard]] explicit Texture(std::shared_ptr<TextureData> data) noexcept;

		std::shared_ptr<TextureData> mData {};
		Pixels<Vector2> mSize {};
		UVs mUvs {};
	};
}
