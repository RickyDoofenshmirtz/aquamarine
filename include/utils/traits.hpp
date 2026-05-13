#pragma once

#include <concepts>
#include <type_traits>

template <typename T, typename... Args>
concept creatable = std::is_nothrow_move_constructible_v<T> && requires {
    { T::create(std::declval<Args>()...) } noexcept -> std::same_as<T>;
};

template <typename T, typename... Args>
concept only_creatable = creatable<T, Args...> && !std::is_nothrow_constructible_v<T, Args...>;