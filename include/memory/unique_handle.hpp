#pragma once

#include "handle_view.hpp"
#include "utils/optional.hpp"
#include "utils/traits.hpp"

#include <algorithm>
#include <cassert>
#include <exception>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T>
    requires(std::is_same_v<T, std::remove_cvref_t<T>>)
class unique_handle
{
    friend class optional<unique_handle<T>>;

public:
    using value_type = T;

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> unique_handle
    {
        void* ptr = ::operator new(sizeof(T), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        T* data_ptr = new(ptr) T(std::forward<Args>(args)...);
        return unique_handle{ data_ptr };
    }

    template <typename... Args>
        requires(only_creatable<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> unique_handle
    {
        void* ptr = ::operator new(sizeof(T), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        T* data_ptr = new(ptr) T(T::create(std::forward<Args>(args)...));
        return unique_handle{ data_ptr };
    }

    [[nodiscard]]
    static auto default_create() noexcept -> unique_handle
        requires(std::is_nothrow_default_constructible_v<T> && std::is_nothrow_destructible_v<T>)
    { return create(); }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto try_create(Args&&... args) noexcept -> optional<unique_handle>
    {
        void* ptr = ::operator new(sizeof(T), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { return std::nullopt; }
        try {
            T* data_ptr = new(ptr) T(std::forward<Args>(args)...);
            return optional{ unique_handle{ data_ptr } };
        } catch (...) {
            ::operator delete(ptr);
            return std::nullopt;
        }
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto force_create(Args&&... args) noexcept -> unique_handle
    {
        void* ptr{};
        try {
            ptr         = ::operator new(sizeof(T));
            T* data_ptr = new(ptr) T(std::forward<Args>(args)...);
            return unique_handle{ data_ptr };
        } catch (...) {
            ::operator delete(ptr);
            std::terminate();
        }
    }

    [[nodiscard]]
    static auto from_raw(T*& data_ptr) noexcept -> optional<unique_handle>
        requires(std::is_nothrow_destructible_v<T>)
    {
        if (data_ptr == nullptr) { return std::nullopt; }
        return optional{ unique_handle{ std::exchange(data_ptr, nullptr) } };
    }

    [[nodiscard]] auto ptr() noexcept -> T* { return m_data_ptr; }
    [[nodiscard]] auto ptr() const noexcept -> T const* { return m_data_ptr; }

    [[nodiscard]]
    auto valueless_after_move() const noexcept -> bool
    { return m_data_ptr == nullptr; }

    auto operator*() noexcept -> T&
    {
        assert(!valueless_after_move());
        return *m_data_ptr;
    }

    auto operator*() const noexcept -> T const&
    {
        assert(!valueless_after_move());
        return *m_data_ptr;
    }

    [[nodiscard]]
    auto operator->() noexcept -> T*
    {
        assert(!valueless_after_move());
        return m_data_ptr;
    }

    [[nodiscard]]
    auto operator->() const noexcept -> T const*
    {
        assert(!valueless_after_move());
        return m_data_ptr;
    }

    [[nodiscard]] auto value(this auto&& self) noexcept -> decltype(auto) { return (*self); }

    [[nodiscard]] auto view() noexcept -> handle_view<T> { return handle_view{ m_data_ptr }; }
    [[nodiscard]] auto view() const noexcept -> handle_view<const T>
    { return handle_view{ m_data_ptr }; }

private:
    explicit unique_handle(T* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

public:
    unique_handle(const unique_handle&)                    = delete;
    auto operator=(const unique_handle&) -> unique_handle& = delete;

    unique_handle(unique_handle&& src) noexcept
        : m_data_ptr(std::exchange(src.m_data_ptr, nullptr))
    {
    }

    auto operator=(unique_handle&& src) noexcept -> unique_handle&
    {
        if (this == std::addressof(src)) { return *this; }
        if (m_data_ptr != nullptr) //
        {
            std::destroy_at(m_data_ptr);
            ::operator delete(m_data_ptr);
        }
        m_data_ptr = std::exchange(src.m_data_ptr, nullptr);
        return *this;
    }

    ~unique_handle() noexcept
    {
        if (m_data_ptr == nullptr) { return; }
        std::destroy_at(m_data_ptr);
        ::operator delete(m_data_ptr);
    }

private:
    T* m_data_ptr;
};

template <typename T>
class optional<unique_handle<T>>
{
public:
    using value_type     = unique_handle<T>;
    using iterator       = unique_handle<T>*;
    using const_iterator = const unique_handle<T>*;

    explicit optional() noexcept
        : m_data(nullptr)
    {
    }

    optional([[maybe_unused]] std::nullopt_t _) noexcept
        : m_data(nullptr)
    {
    }

    explicit optional(unique_handle<T>& data) noexcept
        : m_data(std::move(data))
    {
    }

    explicit optional(unique_handle<T>&& data) noexcept
        : m_data(std::move(data))
    {
    }

    [[nodiscard]] static auto empty() noexcept -> optional { return optional{}; }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> optional
    {
        return force_create(std::forward<Args>(args)...); //
    }

    template <typename... Args>
        requires(only_creatable<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> optional
    {
        void* ptr = ::operator new(sizeof(T), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        try {
            T* data_ptr = new(ptr) T(T::create(std::forward<Args>(args)...));
            return optional{ data_ptr };
        } catch (...) {
            ::operator delete(ptr);
            std::terminate();
        }
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto force_create(Args&&... args) noexcept -> optional
    {
        void* ptr = ::operator new(sizeof(T), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        try {
            T* data_ptr = new(ptr) T(std::forward<Args>(args)...);
            return optional{ data_ptr };
        } catch (...) {
            ::operator delete(ptr);
            std::terminate();
        }
    }

    [[nodiscard]] explicit operator bool() const noexcept { return m_data.m_data_ptr != nullptr; }

    [[nodiscard]] auto has_value() const noexcept -> bool { return m_data.m_data_ptr != nullptr; }

    [[nodiscard]] auto is_empty() const noexcept -> bool { return m_data.m_data_ptr == nullptr; }

    [[nodiscard]] auto begin() noexcept -> iterator
    { return (has_value()) ? std::addressof(m_data) : nullptr; }

    [[nodiscard]] auto begin() const noexcept -> const_iterator
    { return (has_value()) ? std::addressof(m_data) : nullptr; }

    [[nodiscard]] auto end() noexcept -> iterator { return begin() + has_value(); }
    [[nodiscard]] auto end() const noexcept -> const_iterator { return begin() + has_value(); }

    [[nodiscard]]
    auto operator*() & noexcept -> unique_handle<T>&
    {
        assert(has_value());
        return m_data;
    }

    [[nodiscard]]
    auto operator*() const& noexcept -> unique_handle<T> const&
    {
        assert(has_value());
        return m_data;
    }

    [[nodiscard]]
    auto operator*() && noexcept -> unique_handle<T>&&
    {
        assert(has_value());
        return std::move(m_data);
    }

    [[nodiscard]]
    auto operator*() const&& noexcept -> unique_handle<T> const&&
    {
        assert(has_value());
        return std::move(m_data);
    }

    [[nodiscard]]
    auto operator->() noexcept -> T*
    {
        assert(has_value());
        return std::addressof(m_data);
    }

    [[nodiscard]]
    auto operator->() const noexcept -> T const*
    {
        assert(has_value());
        return std::addressof(m_data);
    }

    [[nodiscard]]
    auto value() & noexcept -> unique_handle<T>&
    {
        assert(has_value());
        return m_data;
    }

    [[nodiscard]]
    auto value() const& noexcept -> unique_handle<T> const&
    {
        assert(has_value());
        return m_data;
    }

    [[nodiscard]]
    auto value() && noexcept -> unique_handle<T>&&
    {
        assert(has_value());
        return std::move(m_data);
    }

    [[nodiscard]]
    auto value() const&& noexcept -> unique_handle<T> const&&
    {
        assert(has_value());
        return std::move(m_data);
    }

    [[nodiscard]]
    auto deref(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self.has_value());
        return (*self.m_data);
    }

    void reset() noexcept
    {
        if (is_empty()) { return; }
        std::destroy_at(m_data.m_data_ptr);
        ::operator delete(m_data.m_data_ptr);
        m_data.m_data_ptr = nullptr;
    }

    auto as_ref() noexcept -> optional<unique_handle<T>&>
    {
        if (is_empty()) { return std::nullopt; }
        return optional<unique_handle<T>&>{ m_data };
    }

    auto as_ref() const noexcept -> optional<const unique_handle<T>&>
    {
        if (is_empty()) { return std::nullopt; }
        return optional<const unique_handle<T>&>{ m_data };
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto emplace(this auto& self, Args&&... args) noexcept -> T&
    {
        auto& data_ptr = self.m_data.m_data_ptr;
        if (self.is_empty()) {
            data_ptr = static_cast<T*>(::operator new(sizeof(T), std::nothrow));
            if (data_ptr == nullptr) [[unlikely]] { std::terminate(); }
        } else {
            std::destroy_at(data_ptr);
        }
        std::construct_at(data_ptr, std::forward<Args>(args)...);
        return self.deref();
    }

    template <typename... Args>
        requires(only_creatable<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto emplace(this auto& self, Args&&... args) noexcept -> T&
    {
        auto& data_ptr = self.m_data.m_data_ptr;
        if (self.is_empty()) {
            data_ptr = static_cast<T*>(::operator new(sizeof(T), std::nothrow));
            if (data_ptr == nullptr) [[unlikely]] { std::terminate(); }
        } else {
            std::destroy_at(data_ptr);
        }
        std::construct_at(data_ptr, T::create(std::forward<Args>(args)...));
        return self.deref();
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto force_emplace(this auto& self, Args&&... args) noexcept -> T&
    {
        auto& data_ptr = self.m_data.m_data_ptr;
        if (self.is_empty()) {
            data_ptr = static_cast<T*>(::operator new(sizeof(T), std::nothrow));
            if (data_ptr == nullptr) [[unlikely]] { std::terminate(); }
        } else {
            std::destroy_at(data_ptr);
        }
        try {
            std::construct_at(data_ptr, std::forward<Args>(args)...);
        } catch (...) {
            ::operator delete(data_ptr);
            std::terminate();
        }
        return self.deref();
    }

private:
    explicit optional(T* data_ptr) noexcept
        : m_data(data_ptr)
    {
    }

    unique_handle<T> m_data;
};

template <typename T>
optional(unique_handle<T>&) -> optional<unique_handle<T>>;

template <typename T>
optional(unique_handle<T>&&) -> optional<unique_handle<T>>;