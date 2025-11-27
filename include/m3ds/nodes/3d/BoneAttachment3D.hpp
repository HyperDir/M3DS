#pragma once

#include <m3ds/nodes/3d/Node3D.hpp>
#include <m3ds/nodes/3d/MeshInstance.hpp>

namespace M3DS {
    class BoneAttachment3D : public Node3D {
        M_CLASS(BoneAttachment3D, Node3D)
    public:
        void setBoneIndex(std::uint32_t boneIndex) noexcept;
        [[nodiscard]] std::uint32_t getBoneIndex() const noexcept;
    protected:
        void update(Seconds<float> delta) override;

        void afterTreeEnter() override;
        void beforeTreeExit() override;
    private:
        MeshInstance* mMeshInstance {};
        std::uint32_t mBoneIndex {};
    };
}
