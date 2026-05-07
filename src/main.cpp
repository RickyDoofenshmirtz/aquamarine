#include "utils/optional.hpp"

namespace {
    void func() noexcept
    {
        [[maybe_unused]] const auto op = optional<int>::empty(); //
    }
}

int main()
{
    func();
    return 0;
}