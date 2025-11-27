#include <m3ds/reference/Resource.hpp>

#include <m3ds/utils/binding/Registry.hpp>

namespace M3DS {
    std::filesystem::path Resource::getPath() const {
        return mPath;
    }

    Resource::~Resource() {
        if (ResourceRegistry::mResources.erase(mPath))
            Debug::log<1>("Unloaded Resource via destructor ({})!", mPath);
    }

    void Resource::setPath(std::filesystem::path to) {
        // Might be more efficient to move it from and then back into the registry?
        if (!mPath.empty())
            ResourceRegistry::mResources.erase(mPath);

        if (std::weak_ptr<Resource> ptr = weak_from_this(); !ptr.expired())
            ResourceRegistry::mResources.emplace(mPath = std::move(to), std::move(ptr));
    }

    std::expected<std::shared_ptr<Resource>, Error> ResourceRegistry::load(std::filesystem::path path) {
        BinaryInFile file { path };
        if (!file)
            return std::unexpected{ Error::file_open_fail };

        return load(file.getAccessor()).transform(
            [&](std::shared_ptr<Resource> ptr) {
                ptr->setPath(path);
                mResources.emplace(std::move(path), ptr);
                return ptr;
            }
        );
    }

    std::expected<std::shared_ptr<Resource>, Error> ResourceRegistry::load(BinaryInFileAccessor file) {
        std::uint8_t classNameLength;
        if (!file.read(classNameLength))
            return std::unexpected{ Error::file_read_fail };

        Debug::log("Length: {}", classNameLength);

        std::string className {};
        className.resize(classNameLength);
        if (!file.read(std::span{className}))
            return std::unexpected{ Error::file_read_fail };

        Debug::log<1>("Deserialising class: {}...", className);
        const auto it = mRegistry.find(className);
        if (it == mRegistry.end()) {
            Debug::err("Unable to find class {} in Registry!", className);
            return std::unexpected{ Error::invalid_class_name };
        }
        std::shared_ptr<Resource> ptr = it->second();
        if (!ptr) {
            Debug::err("Class {} does not have a default shared constructor in the Registry!", className);
            return std::unexpected{ Error::non_default_constructible_class };
        }

        if (const Error error = ptr->deserialise(file); error != Error::none)
            return std::unexpected{ error };

        return { std::move(ptr) };
    }

    Error ResourceRegistry::save(const Resource& resource) noexcept {
        if (resource.mPath.empty())
            return Error::no_resource_path;

        if (BinaryOutFile file { resource.mPath })
            return resource.serialise(file.getAccessor());

        return Error::file_open_fail;
    }

    Error Resource::serialise(BinaryOutFileAccessor file) const noexcept {
        return SuperType::serialise(file);
    }

    Error Resource::deserialise(BinaryInFileAccessor file) noexcept {
        return SuperType::deserialise(file);
    }



    REGISTER_NO_METHODS(Resource);
    REGISTER_NO_MEMBERS(Resource);
}
