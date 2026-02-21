#pragma once

#include <filesystem>

#include <m3ds/utils/binding/BoundMember.hpp>
#include <m3ds/utils/binding/BoundMethod.hpp>

#include <m3ds/types/Failure.hpp>

#include <m3ds/reference/Resource.hpp>
#include <m3ds/utils/BinaryFile.hpp>

namespace M3DS {
    class Registry {
        struct Entry {
            std::unique_ptr<Object> (*uniqueInstantiator)() {};
            const GenericMember* (*getMembersFunc)(std::string_view) {};
            BoundMethodPair (*getMethodFunc)(std::string_view) {};
        };

        static inline std::flat_map<std::string_view, Entry> mRegistry {};
    public:
        template <typename T>
        requires (!std::derived_from<T, Resource>)
        static constexpr void registerType();

        template <typename... Ts>
        requires (!std::derived_from<Ts, Resource> &&...)
        static constexpr void registerTypes();

        [[nodiscard]] static constexpr std::unique_ptr<Object> instantiate(std::string_view className);

        [[nodiscard]] static std::expected<std::unique_ptr<Object>, Failure> deserialise(const std::filesystem::path& path) noexcept;
        [[nodiscard]] static std::expected<std::unique_ptr<Object>, Failure> deserialise(Deserialiser& deserialiser) noexcept;

        [[nodiscard]] static Failure serialise(const Object& object, const std::filesystem::path& path) noexcept;
        [[nodiscard]] static Failure serialise(const Object& object, Serialiser& serialiser) noexcept;

        template <ObjectType O>
        [[nodiscard]] static std::expected<std::unique_ptr<O>, Failure> deserialise(const std::filesystem::path& path) noexcept;
        template <ObjectType O>
        [[nodiscard]] static std::expected<std::unique_ptr<O>, Failure> deserialise(Deserialiser& deserialiser) noexcept;

        [[nodiscard]] static constexpr const GenericMember* getMember(std::string_view className, std::string_view memberName);
        [[nodiscard]] static constexpr BoundMethodPair getMethodPair(std::string_view className, std::string_view methodName);

        [[nodiscard]] static constexpr const MutableGenericMethod* getMutableMethod(std::string_view className, std::string_view methodName);
        [[nodiscard]] static constexpr const ConstGenericMethod* getConstMethod(std::string_view className, std::string_view methodName);

        static void clear() noexcept;
    };

    template <ObjectType O>
    std::expected<std::unique_ptr<O>, Failure> Registry::deserialise(const std::filesystem::path& path) noexcept {
        BinaryInFile file { path };
        Deserialiser deserialiser { file.getAccessor() };
        if (!file)
            return std::unexpected{ Failure{ ErrorCode::file_open_fail } };
        return deserialise<O>(deserialiser);
    }

    template <ObjectType O>
    std::expected<std::unique_ptr<O>, Failure> Registry::deserialise(Deserialiser& deserialiser) noexcept {
        if (std::expected exp = deserialise(deserialiser)) {
            if (std::unique_ptr obj = object_pointer_cast<O>(std::move(exp.value())))
                return obj;
            return std::unexpected{ Failure{ ErrorCode::object_cast_failed } };
        } else {
            return std::unexpected{ exp.error() };
        }
    }

    template <typename T>
    requires (!std::derived_from<T, Resource>)
    constexpr void Registry::registerType() {
        mRegistry.emplace(
            T::getClassStatic(),
            Entry {
                [] -> std::unique_ptr<Object> {
                    if constexpr (std::is_default_constructible_v<T>)
                        return std::make_unique<T>();
                    else
                        return {};
                },
                T::getMemberStatic,
                T::getMethodStatic
            }
        );
    }

    template <typename ... Ts>
    requires (!std::derived_from<Ts, Resource> &&...)
    constexpr void Registry::registerTypes() {
        (registerType<Ts>(), ...);
    }

    constexpr std::unique_ptr<Object> Registry::instantiate(std::string_view className) {
        if (const auto it = mRegistry.find(className); it != mRegistry.end()) {
            return it->second.uniqueInstantiator();
        }
        return {};
    }

    constexpr const GenericMember* Registry::getMember(const std::string_view className, const std::string_view memberName) {
        if (const auto it = mRegistry.find(className); it != mRegistry.end()) {
            if (const auto& func = it->second.getMembersFunc) {
                return func(memberName);
            }
        }
        return {};
    }

    constexpr BoundMethodPair Registry::getMethodPair(const std::string_view className, const std::string_view methodName) {
        if (const auto it = mRegistry.find(className); it != mRegistry.end()) {
            if (const auto& func = it->second.getMethodFunc) {
                return func(methodName);
            }
        }
        return {};
    }

    constexpr const MutableGenericMethod* Registry::getMutableMethod(const std::string_view className, const std::string_view methodName) {
        if (const MutableGenericMethod* method = getMethodPair(className, methodName).mutableMethod)
            return method;
        Debug::err("Failed to find mutable method {}::{}", className, methodName);
        return {};
    }

    constexpr const ConstGenericMethod* Registry::getConstMethod(const std::string_view className, const std::string_view methodName) {
        if (const ConstGenericMethod* method = getMethodPair(className, methodName).constMethod)
            return method;
        return {};
    }
}
