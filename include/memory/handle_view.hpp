#pragma once

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

template <typename T>
class handle_view
{
public:
    using value_type = T;

    explicit constexpr handle_view(value_type& data_ptr) noexcept
        : m_data_ptr(std::addressof(data_ptr))
    {
    }

    explicit constexpr handle_view(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

    template <typename U>
        requires(std::is_convertible_v<U*, T*>)
    constexpr handle_view(const handle_view<U>& src) noexcept
        : m_data_ptr(src.ptr())
    {
    }

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool { return m_data_ptr == nullptr; }

    [[nodiscard]] constexpr auto has_value() const noexcept -> bool { return !is_empty(); }

    [[nodiscard]] explicit constexpr operator bool() const noexcept { return has_value(); }

    [[nodiscard]] constexpr auto ptr() noexcept -> value_type* { return m_data_ptr; }
    [[nodiscard]] constexpr auto ptr() const noexcept -> const value_type* { return m_data_ptr; }

    [[nodiscard]]
    constexpr auto operator*(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (*self.ptr());
    }

    [[nodiscard]]
    constexpr auto operator->(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return self.ptr();
    }

    constexpr auto reset() noexcept -> value_type* { return reset_to(nullptr); }

    constexpr auto reset_to(value_type* data_ptr) noexcept -> value_type*
    { return std::exchange(m_data_ptr, data_ptr); }

private:
    value_type* m_data_ptr{};
};

template <typename T>
handle_view(T&) -> handle_view<T>;

template <typename T>
handle_view(T*) -> handle_view<T>;