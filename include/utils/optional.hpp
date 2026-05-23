#pragma once

#include "utils/traits.hpp"

#include <algorithm>
#include <cassert>
#include <exception>
#include <iterator>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

constexpr auto nullopt = std::nullopt;

template <typename T>
class optional
{
public:
    using value_type     = std::optional<T>::value_type;
    using iterator       = std::optional<T>::iterator;
    using const_iterator = std::optional<T>::const_iterator;

    explicit optional() noexcept
        : m_data(std::nullopt)
    {
    }

    optional([[maybe_unused]] std::nullopt_t _) noexcept
        : m_data(std::nullopt)
    {
    }

    explicit optional(T& elm) noexcept
        requires(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
        : m_data(std::move(elm))
    {
    }

    explicit optional(T&& elm) noexcept
        requires(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
        : m_data(std::move(elm))
    {
    }

    explicit optional(const T& elm) noexcept
        requires(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_destructible_v<T>)
        : m_data(elm)
    {
    }

    [[nodiscard]] static auto empty() noexcept -> optional { return optional{}; }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> optional
    {
        return optional{ std::forward<Args>(args)... }; //
    }

    template <typename... Args>
        requires(only_creatable<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> optional
    {
        return optional{ T::create(std::forward<Args>(args)...) }; //
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
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
        requires(only_creatable<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto emplace(Args&&... args) noexcept -> T&
    {
        reset();
        return m_data.emplace(T::create(std::forward<Args>(args)...));
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto force_emplace(Args&&... args) noexcept -> T&
    {
        try {
            reset();
            return m_data.emplace(std::forward<Args>(args)...);
        } catch (...) {
            std::terminate();
        }
    }

    void reset() noexcept { m_data.reset(); }

    [[nodiscard]] explicit operator bool() const noexcept { return m_data.has_value(); }

    [[nodiscard]] auto has_value() const noexcept -> bool { return m_data.has_value(); }

    [[nodiscard]] auto is_empty() const noexcept -> bool { return !m_data.has_value(); }

    [[nodiscard]] auto begin(this auto&& self) noexcept { return self.m_data.begin(); }

    [[nodiscard]] auto end(this auto&& self) noexcept { return self.m_data.end(); }

    [[nodiscard]]
    auto operator*() & noexcept -> T&
    {
        assert(has_value());
        return *m_data;
    }

    [[nodiscard]]
    auto operator*() const& noexcept -> T const&
    {
        assert(has_value());
        return *m_data;
    }

    [[nodiscard]]
    auto operator*() && noexcept -> T&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    [[nodiscard]]
    auto operator*() const&& noexcept -> T const&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    [[nodiscard]]
    auto operator->() noexcept -> T*
    {
        assert(has_value());
        return std::addressof(*m_data);
    }

    [[nodiscard]]
    auto operator->() const noexcept -> T const*
    {
        assert(has_value());
        return std::addressof(*m_data);
    }

    [[nodiscard]]
    auto value() & noexcept -> T&
    {
        assert(has_value());
        return *m_data;
    }

    [[nodiscard]]
    auto value() const& noexcept -> T const&
    {
        assert(has_value());
        return *m_data;
    }

    [[nodiscard]]
    auto value() && noexcept -> T&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    [[nodiscard]]
    auto value() const&& noexcept -> T const&&
    {
        assert(has_value());
        return std::move(*m_data);
    }

    [[nodiscard]]
    auto as_ref() & noexcept -> optional<T&>
    {
        if (is_empty()) { return std::nullopt; }
        return optional<T&>{ *m_data };
    }

    [[nodiscard]]
    auto as_ref() const& noexcept -> optional<T const&>
    {
        if (is_empty()) { return std::nullopt; }
        return optional<T const&>{ *m_data };
    }

    auto as_ref() &&      = delete;
    auto as_ref() const&& = delete;

    auto as_deref() & noexcept
    {
        if (is_empty()) { return optional<std::iter_reference_t<T>>{}; }
        return optional<std::iter_reference_t<T>>{ **m_data };
    }

    auto as_deref() const& noexcept
    {
        if (is_empty()) { return optional<std::iter_reference_t<const T>>{}; }
        return optional<std::iter_reference_t<const T>>{ **m_data };
    }

    auto as_deref() &&      = delete;
    auto as_deref() const&& = delete;

    auto eject() noexcept -> optional<T>
    {
        if (is_empty()) { return std::nullopt; }
        auto data = optional{ std::move(*m_data) };
        m_data.reset();
        return data;
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
optional(T&) -> optional<T>;

template <typename T>
optional(T&&) -> optional<T>;

template <typename T>
class optional<T&>
{
public:
    using value_type = std::optional<T&>::value_type;
    using iterator   = std::optional<T&>::iterator;

    explicit optional() noexcept
        : m_data(std::nullopt)
    {
    }

    optional([[maybe_unused]] std::nullopt_t _) noexcept
        : m_data(std::nullopt)
    {
    }

    explicit optional(T& data) noexcept
        : m_data(data)
    {
    }

    optional(T&& data) noexcept = delete;

    template <typename U>
        requires(std::is_nothrow_constructible_v<T&, U&>)
    explicit(!std::is_convertible_v<U&, T&>) optional(const optional<U&>& data) noexcept
        : m_data(data.value())
    {
    }

    auto emplace(T& src) noexcept -> T& { return m_data.emplace(src); }

    void reset() & noexcept { m_data.reset(); }

    [[nodiscard]] explicit operator bool() const noexcept { return m_data.has_value(); }

    [[nodiscard]] auto has_value() const noexcept -> bool { return m_data.has_value(); }

    [[nodiscard]] auto is_empty() const noexcept -> bool { return !m_data.has_value(); }

    [[nodiscard]] auto begin(this auto&& self) noexcept { return self.m_data.begin(); }

    [[nodiscard]] auto end(this auto&& self) noexcept { return self.m_data.end(); }

    [[nodiscard]]
    auto operator*() noexcept -> T&
    {
        assert(has_value());
        return *m_data;
    }

    [[nodiscard]]
    auto operator*() const noexcept -> T const&
    {
        assert(has_value());
        return *m_data;
    }

    [[nodiscard]]
    auto operator->() noexcept -> T*
    {
        assert(has_value());
        return m_data.operator->();
    }

    [[nodiscard]]
    auto operator->() const noexcept -> T const*
    {
        assert(has_value());
        return m_data.operator->();
    }

    [[nodiscard]]
    auto value(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (*self);
    }

    auto as_deref() noexcept
    {
        if (is_empty()) { return optional<std::iter_reference_t<T>>{}; }
        return optional<std::iter_reference_t<T>>{ **m_data };
    }

    auto as_deref() const noexcept
    {
        if (is_empty()) { return optional<std::iter_reference_t<const T>>{}; }
        return optional<std::iter_reference_t<const T>>{ **m_data };
    }

private:
    std::optional<T&> m_data;
};

namespace opt {
    template <typename T> [[nodiscard]]
    auto take(T& data) noexcept -> optional<T>
    {
        return optional<T>{ std::move(data) }; //
    }

    template <typename T> [[nodiscard]]
    auto as_ref(T& data) noexcept -> optional<T&>
    {
        return optional<T&>{ data }; //
    }
}