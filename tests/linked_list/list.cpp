#include "list.hpp"
#include "utils/optional.hpp"
#include "utils/traits.hpp"

#include <cstdlib>

namespace {
    class coordinate
    {
    public:
        static auto create() noexcept -> coordinate { return { 0, 0 }; }

        static auto create(int x, int y) noexcept -> coordinate { return { x, y }; }

        auto x() const noexcept -> int { return m_x; }
        auto y() const noexcept -> int { return m_y; }

        auto operator==(const coordinate& src) const noexcept -> bool
        {
            return m_x == src.m_x && m_y == src.m_y; //
        }

    private:
        coordinate(int x, int y) noexcept
            : m_x(x), m_y(y)
        {
        }

        int m_x;
        int m_y;
    };

    static_assert(creatable<coordinate, int, int>);

    template <typename T>
    auto correct_last(const optional<T>& val, const T& expected_val) noexcept -> bool
    {
        if (!val) { return false; }
        if (val.value() != expected_val) { return false; }
        return true;
    }

    auto func() noexcept -> bool
    {
        {
            auto l = linked_list<int>::create();
            l.emplace_back(1);
            l.emplace_back(2);
            l.emplace_back(3);
            if (!correct_last(l.pop_back(), 3)) { return false; }
            if (!correct_last(l.pop_back(), 2)) { return false; }
            if (!correct_last(l.pop_back(), 1)) { return false; }
            if (l.pop_back().has_value()) { return false; }
        }
        {
            auto l = linked_list<coordinate>::create();
            l.emplace_back(1, 2);
            l.emplace_back(5, 6);
            l.emplace_back(8, 9);
            if (!correct_last(l.pop_back(), coordinate::create(8, 9))) { return false; }
            if (!correct_last(l.pop_back(), coordinate::create(5, 6))) { return false; }
            if (!correct_last(l.pop_back(), coordinate::create(1, 2))) { return false; }
            if (l.pop_back().has_value()) { return false; }
        }
        return true;
    }
} // namespace

int main()
{
    if (!func()) { return EXIT_FAILURE; }
    return EXIT_SUCCESS;
}