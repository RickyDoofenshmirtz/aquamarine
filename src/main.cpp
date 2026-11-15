#include "linked_list/list.hpp"
#include "utils/traits.hpp"

#include <print>

namespace {
    class coordinate
    {
    public:
        static auto create() noexcept -> coordinate { return { 0, 0 }; }

        static auto create(int x, int y) noexcept -> coordinate { return { x, y }; }

        auto x() const noexcept -> int { return m_x; }
        auto y() const noexcept -> int { return m_y; }

    private:
        coordinate(int x, int y) noexcept
            : m_x(x), m_y(y)
        {
        }

        int m_x;
        int m_y;
    };

    static_assert(creatable<coordinate, int, int>);

    void func() noexcept
    {
        {
            auto l = linked_list<int>::create();
            l.emplace_back(1);
            l.emplace_back(2);
            l.emplace_back(3);
            l.print();
            if (auto last = l.pop_back()) { std::println("last = {}", *last); }
            l.print();
            if (auto last = l.pop_back()) { std::println("last = {}", *last); }
            l.print();
            if (auto last = l.pop_back()) { std::println("last = {}", *last); }
            l.print();
            if (auto last = l.pop_back()) { std::println("last = {}", *last); }
            l.print();
        }
        {
            auto l = linked_list<coordinate>::create();
            l.emplace_back(1, 2);
            l.emplace_back(5, 6);
            l.emplace_back(8, 9);
            while (auto last = l.pop_back()) {
                const auto c = *last;
                std::println("x = {}, y = {}", c.x(), c.y());
            }
        }
    }
} // namespace

int main()
{
    func();
    return 0;
}