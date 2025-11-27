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
        friend constexpr Error serialise(const Resource* resource, BinaryOutFileAccessor file);

        template <std::derived_from<Resource> T>
        friend constexpr Error deserialise(std::shared_ptr<T>& resource, BinaryInFileAccessor file);

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

        [[nodiscard]] static std::expected<std::shared_ptr<Resource>, Error> load(std::filesystem::path path);
        [[nodiscard]] static std::expected<std::shared_ptr<Resource>, Error> load(BinaryInFileAccessor file);

        [[nodiscard]] static Error save(const Resource& resource) noexcept;
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
    [[nodiscard]] constexpr Error serialise(const Resource* resource, BinaryOutFileAccessor file) {
        const std::filesystem::path& path = resource->getPath();
        const bool local = path.empty();

        if (!file.write(local))
            return Error::file_write_fail;

        if (local)
            return resource->serialise(file);

        if (const Error error = serialise(path, file); error != Error::none)
            return error;

        if (BinaryOutFile f { path })
            return resource->serialise(f.getAccessor());

        return Error::file_open_fail;
    }


    template <std::derived_from<Resource> T>
    [[nodiscard]] constexpr Error deserialise(std::shared_ptr<T>& resource, BinaryInFileAccessor file) {
        bool local;
        if (!file.read(local))
            return Error::file_read_fail;

        Debug::log<1>("Local: {}", local);

        if (local) {
            if (std::expected result = ResourceRegistry::load(file)) {
                resource = object_pointer_cast<T>(std::move(result.value()));
            } else {
                return result.error();
            }
        } else {
            std::filesystem::path path {};
            if (const Error error = deserialise(path, file); error != Error::none)
                return error;

            if (std::expected result = ResourceRegistry::load(std::move(path))) {
                resource = object_pointer_cast<T>(std::move(result.value()));
            } else {
                return result.error();
            }
        }

        return Error::none;
    }
}
