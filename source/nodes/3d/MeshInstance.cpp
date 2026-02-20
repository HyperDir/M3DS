#include <m3ds/nodes/3d/MeshInstance.hpp>

namespace M3DS {
    MeshInstance::MeshInstance(std::shared_ptr<const Mesh> mesh) noexcept {
        setMesh(std::move(mesh));
    }

    void MeshInstance::setMesh(std::shared_ptr<const Mesh> mesh) noexcept {
        mMesh = std::move(mesh);

        if (mMesh) {
            mBones.resize(static_cast<std::uint16_t>(mMesh->getBones().size()));
        } else {
            mBones.clear();
        }

        resetPose();
    }

    void MeshInstance::resetPose() noexcept {
        if (mBones.empty() || !mMesh) return;

        const auto bones = mMesh->getBones();

        for (const std::uint16_t& bone : mMesh->getBoneUpdateOrder()) {
            if (bones[bone].parent) {
                mBones[bone].transform = mBones[bones[bone].parent.value()].transform * Matrix4x4::fromTRS(
                    bones[bone].translation,
                    bones[bone].rotation,
                    bones[bone].scale
                );
            } else {
                mBones[bone].transform = Matrix4x4::fromTRS(
                    bones[bone].translation,
                    bones[bone].rotation,
                    bones[bone].scale
                );
            }
        }
    }

    const std::shared_ptr<const Mesh>& MeshInstance::getMesh() const noexcept { return mMesh; }

    std::span<const MeshInstance::BoneInstance> MeshInstance::getBones() const noexcept {
        return mBones;
    }

    void MeshInstance::draw(RenderTarget3D& target) {
        Node3D::draw(target);

        target.render(*this);
    }

    void MeshInstance::playAnimation(const std::string_view animation) noexcept {
        playAnimationPtr(getAnimation(animation));
    }

    void MeshInstance::playAnimationIdx(const std::size_t idx) noexcept {
        playAnimationPtr(getAnimation(idx));
    }

    void MeshInstance::playAnimationPtr(const Mesh::SkeletalAnimation* animation) noexcept {
        mAnimationData.primaryState = { animation };
    }

    void MeshInstance::setPrimaryAnimation(const std::string_view to) noexcept {
        setPrimaryAnimationPtr(getAnimation(to));
    }

    void MeshInstance::setPrimaryAnimationIdx(const std::size_t idx) noexcept {
        setPrimaryAnimationPtr(getAnimation(idx));
    }

    void MeshInstance::setPrimaryAnimationPtr(const Mesh::SkeletalAnimation* to) noexcept {
        if (to != mAnimationData.primaryState.animation) {
            if (to == mAnimationData.secondaryState.animation)
                mAnimationData.primaryState.progress = mAnimationData.secondaryState.progress;
            mAnimationData.primaryState.animation = std::move(to);
        }
    }

    void MeshInstance::setSecondaryAnimation(const std::string_view to) noexcept {
        setSecondaryAnimationPtr(getAnimation(to));
    }

    void MeshInstance::setSecondaryAnimationIdx(const std::size_t idx) noexcept {
        setSecondaryAnimationPtr(getAnimation(idx));
    }

    void MeshInstance::setSecondaryAnimationPtr(const Mesh::SkeletalAnimation* to) noexcept {
        if (to != mAnimationData.secondaryState.animation) {
            if (to == mAnimationData.primaryState.animation)
                mAnimationData.secondaryState.progress = mAnimationData.primaryState.progress;
            mAnimationData.secondaryState.animation = std::move(to);
        }
    }

    void MeshInstance::setAnimationBlend(const float to) noexcept {
        mAnimationData.blend = to;
    }

    void MeshInstance::setAnimationState(
        const std::string_view primary,
        const std::string_view secondary,
        const float blend,
        const bool paused
    ) {
        setAnimationStatePtr(
            getAnimation(primary),
            getAnimation(secondary),
            blend,
            paused
        );
    }

    void MeshInstance::setAnimationStateIdx(
        const std::size_t primaryIdx,
        const std::size_t secondaryIdx,
        const float blend,
        const bool paused
    ) {
        setAnimationStatePtr(
            getAnimation(primaryIdx),
            getAnimation(secondaryIdx),
            blend,
            paused
        );
    }

    void MeshInstance::setAnimationStatePtr(
        const Mesh::SkeletalAnimation* primary,
        const Mesh::SkeletalAnimation* secondary,
        const float blend,
        const bool paused
    ) {
        mAnimationData = {
            { std::move(primary) },
            { std::move(secondary) },
            blend,
            paused
        };
    }

    void MeshInstance::pauseAnimation() noexcept {
        mAnimationData.paused = true;
    }

    void MeshInstance::resumeAnimation() noexcept {
        mAnimationData.paused = false;
    }

    const Mesh::SkeletalAnimation* MeshInstance::getAnimation(std::string_view name) const noexcept {
        if (mMesh)
            return mMesh->getAnimation(name);
        Debug::err("Attempted to get animation '{}' on empty MeshInstance!", name);
        return {};
    }

    const Mesh::SkeletalAnimation* MeshInstance::getAnimation(std::size_t idx) const noexcept {
        if (mMesh)
            return mMesh->getAnimation(idx);
        Debug::err("Attempted to get animation at index '{}' on empty MeshInstance!", idx);
        return {};
    }

    std::size_t MeshInstance::getAnimationCount() const noexcept {
        return mMesh ? mMesh->getAnimationCount() : 0;
    }

    Failure MeshInstance::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        const bool hasMesh = static_cast<bool>(mMesh);
        if (!serialiser.write(hasMesh))
            return Failure{ ErrorCode::file_write_fail };
        if (hasMesh) {
            if (const Failure failure = M3DS::serialise(mMesh->getPath(), serialiser))
                return failure;

            // TODO: Serialise current animation state?
        }

        return Success;
    }

    Failure MeshInstance::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        bool hasMesh;
        if (!deserialiser.read(hasMesh))
            return Failure{ ErrorCode::file_read_fail };

        if (hasMesh) {
            std::filesystem::path path;
            if (const Failure failure = M3DS::deserialise(path, deserialiser))
                return failure;

            if (std::expected exp = Mesh::load(path))
                mMesh = std::move(exp.value());
            else
                return exp.error();

            // TODO: Deserialise current animation state?
        }

        return Success;
    }

    void MeshInstance::updateProgress(Seconds<float> delta) noexcept {
        if (mBones.empty())
            return;
        if (!mAnimationData.secondaryState.animation)
            mAnimationData.blend = 0;

        const bool interpolating = mAnimationData.blend > 0;

        if (interpolating) {
            const bool matchAnimationSpeed = std::abs(mAnimationData.primaryState.animation->duration - mAnimationData.secondaryState.animation->duration) < 1;
            if (matchAnimationSpeed) {
                const float blendedLength = lerp(mAnimationData.primaryState.animation->duration, mAnimationData.secondaryState.animation->duration, mAnimationData.blend);

                mAnimationData.primaryState.progress += delta / blendedLength;
                mAnimationData.secondaryState.progress = mAnimationData.primaryState.progress;
            } else {
                mAnimationData.primaryState.progress += delta / mAnimationData.primaryState.animation->duration;
                mAnimationData.secondaryState.progress += delta / mAnimationData.secondaryState.animation->duration;
            }
        } else {
            mAnimationData.primaryState.progress += delta / mAnimationData.primaryState.animation->duration;
        }

        if (mAnimationData.primaryState.progress > 1) {
            mAnimationData.primaryState.progress -= 1;
            // if (mAnimationData.primaryState.animation.oneShot) {
            //     mAnimationData.primaryState.progress = 1;
            // } else {
            //     mAnimationData.primaryState.progress -= 1;
            // }
        }

        if (interpolating && mAnimationData.secondaryState.progress > 1) {
            mAnimationData.secondaryState.progress -= 1;
            // if (mAnimationData.secondaryState.animation.oneShot) {
            //     mAnimationData.secondaryState.progress = 1;
            // } else {
            //     mAnimationData.secondaryState.progress -= 1;
            // }
        }
    }



    void MeshInstance::update(const Seconds<float> delta) {
        Node3D::update(delta);
        if (!mMesh)
            return;

        updateProgress(delta);

        if (mAnimationData.blend > 0) {
            for (const std::uint16_t& i : mMesh->getBoneUpdateOrder())
                updateBone<true>(i);
        } else {
            for (const std::uint16_t& i : mMesh->getBoneUpdateOrder())
                updateBone<false>(i);
        }
    }

	template <bool blend>
	void MeshInstance::updateBone(const std::uint16_t bone) noexcept {
		const auto getPrimaryAnimationFrame = []<typename T>(const Mesh::SkeletalAnimation::BoneTrack::Track<T>& track, const std::uint8_t interpolationMethod, const float time) -> std::optional<T> {
			if (track.frames.empty())
				return {};
			const auto next = std::lower_bound(
			    track.frames.begin(),
			    track.frames.end(),
			    time,
			    [](const typename Mesh::SkeletalAnimation::BoneTrack::Track<T>::Frame& frame, const float t) {
			        return frame.time < t;
			    }
			);

			if (next == track.frames.begin())
				return { next->value };

			const auto prev = std::prev(next);
			if (next == track.frames.end())
				return { prev->value };

			if (interpolationMethod == 0) { // Linear
				const float t = (time - prev->time) / (next->time - prev->time);
				if constexpr (std::is_same_v<T, Quaternion>) {
					return { slerp(prev->value, next->value, t) };
				} else {
					return { lerp(prev->value, next->value, t) };
				}
			}

			if (interpolationMethod == 1) // Step
				return { prev->value };

			Debug::err("Cubic Spline interpolation method not supported for MeshInstance animation!");
			return { prev->value };
		};

        const std::span<const Mesh::Bone> meshBones = mMesh->getBones();

		if constexpr (blend) {
			const auto& fromBoneTrack = mAnimationData.primaryState.animation->boneTracks[bone];
			const auto& toBoneTrack = mAnimationData.secondaryState.animation->boneTracks[bone];

			const float fromTime = mAnimationData.primaryState.progress * mAnimationData.primaryState.animation->duration;
			const float toTime = mAnimationData.secondaryState.progress * mAnimationData.secondaryState.animation->duration;

			const auto fromTranslation = getPrimaryAnimationFrame(fromBoneTrack.translationTrack, fromBoneTrack.translationInterpolationMethod, fromTime).value_or(meshBones[bone].translation);
			const auto fromRotation = getPrimaryAnimationFrame(fromBoneTrack.rotationTrack, fromBoneTrack.rotationInterpolationMethod, fromTime).value_or(meshBones[bone].rotation);
			const auto fromScale = getPrimaryAnimationFrame(fromBoneTrack.scaleTrack, fromBoneTrack.scaleInterpolationMethod, fromTime).value_or(meshBones[bone].scale);

			const auto toTranslation = getPrimaryAnimationFrame(toBoneTrack.translationTrack, toBoneTrack.translationInterpolationMethod, toTime).value_or(meshBones[bone].translation);
			const auto toRotation = getPrimaryAnimationFrame(toBoneTrack.rotationTrack, toBoneTrack.rotationInterpolationMethod, toTime).value_or(meshBones[bone].rotation);
			const auto toScale = getPrimaryAnimationFrame(toBoneTrack.scaleTrack, toBoneTrack.scaleInterpolationMethod, toTime).value_or(meshBones[bone].scale);

			if (meshBones[bone].parent) {
				const Matrix4x4& parentMatrix = mBones[*meshBones[bone].parent].transform;
				mBones[bone].transform = parentMatrix * Matrix4x4::fromTRS(
					lerp(fromTranslation, toTranslation, mAnimationData.blend),
					slerp(fromRotation, toRotation, mAnimationData.blend),
					lerp(fromScale, toScale, mAnimationData.blend)
				);
			} else {
				mBones[bone].transform = Matrix4x4::fromTRS(
					lerp(fromTranslation, toTranslation, mAnimationData.blend),
					slerp(fromRotation, toRotation, mAnimationData.blend),
					lerp(fromScale, toScale, mAnimationData.blend)
				);
			}
		} else {
			const auto& boneTrack = mAnimationData.primaryState.animation->boneTracks[bone];

			const float time = mAnimationData.primaryState.progress * mAnimationData.primaryState.animation->duration;

			const auto translation = getPrimaryAnimationFrame(boneTrack.translationTrack, boneTrack.translationInterpolationMethod, time).value_or(meshBones[bone].translation);
			const auto rotation = getPrimaryAnimationFrame(boneTrack.rotationTrack, boneTrack.rotationInterpolationMethod, time).value_or(meshBones[bone].rotation);
			const auto scale = getPrimaryAnimationFrame(boneTrack.scaleTrack, boneTrack.scaleInterpolationMethod, time).value_or(meshBones[bone].scale);

			if (meshBones[bone].parent) {
				const Matrix4x4& parentMatrix = mBones[*meshBones[bone].parent].transform;
				mBones[bone].transform = parentMatrix * Matrix4x4::fromTRS(translation, rotation, scale);
			} else {
				mBones[bone].transform = Matrix4x4::fromTRS(translation, rotation, scale);
			}
		}
    }

    REGISTER_METHODS(
        MeshInstance,
        MUTABLE_METHOD(resetPose),
        MUTABLE_METHOD(playAnimation),
        MUTABLE_METHOD(playAnimationIdx),
        MUTABLE_METHOD(setPrimaryAnimation),
        MUTABLE_METHOD(setPrimaryAnimationIdx),
        MUTABLE_METHOD(setSecondaryAnimation),
        MUTABLE_METHOD(setSecondaryAnimationIdx),
        MUTABLE_METHOD(setAnimationBlend),
        MUTABLE_METHOD(setAnimationState),
        MUTABLE_METHOD(setAnimationStateIdx),
        MUTABLE_METHOD(pauseAnimation),
        MUTABLE_METHOD(resumeAnimation),
        CONST_METHOD(getAnimationCount)
    );

    REGISTER_NO_MEMBERS(MeshInstance);
}
