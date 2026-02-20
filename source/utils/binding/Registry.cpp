#include <m3ds/utils/binding/Registry.hpp>

#include <m3ds/reference/Resource.hpp>

namespace M3DS {
    std::expected<std::unique_ptr<Object>, Failure> Registry::deserialise(const std::filesystem::path& path) noexcept {
        if (BinaryInFile file { path }) {
            Deserialiser deserialiser { file.getAccessor() };
            return deserialise(deserialiser);
        }
        return std::unexpected{ Failure{ ErrorCode::file_open_fail } };
    }

    std::expected<std::unique_ptr<Object>, Failure> Registry::deserialise(Deserialiser& deserialiser) noexcept {
        std::uint8_t classNameLength;
        if (!deserialiser.read(classNameLength))
            return std::unexpected{ Failure{ ErrorCode::file_read_fail } };
        std::string className {};
        className.resize(classNameLength);
        if (!deserialiser.read(std::span{className}))
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

        if (const Failure failure = ptr->deserialise(deserialiser))
            return std::unexpected{ failure };

        return ptr;
    }

    Failure Registry::serialise(const Object& object, const std::filesystem::path& path) noexcept {
        if (BinaryOutFile file { path }) {
            Serialiser serialiser { file.getAccessor() };
            return serialise(object, serialiser);
        }
        return Failure{ ErrorCode::file_open_fail };
    }

    Failure Registry::serialise(const Object& object, Serialiser& serialiser) noexcept {
        return object.serialise(serialiser);
    }

    void Registry::clear() noexcept {
        mRegistry.clear();
    }
}
