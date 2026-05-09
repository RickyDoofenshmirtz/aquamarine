#include "memory/unique_handle.hpp"

#include <cstdint>
#include <print>

namespace {
    struct coordinate
    {
        std::int64_t x;
        std::int64_t y;

        static auto create(std::int64_t x, std::int64_t y) noexcept -> coordinate
        { return coordinate{ .x = x, .y = y }; }
    };

    void func() noexcept
    {
        using handle_type     = unique_handle<coordinate>;
        using opt_handle_type = optional<unique_handle<coordinate>>;

        [[maybe_unused]] static constexpr auto s1 = sizeof(handle_type);
        [[maybe_unused]] static constexpr auto s2 = sizeof(opt_handle_type);

        auto c1 = opt_handle_type::create(5, 9);
        if (c1) {
            auto [x, y] = c1.deref();
            std::println("{}, {}", x, y);
        }
    }
}

int main()
{
    func();
    return 0;
}