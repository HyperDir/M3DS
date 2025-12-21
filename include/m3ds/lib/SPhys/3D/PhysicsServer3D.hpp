#pragma once

#include <vector>
#include <unordered_map>
#include <flat_set>

#include "PhysicsEnvironment3D.hpp"
#include "CollisionObjects/Area3D.hpp"
#include "CollisionObjects/KinematicBody3D.hpp"
#include "CollisionObjects/StaticBody3D.hpp"

#include "Intersections3D.hpp"

#include "../Containers/Hive.hpp"
#include "../Utils/Accessor.hpp"

namespace SPhys {
    template <PhysicsEnvironment3D Environment = PhysicsEnvironment3D{}>
    class PhysicsServer3D {
        Hive<StaticBody3D> mStaticBodyHive {};
        Hive<KinematicBody3D> mKinematicBodyHive {};
        Hive<Area3D> mAreaHive {};

        std::flat_set<const Area3D*> mRemovedAreas {};

        std::unordered_map<Vector3i, std::vector<Area3D*>> mAreaSpatialHash {};
        std::unordered_map<Vector3i, std::vector<PhysicsBody3D*>> mBodySpatialHash {};
    public:
        constexpr Accessor<StaticBody3D> emplaceStaticBody();
        constexpr Accessor<KinematicBody3D> emplaceKinematicBody();
        constexpr Accessor<Area3D> emplaceArea();

        constexpr void eraseStaticBody(Accessor<StaticBody3D> iterator) noexcept;
        constexpr void eraseKinematicBody(Accessor<KinematicBody3D> iterator) noexcept;
        constexpr void eraseArea(Accessor<Area3D> iterator) noexcept;

        constexpr void updateAreas() noexcept;
        constexpr void step(Seconds<float> delta) noexcept;

        constexpr const Hive<Area3D>& getAreas() const noexcept;
        constexpr const Hive<KinematicBody3D>& getKinematicBodies() const noexcept;
        constexpr const Hive<StaticBody3D>& getStaticBodies() const noexcept;
    };
}



/* Implementation */
namespace SPhys {
    template <PhysicsEnvironment3D Environment>
    constexpr Accessor<StaticBody3D> PhysicsServer3D<Environment>::emplaceStaticBody() {
        return Accessor{ mStaticBodyHive.insert() };
    }

    template <PhysicsEnvironment3D Environment>
    constexpr Accessor<KinematicBody3D> PhysicsServer3D<Environment>::emplaceKinematicBody() {
        return Accessor{ mKinematicBodyHive.insert() };
    }

    template <PhysicsEnvironment3D Environment>
    constexpr Accessor<Area3D> PhysicsServer3D<Environment>::emplaceArea() {
        return Accessor{ mAreaHive.insert() };
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::eraseStaticBody(Accessor<StaticBody3D> iterator) noexcept {
        mStaticBodyHive.erase(iterator.mIterator);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::eraseKinematicBody(Accessor<KinematicBody3D> iterator) noexcept {
        mKinematicBodyHive.erase(iterator.mIterator);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::eraseArea(Accessor<Area3D> iterator) noexcept {
        mAreaHive.erase(iterator.mIterator);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::updateAreas() noexcept {
        mAreaSpatialHash.clear();

        const auto forOverlappingCells = [&](const Area3D& area, auto&& func) {
            const auto& [min, max] = area.getBoundingBox();

            const Vector3i lower  { (min / Environment.chunkSize).floor() };
            const Vector3i higher { (max / Environment.chunkSize).floor() };

            Vector3i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    func(pos);
        };

        for (Area3D& area : mAreaHive) {
            if (area.isDisabled()) {
                std::erase_if(area.mOverlappingAreas, [&](const Area3D* other) {
                    return mRemovedAreas.contains(other);
                });
                continue;
            }

            std::erase_if(area.mOverlappingAreas, [&](Area3D* other) {
                if (mRemovedAreas.contains(other))
                    return true;

                if (other->isDisabled()) {
                    if (area.areaExited) area.areaExited(other);
                    return true;
                }

                if (!(area.getMask() & other->getLayer())) {
                    if (area.areaExited) area.areaExited(other);
                    return true;
                }

                const bool overlapping = (
                    area.getBoundingBox().isOverlapping(other->getBoundingBox()) &&
                    std::visit(
                        [](const auto& lhs, const auto& rhs) -> bool {
                            return isIntersecting(lhs, rhs);
                        },
                        area.getGlobalShape(),
                        other->getGlobalShape()
                    )
                );

                if (!overlapping) {
                    if (other->getMask() & area.getLayer() && std::erase(other->mOverlappingAreas, &area)) {
                        if (other->areaExited) other->areaExited(&area);
                    }

                    if (area.areaExited) area.areaExited(other);
                    return true;
                }

                return false;
            });

            forOverlappingCells(area, [&](const Vector3i& cell) {
                auto& cellAreas = mAreaSpatialHash[cell];

                for (Area3D* other : cellAreas) {
                    {
                        const bool lhsContainsRhs = std::ranges::contains(area.mOverlappingAreas, other);
                        const bool rhsContainsLhs = std::ranges::contains(other->mOverlappingAreas, &area);

                        if (lhsContainsRhs) {
                            if (
                                other->getMask() & area.getLayer() &&
                                !rhsContainsLhs
                            ) {
                                other->mOverlappingAreas.emplace_back(&area);
                                if (other->areaEntered) other->areaEntered(&area);
                            }

                            continue;
                        }
                        if (rhsContainsLhs) {
                            if (area.getMask() & other->getLayer()) {
                                area.mOverlappingAreas.emplace_back(other);
                                if (other->areaEntered) area.areaEntered(other);
                            }

                            continue;
                        }
                    }

                    if (
                        area.getBoundingBox().isOverlapping(other->getBoundingBox()) &&
                        std::visit(
                            [](const auto& lhs, const auto& rhs) -> bool {
                                return isIntersecting(lhs, rhs);
                            },
                            area.getGlobalShape(),
                            other->getGlobalShape()
                        )
                    ) {
                        if (area.getMask() & other->getLayer()) {
                            area.mOverlappingAreas.emplace_back(other);
                            if (area.areaEntered) area.areaEntered(other);
                        }
                        if (other->getMask() & area.getLayer()) {
                            other->mOverlappingAreas.emplace_back(&area);
                            if (area.areaEntered) other->areaEntered(&area);
                        }
                    }
                }

                cellAreas.emplace_back(&area);
            });
        }
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::step(const Seconds<float> delta) noexcept {
        mBodySpatialHash.clear();

        const auto forOverlappingCells = [&](const PhysicsBody3D& body, const Vector3& velocity, auto&& func) {
            const auto [min, max] = body.getBoundingBox().expand(velocity * delta);

            const Vector3i lower  { (min / Environment.chunkSize).floor() };
            const Vector3i higher { (max / Environment.chunkSize).floor() };

            Vector3i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    func(pos);
        };

        for (StaticBody3D& body : mStaticBodyHive) {
            forOverlappingCells(
                body,
                {},
                [&](const Vector3i& cell) {
                    mBodySpatialHash[cell].emplace_back(&body);
                }
            );
        }

        for (KinematicBody3D& body : mKinematicBodyHive) {
            forOverlappingCells(
                body,
                body.getVelocity(),
                [&](const Vector3i& cell) {
                    // Don't bother if no bodies are present in cell
                    if (const auto it = mBodySpatialHash.find(cell); it != mBodySpatialHash.end())
                        it->second.emplace_back(&body);
                }
            );
        }

        std::vector<const PhysicsBody3D*> otherBodies {};

        for (auto& body : mKinematicBodyHive) {
            body.mOnGround = false;
            otherBodies.clear();

            body.addTranslation(body.getVelocity() * delta);

            // Slight bias toward the ground
            // Hack to keep bodies grounded when moving down slopes
            body.addTranslation(body.getUpDirection() * -Environment.groundBias * delta);

            forOverlappingCells(body, body.getVelocity(), [&](const Vector3i& cell) {
                const auto it = mBodySpatialHash.find(cell);
                if (it == mBodySpatialHash.end()) return;

                for (const PhysicsBody3D* other : it->second) {
                    if (
                        other != &body &&
                        body.getMask() & other->getLayer() &&
                        !std::ranges::contains(otherBodies, other)
                    ) {
                        otherBodies.emplace_back(other);
                    }
                }
            });

            static constexpr int depthLimit = 4;
            bool resolvedAll = false;

            Vector3 aggregateNormal {};

            for (int depth = 0; !resolvedAll && depth < depthLimit; ++depth) {
                resolvedAll = true;

                for (const PhysicsBody3D* other : otherBodies) {
                    if (!body.getBoundingBox().isOverlapping(other->getBoundingBox())) {
                        continue;
                    }

                    const std::optional<MTV3D> mtv = std::visit(
                        [&](const ShapeType3D auto& lhs, const ShapeType3D auto& rhs) {
                            return separatingAxisTest(lhs, rhs);
                        },
                        body.getGlobalShape<DirtyCheck::perform>(),
                        other->getGlobalShape<DirtyCheck::skip>()
                    );

                    if (!mtv) continue;

                    resolvedAll = false;

                    const Vector3 normal = mtv->normal;
                    const Vector3 separation = mtv->get();

                    aggregateNormal += normal;

                    const bool isFloor = body.getUpDirection().dot(normal) > 0.6;

                    body.mOnGround = body.mOnGround || isFloor;

                    if (isFloor && !body.getSlideOnSlope()) {
                        const float verticalPush = separation.dot(body.getUpDirection());
                        body.addTranslation(body.getUpDirection() * verticalPush);
                    } else {
                        body.addTranslation(separation);
                    }

                    const float normalVel = body.getVelocity().dot(normal);

                    if (normalVel < 0)
                        body.setVelocity(body.getVelocity() - normal * normalVel);
                }
            }

            if (aggregateNormal.lengthSquared() > 1e-9) {
                if (body.getUpDirection().dot(aggregateNormal.normalise()) > 0.6)
                    body.mOnGround = true;
            }
        }
    }

    template <PhysicsEnvironment3D Environment>
    constexpr const Hive<Area3D>& PhysicsServer3D<Environment>::getAreas() const noexcept {
        return mAreaHive;
    }

    template <PhysicsEnvironment3D Environment>
    constexpr const Hive<KinematicBody3D>& PhysicsServer3D<Environment>::getKinematicBodies() const noexcept {
        return mKinematicBodyHive;
    }

    template <PhysicsEnvironment3D Environment>
    constexpr const Hive<StaticBody3D>& PhysicsServer3D<Environment>::getStaticBodies() const noexcept {
        return mStaticBodyHive;
    }
}
