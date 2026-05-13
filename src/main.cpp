#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"

#include <optional>
#include <print>

namespace {

    [[maybe_unused]]
    auto get_val(int x) noexcept -> optional<int>
    {
        if (x == 0) { return std::nullopt; }
        return optional{ x };
    }

    auto get_data(int x) noexcept -> optional<unique_handle<int>>
    {
        if (x == 0) { return std::nullopt; }
        auto data = unique_handle<int>::create(x);
        return optional{ data };
        return optional<unique_handle<int>>::create(x);
    }

    void func() noexcept
    {
        auto op_data = get_data(5);
        for (auto& data : op_data) {
            auto val = *data;
            std::println("{}", val);
        }
    }
}

int main()
{
    func();
    return 0;
}