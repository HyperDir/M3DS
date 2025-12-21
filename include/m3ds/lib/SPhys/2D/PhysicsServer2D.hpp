#pragma once

#include <vector>
#include <unordered_map>
#include <flat_set>

#include "PhysicsEnvironment2D.hpp"
#include "CollisionObjects/Area2D.hpp"
#include "CollisionObjects/KinematicBody2D.hpp"
#include "CollisionObjects/StaticBody2D.hpp"

#include "Intersections2D.hpp"

#include "../Containers/Hive.hpp"
#include "../Utils/Accessor.hpp"

namespace SPhys {
    template <PhysicsEnvironment2D Environment = PhysicsEnvironment2D{}>
    class PhysicsServer2D {
        Hive<StaticBody2D> mStaticBodyHive {};
        Hive<KinematicBody2D> mKinematicBodyHive {};
        Hive<Area2D> mAreaHive {};

        std::flat_set<const Area2D*> mRemovedAreas {};

        std::unordered_map<Vector2i, std::vector<Area2D*>> mAreaSpatialHash {};
        std::unordered_map<Vector2i, std::vector<PhysicsBody2D*>> mBodySpatialHash {};
    public:
        constexpr Accessor<StaticBody2D> emplaceStaticBody();
        constexpr Accessor<KinematicBody2D> emplaceKinematicBody();
        constexpr Accessor<Area2D> emplaceArea();

        constexpr void eraseStaticBody(Accessor<StaticBody2D> iterator) noexcept;
        constexpr void eraseKinematicBody(Accessor<KinematicBody2D> iterator) noexcept;
        constexpr void eraseArea(Accessor<Area2D> iterator) noexcept;

        constexpr void updateAreas() noexcept;
        constexpr void step(Seconds<float> delta) noexcept;

        constexpr const Hive<Area2D>& getAreas() const noexcept;
        constexpr const Hive<KinematicBody2D>& getKinematicBodies() const noexcept;
        constexpr const Hive<StaticBody2D>& getStaticBodies() const noexcept;
    };
}



/* Implementation */
namespace SPhys {
    template <PhysicsEnvironment2D Environment>
    constexpr Accessor<StaticBody2D> PhysicsServer2D<Environment>::emplaceStaticBody() {
        return Accessor{ mStaticBodyHive.insert() };
    }

    template <PhysicsEnvironment2D Environment>
    constexpr Accessor<KinematicBody2D> PhysicsServer2D<Environment>::emplaceKinematicBody() {
        return Accessor{ mKinematicBodyHive.insert() };
    }

    template <PhysicsEnvironment2D Environment>
    constexpr Accessor<Area2D> PhysicsServer2D<Environment>::emplaceArea() {
        return Accessor{ mAreaHive.insert() };
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::eraseStaticBody(Accessor<StaticBody2D> iterator) noexcept {
        mStaticBodyHive.erase(iterator.mIterator);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::eraseKinematicBody(Accessor<KinematicBody2D> iterator) noexcept {
        mKinematicBodyHive.erase(iterator.mIterator);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::eraseArea(Accessor<Area2D> iterator) noexcept {
        mAreaHive.erase(iterator.mIterator);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::updateAreas() noexcept {
        mAreaSpatialHash.clear();

        const auto forOverlappingCells = [&](const Area2D& area, auto&& func) {
            const auto& [min, max] = area.getBoundingBox();

            const Vector2i lower  { (min / Environment.chunkSize).floor() };
            const Vector2i higher { (max / Environment.chunkSize).floor() };

            Vector2i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    func(pos);
        };

        for (Area2D& area : mAreaHive) {
            if (area.isDisabled()) {
                std::erase_if(area.mOverlappingAreas, [&](const Area2D* other) {
                    return mRemovedAreas.contains(other);
                });
                continue;
            }

            std::erase_if(area.mOverlappingAreas, [&](Area2D* other) {
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

            forOverlappingCells(area, [&](const Vector2i& cell) {
                auto& cellAreas = mAreaSpatialHash[cell];

                for (Area2D* other : cellAreas) {
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

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::step(const Seconds<float> delta) noexcept {
        mBodySpatialHash.clear();

        const auto forOverlappingCells = [&](const PhysicsBody2D& body, const Vector2& velocity, auto&& func) {
            const auto [min, max] = body.getBoundingBox().expand(velocity * delta);

            const Vector2i lower  { (min / Environment.chunkSize).floor() };
            const Vector2i higher { (max / Environment.chunkSize).floor() };

            Vector2i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    func(pos);
        };

        for (StaticBody2D& body : mStaticBodyHive) {
            forOverlappingCells(
                body,
                {},
                [&](const Vector2i& cell) {
                    mBodySpatialHash[cell].emplace_back(&body);
                }
            );
        }

        for (KinematicBody2D& body : mKinematicBodyHive) {
            forOverlappingCells(
                body,
                body.getVelocity(),
                [&](const Vector2i& cell) {
                    // Don't bother if no bodies are present in cell
                    if (const auto it = mBodySpatialHash.find(cell); it != mBodySpatialHash.end())
                        it->second.emplace_back(&body);
                }
            );
        }

        std::vector<const PhysicsBody2D*> otherBodies {};

        for (auto& body : mKinematicBodyHive) {
            body.mOnGround = false;
            otherBodies.clear();

            body.addTranslation(body.getVelocity() * delta);

            // Slight bias toward the ground
            // Hack to keep bodies grounded when moving down slopes
            body.addTranslation(body.getUpDirection() * -Environment.groundBias * delta);

            forOverlappingCells(body, body.getVelocity(), [&](const Vector2i& cell) {
                const auto it = mBodySpatialHash.find(cell);
                if (it == mBodySpatialHash.end()) return;

                for (const PhysicsBody2D* other : it->second) {
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

            Vector2 aggregateNormal {};

            for (int depth = 0; !resolvedAll && depth < depthLimit; ++depth) {
                resolvedAll = true;

                for (const PhysicsBody2D* other : otherBodies) {
                    if (!body.getBoundingBox().isOverlapping(other->getBoundingBox())) {
                        continue;
                    }

                    const std::optional<MTV2D> mtv = std::visit(
                        [&](const ShapeType2D auto& lhs, const ShapeType2D auto& rhs) {
                            return separatingAxisTest(lhs, rhs);
                        },
                        body.getGlobalShape<DirtyCheck::perform>(),
                        other->getGlobalShape<DirtyCheck::skip>()
                    );

                    if (!mtv) continue;

                    resolvedAll = false;

                    const Vector2 normal = mtv->normal;
                    const Vector2 separation = mtv->get();

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

    template <PhysicsEnvironment2D Environment>
    constexpr const Hive<Area2D>& PhysicsServer2D<Environment>::getAreas() const noexcept {
        return mAreaHive;
    }

    template <PhysicsEnvironment2D Environment>
    constexpr const Hive<KinematicBody2D>& PhysicsServer2D<Environment>::getKinematicBodies() const noexcept {
        return mKinematicBodyHive;
    }

    template <PhysicsEnvironment2D Environment>
    constexpr const Hive<StaticBody2D>& PhysicsServer2D<Environment>::getStaticBodies() const noexcept {
        return mStaticBodyHive;
    }
}
