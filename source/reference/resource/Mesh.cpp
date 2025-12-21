#include <m3ds/reference/resource/Mesh.hpp>

namespace M3DS {
	static std::flat_map<std::filesystem::path, std::weak_ptr<Mesh>> meshes {};

	Mesh::SkeletalAnimation::~SkeletalAnimation() noexcept {
		if (!path.empty())
			animations.erase(path);
	}

	std::expected<std::shared_ptr<Mesh::SkeletalAnimation>, Error> Mesh::SkeletalAnimation::load(
		const std::filesystem::path& path
	) {
		if (const auto it = animations.find(path); it != animations.end())
			if (auto animation = it->second.lock())
				return animation;

	    BinaryInFile file { path };

		if (!file) {
		    Debug::err("Failed to open file with path {}", path);
		    return std::unexpected{ Error::file_open_fail };
		}

		return load(file.getAccessor()).transform([&](std::unique_ptr<SkeletalAnimation>&& anim) {
			std::shared_ptr shared = std::move(anim);
		    animations.emplace(path, shared);
			return shared;
		});
	}

	std::expected<std::unique_ptr<Mesh::SkeletalAnimation>, Error> Mesh::SkeletalAnimation::load(
		BinaryInFileAccessor file
	) {
		struct AnimationHeader {
			std::uint32_t nameLength {};
			std::uint32_t boneTrackCount {};
			Seconds<float> duration {};
		} animationHeader {};

		if (!file.read(animationHeader)) {
		    Debug::err("Failed to read animation header!");
		    return std::unexpected{ Error::file_read_fail };
		}

		if (animationHeader.duration <= 0.0f) {
		    Debug::err("Invalid animation duration ({})!", animationHeader.duration);
		    return std::unexpected{ Error::file_invalid_data };
		}

		if (animationHeader.nameLength > 256) {
		    Debug::err("Unreasonable animation name length: {}!", animationHeader.nameLength);
		    return std::unexpected{ Error::file_invalid_data };
		}

		auto animation = std::make_unique<SkeletalAnimation>();
		animation->name.resize(animationHeader.nameLength);
		if (!file.read(std::span{
			animation->name.data(),
			animationHeader.nameLength
		})) {
		    Debug::err("Failed to read animation name!");
		    return std::unexpected{ Error::file_read_fail };
		}

		animation->duration = animationHeader.duration;

		if (animationHeader.boneTrackCount > 1024) {
		    Debug::err("Unreasonable bone track count: {}!", animationHeader.boneTrackCount);
		    return std::unexpected{ Error::file_invalid_data };
		}

		animation->boneTracks.resize(animationHeader.boneTrackCount);
		for (auto& boneTrack : animation->boneTracks) {
			struct BoneTrackHeader {
				std::uint16_t boneIndex {};

				std::uint32_t translationTrackCount {};
				std::uint32_t rotationTrackCount {};
				std::uint32_t scaleTrackCount {};

				std::uint8_t translationMethod {};
				std::uint8_t rotationMethod {};
				std::uint8_t scaleMethod {};
			} boneTrackHeader {};
			if (!file.read(boneTrackHeader)) {
			    Debug::err("Failed to read bone track header!");
		        return std::unexpected{ Error::file_read_fail };
			}

			const std::uint32_t translationTrackSize = boneTrackHeader.translationTrackCount * (sizeof(BoneTrack::TranslationTrack::Frame) / sizeof(float));
			const std::uint32_t rotationTrackSize = boneTrackHeader.rotationTrackCount * (sizeof(BoneTrack::RotationTrack::Frame) / sizeof(float));
			const std::uint32_t scaleTrackSize = boneTrackHeader.scaleTrackCount * (sizeof(BoneTrack::ScaleTrack::Frame) / sizeof(float));

			boneTrack.boneIndex = boneTrackHeader.boneIndex;
			boneTrack.translationInterpolationMethod = boneTrackHeader.translationMethod;
			boneTrack.rotationInterpolationMethod = boneTrackHeader.rotationMethod;
			boneTrack.scaleInterpolationMethod = boneTrackHeader.scaleMethod;

			boneTrack.trackData = M3DS::make_unique_for_overwrite_nothrow<float[]>(translationTrackSize + rotationTrackSize + scaleTrackSize);
			if (!boneTrack.trackData) {
			    Debug::err("Failed to allocate bone track data!");
		        return std::unexpected{ Error::allocation_failed };
			}

			for (std::size_t i{}; i < translationTrackSize + rotationTrackSize + scaleTrackSize; ++i) {
				float16 f16;
				if (!file.read(f16)) {
				    Debug::err("Failed to read bone track frame data!");
		            return std::unexpected{ Error::file_read_fail };
				}
				boneTrack.trackData[i] = static_cast<float>(f16);
			}

			boneTrack.translationTrack.frames = {
				reinterpret_cast<BoneTrack::TranslationTrack::Frame*>(boneTrack.trackData.get()),
				boneTrackHeader.translationTrackCount
			};
			boneTrack.rotationTrack.frames = {
				reinterpret_cast<BoneTrack::RotationTrack::Frame*>(boneTrack.trackData.get() + translationTrackSize),
				boneTrackHeader.rotationTrackCount
			};
			boneTrack.scaleTrack.frames = {
				reinterpret_cast<BoneTrack::ScaleTrack::Frame*>(boneTrack.trackData.get() + translationTrackSize + rotationTrackSize),
				boneTrackHeader.scaleTrackCount
			};
		}
		return animation;
	}

	Mesh::~Mesh() noexcept {
		meshes.erase(mPath);
	}

	std::expected<std::shared_ptr<Mesh>, Error> Mesh::load(std::filesystem::path path) noexcept {
		if (const auto it = meshes.find(path); it != meshes.end())
			if (auto mesh = it->second.lock())
				return mesh;

		Debug::log<1>("Loading mesh {}", path);
		BinaryInFile file { path };
		if (!file) {
		    Debug::err("Failed to open file {}", path);
		    return std::unexpected{ Error::file_open_fail };
		}

		std::expected mesh = load(file.getAccessor());
		if (mesh) {
			mesh.value()->mPath = path;
			meshes[std::move(path)] = mesh.value();
		}
		return mesh;
	}

	std::expected<std::shared_ptr<Mesh>, Error> Mesh::load(BinaryInFileAccessor file) noexcept {
		struct MeshHeader {
			std::array<char, 4> magic {};
			std::uint32_t surfaceDataOffset {};
			std::uint32_t triangleDataOffset {};
			std::uint32_t t3xDataOffset {};
			std::uint32_t boneDataOffset {};
			std::uint32_t animationDataOffset {};
		} header {};

		if (!file.read(header)) {
		    Debug::err("Failed to read Mesh header!");
		    return std::unexpected{ Error::file_read_fail };
		}

		if (header.magic != std::array{ 'M', '3', 'D', 'S' }) {
		    Debug::err("Invalid M3DS magic of {}", std::string_view{header.magic});
		    return std::unexpected{ Error::file_invalid_data };
		}

		if (!header.surfaceDataOffset) {
		    Debug::err("Surface Data Offset must be greater than 0!");
		    return std::unexpected{ Error::file_invalid_data };
		}

		if (!header.triangleDataOffset) {
		    Debug::err("Triangle Data Offset must be greater than 0!");
		    return std::unexpected{ Error::file_invalid_data };
		}

		struct MakeSharedEnabler : Mesh {};
		std::shared_ptr<Mesh> mesh = std::make_shared<MakeSharedEnabler>();

		// T3X Data
		if (header.t3xDataOffset) {
			const auto t3xStart = std::chrono::high_resolution_clock::now();

			struct T3XDataHeader {
				std::array<char, 4> magic {};
				std::uint32_t t3xCount {};
			} t3xDataHeader {};

			if (!file.seek(static_cast<long>(header.t3xDataOffset))) {
			    Debug::err("Failed to seek to t3x data! at {}", header.t3xDataOffset);
			    return std::unexpected{ Error::file_seek_fail };
			}

			if (!file.read(t3xDataHeader)) {
			    Debug::err("Failed to read t3x data header!");
			    return std::unexpected{ Error::file_read_fail };
			}

			if (t3xDataHeader.magic != std::array{ 'T', '3', 'X', 's' }) {
			    Debug::err("Invalid T3Xs header!");
			    return std::unexpected{ Error::file_invalid_data };
			}

			if (t3xDataHeader.t3xCount > 16) {
			    Debug::err("Unreasonable texture count: {}!", t3xDataHeader.t3xCount);
			    return std::unexpected{ Error::file_invalid_data };
			}

			mesh->textures.resize(t3xDataHeader.t3xCount);

			for (auto& tex : mesh->textures) {
				std::uint32_t t3xSize {};

				if (!file.read(t3xSize)) {
				    Debug::err("Failed to read t3x size!");
				    return std::unexpected{ Error::file_read_fail };
				}

				const auto t3xData = M3DS::make_unique_for_overwrite_nothrow<std::byte[]>(t3xSize);
				if (!t3xData) {
				    Debug::err("Failed to allocate t3x data!");
				    return std::unexpected{ Error::file_read_fail };
				}

				if (!file.read(std::span{t3xData.get(), t3xSize})) {
				    Debug::err("Failed to read t3x data!");
				    return std::unexpected{ Error::file_read_fail };
				}

				auto opt = SharedRawImage::load(std::span{t3xData.get(), t3xSize});
				if (!opt)
					return std::unexpected{ opt.error() };

				tex = std::move(opt.value());
			}

			const auto t3xEnd = std::chrono::high_resolution_clock::now();
			Debug::log<1>( "Loaded {} Textures in {}ms", t3xDataHeader.t3xCount, std::chrono::duration_cast<std::chrono::milliseconds>(t3xEnd - t3xStart).count());
		}

		// Triangle Data
		{
			const auto triangleStart = std::chrono::high_resolution_clock::now();

			struct TriangleDataHeader {
				std::array<char, 4> magic {};
				std::uint32_t triangleCount {};
			} triangleDataHeader {};

			if (!file.seek(static_cast<long>(header.triangleDataOffset))) {
			    Debug::err("Failed to seek to triangle data! at {}", header.triangleDataOffset);
		        return std::unexpected{ Error::file_seek_fail };
			}

			if (!file.read(triangleDataHeader)) {
			    Debug::err("Failed to read triangle data header!");
			    return std::unexpected{ Error::file_read_fail };
			}

			if (triangleDataHeader.magic != std::array{ 'T', 'R', 'I', 'A' }) {
			    Debug::err("Invalid TRIA magic!");
			    return std::unexpected{ Error::file_read_fail };
			}

			if (triangleDataHeader.triangleCount > 30'000) {
			    Debug::err("Unreasonable triangle count: {}!", triangleDataHeader.triangleCount);
			    return std::unexpected{ Error::file_invalid_data };
			}

			mesh->mSurfaceData.resize(triangleDataHeader.triangleCount * sizeof(Triangle));
			if (!file.read(std::span{mesh->mSurfaceData})) {
			    Debug::err("Failed to read triangle data!");
			    return std::unexpected{ Error::file_read_fail };
			}

			const auto triangleEnd = std::chrono::high_resolution_clock::now();
			Debug::log<1>( "Loaded {} Triangles in {}ms", triangleDataHeader.triangleCount, std::chrono::duration_cast<std::chrono::milliseconds>(triangleEnd - triangleStart).count());
		}

		// Surface Data
		{
			const auto surfaceStart = std::chrono::high_resolution_clock::now();

			struct SurfaceDataHeader {
				std::array<char, 4> magic {};
				std::uint32_t surfaceCount {};
			} surfaceDataHeader {};

			if (!file.seek(static_cast<long>(header.surfaceDataOffset))) {
			    Debug::err("Failed to seek to surface data! at {}", header.surfaceDataOffset);
			    return std::unexpected{ Error::file_seek_fail };
			}

			if (!file.read(surfaceDataHeader)) {
			    Debug::err("Failed to read surface data header!");
			    return std::unexpected{ Error::file_read_fail };
			}

			if (surfaceDataHeader.magic != std::array{ 'S', 'U', 'R', 'F' }) {
			    Debug::err("Invalid SURF magic!");
			    return std::unexpected{ Error::file_read_fail };
			}

			if (surfaceDataHeader.surfaceCount > 128) {
			    Debug::err("Unreasonable surface count: {}!", surfaceDataHeader.surfaceCount);
			    return std::unexpected{ Error::file_invalid_data };
			}

			mesh->surfaces.resize(surfaceDataHeader.surfaceCount);
			for (size_t i{}; i<surfaceDataHeader.surfaceCount; ++i) {
				struct SurfaceHeader {
					Material material {};
					bool hasTexture {};
					std::uint16_t textureIndex {};
					std::uint32_t triangleStartIndex {};
					std::uint32_t triangleCount {};
					std::array<std::uint16_t, maxBonesPerSurface> boneMappings {};
				} surfaceHeader {};

				if (!file.read(surfaceHeader)) {
				    Debug::err("Failed to read surface data!");
				    return std::unexpected{ Error::file_read_fail };
				}

				if (surfaceHeader.hasTexture && surfaceHeader.textureIndex >= static_cast<int32_t>(mesh->textures.size())) {
				    Debug::err("Invalid texture index! {}/{}", surfaceHeader.textureIndex, mesh->textures.size());
				    return std::unexpected{ Error::file_invalid_data };
				}

				mesh->surfaces[i] = {
					.material = surfaceHeader.material,
					.texture = surfaceHeader.hasTexture ? &mesh->textures[surfaceHeader.textureIndex] : nullptr,
					.triangles = { reinterpret_cast<Triangle*>(mesh->mSurfaceData.data() + surfaceHeader.triangleStartIndex), surfaceHeader.triangleCount },
					.boneMappings = surfaceHeader.boneMappings
				};
			}

			const auto surfaceEnd = std::chrono::high_resolution_clock::now();
			Debug::log<1>( "Loaded {} Surfaces in {}ms", surfaceDataHeader.surfaceCount, std::chrono::duration_cast<std::chrono::milliseconds>(surfaceEnd - surfaceStart).count());
		}

		// Bone Data
		if (header.boneDataOffset) {
			const auto boneStart = std::chrono::high_resolution_clock::now();

			struct BoneDataHeader {
				std::array<char, 4> magic {};
				std::uint16_t boneCount {};
				std::uint32_t boneNameDataLength {};
			} boneDataHeader {};

			if (!file.seek(static_cast<long>(header.boneDataOffset))) {
				Debug::err("Failed to seek to bone data!");
		        return std::unexpected{ Error::file_seek_fail };
            }

			if (!file.read(boneDataHeader)) {
				Debug::err("Failed to read bone data header!");
		        return std::unexpected{ Error::file_read_fail };
            }

			if (boneDataHeader.magic != std::array{ 'B', 'O', 'N', 'E' }) {
				Debug::err("Invalid BONE magic!");
		        return std::unexpected{ Error::file_invalid_data };
            }

			if (boneDataHeader.boneNameDataLength > 10'000) {
				Debug::err("Unreasonable bone name data length: {}!", boneDataHeader.boneNameDataLength);
		        return std::unexpected{ Error::file_invalid_data };
            }

			if (boneDataHeader.boneCount > 512) {
				Debug::err("Unreasonable bone count: {}!", boneDataHeader.boneCount);
		        return std::unexpected{ Error::file_invalid_data };
            }

			mesh->mBoneNameData.resize(boneDataHeader.boneNameDataLength);
			if (!file.read(std::span{ mesh->mBoneNameData })) {
				Debug::err("Failed to read bone name data!");
		        return std::unexpected{ Error::file_read_fail };
			}

			mesh->mBones.resize(boneDataHeader.boneCount);
			for (auto& bone : mesh->mBones) {
				std::int32_t parentIndex {};
				if (!file.read(parentIndex)) {
				    Debug::err("Failed to read bone parent index!");
				    return std::unexpected{ Error::file_read_fail };
				}

				if (parentIndex >= 0)
					bone.parent = static_cast<std::uint16_t>(parentIndex);

				if (
					!file.read(bone.translation) ||
					!file.read(bone.rotation) ||
					!file.read(bone.scale) ||
					!file.read(bone.inverseBindMatrix)
				) {
				    Debug::err("Failed to read bone data!");
				    return std::unexpected{ Error::file_read_fail };
				}

				struct BoneName {
					std::uint32_t offset {};
					std::uint32_t length {};
				} boneName {};

				if (!file.read(boneName)) {
					Debug::err("Failed to read bone name!");
		            return std::unexpected{ Error::file_read_fail };
                }

				if (boneName.offset >= mesh->mBoneNameData.size()) {
					Debug::err("Invalid bone name offset {}!", boneName.offset);
		            return std::unexpected{ Error::file_invalid_data };
                }

				if (boneName.offset + boneName.length > mesh->mBoneNameData.size()) {
					Debug::err("Invalid bone name length {} at offset {}!", boneName.length, boneName.offset);
		            return std::unexpected{ Error::file_invalid_data };
                }

				bone.name = { mesh->mBoneNameData.data() + boneName.offset, boneName.length };
			}

			mesh->mBoneUpdateOrder.resize(boneDataHeader.boneCount);
			if (!file.read(std::span{
				reinterpret_cast<char*>(mesh->mBoneUpdateOrder.data()),
				mesh->mBoneUpdateOrder.size() * sizeof(mesh->mBoneUpdateOrder[0])
			})) {
			    Debug::err("Failed to read bone data!");
		        return std::unexpected{ Error::file_read_fail };
			}

			const auto boneEnd = std::chrono::high_resolution_clock::now();
			Debug::log<1>( "Loaded {} Bones in {}ms", boneDataHeader.boneCount, std::chrono::duration_cast<std::chrono::milliseconds>(boneEnd - boneStart).count());
		}

		// Animation
		if (header.animationDataOffset) {
			const auto animationStart = std::chrono::high_resolution_clock::now();
			struct AnimationDataHeader {
				std::array<char, 4> magic {};
				std::uint32_t animationCount {};
			} animationDataHeader {};

			if (!file.seek(static_cast<long>(header.animationDataOffset))) {
				Debug::err("Failed to seek to animation data at {}!", header.animationDataOffset);
		        return std::unexpected{ Error::file_seek_fail };
            }

			if (!file.read(animationDataHeader)) {
				Debug::err("Failed to read animation data header!");
		            return std::unexpected{ Error::file_read_fail };
            }

			if (animationDataHeader.magic != std::array{ 'A', 'N', 'I', 'M' }) {
				Debug::err("Invalid ANIM magic!");
		            return std::unexpected{ Error::file_invalid_data };
            }

			for (std::uint32_t i{}; i < animationDataHeader.animationCount; ++i) {
				auto anim = SkeletalAnimation::load(file);
				if (!anim)
					return std::unexpected{ anim.error() };

				mesh->mAnimations.emplace_back(std::move(*anim));
			}

			const auto animationEnd = std::chrono::high_resolution_clock::now();
			Debug::log<1>( "Loaded {} Animations in {}ms", animationDataHeader.animationCount, std::chrono::duration_cast<std::chrono::milliseconds>(animationEnd - animationStart).count());
		}

		return mesh;
	}
	
	Mesh::SkeletalAnimation* Mesh::getAnimation(const std::string_view anim) noexcept {
		const auto it = std::ranges::find_if(mAnimations, [&](const std::unique_ptr<SkeletalAnimation>& a) { return a->name == anim; });
		if (it == mAnimations.end())
			return {};
		return it->get();
	}

	const Mesh::SkeletalAnimation* Mesh::getAnimation(const std::string_view anim) const noexcept {
		const auto it = std::ranges::find_if(mAnimations, [&](const std::unique_ptr<SkeletalAnimation>& a) { return a->name == anim; });
		if (it == mAnimations.end())
			return {};
		return it->get();
	}

    Mesh::SkeletalAnimation* Mesh::getAnimation(const std::size_t idx) noexcept {
	    return idx < mAnimations.size() ? mAnimations[idx].get() : nullptr;
    }

    const Mesh::SkeletalAnimation* Mesh::getAnimation(std::size_t idx) const noexcept {
	    return idx < mAnimations.size() ? mAnimations[idx].get() : nullptr;
    }

    std::span<const Mesh::Surface> Mesh::getSurfaces() const noexcept {
		return surfaces;
	}

	std::span<const SharedRawImage> Mesh::getTextures() const noexcept {
		return textures;
	}

	std::span<const Mesh::Bone> Mesh::getBones() const noexcept {
		return mBones;
	}

	std::span<const std::uint16_t> Mesh::getBoneUpdateOrder() const noexcept {
		return mBoneUpdateOrder;
	}

    std::size_t Mesh::getAnimationCount() const {
	    return mAnimations.size();
    }

    const std::filesystem::path& Mesh::getPath() const noexcept {
	    return mPath;
    }

    // REGISTER_NO_METHODS(Mesh);
    // REGISTER_NO_MEMBERS(Mesh);
}
