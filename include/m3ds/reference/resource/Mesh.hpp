#pragma once

#include <expected>
#include <flat_map>

#include <m3ds/reference/resource/ParticleMaterial2D.hpp>
#include <m3ds/containers/HeapArray.hpp>
#include <m3ds/nodes/AnimationPlayer.hpp>
#include <m3ds/utils/LinearAllocator.hpp>
#include <m3ds/types/Float16.hpp>

#include <m3ds/spatial/Vector3.hpp>
#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/spatial/Matrix4x4.hpp>
#include <m3ds/types/Error.hpp>
#include <m3ds/utils/BinaryFile.hpp>

namespace M3DS {
#ifdef __3DS__
	using Material = C3D_Material;
#elifdef M3DS_SFML
	struct Material {
		float ambient[3];
		float diffuse[3];
		float specular0[3];
		float specular1[3];
		float emission[3];
	};
#endif
    class Mesh {
    public:
    	struct Vertex {
    		Vector3 coords {};
    		Vector3 normal {};
    		Vector2 texCoords {};
    		std::array<std::uint8_t, 4> boneIds {};
    		std::array<float, 4> boneWeights {};
    	};

    	using Triangle = std::array<Vertex, 3>;

    	static constexpr std::uint16_t maxBonesPerSurface = 28;

    	struct Surface {
    		Material material {};
    		SharedRawImage* texture {};
    		std::span<const Triangle> triangles {};
    		std::array<std::uint16_t, maxBonesPerSurface> boneMappings {};
    	};

    	struct SkeletalAnimation {
    		struct BoneTrack {
    			template <typename T>
    			struct Track {
    				struct Frame {
    					float time {};
    					T value {};

    					constexpr auto operator<=>(const Frame& other) const noexcept {
    						return time <=> other.time;
    					}

    					constexpr auto operator<=>(const float t) const noexcept {
    						return time <=> t;
    					}
    				};

    				std::span<const Frame> frames {};
    			};
    			using TranslationTrack = Track<Vector3>;
    			using RotationTrack = Track<Quaternion>;
    			using ScaleTrack = Track<Vector3>;

    			std::uint16_t boneIndex {};

    			std::uint8_t translationInterpolationMethod {};
    			std::uint8_t rotationInterpolationMethod {};
    			std::uint8_t scaleInterpolationMethod {};

    			std::unique_ptr<float[]> trackData {};

    			TranslationTrack translationTrack {};
    			RotationTrack rotationTrack {};
    			ScaleTrack scaleTrack {};
    		};

    		std::filesystem::path path {};
    		std::string name {};
    		Seconds<float> duration {};
    		HeapArray<BoneTrack> boneTracks {};

    		static inline std::flat_map<std::filesystem::path, std::weak_ptr<SkeletalAnimation>> animations {};

    		SkeletalAnimation() = default;
    		SkeletalAnimation(const SkeletalAnimation&) = delete;
    		SkeletalAnimation(SkeletalAnimation&&) = default;
    		SkeletalAnimation& operator=(const SkeletalAnimation&) = delete;
    		SkeletalAnimation& operator=(SkeletalAnimation&&) = default;

    		~SkeletalAnimation() noexcept;

    		[[nodiscard]] static std::expected<std::shared_ptr<SkeletalAnimation>, Error> load(const std::filesystem::path& path);
    		[[nodiscard]] static std::expected<std::unique_ptr<SkeletalAnimation>, Error> load(BinaryInFileAccessor file);
    	};

    	struct Bone {
    		std::optional<std::uint16_t> parent {};
    		Vector3 translation {};
    		Quaternion rotation {};
    		Vector3 scale {};
    		Matrix4x4 inverseBindMatrix = Matrix4x4::identity();
    		std::string_view name {};
    	};
    private:
    	std::filesystem::path mPath {};
    public:
    	HeapArray<SharedRawImage> textures {};
    	HeapArray<Surface> surfaces {};
    private:
    	LinearHeapArray<std::byte> mSurfaceData {};

    	HeapArray<Bone, std::uint16_t> mBones {};
    	HeapArray<char, std::uint32_t> mBoneNameData {};
    	HeapArray<std::uint16_t, std::uint16_t> mBoneUpdateOrder {};
    	std::vector<std::unique_ptr<SkeletalAnimation>> mAnimations {};

    	Mesh() = default;
    	~Mesh() noexcept;
    public:
    	[[nodiscard]] static std::expected<std::shared_ptr<Mesh>, Error> load(std::filesystem::path path) noexcept;
    	[[nodiscard]] static std::expected<std::shared_ptr<Mesh>, Error> load(BinaryInFileAccessor file) noexcept;

        [[nodiscard]] SkeletalAnimation* getAnimation(std::string_view anim) noexcept;
        [[nodiscard]] const SkeletalAnimation* getAnimation(std::string_view anim) const noexcept;

        [[nodiscard]] SkeletalAnimation* getAnimation(std::size_t idx) noexcept;
        [[nodiscard]] const SkeletalAnimation* getAnimation(std::size_t idx) const noexcept;

        [[nodiscard]] std::span<const Surface> getSurfaces() const noexcept;
        [[nodiscard]] std::span<const SharedRawImage> getTextures() const noexcept;
        [[nodiscard]] std::span<const Bone> getBones() const noexcept;
        [[nodiscard]] std::span<const std::uint16_t> getBoneUpdateOrder() const noexcept;

        [[nodiscard]] std::size_t getAnimationCount() const;

        [[nodiscard]] const std::filesystem::path& getPath() const noexcept;
    };
}

