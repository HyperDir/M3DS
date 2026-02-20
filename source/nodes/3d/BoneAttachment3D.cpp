#include <m3ds/nodes/3d/BoneAttachment3D.hpp>

namespace M3DS {
    void BoneAttachment3D::setBoneIndex(const std::uint32_t boneIndex) noexcept {
        mBoneIndex = boneIndex;
    }

    std::uint32_t BoneAttachment3D::getBoneIndex() const noexcept {
        return mBoneIndex;
    }

    void BoneAttachment3D::update(const Seconds<float> delta) {
        Node3D::update(delta);

        if (!mMeshInstance) {
            Debug::warn("BoneAttachment3D: no parent MeshInstance found!");
            return;
        }

        const std::span<const MeshInstance::BoneInstance> boneInstances = mMeshInstance->getBones();
        if (mBoneIndex >= boneInstances.size()) {
            Debug::err("BoneAttachment3D: bone index out of range! {}/{}!", mBoneIndex, boneInstances.size());
            return;
        }

        setTransform(boneInstances[mBoneIndex].transform);
    }

    void BoneAttachment3D::afterTreeEnter() {
        mMeshInstance = object_cast<MeshInstance*>(getParent());
    }

    void BoneAttachment3D::beforeTreeExit() {
        Node3D::beforeTreeExit();
        mMeshInstance = {};
    }

    Failure BoneAttachment3D::serialise(Serialiser& serialiser) const noexcept {
        return SuperType::serialise(serialiser);
    }

    Failure BoneAttachment3D::deserialise(Deserialiser& deserialiser) noexcept {
        return SuperType::deserialise(deserialiser);
    }


    REGISTER_METHODS(
        BoneAttachment3D,
        MUTABLE_METHOD(setBoneIndex),
        CONST_METHOD(getBoneIndex)
    );

    REGISTER_MEMBERS(
        BoneAttachment3D,
        bindMember("boneIndex", &BoneAttachment3D::getBoneIndex, &BoneAttachment3D::setBoneIndex)
    );
}
