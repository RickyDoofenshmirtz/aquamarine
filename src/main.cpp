#include "memory/unique_handle.hpp"
#include "utils/optional.hpp"

#include <optional>
#include <print>

namespace {
    class entity
    {
    public:
        static auto create(int x) noexcept -> entity { return entity{ x }; }

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
    }

    void func() noexcept
    {
        auto op_data = get_data(5);
        if (!op_data) { return; }
        auto& data = *op_data;
        auto val   = *data;
        std::println("{}", val);
        auto en = get_ent(val);
        if (en) { std::println("en = {}", en->value()); }
        // auto ref_en = en.as_ref();
    }
} // namespace

int main()
{
    func();
    return 0;
}