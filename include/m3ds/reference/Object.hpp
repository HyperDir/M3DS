#pragma once

#include <type_traits>

#include <m3ds/utils/binding/BoundMember.hpp>
#include <m3ds/utils/binding/BoundMethod.hpp>

#include <m3ds/types/Failure.hpp>
#include <m3ds/utils/BinaryFile.hpp>

namespace M3DS {
    class Object {
        friend class Registry;
    public:
        using SelfType = Object;
        virtual ~Object() = default;

        [[nodiscard]] static constexpr std::string_view getParentClassStatic() noexcept;
        [[nodiscard]] static constexpr std::string_view getClassStatic() noexcept;
        [[nodiscard]] virtual std::string_view getParentClass() const noexcept;
        [[nodiscard]] virtual std::string_view getClass() const noexcept;

        [[nodiscard]] static constexpr bool inheritsFromHash(std::size_t fromClass) noexcept;
        [[nodiscard]] virtual bool inheritsFromHashVirtual(std::size_t fromClass) const noexcept;
        [[nodiscard]] bool inherits(std::string_view typeName) const noexcept;

        [[nodiscard]] static BoundMethodPair getMethodStatic(std::string_view name) noexcept;
        [[nodiscard]] virtual BoundMethodPair getMethod(std::string_view name) noexcept;

        [[nodiscard]] static const GenericMember* getMemberStatic(std::string_view) noexcept;
        [[nodiscard]] virtual const GenericMember* getMember(std::string_view name) noexcept;
    protected:
        [[nodiscard]] virtual Failure serialise(BinaryOutFileAccessor file) const noexcept;
        [[nodiscard]] virtual Failure deserialise(BinaryInFileAccessor file) noexcept;
    };

    template <typename T> concept ObjectType = std::derived_from<T, Object>;
    template <typename T> concept ObjectTypePointer = ObjectType<std::remove_pointer_t<T>> && std::is_pointer_v<T>;

    template <ObjectTypePointer T, ObjectType U> requires (!std::is_const_v<U>)
    T object_cast(U* obj) noexcept;

    template <ObjectTypePointer T> requires (std::is_const_v<std::remove_pointer_t<T>>)
    T object_cast(const std::derived_from<Object> auto* obj) noexcept;

    template <ObjectType T, ObjectType U>
    std::unique_ptr<T> object_pointer_cast(std::unique_ptr<U> obj) noexcept;

    template <ObjectType T, ObjectType U>
    std::shared_ptr<T> object_pointer_cast(const std::shared_ptr<U>& obj) noexcept;
}

#include <m3ds/utils/binding/BoundMemberImpl.hpp>
#include <m3ds/utils/binding/BoundMethodImpl.hpp>

namespace M3DS {
    constexpr std::size_t fnv1a_hash(const std::string_view str) noexcept {
        std::size_t hash = 0x811C9DC5; // FNV-1a offset basis
        for (const char c : str) {
            hash ^= static_cast<unsigned char>(c);
            hash *= 0x01000193; // FNV-1a prime
        }
        return hash;
    }

    constexpr std::string_view Object::getParentClassStatic() noexcept {
        return {};
    }

    constexpr std::string_view Object::getClassStatic() noexcept {
        return "Object";
    }

    constexpr bool Object::inheritsFromHash(const std::size_t fromClass) noexcept {
        return fromClass == fnv1a_hash(getClassStatic());
    }

    template <ObjectTypePointer T, ObjectType U> requires (!std::is_const_v<U>)
    T object_cast(U* obj) noexcept {
        if (!obj || !obj->inherits(std::remove_pointer_t<T>::getClassStatic())) return nullptr;
        return reinterpret_cast<T>(obj);
    }

    template <ObjectTypePointer T>
    requires (std::is_const_v<std::remove_pointer_t<T>>)
    T object_cast(const std::derived_from<Object> auto* obj) noexcept {
        if (!obj || !obj->inherits(std::remove_pointer_t<T>::getClassStatic())) return nullptr;
        return reinterpret_cast<T>(obj);
    }

    template <ObjectType T, ObjectType U>
    std::unique_ptr<T> object_pointer_cast(std::unique_ptr<U> obj) noexcept {
        if (auto* p = object_cast<T*>(obj.get())) {
            obj.release();
            return std::unique_ptr<T>{ p };
        }
        return {};
    }

    template <ObjectType T, ObjectType U>
    std::shared_ptr<T> object_pointer_cast(const std::shared_ptr<U>& obj) noexcept {
        if (auto* p = object_cast<T*>(obj.get()))
            return std::shared_ptr<T>{ obj, p };
        return {};
    }
}

#define M_CLASS(m_class, m_inherits)                                                                            \
public:                                                                                                         \
using SelfType = m_class;                                                                                       \
using SuperType = m_inherits;                                                                                   \
[[nodiscard]] static constexpr std::string_view getParentClassStatic() noexcept {                               \
    return SuperType::getClassStatic();                                                                         \
}                                                                                                               \
[[nodiscard]] std::string_view getParentClass() const noexcept override {                                       \
    return getParentClassStatic();                                                                              \
}                                                                                                               \
[[nodiscard]] static constexpr std::string_view getClassStatic() noexcept {                                     \
    return #m_class;                                                                                            \
}                                                                                                               \
[[nodiscard]] std::string_view getClass() const noexcept override {                                             \
    return getClassStatic();                                                                                    \
}                                                                                                               \
[[nodiscard]] static constexpr bool inheritsFromHash(const std::size_t fromClassHash) noexcept {                \
    if (M3DS::fnv1a_hash(getClassStatic()) == fromClassHash) return true;                                       \
    return SuperType::inheritsFromHash(fromClassHash);                                                          \
}                                                                                                               \
[[nodiscard]] bool inheritsFromHashVirtual(const std::size_t fromClassHash) const noexcept override {           \
    return inheritsFromHash(fromClassHash);                                                                     \
}                                                                                                               \
static_assert(std::string_view{#m_class}.size() < std::numeric_limits<std::uint8_t>::max());                    \
[[nodiscard]] static M3DS::BoundMethodPair getMethodStatic(const std::string_view name) noexcept;               \
[[nodiscard]] M3DS::BoundMethodPair getMethod(const std::string_view name) noexcept override {                  \
    return getMethodStatic(name);                                                                               \
}                                                                                                               \
[[nodiscard]] static const M3DS::GenericMember* getMemberStatic(const std::string_view name) noexcept;          \
[[nodiscard]] const M3DS::GenericMember* getMember(const std::string_view name) noexcept override {             \
    return getMemberStatic(name);                                                                               \
}                                                                                                               \
protected:                                                                                                      \
[[nodiscard]] M3DS::Failure serialise(M3DS::BinaryOutFileAccessor file) const noexcept override;                \
[[nodiscard]] M3DS::Failure deserialise(M3DS::BinaryInFileAccessor file) noexcept override;                     \
private:

#define REGISTER_NO_METHODS(CLASS_NAME) M3DS::BoundMethodPair CLASS_NAME::getMethodStatic(const std::string_view name) noexcept { return SuperType::getMethodStatic(name); }
#define REGISTER_METHODS(CLASS_NAME, ...)                                                                       \
BoundMethodPair CLASS_NAME::getMethodStatic(const std::string_view name) noexcept {                             \
    static constexpr M3DS::MethodPack methods { __VA_ARGS__ };                                                  \
    for (const M3DS::BoundMethodPair& pair : methods.getMethods()) {                                            \
        const auto& [constMethod, mutableMethod] = pair;                                                        \
        if (                                                                                                    \
            (constMethod && constMethod->getName() == name) ||                                                  \
            (mutableMethod && mutableMethod->getName() == name)                                                 \
        )                                                                                                       \
        return pair;                                                                                            \
    }                                                                                                           \
    return SuperType::getMethodStatic(name);                                                                    \
}

#define CONST_METHOD(method)                                                                                    \
M3DS::MethodPair{                                                                                               \
    M3DS::ConstSpecialisedMethod{ #method, &SelfType::method },                                                 \
    M3DS::MutableWrapperForConstSpecialisedMethod{ #method, &SelfType::method }                                 \
}
#define BOTH_METHOD(method)                                                                                     \
M3DS::MethodPair{                                                                                               \
    M3DS::ConstSpecialisedMethod{ #method, &SelfType::method },                                                 \
    M3DS::MutableSpecialisedMethod{ #method, &SelfType::method }                                                \
}
#define MUTABLE_METHOD(method)                                                                                  \
M3DS::SingleMethod{                                                                                             \
    M3DS::MutableSpecialisedMethod{ #method, &SelfType::method }                                                \
}

#define MEMBER(member) M3DS::bindMember(#member, &SelfType::member)
#define PRIVATE_MEMBER(name, getter, setter) M3DS::bindMember(#name, &SelfType::getter, &SelfType::setter)
#define PRIVATE_GET_ONLY_MEMBER(name, getter) M3DS::bindMember(#name, &SelfType::getter)

#define REGISTER_NO_MEMBERS(CLASS_NAME) const M3DS::GenericMember* CLASS_NAME::getMemberStatic(const std::string_view name) noexcept { return SuperType::getMemberStatic(name); }
#define REGISTER_MEMBERS(CLASS_NAME, ...)                                                                       \
const M3DS::GenericMember* CLASS_NAME::getMemberStatic(const std::string_view name) noexcept {                  \
    static constexpr M3DS::ObjectPack members = M3DS::createObjectPack<M3DS::GenericMember>( __VA_ARGS__ );     \
    for (const M3DS::GenericMember* member : members.getElements())                                             \
        if (member->getName() == name)                                                                          \
            return member;                                                                                      \
    return SuperType::getMemberStatic(name);                                                                    \
}
