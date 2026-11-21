#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"

#include <cstddef>
#include <iostream>
#include <ostream>
#include <utility>

namespace {
    using T = int;

    class dll_node
    {
    public:
        explicit dll_node(T data) noexcept
            : m_data(data)
        {
        }

        auto has_next() const noexcept -> bool { return m_next.has_value(); }
        auto has_prev() const noexcept -> bool { return m_prev.has_value(); }

        auto data() noexcept -> T& { return m_data; }
        auto data() const noexcept -> T const& { return m_data; }

        auto next() noexcept -> optional<dll_node&> { return m_next.as_deref(); }
        auto next() const noexcept -> optional<dll_node const&> { return m_next.as_deref(); }

        auto prev() noexcept -> optional<dll_node&> { return m_prev; }
        auto prev() const noexcept -> optional<dll_node const&> { return m_prev; }

        auto make_next(T data) noexcept -> dll_node& { return m_next.emplace(data); }

        auto set_next(optional<unique_handle<dll_node>> next) noexcept -> optional<dll_node&>
        {
            m_next = std::move(next);
            return m_next.as_deref();
        }

        auto set_prev(optional<dll_node&> prev) noexcept -> optional<dll_node&>
        { return (m_prev = prev); }

        auto eject_next() noexcept -> optional<dll_node> { return m_next.eject_deref(); }

        void reset_prev() noexcept { m_prev.reset(); }

    private:
        T m_data;
        optional<unique_handle<dll_node>> m_next;
        optional<dll_node&> m_prev;
    };

    class doubly_linked_list
    {
    public:
        static auto create() noexcept -> doubly_linked_list { return {}; }

        auto is_empty() const noexcept -> bool { return m_head.is_empty(); }

        auto size() const noexcept -> std::size_t
        {
            auto curr = m_head.as_deref();
            std::size_t size{};
            while (curr) {
                ++size;
                curr = curr->next();
            }
            return size;
        }

        auto push_back(T data) noexcept -> T&
        {
            if (m_head.is_empty()) { return m_tail.emplace(m_head.emplace(data)).data(); }
            auto curr = m_head.as_deref();
            while (curr->has_next()) { curr = curr->next(); }
            curr->make_next(data).data();
            return (curr->next()->prev() = curr)->data();
        }

        auto push_front(T data) noexcept -> T&
        {
            if (m_head.is_empty()) { return m_tail.emplace(m_head.emplace(data)).data(); }
            auto first_dll_node  = optional<unique_handle<dll_node>>::create(data);
            auto first_ref       = first_dll_node.as_deref();
            auto second_dll_node = first_ref->set_next(std::move(m_head));
            second_dll_node->set_prev(first_ref);
            m_head = std::move(first_dll_node);
            return m_head.deref().data();
        }

        friend auto operator<<(std::ostream& stream, const doubly_linked_list& elm) -> std::ostream&
        {
            auto curr = elm.m_head.as_deref();
            while (curr) {
                stream << curr->data() << ' ';
                curr = curr->next();
            }
            return stream;
        }

    private:
        doubly_linked_list() noexcept = default;

        optional<unique_handle<dll_node>> m_head;
        optional<dll_node&> m_tail;
    };

    void func() noexcept
    {
        auto list = doubly_linked_list::create();
        list.push_front(1);
        list.push_back(2);
        list.push_front(3);
        list.push_front(7);
        list.push_front(9);
        list.push_back(4);

        std::cout << list << '\n';
        std::cout << list.size() << '\n';
    }

} // namespace

int main()
{
    func();
    return 0;
}