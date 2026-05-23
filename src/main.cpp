#include "list/node.hpp"
#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"

#include <exception>
#include <memory>
#include <new>
#include <print>
#include <utility>

namespace {
    struct entity
    {
        using value_type = int;

        explicit entity(int x) noexcept { set(x); }

        entity(const entity&)                    = delete;
        auto operator=(const entity&) -> entity& = delete;

        entity(entity&& src) noexcept
            : m_ptr(std::exchange(src.m_ptr, nullptr))
        {
        }

        auto operator=(entity&& src) noexcept -> entity&
        {
            if (this != std::addressof(src)) {
                delete m_ptr;
                m_ptr = std::exchange(src.m_ptr, nullptr);
            }
            return *this;
        }

        ~entity()
        {
            if (m_ptr == nullptr) { return; }
            std::println("deleting entity");
            delete m_ptr;
            m_ptr = nullptr;
        }

        void set(int value) noexcept
        {
            m_ptr = new(std::nothrow) int(value);
            if (m_ptr == nullptr) [[unlikely]] { std::terminate(); }
        }

        auto ptr() noexcept -> int* { return m_ptr; }
        auto ptr() const noexcept -> int const* { return m_ptr; }

        auto get(this auto&& self) noexcept -> decltype(auto) { return (*self.ptr()); }

        auto operator*() noexcept -> int& { return *m_ptr; }
        auto operator*() const noexcept -> int const& { return *m_ptr; }

        auto operator->() noexcept -> int* { return m_ptr; }
        auto operator->() const noexcept -> int const* { return m_ptr; }

    private:
        int* m_ptr = nullptr;
    };

    [[maybe_unused]]
    auto get_data() noexcept -> optional<unique_handle<entity>>
    {
        auto elm = unique_handle<entity>::create(4);
        return optional{ elm };
    }

    void func() noexcept
    {
        auto l = linked_list::create();
        l.push_back(1);
        l.push_back(2);
        l.push_back(3);
        l.print();
        l.pop_back();
        l.print();
        l.pop_back();
        l.print();
        l.pop_back();
        l.print();
        l.pop_back();
        l.print();

        auto elm = get_data().eject_deref();
    }
} // namespace

int main()
{
    func();
    return 0;
}