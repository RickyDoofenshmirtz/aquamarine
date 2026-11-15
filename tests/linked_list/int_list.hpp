#pragma once

#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"

#include <iostream>

using T = int;

class node
{
public:
    [[nodiscard]] static auto create(T data) noexcept -> node { return node{ data }; }

    [[nodiscard]] auto data() noexcept -> T& { return m_data; }
    [[nodiscard]] auto data() const noexcept -> T const& { return m_data; }

    [[nodiscard]] auto has_next() const noexcept -> bool { return m_next.has_value(); }
    [[nodiscard]] auto is_last() const noexcept -> bool { return !m_next.has_value(); }

    [[nodiscard]] auto next() & noexcept -> optional<node&> { return m_next.as_deref(); }
    [[nodiscard]] auto next() const& noexcept -> optional<node const&> { return m_next.as_deref(); }

    auto next() &&      = delete;
    auto next() const&& = delete;

    auto make_next(T data) noexcept -> node& { return m_next.emplace(data); }

    auto eject_next() noexcept -> optional<node> { return m_next.eject_deref(); }

    auto eject_next_data() noexcept -> optional<T>
    {
        if (m_next.is_empty()) { return optional<T>{}; }
        return optional{ m_next.eject_deref().value().data() };
    }

private:
    explicit node(T data) noexcept
        : m_data(data)
    {
    }

    T m_data;
    optional<unique_handle<node>> m_next;
};

class linked_list
{
public:
    static auto create() noexcept -> linked_list { return {}; }

    auto push_back(T data) noexcept -> T&
    {
        if (m_root.is_empty()) { return m_root.emplace(data).data(); }
        optional<node&> curr = m_root.as_deref();
        while (curr->has_next()) { curr = curr->next(); }
        return curr->make_next(data).data();
    }

    auto pop_back() noexcept -> optional<T>
    {
        if (m_root.is_empty()) { return optional<T>::empty(); }
        optional<node&> curr = m_root.as_deref();
        if (curr->is_last()) { return optional{ m_root->eject().data() }; }
        while (curr->has_next() && curr->next()->has_next()) { curr = curr->next(); }
        return curr->eject_next_data();
    }

    void print() const noexcept
    {
        if (!m_root) { return; }
        optional<node const&> curr = m_root.as_deref();
        while (curr->has_next()) {
            std::cout << curr->data() << ' ';
            curr = curr->next();
        }
        std::cout << curr->data() << '\n';
    }

private:
    optional<unique_handle<node>> m_root;
};