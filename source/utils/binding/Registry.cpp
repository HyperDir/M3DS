#include <m3ds/utils/binding/Registry.hpp>

#include <m3ds/reference/Resource.hpp>

namespace M3DS {
    std::expected<std::unique_ptr<Object>, Failure> Registry::deserialise(const std::filesystem::path& path) noexcept {
        if (BinaryInFile file { path })
            return deserialise(file.getAccessor());
        return std::unexpected{ Failure{ ErrorCode::file_open_fail } };
    }

    std::expected<std::unique_ptr<Object>, Failure> Registry::deserialise(const BinaryInFileAccessor file) noexcept {
        std::uint8_t classNameLength;
        if (!file.read(classNameLength))
            return std::unexpected{ Failure{ ErrorCode::file_read_fail } };
        std::string className {};
        className.resize(classNameLength);
        if (!file.read(std::span{className}))
            return std::unexpected{ Failure{ ErrorCode::file_read_fail } };

        Debug::log<1>("Deserialising class: {}...", className);
        const auto it = mRegistry.find(className);
        if (it == mRegistry.end()) {
            Debug::err("Unable to find class {} in Registry!", className);
            return std::unexpected{ Failure{ ErrorCode::invalid_class_name } };
        }
        std::unique_ptr<Object> ptr = it->second.uniqueInstantiator();
        if (!ptr) {
            Debug::err("Class {} does not have a default unique constructor in the Registry!", className);
            return std::unexpected{ Failure{ ErrorCode::non_default_constructible_class } };
        }

        if (const Failure failure = ptr->deserialise(file))
            return std::unexpected{ failure };

        return ptr;
    }

    Failure Registry::serialise(const Object& object, const std::filesystem::path& path) noexcept {
        BinaryOutFile file { path };
        if (!file)
            return Failure{ ErrorCode::file_open_fail };
        return serialise(object, file.getAccessor());
    }

    Failure Registry::serialise(const Object& object, BinaryOutFileAccessor file) noexcept {
        return object.serialise(file);
    }

    void Registry::clear() noexcept {
        mRegistry.clear();
    }
}
