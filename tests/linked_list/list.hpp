#pragma once

#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"
#include "utils/traits.hpp"

#include <print>
#include <type_traits>
#include <utility>

template <typename T>
class node
{
public:
    using value_type = T;

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    explicit node(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : m_data(std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> node
    {
        return node{ std::forward<Args>(args)... }; //
    }

    template <typename... Args>
        requires(only_creatable<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]]
    static auto create(Args&&... args) noexcept -> node
    {
        return node{ T::create(std::forward<Args>(args)...) }; //
    }

    [[nodiscard]] auto data() & noexcept -> T& { return m_data; }
    [[nodiscard]] auto data() const& noexcept -> T const& { return m_data; }

    [[nodiscard]] auto data() && noexcept -> T&& { return std::move(m_data); }
    [[nodiscard]] auto data() const&& noexcept -> T const&& { return std::move(m_data); }

    [[nodiscard]] auto has_next() const noexcept -> bool { return m_next.has_value(); }
    [[nodiscard]] auto is_last() const noexcept -> bool { return !m_next.has_value(); }

    [[nodiscard]] auto next() & noexcept -> optional<node&> { return m_next.as_deref(); }
    [[nodiscard]] auto next() const& noexcept -> optional<node const&> { return m_next.as_deref(); }

    auto next() &&      = delete;
    auto next() const&& = delete;

    template <typename... Args>
    auto emplace_next(Args&&... args) noexcept -> node&
    {
        return m_next.emplace(std::forward<Args>(args)...); //
    }

    auto eject_next() noexcept -> optional<node> { return m_next.eject_deref(); }

    auto eject_next_data() noexcept -> optional<T>
    {
        if (m_next.is_empty()) { return optional<T>{}; }
        return optional{ m_next.eject_deref().value().data() };
    }

private:
    T m_data;
    optional<unique_handle<node>> m_next;
};

template <typename T>
class linked_list
{
public:
    using value_type = T;
    using node_type  = node<T>;

    static auto create() noexcept -> linked_list { return {}; }

    auto is_empty() noexcept -> bool { return m_root.is_empty(); }

    template <typename... Args>
    auto emplace_back(Args&&... args) noexcept -> T&
    {
        if (m_root.is_empty()) { return m_root.emplace(std::forward<Args>(args)...).data(); }
        optional<node_type&> curr = m_root.as_deref();
        while (curr->has_next()) { curr = curr->next(); }
        return curr->emplace_next(std::forward<Args>(args)...).data();
    }

    auto pop_back() noexcept -> optional<T>
    {
        if (m_root.is_empty()) { return optional<T>{}; }
        optional<node_type&> curr = m_root.as_deref();
        if (curr->is_last()) { return optional{ m_root.eject_deref().value().data() }; }
        while (curr->has_next() && curr->next()->has_next()) { curr = curr->next(); }
        return curr->eject_next_data();
    }

    void print() const noexcept
    {
        if (m_root.is_empty()) {
            std::println("Empty list");
            return;
        }
        optional<node_type const&> curr = m_root.as_deref();
        while (curr->has_next()) {
            std::print("{} ", curr->data());
            curr = curr->next();
        }
        std::println("{}", curr->data());
    }

private:
    optional<unique_handle<node<T>>> m_root;
};
