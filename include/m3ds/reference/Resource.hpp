#pragma once

#include <flat_map>
#include <memory>

#include <m3ds/reference/Object.hpp>
#include <m3ds/utils/BinaryFile.hpp>
#include <m3ds/utils/Path.hpp>

namespace M3DS {
    class Resource : public Object, std::enable_shared_from_this<Resource> {
        M_CLASS(Resource, Object)
        friend class ResourceRegistry;
        friend constexpr Failure serialise(const Resource* resource, Serialiser& serialiser);

        template <std::derived_from<Resource> T>
        friend constexpr Failure deserialise(std::shared_ptr<T>& resource, Deserialiser& deserialiser);

        std::filesystem::path mPath {};
    public:
        Resource() = default;
        ~Resource() override;

        void setPath(std::filesystem::path to);
        [[nodiscard]] std::filesystem::path getPath() const;
    };

    class ResourceRegistry {
        friend class Resource;

        static inline std::flat_map<std::string_view, std::shared_ptr<Resource>(*)()> mRegistry {};
        static inline std::flat_map<std::filesystem::path, std::weak_ptr<Resource>> mResources {};
    public:
        template <typename T>
        requires (std::derived_from<T, Resource>)
        static constexpr void registerResource();

        template <typename... Ts>
        requires (std::derived_from<Ts, Resource> && ...)
        static constexpr void registerResources();

        [[nodiscard]] static std::expected<std::shared_ptr<Resource>, Failure> load(std::filesystem::path path);
        [[nodiscard]] static std::expected<std::shared_ptr<Resource>, Failure> load(Deserialiser& deserialiser);

        [[nodiscard]] static Failure save(const Resource& resource) noexcept;
    };

    template <typename T> requires (std::derived_from<T, Resource>)
    constexpr void ResourceRegistry::registerResource() {
        mRegistry.emplace(
            T::getClassStatic(),
            [] -> std::shared_ptr<Resource> {
                return std::make_shared<T>();
            }
        );
    }

    template <typename ... Ts> requires (std::derived_from<Ts, Resource> && ...)
    constexpr void ResourceRegistry::registerResources() {
        (registerResource<Ts>(), ...);
    }


    // TODO: Can be optimised by removing repeated writes in a single serialisation pass
    // TODO: where the same resource is referenced more than once
    [[nodiscard]] constexpr Failure serialise(const Resource* resource, Serialiser& serialiser) {
        const std::filesystem::path& path = resource->getPath();
        const bool local = path.empty();

        if (!serialiser.write(local))
            return Failure{ ErrorCode::file_write_fail };

        if (local)
            return resource->serialise(serialiser);

        if (const Failure failure = serialise(path, serialiser))
            return failure;

        if (BinaryOutFile f { path })
            return resource->serialise(serialiser);

        return Failure{ ErrorCode::file_open_fail };
    }


    template <std::derived_from<Resource> T>
    [[nodiscard]] constexpr Failure deserialise(std::shared_ptr<T>& resource, Deserialiser& des) {
        bool local;
        if (!des.read(local))
            return Failure{ ErrorCode::file_read_fail };

        Debug::log<1>("Local: {}", local);

        if (local) {
            if (std::expected result = ResourceRegistry::load(des)) {
                resource = object_pointer_cast<T>(std::move(result.value()));
            } else {
                return result.error();
            }
        } else {
            std::filesystem::path path {};
            if (const Failure failure = deserialise(path, des))
                return failure;

            if (std::expected result = ResourceRegistry::load(std::move(path))) {
                resource = object_pointer_cast<T>(std::move(result.value()));
            } else {
                return result.error();
            }
        }

        return Success;
    }
}
