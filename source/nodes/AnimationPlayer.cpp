#include <m3ds/nodes/AnimationPlayer.hpp>

namespace M3DS {
    struct AnimationHeader {
        std::uint8_t nameLength {};
        std::uint16_t animationEntryCount {};
    };

    struct AnimationEntryHeader {
        std::uint16_t entryCount {};
        std::uint16_t pathLength {};
    };

    std::expected<void, Failure> Animation::serialise(BinaryOutFileAccessor file) const noexcept {
        const AnimationHeader header {
            .nameLength = static_cast<std::uint8_t>(mName.size()),
            .animationEntryCount = static_cast<std::uint16_t>(mAnimationEntries.size())
        };

        if (!file.write(header))
            return std::unexpected{ Failure{ ErrorCode::file_write_fail } };
        if (!file.write(std::span{mName}))
            return std::unexpected{ Failure{ ErrorCode::file_write_fail } };

        for (const auto& animationEntry : mAnimationEntries) {
            std::string_view pathString = animationEntry.path.toString();
            const AnimationEntryHeader entryHeader {
                .entryCount = static_cast<std::uint8_t>(animationEntry.tracks.size()),
                .pathLength = static_cast<std::uint8_t>(pathString.size())
            };
            if (!file.write(entryHeader))
                return std::unexpected{ Failure{ ErrorCode::file_write_fail } };
            if (!file.write(std::span{pathString}))
                return std::unexpected{ Failure{ ErrorCode::file_write_fail } };

            for (const AnimationTrackVariant& variant : animationEntry.tracks) {
                std::expected exp = variant.visit(
                    [&]<typename T>(const AnimationTrack<T>& track) -> std::expected<void, Failure> {
                        if (!file.write(getIdentifier<T>())) 
                            return std::unexpected{ Failure{ ErrorCode::file_write_fail } };
                        return track.serialise(file);
                    }
                );
                if (!exp)
                    return exp;
            }
        }

        return {};
    }

    std::expected<Animation, Failure> Animation::deserialise(Deserialiser& deserialiser) noexcept {
        AnimationHeader header {};
        if (!deserialiser.read(header))
            return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

        Animation animation {};
        animation.mName.resize(header.nameLength);
        if (!deserialiser.read(std::span{animation.mName}))
            return std::unexpected{ Failure{ ErrorCode::file_read_fail } };
        
        if (header.animationEntryCount == 0) {
            M3DS::Debug::log<1>("Animation Entry Count is 0!");
            return std::unexpected{ Failure{ ErrorCode::invalid_data } };
        }

        animation.mAnimationEntries.resize(header.animationEntryCount);

        for (auto& animationEntry : animation.mAnimationEntries) {
            AnimationEntryHeader entryHeader {};
            if (!deserialiser.read(entryHeader))
                return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

            animationEntry.tracks.resize(entryHeader.entryCount);

            std::string pathData {};
            pathData.resize(entryHeader.pathLength);
            if (!deserialiser.read(std::span{pathData}))
                return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

            animationEntry.path = NodePath{ std::move(pathData) };

            for (auto& trackVariant : animationEntry.tracks) {
                TypeIdentifier identifier {};
                if (!deserialiser.read(identifier))
                    return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

                if (!isValidIdentifier(identifier)) {
                    Debug::err("Invalid type identifier {} when deserialising AnimationTrack!", identifier);
                    return std::unexpected{ Failure{ ErrorCode::invalid_data } };
                }

                auto exp = visit(identifier, [&]<BindableType T>() -> std::expected<AnimationTrackVariant, Failure> {
                    if constexpr (AnimationTypes::contains<T>()) {
                        return AnimationTrack<T>::deserialise(deserialiser);
                    } else {
                        Debug::err("Invalid identifier: {}", identifier);
                        return std::unexpected{ Failure{ ErrorCode::invalid_data } };
                    }
                });
                
                if (exp)
                    trackVariant = std::move(exp.value());
                else
                    return std::unexpected{std::move(exp.error())};
            }
        }

        return animation;
    }

    Animation* AnimationPlayer::getAnimation(const std::string_view name) noexcept {
        for (const std::unique_ptr<Animation>& animation : mAnimations) {
            if (animation->getName() == name)
                return animation.get();
        }
        return {};
    }

    Animation* AnimationPlayer::getAnimationIdx(const std::size_t idx) noexcept {
        return mAnimations.size() < idx ? mAnimations[idx].get() : nullptr;
    }

    Animation* AnimationPlayer::getOrCreateAnimation(const std::string_view name) {
        if (Animation* animation = getAnimation(name))
            return animation;

        return mAnimations.emplace_back(std::make_unique<Animation>(std::string{name})).get();
    }

    Animation* AnimationPlayer::addAnimation(Animation&& animation) {
        return mAnimations.emplace_back(std::make_unique<Animation>(std::move(animation))).get();
    }

    std::size_t AnimationPlayer::getAnimationCount() const noexcept {
        return mAnimations.size();
    }

    void AnimationPlayer::removeAnimation(const std::string_view name) noexcept {
        std::erase_if(mAnimations, [&](const std::unique_ptr<Animation>& anim) {
            if (anim->getName() == name) {
                std::erase_if(mAnimationLayers, [&](const AnimationLayer& layer) {
                    return layer.animation == anim.get();
                });
                return true;
            }
            return false;
        });
    }

    void AnimationPlayer::removeAllAnimations() noexcept {
        stop();
        mAnimations.clear();
        mAnimationLayers.clear();
    }

    void AnimationPlayer::play() noexcept {
        if (!mAnimationLayers.empty())
            mActive = true;
    }

    void AnimationPlayer::pause() noexcept {
        mActive = false;
    }

    void AnimationPlayer::stop() noexcept {
        mActive = false;
        mAnimationLayers.clear();
    }

    void AnimationPlayer::restart() noexcept {
        if (mAnimationLayers.empty())
            return;
        for (AnimationLayer& layer : mAnimationLayers)
            layer.elapsed = 0.f;
        mActive = true;
    }

    void AnimationPlayer::playAnimationPtr(const Animation* animation, const float weight) noexcept {
        mAnimationLayers.clear();
        addLayerPtr(animation, weight);
    }

    void AnimationPlayer::playAnimation(const std::string_view animation, const float weight) noexcept {
        playAnimationPtr(getAnimation(animation), weight);
    }

    void AnimationPlayer::playAnimationIdx(const std::size_t idx, const float weight) noexcept {
        playAnimationPtr(getAnimationIdx(idx), weight);
    }

    void AnimationPlayer::addLayerPtr(const Animation* animation, const float weight) noexcept {
        if (!animation) {
            Debug::err("Attempted to play null animation!");
        } else if (weight < 0.01f) {
            Debug::warn("Attempted to play zero weight animation!");
        } else {
            mAnimationLayers.emplace_back(animation, weight);
            mActive = true;
            Debug::log<1>("Playing animation {}", animation->getName());
        }
    }

    void AnimationPlayer::addLayer(const std::string_view animation, const float weight) noexcept {
        addLayerPtr(getAnimation(animation), weight);
    }

    void AnimationPlayer::addLayerIdx(const std::size_t idx, const float weight) noexcept {
        addLayerPtr(getAnimationIdx(idx), weight);
    }

    bool AnimationPlayer::isActive() const noexcept {
        return mActive;
    }

    Failure AnimationPlayer::serialise(Serialiser& serialiser) const noexcept {
        if (const Failure failure = SuperType::serialise(serialiser))
            return failure;

        if (mAnimations.size() > 1024)
            return Failure{ ErrorCode::invalid_data };

        if (!serialiser.write(static_cast<std::uint16_t>(mAnimations.size())))
            return Failure{ ErrorCode::file_write_fail };

        for (const std::unique_ptr<Animation>& animation : mAnimations) {
            if (std::expected exp = animation->serialise(serialiser); !exp.has_value()) {
                Debug::err(exp.error());
                return Failure{ ErrorCode::invalid_data };
            }
        }

        // TODO: serialise current animation state?

        return Success;
    }

    Failure AnimationPlayer::deserialise(Deserialiser& deserialiser) noexcept {
        if (const Failure failure = SuperType::deserialise(deserialiser))
            return failure;

        std::uint16_t animationCount;
        if (!deserialiser.read(animationCount))
            return Failure{ ErrorCode::file_read_fail };

        if (animationCount > 1024)
            return Failure{ ErrorCode::invalid_data };

        mAnimations.resize(animationCount);

        for (std::unique_ptr<Animation>& animation : mAnimations) {
            if (std::expected exp = Animation::deserialise(deserialiser)) {
                animation = std::make_unique<Animation>(std::move(exp.value()));
            } else {
                Debug::err(exp.error());
                return Failure{ ErrorCode::invalid_data };
            }
        }

        // TODO: deserialise current animation state?

        return Success;
    }

    void AnimationPlayer::update(const Seconds<float> delta) {
        Node::update(delta);

        if (!mActive)
            return;

        if (mAnimationLayers.empty()) {
            mActive = false;
            return;
        }

        for (AnimationLayer& layer : mAnimationLayers) {
            layer.elapsed += delta * layer.timeScale;
            layer.completed = true;
        }

        static std::flat_map<Node*, std::vector<std::pair<const GenericMember*, BindableTypesVariant>>> values {};

        for (AnimationLayer& layer : mAnimationLayers) {
            if (!layer.active)
                continue;

            const Animation* animation = layer.animation;

            for (auto&& [path, entries] : animation->getEntries()) {
                if (entries.empty())
                    continue;

                Node* node = getNode(path);

                if (!node)
                    Debug::err("Invalid animation NodePath: {}", path);

                auto& memberValues = values[node];

                for (const AnimationTrackVariant& trackVariant : entries) {
                    trackVariant.visit([&]<typename T>(const AnimationTrack<T>& track) {
                        auto&& [value, isCompleted] = track.getValue(layer.elapsed);

                        if (!isCompleted)
                            layer.completed = false;

                        const auto it = std::ranges::find_if(
                            memberValues,
                            [&](const auto& e) { return e.first == track.getMember(); }
                        );
                        if (it == memberValues.end()) {
                            memberValues.emplace_back(track.getMember(), scale(value, layer.weight));
                        } else {
                            T& val = std::get<T>(it->second);
                            val = combine(val, scale(value, layer.weight));
                        }
                    });
                }
            }
        }

        for (auto&& [node, vec] : values)
            for (auto&& [member, value] : vec)
                member->variantSet(node, value);

        values.clear();

        for (AnimationLayer& layer : mAnimationLayers) {
            if (layer.completed) {
                Debug::log<1>("Completed animation {}", layer.animation->getName());
                layer.elapsed = 0.f;
                if (layer.animation->oneShot)
                    layer.active = false;
            }
        }
    }

    REGISTER_METHODS(
        AnimationPlayer,

        MUTABLE_METHOD(removeAnimation),
        MUTABLE_METHOD(removeAllAnimations),
        MUTABLE_METHOD(playAnimation),
        MUTABLE_METHOD(pause),
        MUTABLE_METHOD(stop),
        MUTABLE_METHOD(restart),
        MUTABLE_METHOD(playAnimation),
        MUTABLE_METHOD(playAnimationIdx),
        MUTABLE_METHOD(addLayer),
        MUTABLE_METHOD(addLayerIdx),
        CONST_METHOD(isActive)
    );

    REGISTER_NO_MEMBERS(AnimationPlayer);
}
