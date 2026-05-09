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
        using opt_handle_type = optional<handle_type>;
        static_assert(sizeof(handle_type) == sizeof(opt_handle_type));

        auto c1 = opt_handle_type::create();
        c1.emplace(5, 6);
        if (c1) {
            auto [x, y] = c1.deref();
            std::println("{}, {}", x, y);
        }
        auto c_ref       = c1.as_ref();
        c_ref->value().x = 100;
        c_ref->value().y = 200;
        auto [x, y]      = c1.deref();
        std::println("{}, {}", x, y);
    }
}

int main()
{
    func();
    return 0;
}