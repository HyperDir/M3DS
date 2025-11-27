#include <m3ds/utils/binding/Registry.hpp>

#include <m3ds/reference/Resource.hpp>

namespace M3DS {
    std::expected<std::unique_ptr<Object>, Error> Registry::deserialise(const std::filesystem::path& path) noexcept {
        if (BinaryInFile file { path })
            return deserialise(file.getAccessor());
        return std::unexpected{ Error::file_open_fail };
    }

    std::expected<std::unique_ptr<Object>, Error> Registry::deserialise(const BinaryInFileAccessor file) noexcept {
        std::uint8_t classNameLength;
        if (!file.read(classNameLength))
            return std::unexpected{ Error::file_read_fail };
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
        std::unique_ptr<Object> ptr = it->second.uniqueInstantiator();
        if (!ptr) {
            Debug::err("Class {} does not have a default unique constructor in the Registry!", className);
            return std::unexpected{ Error::non_default_constructible_class };
        }

        if (const Error error = ptr->deserialise(file); error != Error::none)
            return std::unexpected{ error };

        return ptr;
    }

    Error Registry::serialise(const Object& object, const std::filesystem::path& path) noexcept {
        BinaryOutFile file { path };
        if (!file)
            return Error::file_open_fail;
        return serialise(object, file.getAccessor());
    }

    Error Registry::serialise(const Object& object, BinaryOutFileAccessor file) noexcept {
        return object.serialise(file);
    }

    void Registry::clear() noexcept {
        mRegistry.clear();
    }
}
