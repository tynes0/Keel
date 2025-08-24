#pragma once
#ifndef _KEEL_OPTIONAL_H_DEFINED_
#define _KEEL_OPTIONAL_H_DEFINED_

#include <new> // std::launder
#include <cassert> // assert
#include <cstdint> // uint8_t
#include <type_traits> // std::is_nothrow_copy_constructible_v, std::is_nothrow_move_constructible_v, is_nothrow_constructible_v

#if __cplusplus >= 202002L
#define KEEL_CONSTEXPR constexpr
#else
#define KEEL_CONSTEXPR
#endif

#ifndef KEEL_ASSERT
#   define KEEL_ASSERT(expr, message)  assert(expr && message)
#endif // !KEEL_ASSERT

namespace keel
{
    namespace detail
    {
        template <typename T>
        struct RemoveReference { using Type = T; };
        template <typename T>
        struct RemoveReference<T& > { using Type = T; };
        template <typename T>
        struct RemoveReference<T&&> { using Type = T; };


        template <typename T>
        KEEL_CONSTEXPR typename RemoveReference<T>::Type&& Move(T&& Obj)
        {
            using CastType = typename RemoveReference<T>::Type;
            return (CastType&&)Obj;
        }

        template <typename T>
        KEEL_CONSTEXPR T&& Forward(typename RemoveReference<T>::Type& Obj)
        {
            return (T&&)Obj;
        }

        template <typename T>
        KEEL_CONSTEXPR T&& Forward(typename RemoveReference<T>::Type&& Obj)
        {
            return (T&&)Obj;
        }

        template <class T>
        struct TypeCompatibleBytes
        {
            T& Get()
            {
                return *std::launder(reinterpret_cast<T*>(m_Bytes));
            }
            const T& Get() const
            {
                return *std::launder(reinterpret_cast<const T*>(m_Bytes));
            }

        private:
            alignas(T) uint8_t m_Bytes[sizeof(T)];
        };
    }

    template <class T>
    struct Optional
    {
    public:
        Optional(const T& Value)
        {
            new(&m_Value.Get()) T(Value);
            m_HasValue = true;
        }

        Optional(T&& Value)
        {
            new(&m_Value.Get()) T(keel::detail::Move(Value));
            m_HasValue = true;
        }

        Optional() noexcept
            : m_HasValue(false)
        {
        }

        Optional(const Optional& Value) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : m_HasValue(Value.m_HasValue)
        {
            if (Value.m_HasValue)
                new(&m_Value.Get()) T(Value.m_Value.Get());
        }

        Optional(Optional&& Value) noexcept(std::is_nothrow_move_constructible_v<T>)
            : m_HasValue(Value.m_HasValue)
        {
            if (Value.m_HasValue)
            {
                new(&m_Value.Get()) T(keel::detail::Move(Value.m_Value.Get()));
            }
        }

        Optional& operator=(const Optional& Value)
        {
            if (&Value != this)
            {
                Reset();
                m_HasValue = Value.m_HasValue;
                if (Value.m_HasValue)
                    new(&m_Value.Get()) T(Value.m_Value.Get());
            }
            return *this;
        }

        Optional& operator=(Optional&& Value)
        {
            if (&Value != this)
            {
                Reset();
                m_HasValue = Value.m_HasValue;
                if (Value.m_HasValue)
                    new(&m_Value.Get()) T(keel::detail::Move(Value.m_Value.Get()));
            }
            return *this;
        }

        Optional& operator=(const T& Value)
        {
            if (&Value != &m_Value.Get())
            {
                Reset();
                new(&m_Value.Get()) T(Value);
                m_HasValue = true;
            }
            return *this;
        }
        Optional& operator=(T&& Value)
        {
            if (&Value != &m_Value.Get())
            {
                Reset();
                new(&m_Value.Get()) T(keel::detail::Move(Value));
                m_HasValue = true;
            }
            return *this;
        }

        ~Optional() noexcept
        {
            Reset();
        }

        void Reset() noexcept
        {
            if (m_HasValue)
            {
                m_Value.Get().~T();
                m_HasValue = false;
            }
        }

        template <typename... ArgsT>
        void Emplace(ArgsT&&... Args) noexcept(std::is_nothrow_constructible_v<T, ArgsT...>)
        {
            Reset();
            new(&m_Value.Get()) T(keel::detail::Forward<ArgsT>(Args)...);
            m_HasValue = true;
        }

        friend bool operator==(const Optional& lhs, const Optional& rhs);
        friend bool operator!=(const Optional& lhs, const Optional& rhs);

        KEEL_CONSTEXPR bool HasValue() const { return m_HasValue; }
        KEEL_CONSTEXPR explicit operator bool() const { return m_HasValue; }

        KEEL_CONSTEXPR operator T* () { return HasValue() ? &m_Value.Get() : nullptr; }
        KEEL_CONSTEXPR operator const T* () const { return HasValue() ? &m_Value.Get() : nullptr; }

        const T& GetValue() const { KEEL_ASSERT(HasValue(), "GetValue() called on an empty Optional."); return m_Value.Get(); }
        T& GetValue() { KEEL_ASSERT(HasValue(), "GetValue() called on an empty Optional."); return m_Value.Get(); }

        const T& operator*() const { return GetValue(); }
        T& operator*() { return GetValue(); }

        const T* operator->() const { return &GetValue(); }
        T* operator->() { return &GetValue(); }

        const T& Get(const T& DefaultValue) const { return HasValue() ? m_Value.Get() : DefaultValue; }

    private:
        keel::detail::TypeCompatibleBytes<T> m_Value;
        bool m_HasValue;
    };

    template <class T>
    bool operator==(const Optional<T>& lhs, const Optional<T>& rhs)
    {
        if (lhs.m_HasValue != rhs.m_HasValue)
        {
            return false;
        }
        if (!lhs.m_HasValue)
        {
            return true;
        }
        return lhs.m_Value.Get() == rhs.m_Value.Get();
        }

    template <class T>
    bool operator!=(const Optional<T>& lhs, const Optional<T>& rhs)
    {
        if (lhs.m_HasValue != rhs.m_HasValue)
        {
            return true;
        }
        if (!lhs.m_HasValue)
        {
            return false;
        }
        return lhs.m_Value.Get() != rhs.m_Value.Get();
        }

    template <class T> 
    struct IsOptional { static constexpr bool Value = false; };
    template <class T> 
    struct IsOptional<Optional<T>> { static constexpr bool Value = true; };
}

#endif // !_KEEL_OPTIONAL_H_DEFINED_
