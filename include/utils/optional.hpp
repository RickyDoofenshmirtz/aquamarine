#pragma once

#include <cassert>
#include <exception>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T>
class optional
{
public:
    using value_type = std::optional<T>::value_type;

    optional([[maybe_unused]] std::nullopt_t _) noexcept
        : m_data(std::nullopt)
    {
    }

    static auto empty() noexcept -> optional { return optional{ std::nullopt }; }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    static auto create(Args&&... args) noexcept -> optional
    {
        return optional{ std::forward<Args>(args)... }; //
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    static auto except_create(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        -> optional
    {
        return optional{ std::forward<Args>(args)... }; //
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    static auto force_create(Args&&... args) noexcept -> optional
    {
        try {
            return optional{ std::forward<Args>(args)... };
        } catch (...) {
            std::terminate();
        }
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto emplace(Args&&... args) noexcept -> T&
    {
        reset();
        return m_data.emplace(std::forward<Args>(args)...);
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto except_emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T&
    {
        reset();
        return m_data.emplace(std::forward<Args>(args)...);
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto force_emplace(Args&&... args) noexcept -> T&
    {
        try {
            reset();
            auto& elm = m_data.emplace(std::forward<Args>(args)...);
            return elm;
        } catch (...) {
            std::terminate();
        }
    }

    void reset() noexcept { m_data.reset(); }

    explicit operator bool() const noexcept { return m_data.has_value(); }

    auto has_value() const noexcept -> bool { return m_data.has_value(); }

    auto operator*() & noexcept -> T&
    {
        assert(has_value());
        return *m_data;
    }

    auto operator*() const& noexcept -> T const&
    {
        assert(has_value());
        return *m_data;
    }

    auto operator*() && noexcept -> T&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    auto operator*() const&& noexcept -> T const&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    auto operator->(this auto&& self) noexcept
    {
        assert(self);
        return std::addressof(*self.m_data);
    }

    auto value() & noexcept -> T&
    {
        assert(has_value());
        return *m_data;
    }

    auto value() const& noexcept -> T const&
    {
        assert(has_value());
        return *m_data;
    }

    auto value() && noexcept -> T&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    auto value() const&& noexcept -> T const&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    auto begin(this auto&& self) noexcept { return self.m_data.begin(); }

    auto end(this auto&& self) noexcept { return self.m_data.end(); }

    auto as_ref() noexcept -> optional<T&>
    {
        if (!has_value()) { return optional<T&>::empty(); }
        return optional<T&>::create(*m_data);
    }

    auto as_ref() const noexcept -> optional<const T&>
    {
        if (!has_value()) { return optional<T&>::empty(); }
        return optional<const T&>::create(*m_data);
    }

private:
    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    explicit optional(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : m_data(std::forward<Args>(args)...)
    {
    }

    std::optional<T> m_data;
};

template <typename T>
inline auto opt_ref(T& ref) noexcept -> optional<T&>
{
    return optional<T&>::create(ref); //
}

template <typename T>
inline auto some(T&& val) noexcept -> optional<T>
{
    return optional<T>::create(std::forward<T>(val)); //
}

template <typename T>
inline auto nullopt() noexcept -> optional<T>
{
    return optional<T>::empty(); //
}