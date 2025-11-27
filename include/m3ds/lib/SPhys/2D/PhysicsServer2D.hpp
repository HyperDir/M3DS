#pragma once

#include <vector>
#include <unordered_map>
#include <flat_set>

#include "PhysicsEnvironment2D.hpp"
#include "CollisionObjects/Area2D.hpp"
#include "CollisionObjects/KinematicBody2D.hpp"
#include "CollisionObjects/StaticBody2D.hpp"

#include "Intersections2D.hpp"

namespace SPhys {
    template <PhysicsEnvironment2D Environment = PhysicsEnvironment2D{}>
    class PhysicsServer2D {
        std::vector<StaticBody2D*> mStaticBodies {};
        std::vector<KinematicBody2D*> mKinematicBodies {};
        std::vector<Area2D*> mAreas {};

        std::flat_set<const Area2D*> mRemovedAreas {};
    public:
        [[nodiscard]] std::span<Area2D* const> getAreas() const noexcept;
        [[nodiscard]] std::span<KinematicBody2D* const> getKinematicBodies() const noexcept;
        [[nodiscard]] std::span<StaticBody2D* const> getStaticBodies() const noexcept;

        constexpr void registerBody(StaticBody2D& body) noexcept;
        constexpr void unregisterBody(const StaticBody2D& body) noexcept;

        constexpr void registerBody(KinematicBody2D& body) noexcept;
        constexpr void unregisterBody(const KinematicBody2D& body) noexcept;

        constexpr void registerArea(Area2D& area) noexcept;
        constexpr void unregisterArea(const Area2D& area) noexcept;

        void registerObject(CollisionObject2D& object) noexcept;
        void unregisterObject(CollisionObject2D& object) noexcept;

        void updateAreas() noexcept;
        void step(Seconds<float> delta) noexcept;
    };

    template <PhysicsEnvironment2D Environment>
    void PhysicsServer2D<Environment>::registerObject(CollisionObject2D& object) noexcept {
        const ObjectType2D type = object.getObjectType();
        if (type == ObjectType2D::area)
            registerArea(reinterpret_cast<Area2D&>(object));
        else if (type == ObjectType2D::static_body)
            registerBody(reinterpret_cast<StaticBody2D&>(object));
        else if (type == ObjectType2D::kinematic_body)
            registerBody(reinterpret_cast<KinematicBody2D&>(object));
        else
            std::cerr << std::format("Invalid object type: {}!", static_cast<std::uint32_t>(type)) << std::endl;
    }

    template <PhysicsEnvironment2D Environment>
    void PhysicsServer2D<Environment>::unregisterObject(CollisionObject2D& object) noexcept {
        const ObjectType2D type = object.getObjectType();
        if (type == ObjectType2D::area)
            unregisterArea(reinterpret_cast<Area2D&>(object));
        else if (type == ObjectType2D::static_body)
            unregisterBody(reinterpret_cast<StaticBody2D&>(object));
        else if (type == ObjectType2D::kinematic_body)
            unregisterBody(reinterpret_cast<KinematicBody2D&>(object));
        else
            std::cerr << std::format( "Invalid object type: {}!", static_cast<std::uint32_t>(type)) << std::endl;
    }

    template <PhysicsEnvironment2D Environment>
    void PhysicsServer2D<Environment>::updateAreas() noexcept {
        std::unordered_map<Vector2i, std::vector<Area2D*>> spatialHash2D {};

        const auto forOverlappingCells = [&](const Area2D* area, auto&& func) {
            const auto& [min, max] = area->getBoundingBox();

            const Vector2i lower  { min / Environment.chunkSize };
            const Vector2i higher { (max / Environment.chunkSize).ceil() };

            Vector2i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    func(pos);
        };

        for (Area2D* area : mAreas) {
            if (area->isDisabled()) {
                std::erase_if(area->mOverlappingAreas, [&](const Area2D* other) {
                    return mRemovedAreas.contains(other);
                });
                continue;
            }

            std::erase_if(area->mOverlappingAreas, [&](Area2D* other) {
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

            forOverlappingCells(area, [&](const Vector2i& cell) {
                auto& cellAreas = spatialHash2D[cell];

                for (Area2D* other : cellAreas) {
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

    template <PhysicsEnvironment2D Environment>
    void PhysicsServer2D<Environment>::step(const Seconds<float> delta) noexcept {
        std::unordered_map<Vector2i, std::vector<PhysicsBody2D*>> spatialHash2D {};

        const auto forOverlappingCells = [&](const PhysicsBody2D* body, const Vector2& velocity, auto&& func) {
            const auto [min, max] = body->getBoundingBox().expand(velocity * delta);

            const Vector2i lower  { min / Environment.chunkSize };
            const Vector2i higher { (max / Environment.chunkSize).ceil() };

            Vector2i pos;
            for (pos.x = lower.x; pos.x <= higher.x; ++pos.x)
                for (pos.y = lower.y; pos.y <= higher.y; ++pos.y)
                    func(pos);
        };

        for (KinematicBody2D* body : mKinematicBodies) {
            forOverlappingCells(body, body->getVelocity(), [&](const Vector2i& cell) {
                spatialHash2D[cell].push_back(body);
            });
        }

        for (StaticBody2D* body : mStaticBodies) {
            forOverlappingCells(body, {}, [&](const Vector2i& cell) {
                // Don't bother if no bodies are present in cell
                if (const auto it = spatialHash2D.find(cell); it != spatialHash2D.end())
                    it->second.push_back(body);
            });
        }

        std::vector<const PhysicsBody2D*> otherBodies {};

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
                forOverlappingCells(body, body->getVelocity(), [&](const Vector2i& cell) {
                    const auto it = spatialHash2D.find(cell);
                    if (it == spatialHash2D.end()) return;

                    for (const PhysicsBody2D* other : it->second) {
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

                    for (const PhysicsBody2D* other : otherBodies) {
                        if (!body->getBoundingBox().isOverlapping(other->getBoundingBox())) {
                            continue;
                        }

                        const std::optional<MTV2D> mtv = std::visit(
                            [&](const ShapeType2D auto& lhs, const ShapeType2D auto& rhs) {
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

                            const Vector2 downSlope = -body->getUpDirection();
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

    template <PhysicsEnvironment2D Environment>
    std::span<Area2D* const> PhysicsServer2D<Environment>::getAreas() const noexcept {
        return mAreas;
    }

    template <PhysicsEnvironment2D Environment>
    std::span<KinematicBody2D* const> PhysicsServer2D<Environment>::getKinematicBodies() const noexcept {
        return mKinematicBodies;
    }

    template <PhysicsEnvironment2D Environment>
    std::span<StaticBody2D* const> PhysicsServer2D<Environment>::getStaticBodies() const noexcept {
        return mStaticBodies;
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::registerBody(StaticBody2D& body) noexcept {
        mStaticBodies.emplace_back(&body);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::unregisterBody(const StaticBody2D& body) noexcept {
        std::erase(mStaticBodies, &body);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::registerBody(KinematicBody2D& body) noexcept {
        mKinematicBodies.emplace_back(&body);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::unregisterBody(const KinematicBody2D& body) noexcept {
        std::erase(mKinematicBodies, &body);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::registerArea(Area2D& area) noexcept {
        mAreas.emplace_back(&area);
    }

    template <PhysicsEnvironment2D Environment>
    constexpr void PhysicsServer2D<Environment>::unregisterArea(const Area2D& area) noexcept {
        mRemovedAreas.emplace(&area);
        std::erase(mAreas, &area);
    }
}
