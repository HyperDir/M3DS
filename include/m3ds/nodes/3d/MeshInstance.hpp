#pragma once

#include <m3ds/nodes/3d/Node3D.hpp>
#include <m3ds/render/Mesh.hpp>

namespace M3DS {
    class MeshInstance : public Node3D {
        M_CLASS(MeshInstance, Node3D)
    public:
        struct BoneInstance {
            Matrix4x4 transform = Matrix4x4::identity();
        };

        explicit MeshInstance() noexcept = default;
        explicit MeshInstance(std::shared_ptr<const Mesh> mesh) noexcept;

        void setMesh(std::shared_ptr<const Mesh> mesh) noexcept;

        void resetPose() noexcept;

        const std::shared_ptr<const Mesh>& getMesh() const noexcept;
        std::span<const BoneInstance> getBones() const noexcept;

        void playAnimation(std::string_view animation) noexcept;
        void playAnimationIdx(std::size_t idx) noexcept;
        void playAnimationPtr(const Mesh::SkeletalAnimation* animation) noexcept;

        void setPrimaryAnimation(std::string_view to) noexcept;
        void setPrimaryAnimationIdx(std::size_t idx) noexcept;
        void setPrimaryAnimationPtr(const Mesh::SkeletalAnimation* to) noexcept;

        void setSecondaryAnimation(std::string_view to) noexcept;
        void setSecondaryAnimationIdx(std::size_t idx) noexcept;
        void setSecondaryAnimationPtr(const Mesh::SkeletalAnimation* to) noexcept;

        void setAnimationBlend(float to) noexcept;

        void setAnimationState(
            std::string_view primary,
            std::string_view secondary,
            float blend,
            bool paused
        );
        void setAnimationStateIdx(
            std::size_t primaryIdx,
            std::size_t secondaryIdx,
            float blend,
            bool paused
        );
        void setAnimationStatePtr(
            const Mesh::SkeletalAnimation* primary,
            const Mesh::SkeletalAnimation* secondary,
            float blend,
            bool paused
        );

        void pauseAnimation() noexcept;
        void resumeAnimation() noexcept;

        [[nodiscard]] const Mesh::SkeletalAnimation* getAnimation(std::string_view name) const noexcept;
        [[nodiscard]] const Mesh::SkeletalAnimation* getAnimation(std::size_t idx) const noexcept;

        [[nodiscard]] std::size_t getAnimationCount() const noexcept;
    protected:
        void draw(RenderTarget3D& target) override;
        void update(Seconds<float> delta) override;
    private:
        std::shared_ptr<const Mesh> mMesh {};
        HeapArray<BoneInstance, std::uint16_t> mBones {};

        struct AnimationState {
            const Mesh::SkeletalAnimation* animation {};
            float progress {};
        };

        struct AnimationData {
            AnimationState primaryState {};
            AnimationState secondaryState {};

            float blend {};
            bool paused {};
        } mAnimationData {};

        void updateProgress(Seconds<float> delta) noexcept;
        template <bool blend>
        void updateBone(std::uint16_t bone) noexcept;
    };
}
