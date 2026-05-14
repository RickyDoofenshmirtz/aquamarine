#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"

#include <optional>
#include <print>

namespace {
    class entity
    {
    public:
        static auto create(int x) noexcept -> entity { return entity{ x }; }

        auto value() noexcept -> int& { return m_x; }
        auto value() const noexcept -> int const& { return m_x; }

    private:
        explicit entity(int x) noexcept
            : m_x(x)
        {
        }

        int m_x;
    };

    [[maybe_unused]]
    auto get_ent(int x) noexcept -> optional<entity>
    {
        if (x == 0) { return std::nullopt; }
        auto op_en = optional<entity>{};
        op_en.emplace(x);
        auto res = optional<entity>::create(x);
        return res;
    }

    auto get_data(int x) noexcept -> optional<unique_handle<int>>
    {
        if (x == 0) { return std::nullopt; }
        auto data = unique_handle<int>::create(x);
        return optional{ data };
        return optional<unique_handle<int>>::create(x);
        return as_opt(data);
    }

    [[maybe_unused]]
    void func() noexcept
    {
        const auto op_data = get_data(5);
        if (!op_data) { return; }
        auto& data = *op_data;
        auto& val  = *data;
        std::println("{}", val);
        auto en     = get_ent(val);
        auto ref_en = en.as_ref();
        if (en) { std::println("en = {}", ref_en->value()); }
    }

    void print_value(optional<const unique_handle<int>&> op_hand) noexcept
    {
        if (op_hand) { std::println("{}", op_hand->value()); }
    }

    [[maybe_unused]]
    void junk() noexcept
    {
        const auto op_data = get_data(59);
        auto ref_data      = op_data.as_ref();
        print_value(ref_data);
    }
} // namespace

int main()
{
    func();
    junk();
    return 0;
}