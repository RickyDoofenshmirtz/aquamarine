#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

template <typename T, typename... Args>
concept creatable = std::is_nothrow_move_constructible_v<T> && requires {
    { T::create(std::declval<Args>()...) } noexcept -> std::same_as<T>;
};

template <typename T, typename... Args>
concept only_creatable = creatable<T, Args...> && !std::is_nothrow_constructible_v<T, Args...>;

template <typename T>
concept dereferenceable = requires(T elm) {
    { *elm } noexcept -> std::same_as<std::iter_reference_t<T>>;
    { elm.operator->() } noexcept -> std::same_as<std::add_pointer_t<std::iter_value_t<T>>>;
};