#pragma once

#include <flat_map>
#include <flat_set>

#include <m3ds/nodes/Node.hpp>
#include <m3ds/containers/HeapArray.hpp>
#include <m3ds/utils/Interpolate.hpp>

#include <m3ds/utils/BinaryFile.hpp>

#include <m3ds/utils/binding/Registry.hpp>

namespace M3DS {
    template <typename T>
    struct TrackEntry {
        Seconds<float> time {};
        T value {};
        Easing easing {};
    };

    using IsAnimationCompleted = bool;

    template <typename T>
    class AnimationTrack {
    public:
        InterpolationMethod interpolationMethod = InterpolationMethod::linear;

        constexpr AnimationTrack() noexcept = default;
        explicit constexpr AnimationTrack(const Member<T>* member) noexcept;

        void setEntries(std::vector<TrackEntry<T>>&& entries) noexcept;
        void addEntry(Seconds<float> time, T value, Easing easing = Easing::in_out);
        void removeEntry(Seconds<float> time) noexcept;

        IsAnimationCompleted update(Object* object, Seconds<float> elapsed) const noexcept;
        [[nodiscard]] std::pair<T, IsAnimationCompleted> getValue(Seconds<float> elapsed) const noexcept;

        [[nodiscard]] const Member<T>* getMember() const noexcept;

        [[nodiscard]] std::expected<void, StringError> serialise(BinaryOutFileAccessor& file) const noexcept;
        [[nodiscard]] static std::expected<AnimationTrack, StringError> deserialise(BinaryInFileAccessor& file) noexcept;
    private:
        const Member<T>* mMember {};
        std::vector<TrackEntry<T>> mEntries {};
    };

    template <typename T>
    constexpr AnimationTrack<T>::AnimationTrack(const Member<T>* member) noexcept
        : mMember(member)
    {}

    using AnimationTrackVariant = AnimationTypes::transform<AnimationTrack>::apply<std::variant>;

    class Animation {
    public:
        struct AnimationEntry {
            NodePath path {};
            std::vector<AnimationTrackVariant> tracks {};
        };

        bool oneShot {};

        [[nodiscard]] explicit constexpr Animation() = default;
        [[nodiscard]] explicit constexpr Animation(std::string animationName) noexcept;

        template <typename T>
        [[nodiscard]] AnimationTrack<T>* getTrack(const NodePath& path, const Member<T>* member) noexcept;

        template <typename T>
        [[nodiscard]] AnimationTrack<T>* getOrCreateTrack(const NodePath& path, const Member<T>* member) noexcept;

        [[nodiscard]] constexpr std::span<AnimationTrackVariant> getTracks(const NodePath& path) noexcept;
        [[nodiscard]] constexpr std::span<const AnimationTrackVariant> getTracks(const NodePath& path) const noexcept;

        constexpr void setName(std::string to) noexcept;
        [[nodiscard]] constexpr std::string_view getName() const noexcept;

        [[nodiscard]] constexpr std::span<AnimationEntry> getEntries() noexcept;
        [[nodiscard]] constexpr std::span<const AnimationEntry> getEntries() const noexcept;

        [[nodiscard]] std::expected<void, StringError> serialise(BinaryOutFileAccessor file) const noexcept;
        [[nodiscard]] static std::expected<Animation, StringError> deserialise(BinaryInFileAccessor file) noexcept;
    private:
        std::string mName {};
        std::vector<AnimationEntry> mAnimationEntries {};
    };

    struct AnimationLayer {
        const Animation* animation {};
        float weight = 1.f;
        float timeScale = 1.f;
        Seconds<float> elapsed {};
        bool completed {};
        bool active = true;
    };

    class AnimationPlayer : public Node {
        M_CLASS(AnimationPlayer, Node)
    public:
        [[nodiscard]] Animation* getAnimation(std::string_view name) noexcept;
        [[nodiscard]] Animation* getAnimationIdx(std::size_t idx) noexcept;
        [[nodiscard]] Animation* getOrCreateAnimation(std::string_view name);
        [[nodiscard]] Animation* addAnimation(Animation&& animation);

        [[nodiscard]] std::size_t getAnimationCount() const noexcept;

        void removeAnimation(std::string_view name) noexcept;
        void removeAllAnimations() noexcept;

        void play() noexcept;
        void pause() noexcept;
        void stop() noexcept;
        void restart() noexcept;

        void playAnimation(std::string_view animation, float weight = 1.f) noexcept;
        void playAnimationIdx(std::size_t idx, float weight = 1.f) noexcept;
        void playAnimationPtr(const Animation* animation, float weight = 1.f) noexcept;

        void addLayer(std::string_view animation, float weight = 1.f) noexcept;
        void addLayerIdx(std::size_t idx, float weight = 1.f) noexcept;
        void addLayerPtr(const Animation* animation, float weight = 1.f) noexcept;

        [[nodiscard]] bool isActive() const noexcept;
    protected:
        void update(Seconds<float> delta) override;
    private:
        std::vector<std::unique_ptr<Animation>> mAnimations {};

        std::vector<AnimationLayer> mAnimationLayers {};
        bool mActive {};
    };
}


/* Implementations */
namespace M3DS {
    template <typename T>
    auto operator<=>(const TrackEntry<T>& lhs, const TrackEntry<T>& rhs) {
        return lhs.time <=> rhs.time;
    }

    template <typename T>
    auto operator<=>(const TrackEntry<T>& lhs, Seconds<float> rhs) {
        return lhs.time <=> rhs;
    }

    template <typename T>
    void AnimationTrack<T>::setEntries(std::vector<TrackEntry<T>>&& entries) noexcept {
        mEntries = std::move(entries);
    }

    template <typename T>
    void AnimationTrack<T>::addEntry(Seconds<float> time, T value, const Easing easing) {
        const auto it = std::ranges::lower_bound(mEntries, time, std::less{});
        if (it != mEntries.end() && it->time == time)
            *it = { time, value, easing };
        else
            mEntries.emplace(it, time, value, easing);
    }

    template <typename T>
    void AnimationTrack<T>::removeEntry(Seconds<float> time) noexcept {
        erase_if(mEntries, [time](const auto& entry) { return entry.time == time; });
    }

    template <typename T>
    IsAnimationCompleted AnimationTrack<T>::update(Object* object, const Seconds<float> elapsed) const noexcept {
        auto&& [val, completed] = getValue(elapsed);
        mMember->set(object, val);
        return completed;
    }

    template <typename T>
    std::pair<T, IsAnimationCompleted> AnimationTrack<T>::getValue(const Seconds<float> elapsed) const noexcept {
        if (mEntries.empty())
            return { {}, true };

        const auto next = std::lower_bound(mEntries.begin(), mEntries.end(), elapsed);

        if (next == mEntries.begin())
            return { next->value, false };

        const auto prev = std::prev(next);

        if (next == mEntries.end())
            return { prev->value, true };

        if (std::abs(next->time - elapsed) < 1e-3)
            return { next->value, false };

        const Easing easing =
            (static_cast<bool>(prev->easing & Easing::out) ? Easing::in : Easing::none) |
            (static_cast<bool>(next->easing & Easing::in) ? Easing::out : Easing::none);

        if constexpr (CanInterpolate<T>) {
            return {
                interpolate(
                    prev->value,
                    next->value,
                    (elapsed - prev->time) / (next->time - prev->time),
                    interpolationMethod,
                    easing
                ),
                false
            };
        }
        return { prev->value, false };
    }

    template <typename T>
    const Member<T>* AnimationTrack<T>::getMember() const noexcept {
        return mMember;
    }

    struct TrackHeader {
        std::uint8_t classNameLength {};
        std::uint8_t memberNameLength {};
        std::uint8_t entryCount {};
        InterpolationMethod interpolationMethod {};
    };

    template <typename T>
    std::expected<void, StringError> AnimationTrack<T>::serialise(BinaryOutFileAccessor& file) const noexcept {
        std::string_view className = mMember->getClassName();
        std::string_view memberName = mMember->getName();

        static constexpr std::size_t u8max = std::numeric_limits<std::uint8_t>::max();
        if (className.length() > u8max)
            return std::unexpected{ std::format("AnimationTrack class name too long: {}! Max: {}", className.length(), u8max) };
        if (memberName.length() > u8max)
            return std::unexpected{ std::format("AnimationTrack member name too long: {}! Max: {}", memberName.length(), u8max) };
        if (mEntries.size() > u8max)
            return std::unexpected{ std::format("AnimationTrack has too many entries: {}! Max: {}", mEntries.size(), u8max) };

        const TrackHeader header {
            .classNameLength = static_cast<std::uint8_t>(className.length()),
            .memberNameLength = static_cast<std::uint8_t>(memberName.length()),
            .entryCount = static_cast<std::uint8_t>(mEntries.size()),
            .interpolationMethod = interpolationMethod
        };

        if (
            !file.write(header) ||
            !file.write(std::span{ className }) ||
            !file.write(std::span{ memberName })
        ) return std::unexpected{ "Failed to write AnimationTrack header and class/member name data!" };

        for (const auto& [time, value, easing] : mEntries) {
            if constexpr (std::same_as<T, std::string>) {
                if (value.size() > u8max)
                    return std::unexpected{ std::format("AnimationTrack string entry too long: {}! Max: {}", value.size(), u8max) };
                
                if (
                    !file.write(time) ||
                    !file.write(value.size()) ||
                    !file.write(std::span{ value }) ||
                    !file.write(easing)
                ) return std::unexpected{ "Failed to write AnimationTrack entry!" };
            } else {
                if (
                    !file.write(time) ||
                    !file.write(value) ||
                    !file.write(easing)
                ) return std::unexpected{ "Failed to write AnimationTrack entry!" };
            }
        }

        return {};
    }

    template <typename T>
    std::expected<AnimationTrack<T>, StringError> AnimationTrack<T>::deserialise(BinaryInFileAccessor& file) noexcept {
        TrackHeader header {};
        if (!file.read(header))
            return std::unexpected{ "Failed to read AnimationTrack header!" };

        HeapArray<char> nameData {};
        // Cannot be greater than 2x uint8_t max
        nameData.resize(header.classNameLength + header.memberNameLength);
        if (!file.read(std::span{ nameData }))
            return std::unexpected{ "Failed to read AnimationTrack class/member name data!" };

        const std::string_view className { nameData.data(), header.classNameLength };
        const std::string_view memberName { nameData.data() + header.classNameLength, header.memberNameLength };

        const GenericMember* member = Registry::getMember(className, memberName);
        if (!member)
            return std::unexpected{ std::format("Failed to get member {}::{} when loading AnimationTrack!", className, memberName) };

        const Member<T>* specialisation = member->specialise<T>();
        if (!specialisation)
            return std::unexpected{ std::format("Failed to specialise member {}::{} when loading AnimationTrack!", className, memberName) };

        AnimationTrack track { specialisation };
        track.interpolationMethod = header.interpolationMethod;

        track.mEntries.resize(header.entryCount);
        for (auto& [time, value, easing] : track.mEntries) {
            if constexpr (std::same_as<T, std::string>) {
                std::uint8_t strLength {};
                if (
                    !file.read(time) ||
                    !file.read(strLength)
                ) return std::unexpected{ "Failed to read AnimationTrack entry!" };

                value.resize(strLength);
                if (
                    !file.read(std::span{ value }) ||
                    !file.read(easing)
                ) return std::unexpected{ "Failed to read AnimationTrack entry!" };
            } else {
                if (
                    !file.read(time) ||
                    !file.read(value) ||
                    !file.read(easing)
                ) return std::unexpected{ "Failed to read AnimationTrack entry!" };
            }
        }

        return track;
    }

    template <typename T>
    AnimationTrack<T>* Animation::getTrack(const NodePath& path, const Member<T>* member) noexcept {
        const auto& tracks = getTracks(path);
        for (auto& track : tracks) {
            if (
                AnimationTrack<T>* t = std::get_if<AnimationTrack<T>>(&track);
                t && t->getMember() == member
            ) {
                return t;
            }
        }
        return {};
    }

    template <typename T>
    AnimationTrack<T>* Animation::getOrCreateTrack(const NodePath& path, const Member<T>* member) noexcept {
        const auto trackIt = std::ranges::find_if(
            mAnimationEntries,
            [&](const auto& entry) {
                return entry.path == path;
            }
        );

        if (trackIt == mAnimationEntries.end()) {
            std::vector<AnimationTrackVariant>& vec = mAnimationEntries.emplace_back(path).tracks;
            return &std::get<AnimationTrack<T>>(vec.emplace_back(AnimationTrack<T>{ member }));
        }

        auto& tracks = trackIt->tracks;

        for (auto& track : tracks) {
            if (
                AnimationTrack<T>* t = std::get_if<AnimationTrack<T>>(&track);
                t && t->getMember() == member
            ) {
                return t;
            }
        }

        return &std::get<AnimationTrack<T>>(tracks.emplace_back(AnimationTrack<T>{ member }));
    }

    constexpr Animation::Animation(std::string animationName) noexcept
        : mName(std::move(animationName))
    {}

    constexpr std::span<AnimationTrackVariant> Animation::getTracks(const NodePath& path) noexcept {
        const auto it = std::ranges::find_if(
            mAnimationEntries,
            [&](const auto& val) {
                return val.path == path;
            }
        );

        if (it == mAnimationEntries.end())
            return {};

        return it->tracks;
    }

    constexpr std::span<const AnimationTrackVariant> Animation::getTracks(const NodePath& path) const noexcept {
        const auto it = std::ranges::find_if(
            mAnimationEntries,
            [&](const auto& val) {
                return val.path == path;
            }
        );

        if (it == mAnimationEntries.end())
            return {};

        return it->tracks;
    }

    constexpr void Animation::setName(std::string to) noexcept {
        mName = std::move(to);
    }

    constexpr std::string_view Animation::getName() const noexcept {
        return {mName.begin(), mName.end()};
    }

    constexpr std::span<Animation::AnimationEntry> Animation::getEntries() noexcept {
        return mAnimationEntries;
    }

    constexpr std::span<const Animation::AnimationEntry> Animation::getEntries() const noexcept {
        return mAnimationEntries;
    }
}
