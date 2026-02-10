#include <m3ds/reference/Object.hpp>


namespace M3DS {
    std::string_view Object::getParentClass() const noexcept {
        return getParentClassStatic();
    }

    std::string_view Object::getClass() const noexcept {
        return getClassStatic();
    }

    bool Object::inheritsFromHashVirtual(const std::size_t fromClass) const noexcept {
        return inheritsFromHash(fromClass);
    }

    bool Object::inherits(const std::string_view typeName) const noexcept {
        return inheritsFromHashVirtual(fnv1a_hash(typeName));
    }

    BoundMethodPair Object::getMethod(const std::string_view name) noexcept {
        return getMethodStatic(name);
    }

    const GenericMember* Object::getMember(const std::string_view name) noexcept {
        return getMemberStatic(name);
    }

    Failure Object::serialise(const BinaryOutFileAccessor file) const noexcept {
        const std::string_view className = getClass();

        if (className.length() > std::numeric_limits<std::uint8_t>::max())
            return Failure{ ErrorCode::out_of_bounds };

        if (
            !file.write(static_cast<std::uint8_t>(className.length())) ||
            !file.write(std::span{className})
        )
            return Failure{ ErrorCode::file_write_fail };

        return Success;
    }

    Failure Object::deserialise([[maybe_unused]] const BinaryInFileAccessor file) noexcept {
        return Success;
    }

    BoundMethodPair Object::getMethodStatic(const std::string_view name) noexcept {
        static constexpr MethodPack methods {
            CONST_METHOD(inherits),
            CONST_METHOD(getClass)
        };

        for (const BoundMethodPair& pair : methods.getMethods()) {
            const auto& [constMethod, mutableMethod] = pair;
            if (
                (constMethod && constMethod->getName() == name) ||
                (mutableMethod && mutableMethod->getName() == name)
            )
                return pair;
        }
        return {};
    }

    const GenericMember* Object::getMemberStatic(const std::string_view) noexcept {
        return {};
    }
}
