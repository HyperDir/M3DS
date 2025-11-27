#pragma once

#include <vector>
#include <unordered_map>
#include <flat_set>

#include "PhysicsEnvironment3D.hpp"
#include "CollisionObjects/Area3D.hpp"
#include "CollisionObjects/KinematicBody3D.hpp"
#include "CollisionObjects/StaticBody3D.hpp"

#include "Intersections3D.hpp"

namespace SPhys {
    template <PhysicsEnvironment3D Environment = PhysicsEnvironment3D{}>
    class PhysicsServer3D {
        std::vector<StaticBody3D*> mStaticBodies {};
        std::vector<KinematicBody3D*> mKinematicBodies {};
        std::vector<Area3D*> mAreas {};

        std::flat_set<const Area3D*> mRemovedAreas {};
    public:
        [[nodiscard]] std::span<Area3D* const> getAreas() const noexcept;
        [[nodiscard]] std::span<KinematicBody3D* const> getKinematicBodies() const noexcept;
        [[nodiscard]] std::span<StaticBody3D* const> getStaticBodies() const noexcept;

        constexpr void registerBody(StaticBody3D& body) noexcept;
        constexpr void unregisterBody(const StaticBody3D& body) noexcept;

        constexpr void registerBody(KinematicBody3D& body) noexcept;
        constexpr void unregisterBody(const KinematicBody3D& body) noexcept;

        constexpr void registerArea(Area3D& area) noexcept;
        constexpr void unregisterArea(const Area3D& area) noexcept;

        void registerObject(CollisionObject3D& object) noexcept;
        void unregisterObject(CollisionObject3D& object) noexcept;

        void updateAreas() noexcept;
        void step(Seconds<float> delta) noexcept;
    };

    template <PhysicsEnvironment3D Environment>
    void PhysicsServer3D<Environment>::registerObject(CollisionObject3D& object) noexcept {
        const ObjectType3D type = object.getObjectType();
        if (type == ObjectType3D::area)
            registerArea(reinterpret_cast<Area3D&>(object));
        else if (type == ObjectType3D::static_body)
            registerBody(reinterpret_cast<StaticBody3D&>(object));
        else if (type == ObjectType3D::kinematic_body)
            registerBody(reinterpret_cast<KinematicBody3D&>(object));
        else
            std::cerr << std::format("Invalid object type: {}!", static_cast<std::uint32_t>(type)) << std::endl;
    }

    template <PhysicsEnvironment3D Environment>
    void PhysicsServer3D<Environment>::unregisterObject(CollisionObject3D& object) noexcept {
        const ObjectType3D type = object.getObjectType();
        if (type == ObjectType3D::area)
            unregisterArea(reinterpret_cast<Area3D&>(object));
        else if (type == ObjectType3D::static_body)
            unregisterBody(reinterpret_cast<StaticBody3D&>(object));
        else if (type == ObjectType3D::kinematic_body)
            unregisterBody(reinterpret_cast<KinematicBody3D&>(object));
        else
            std::cerr << std::format( "Invalid object type: {}!", static_cast<std::uint32_t>(type)) << std::endl;
    }

    template <PhysicsEnvironment3D Environment>
    void PhysicsServer3D<Environment>::updateAreas() noexcept {
        std::unordered_map<Vector3i, std::vector<Area3D*>> spatialHash3D {};

        const auto forOverlappingCells = [&](const Area3D* area, auto&& func) {
            const auto& [min, max] = area->getBoundingBox();

            const Vector3i lower  { min / Environment.chunkSize };
            const Vector3i higher { (max / Environment.chunkSize).ceil() };

            Vector3i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    for (pos.z = lower.z; pos.z <= higher.z; ++pos.z)
                        func(pos);
        };

        for (Area3D* area : mAreas) {
            if (area->isDisabled()) {
                std::erase_if(area->mOverlappingAreas, [&](const Area3D* other) {
                    return mRemovedAreas.contains(other);
                });
                continue;
            }

            std::erase_if(area->mOverlappingAreas, [&](Area3D* other) {
                if (mRemovedAreas.contains(other))
                    return true;

                if (other->isDisabled()) {
                    area->areaExited(other);
                    return true;
                }

                if (!(area->getMask() & other->getLayer())) {
                    area->areaExited(other);
                    return true;
                }

                const bool overlapping = (
                    area->getBoundingBox().isOverlapping(other->getBoundingBox()) &&
                    std::visit(
                        [](const auto& lhs, const auto& rhs) -> bool {
                            return isIntersecting(lhs, rhs);
                        },
                        area->getGlobalShape(),
                        other->getGlobalShape()
                    )
                );

                if (!overlapping) {
                    if (other->getMask() & area->getLayer() && std::erase(other->mOverlappingAreas, area)) {
                        other->areaExited(area);
                    }

                    area->areaExited(other);
                    return true;
                }

                return false;
            });

            forOverlappingCells(area, [&](const Vector3i& cell) {
                auto& cellAreas = spatialHash3D[cell];

                for (Area3D* other : cellAreas) {
                    {
                        const bool lhsContainsRhs = std::ranges::contains(area->mOverlappingAreas, other);
                        const bool rhsContainsLhs = std::ranges::contains(other->mOverlappingAreas, area);

                        if (lhsContainsRhs) {
                            if (
                                other->getMask() & area->getLayer() &&
                                !rhsContainsLhs
                            ) {
                                other->mOverlappingAreas.emplace_back(area);
                                other->areaEntered(area);
                            }

                            continue;
                        }
                        if (rhsContainsLhs) {
                            if (area->getMask() & area->getLayer()) {
                                area->mOverlappingAreas.emplace_back(other);
                                area->areaEntered(other);
                            }

                            continue;
                        }
                    }

                    if (
                        area->getBoundingBox().isOverlapping(other->getBoundingBox()) &&
                        std::visit(
                            [](const auto& lhs, const auto& rhs) -> bool {
                                return isIntersecting(lhs, rhs);
                            },
                            area->getGlobalShape(),
                            other->getGlobalShape()
                        )
                    ) {
                        if (area->getMask() & other->getLayer()) {
                            area->mOverlappingAreas.emplace_back(other);
                            area->areaEntered(other);
                        }
                        if (other->getMask() & area->getLayer()) {
                            other->mOverlappingAreas.emplace_back(area);
                            other->areaEntered(area);
                        }
                    }
                }

                cellAreas.push_back(area);
            });
        }
    }

    template <PhysicsEnvironment3D Environment>
    void PhysicsServer3D<Environment>::step(const Seconds<float> delta) noexcept {
        std::unordered_map<Vector3i, std::vector<PhysicsBody3D*>> spatialHash3D {};

        const auto forOverlappingCells = [&](const PhysicsBody3D* body, const Vector3& velocity, auto&& func) {
            const auto [min, max] = body->getBoundingBox().expand(velocity * delta);

            const Vector3i lower  { min / Environment.chunkSize };
            const Vector3i higher { (max / Environment.chunkSize).ceil() };

            Vector3i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    for (pos.z = lower.z; pos.z <= higher.z; ++pos.z)
                        func(pos);
        };

        for (KinematicBody3D* body : mKinematicBodies) {
            forOverlappingCells(body, body->getVelocity(), [&](const Vector3i& cell) {
                spatialHash3D[cell].push_back(body);
            });
        }

        for (StaticBody3D* body : mStaticBodies) {
            forOverlappingCells(body, {}, [&](const Vector3i& cell) {
                // Don't bother if no bodies are present in cell
                if (const auto it = spatialHash3D.find(cell); it != spatialHash3D.end())
                    it->second.push_back(body);
            });
        }

        std::vector<const PhysicsBody3D*> otherBodies {};

        static constexpr std::uint_fast8_t subStepCount = 4;
        const float subDelta = delta / subStepCount;

        for (auto* body : mKinematicBodies) {
            body->mOnGround = false;

            for (std::uint_fast8_t i{}; i < subStepCount && body->getVelocity().lengthSquared() > 1e-9; ++i) {
                body->setTranslation(body->getTranslation() + body->getVelocity() * subDelta);

                // Slight bias toward the ground
                // Hack to keep bodies grounded when moving down slopes
                // Causes extra acceleration momentarily when stepping off edge
                body->setTranslation(body->getTranslation() + body->getUpDirection() * -2.f * subDelta);

                otherBodies.clear();
                forOverlappingCells(body, body->getVelocity(), [&](const Vector3i& cell) {
                    const auto it = spatialHash3D.find(cell);
                    if (it == spatialHash3D.end()) return;

                    for (const PhysicsBody3D* other : it->second) {
                        if (
                            other != body &&
                            body->getMask() & other->getLayer() &&
                            !std::ranges::contains(otherBodies, other)
                        ) {
                            otherBodies.push_back(other);
                        }
                    }
                });

                static constexpr int depthLimit = 4;
                bool resolvedAll = false;
                for (int depth = 0; !resolvedAll && depth < depthLimit; ++depth) {
                    resolvedAll = true;

                    for (const PhysicsBody3D* other : otherBodies) {
                        if (!body->getBoundingBox().isOverlapping(other->getBoundingBox())) {
                            continue;
                        }

                        const std::optional<MTV3D> mtv = std::visit(
                            [&](const ShapeType3D auto& lhs, const ShapeType3D auto& rhs) {
                                return separatingAxisTest(lhs, rhs);
                            },
                            body->getGlobalShape<DirtyCheck::skip>(),
                            other->getGlobalShape<DirtyCheck::skip>()
                        );

                        if (!mtv) continue;

                        resolvedAll = false;

                        const bool onGround = body->getUpDirection().dot(mtv->normal) > 0.8;
                        body->mOnGround = body->mOnGround || onGround;

                        if (onGround && !body->getSlideOnSlope()) {
                            body->setTranslation(body->getTranslation() + body->getUpDirection() * mtv->get().dot(body->getUpDirection()));

                            const Vector3 downSlope = -body->getUpDirection();
                            const float dotProduct = body->getVelocity().dot(downSlope);
                            if (dotProduct > 0)
                                body->setVelocity(body->getVelocity() - downSlope * dotProduct);
                        } else {
                            body->setTranslation(body->getTranslation() + mtv->get());
                            const float dotProduct = body->getVelocity().dot(mtv->normal);
                            if (dotProduct < 0)
                                body->setVelocity(body->getVelocity() - mtv->normal * dotProduct);
                        }
                        if (body->getVelocity().lengthSquared() < 1e-9) break;
                    }
                }
            }
        }
    }

    template <PhysicsEnvironment3D Environment>
    std::span<Area3D* const> PhysicsServer3D<Environment>::getAreas() const noexcept {
        return mAreas;
    }

    template <PhysicsEnvironment3D Environment>
    std::span<KinematicBody3D* const> PhysicsServer3D<Environment>::getKinematicBodies() const noexcept {
        return mKinematicBodies;
    }

    template <PhysicsEnvironment3D Environment>
    std::span<StaticBody3D* const> PhysicsServer3D<Environment>::getStaticBodies() const noexcept {
        return mStaticBodies;
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::registerBody(StaticBody3D& body) noexcept {
        mStaticBodies.emplace_back(&body);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::unregisterBody(const StaticBody3D& body) noexcept {
        std::erase(mStaticBodies, &body);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::registerBody(KinematicBody3D& body) noexcept {
        mKinematicBodies.emplace_back(&body);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::unregisterBody(const KinematicBody3D& body) noexcept {
        std::erase(mKinematicBodies, &body);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::registerArea(Area3D& area) noexcept {
        mAreas.emplace_back(&area);
    }

    template <PhysicsEnvironment3D Environment>
    constexpr void PhysicsServer3D<Environment>::unregisterArea(const Area3D& area) noexcept {
        mRemovedAreas.emplace(&area);
        std::erase(mAreas, &area);
    }
}
