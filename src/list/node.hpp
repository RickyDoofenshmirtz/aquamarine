#pragma once

#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"

#include <print>
#include <utility>

using T = int;

class node
{
public:
    explicit node(T data) noexcept
        : m_data(data)
    {
    }

    auto data() & noexcept -> T& { return m_data; }
    auto data() const& noexcept -> T const& { return m_data; }

    auto data() && noexcept -> T&& { return std::move(m_data); }
    auto data() const&& noexcept -> T const&& { return std::move(m_data); }

    auto next() & noexcept -> optional<node&> { return m_next.as_deref(); }
    auto next() const& noexcept -> optional<node const&> { return m_next.as_deref(); }

    auto next() && noexcept -> optional<node> { return m_next.eject_deref(); }
    auto next() const&& noexcept = delete;

    auto has_next() const noexcept -> bool { return m_next.has_value(); }

    auto is_last() const noexcept -> bool { return !has_next(); }

    auto make_next(T next_data) noexcept -> T& { return m_next.emplace(next_data).data(); }

    void reset_next() noexcept { m_next.reset(); }

    auto detach_next() noexcept -> optional<T>
    {
        return optional{ m_next.eject_deref().value().data() }; //
    }

private:
    T m_data;
    optional<unique_handle<node>> m_next;
};

class linked_list
{
public:
    linked_list() noexcept = default;

    static auto create() noexcept -> linked_list { return linked_list{}; }

    auto is_empty() const noexcept -> bool { return m_root.is_empty(); }

    auto push_back(T data) noexcept -> T&
    {
        if (is_empty()) { return m_root.emplace(data).data(); }
        auto curr = m_root.as_deref();
        while (curr->next()) { curr = curr->next(); }
        return curr->make_next(data);
    }

    void pop_back() noexcept
    {
        if (is_empty()) { return; }
        auto curr = m_root.as_deref();
        if (curr->is_last()) {
            m_root.reset();
            return;
        }
        while (curr->next()->next()) { curr = curr->next(); }
        curr->reset_next();
    }

    auto eject_back() noexcept -> optional<T>
    {
        if (is_empty()) { return nullopt; }
        auto curr = m_root.as_deref();
        if (curr->is_last()) {
            auto root = m_root.eject_deref().value();
            return optional<T>{ root.data() };
        }
        while (curr->next()->next()) { curr = curr->next(); }
        return curr->detach_next();
    }

    void print() const noexcept
    {
        if (!m_root) { return; }
        std::print("[");
        auto curr = m_root.as_deref();
        while (curr->next()) {
            std::print("{}, ", curr->data());
            curr = curr->next();
        }
        std::println("{}]", curr->data());
    }

private:
    optional<unique_handle<node>> m_root;
};