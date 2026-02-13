#pragma once

#include <string_view>

#include <m3ds/types/TypePack.hpp>

namespace M3DS {
    template <typename MemberType>
    class Member;

    class Object;

    class GenericMember {
    public:
        template <std::size_t N>
        explicit consteval GenericMember(const char (&name)[N]) noexcept;

        virtual ~GenericMember() = default;

        virtual void variantSet(Object* object, BindableTypesVariant to) const noexcept = 0;
        [[nodiscard]] virtual BindableTypesVariant variantGet(const Object* object) const noexcept = 0;
        [[nodiscard]] virtual std::string_view getClassName() const noexcept = 0;

        template <typename MemberType>
        [[nodiscard]] const Member<MemberType>* specialise() const noexcept;

        [[nodiscard]] constexpr std::string_view getName() const noexcept;
    protected:
        [[nodiscard]] virtual std::size_t getTypeIndex() const noexcept = 0;

        std::string_view mName {};
    };

    template <typename MemberType>
    class Member : public GenericMember {
    public:
        template <std::size_t N>
        explicit consteval Member(const char (&name)[N]) noexcept;

        virtual void set(Object* object, PassEfficiently<MemberType> to) const noexcept = 0;
        virtual MemberType get(const Object* object) const noexcept = 0;
    protected:
        [[nodiscard]] std::size_t getTypeIndex() const noexcept final;
    };

    // This could be de-virtualised, but this makes the API very consistent so it's fine for now
    // Also, de-virtualising this would slightly slow down private members
    template <typename ClassType, typename MemberType>
    class PublicMemberSpecialisation final : public Member<MemberType> {
    public:
        template <std::size_t N>
        consteval PublicMemberSpecialisation(
            const char (&name)[N],
            MemberType ClassType::* member
        );

        void variantSet(Object* object, BindableTypesVariant to) const noexcept override;
        [[nodiscard]] BindableTypesVariant variantGet(const Object* object) const noexcept override;

        void set(Object* object, PassEfficiently<MemberType> to) const noexcept override;
        [[nodiscard]] MemberType get(const Object* object) const noexcept override;

        [[nodiscard]] std::string_view getClassName() const noexcept override;
    private:
        MemberType ClassType::*mMember {};
    };

    template <typename ClassType, typename MemberType, typename SetterType, typename GetterType>
    class PrivateMemberSpecialisation final : public Member<MemberType> {
    public:
        template <std::size_t N>
        consteval PrivateMemberSpecialisation(
            const char (&name)[N],
            GetterType (ClassType::*getter)() const noexcept,
            void (ClassType::*setter)(SetterType) noexcept = nullptr
        );

        void variantSet(Object* object, BindableTypesVariant to) const noexcept override;
        [[nodiscard]] BindableTypesVariant variantGet(const Object* object) const noexcept override;

        [[nodiscard]] MemberType get(const Object* object) const noexcept override;
        void set(Object* object, PassEfficiently<MemberType> to) const noexcept override;

        [[nodiscard]] std::string_view getClassName() const noexcept override;
    private:
        GetterType (ClassType::*mGetter)() const noexcept {};
        void (ClassType::*mSetter)(SetterType) noexcept {};
    };




    template <std::size_t N, typename ClassType, typename MemberType>
    constexpr PublicMemberSpecialisation<ClassType, MemberType> bindMember(
        const char (&name)[N],
        MemberType ClassType::* member
    ) noexcept;

    template <std::size_t N, typename ClassType, typename GetterType, typename SetterType = GetterType>
    constexpr PrivateMemberSpecialisation<
        ClassType,
        std::remove_cvref_t<GetterType>,
        SetterType,
        GetterType
    > bindMember(
        const char (&name)[N],
        GetterType (ClassType::*getter)() const noexcept,
        void (ClassType::*setter)(SetterType) noexcept = nullptr
    ) noexcept;

    template <std::size_t N, typename ClassType, typename SetterType>
    constexpr PrivateMemberSpecialisation<
        ClassType,
        std::remove_cvref_t<SetterType>,
        SetterType,
        SetterType
    > bindMember(
        const char (&name)[N],
        nullptr_t getter,
        void (ClassType::*setter)(SetterType) noexcept = nullptr
    ) noexcept;
}
