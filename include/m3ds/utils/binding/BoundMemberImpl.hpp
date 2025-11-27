#pragma once

#include <m3ds/utils/binding/BoundMember.hpp>
#include <functional>

namespace M3DS {
    template <std::size_t N>
    consteval GenericMember::GenericMember(const char(& name)[N]) noexcept : mName(name) {}

    constexpr std::string_view GenericMember::getName() const noexcept {
        return mName;
    }

    template <typename MemberType>
    template <std::size_t N>
    consteval Member<MemberType>::Member(const char(& name)[N]) noexcept : GenericMember(name) {}

    template <typename MemberType>
    const Member<MemberType>* GenericMember::specialise() const noexcept {
        if (BindableTypes::indexOf<MemberType>() == getTypeIndex())
            return &static_cast<const Member<MemberType>&>(*this);
        return {};
    }

    template <typename MemberType>
    std::size_t Member<MemberType>::getTypeIndex() const noexcept {
        return BindableTypes::indexOf<MemberType>();
    }

    template <typename ClassType, typename MemberType>
    template <std::size_t N>
    consteval PublicMemberSpecialisation<ClassType, MemberType>::PublicMemberSpecialisation(
        const char(& name)[N],
        MemberType ClassType::* member
    )
        : Member<MemberType>(name)
        , mMember(member)
    {}

    template <typename ClassType, typename MemberType>
    void PublicMemberSpecialisation<ClassType, MemberType>::variantSet(
        Object* object,
        BindableTypesVariant to
    ) const noexcept {
        if (auto* t = std::get_if<MemberType>(&to)) {
            set(object, std::move(*t));
        } else {
            Debug::terminate("Type mismatch on Public Member Specialisation Set!");
        }
    }

    template <typename ClassType, typename MemberType>
    BindableTypesVariant PublicMemberSpecialisation<ClassType, MemberType>::variantGet(
        const Object* object
    ) const noexcept {
        return get(object);
    }

    template <typename ClassType, typename MemberType>
    void PublicMemberSpecialisation<ClassType, MemberType>::set(Object* object,
        PassEfficiently<MemberType> to
    ) const noexcept {
        if (ClassType* o = object_cast<ClassType*>(object)) {
            std::invoke(mMember, o) = to;
        } else {
            Debug::terminate("Type mismatch! {} is not convertible to {}!", object->getClass(), ClassType::getClassStatic());
        }
    }

    template <typename ClassType, typename MemberType>
    MemberType PublicMemberSpecialisation<ClassType, MemberType>::get(
        const Object* object
    ) const noexcept {
        if (const ClassType* o = object_cast<const ClassType*>(object)) {
            return std::invoke(mMember, o);
        }

        Debug::terminate("Type mismatch! {} is not convertible to {}!", object->getClass(), ClassType::getClassStatic());
    }

    template <typename ClassType, typename MemberType>
    std::string_view PublicMemberSpecialisation<ClassType, MemberType>::getClassName() const noexcept {
        return ClassType::getClassStatic();
    }

    template <typename ClassType, typename MemberType, typename SetterType, typename GetterType>
    template <std::size_t N>
    consteval PrivateMemberSpecialisation<ClassType, MemberType, SetterType, GetterType>::PrivateMemberSpecialisation(
        const char(& name)[N],
        GetterType(ClassType::* getter)() const noexcept,
        void(ClassType::* setter)(SetterType) noexcept
    )
        : Member<MemberType>(name)
        , mGetter(getter)
        , mSetter(setter)
    {}

    template <typename ClassType, typename MemberType, typename SetterType, typename GetterType>
    void PrivateMemberSpecialisation<ClassType, MemberType, SetterType, GetterType>::variantSet(
        Object* object,
        BindableTypesVariant to
    ) const noexcept {
        if (auto* t = std::get_if<MemberType>(&to)) {
            set(object, std::move(*t));
        } else {
            Debug::terminate("Type mismatch on Private Member Specialisation Set!");
        }
    }

    template <typename ClassType, typename MemberType, typename SetterType, typename GetterType>
    BindableTypesVariant PrivateMemberSpecialisation<ClassType, MemberType, SetterType, GetterType>::variantGet(
        const Object* object
    ) const noexcept {
        return get(object);
    }

    template <typename ClassType, typename MemberType, typename SetterType, typename GetterType>
    void PrivateMemberSpecialisation<ClassType, MemberType, SetterType, GetterType>::set(
        Object* object,
        PassEfficiently<MemberType> to
    ) const noexcept {
        if (!mSetter)
            Debug::terminate("Attempted to set a field {} that doesn't have a setter!", GenericMember::getName());

        if (ClassType* o = object_cast<ClassType*>(object))
            return std::invoke(mSetter, o, to);

        Debug::terminate("Type mismatch! {} is not convertible to {}!", object->getClass(), ClassType::getClassStatic());

    }

    template <typename ClassType, typename MemberType, typename SetterType, typename GetterType>
    std::string_view PrivateMemberSpecialisation<ClassType, MemberType, SetterType, GetterType>::getClassName() const noexcept {
        return ClassType::getClassStatic();
    }

    template <std::size_t N, typename ClassType, typename MemberType>
    constexpr PublicMemberSpecialisation<ClassType, MemberType> bindMember(
        const char(& name)[N], 
        MemberType ClassType::* member
    ) noexcept {
        return { name, member };
    }

    template <std::size_t N, typename ClassType, typename GetterType, typename SetterType>
    constexpr PrivateMemberSpecialisation<
        ClassType, 
        std::remove_cvref_t<GetterType>, 
        SetterType, 
        GetterType
    > bindMember(
        const char(& name)[N], 
        GetterType(ClassType::* getter)() const noexcept,
        void(ClassType::* setter)(SetterType) noexcept
    ) noexcept {
        return { name, getter, setter };
    }

    template <std::size_t N, typename ClassType, typename SetterType>
    constexpr PrivateMemberSpecialisation<
        ClassType,
        std::remove_cvref_t<SetterType>,
        SetterType,
        SetterType
    > bindMember(
        const char(& name)[N], 
        nullptr_t getter,
        void(ClassType::* setter)(SetterType) noexcept
    ) noexcept {
        return { name, getter, setter };
    }

    template <typename ClassType, typename MemberType, typename SetterType, typename GetterType>
    MemberType PrivateMemberSpecialisation<ClassType, MemberType, SetterType, GetterType>::get(
        const Object* object
    ) const noexcept {
        if (!mGetter)
            Debug::terminate("Attempted to get a field {} that doesn't have a getter!", GenericMember::getName());

        if (const ClassType* o = object_cast<const ClassType*>(object))
            return std::invoke(mGetter, o);

        Debug::terminate("Type mismatch! {} is not convertible to {}!", object->getClass(), ClassType::getClassStatic());
    }
}