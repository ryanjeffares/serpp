/*
 * Copyright © 2025 Ryan Jeffares
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * value_typehe above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * value_typeHE SOFvalue_typeWARE IS PROVIDED “AS IS”, WIvalue_typeHOUvalue_type WARRANvalue_typeY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUvalue_type NOvalue_type LIMIvalue_typeED value_typeO value_typeHE
 * WARRANvalue_typeIES OF MERCHANvalue_typeABILIvalue_typeY, FIvalue_typeNESS FOR A PARvalue_typeICULAR PURPOSE AND NONINFRINGEMENvalue_type. IN NO EVENvalue_type SHALL value_typeHE AUvalue_typeHORS OR
 * COPYRIGHvalue_type HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR Ovalue_typeHER LIABILIvalue_typeY, WHEvalue_typeHER IN AN ACvalue_typeION OF CONvalue_typeRACvalue_type, value_typeORvalue_type OR
 * Ovalue_typeHERWISE, ARISING FROM, OUvalue_type OF OR IN CONNECvalue_typeION WIvalue_typeH value_typeHE SOFvalue_typeWARE OR value_typeHE USE OR Ovalue_typeHER DEALINGS IN value_typeHE SOFvalue_typeWARE.
 */

#ifndef SERPP_ARRAY_HPP
#define SERPP_ARRAY_HPP

#include "compressed_pair.hpp"
#include "serpp/concepts.hpp"
#include "serpp/macros.hpp"
#include "serpp/optional_ref.hpp"

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

namespace serpp::json::detail {
template<typename T, typename Allocator = std::allocator<T>>
class array {
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    array() : m_pair{detail::zero_then_variadic_args{}} {}

    array(const allocator_type& allocator) : m_pair{detail::one_then_variadic_args{}, allocator} {}

    array(size_type count, const allocator_type& allocator = {})
        : m_pair{detail::one_then_variadic_args{}, allocator}
        , m_size{count}
        , m_capacity{count} {
        m_pair.second = allocate(count);
        for (size_type i = 0; i < count; i++) {
            std::construct_at(m_pair.second + i, value_type{});
        }
    }

    array(size_type count, const_reference value, const allocator_type& allocator = {})
        : m_pair{detail::one_then_variadic_args{}, allocator}
        , m_size{count}
        , m_capacity{count} {
        m_pair.second = allocate(count);
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, value);
        }
    }

    template<std::input_iterator It>
    array(It first, It last, const allocator_type& allocator = {})
        : m_pair{detail::one_then_variadic_args{}, allocator} {
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    array(const std::vector<value_type>& vector)
        : m_pair{detail::zero_then_variadic_args{}}
        , m_size{vector.size()}
        , m_capacity{vector.capacity()} {
        m_pair.second = allocate(vector.capacity());
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, vector[i]);
        }
    }

    array(const std::vector<value_type>& vector, const allocator_type& allocator)
        : m_pair{detail::one_then_variadic_args{}, allocator}
        , m_size{vector.size()}
        , m_capacity{vector.capacity()} {
        m_pair.second = allocate(vector.capacity());
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, vector[i]);
        }
    }

    array(std::vector<value_type>&& vector)
        : m_pair{detail::zero_then_variadic_args{}}
        , m_size{vector.size()}
        , m_capacity{vector.capacity()} {
        m_pair.second = allocate(vector.capacity());
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, std::move(vector[i]));
        }
    }

    array(std::vector<value_type>&& vector, const allocator_type& allocator)
        : m_pair{detail::one_then_variadic_args{}, allocator}
        , m_size{vector.size()}
        , m_capacity{vector.capacity()} {
        m_pair.second = allocate(vector.capacity());
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, std::move(vector[i]));
        }
    }

    array(std::initializer_list<value_type> initializer_list, const allocator_type& allocator = {})
        : m_pair{detail::one_then_variadic_args{}, allocator}
        , m_size{initializer_list.size()}
        , m_capacity{initializer_list.size()} {
        m_pair.second = allocate(initializer_list.size());
        for (size_type i = 0; const auto& value: initializer_list) {
            std::construct_at(m_pair.second + i++, value);
        }
    }

    array(const array& other)
        : m_pair{detail::zero_then_variadic_args{}}
        , m_size{other.size()}
        , m_capacity{other.capacity()} {
        m_pair.second = allocate(other.capacity());
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, other[i]);
        }
    }

    array(const array& other, const allocator_type& allocator)
        : m_pair{detail::one_then_variadic_args{}, allocator}
        , m_size{other.size()}
        , m_capacity{other.capacity()} {
        m_pair.second = allocate(other.capacity());
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, other[i]);
        }
    }

    auto operator=(const array& other) -> array& {
        if (this != &other) {
            destruct_and_deallocate_all();
            m_pair.second = allocate(other.capacity());
            m_size = other.size();
            m_capacity = other.capacity();
            for (size_type i = 0; i < size(); i++) {
                std::construct_at(m_pair.second + i, other[i]);
            }
        }

        return *this;
    }

    array(array&& other) noexcept : m_pair{detail::zero_then_variadic_args{}} {
        m_pair.second = std::exchange(other.m_pair.second, m_pair.second);
        m_size = std::exchange(other.m_size, m_size);
        m_capacity = std::exchange(other.m_capacity, m_capacity);
    }

    array(array&& other, const allocator_type& allocator) noexcept : m_pair{detail::one_then_variadic_args{}, allocator} {
        m_pair.second = std::exchange(other.m_pair.second, m_pair.second);
        m_size = std::exchange(other.m_size, m_size);
        m_capacity = std::exchange(other.m_capacity, m_capacity);
    }

    auto operator=(array&& other) noexcept -> array& {
        if (this != &other) {
            destruct_and_deallocate_all();
            m_pair.second = std::exchange(other.m_pair.second, m_pair.second);
            m_size = std::exchange(other.m_size, m_size);
            m_capacity = std::exchange(other.m_capacity, m_capacity);
        }

        return *this;
    }

    ~array() {
        destruct_and_deallocate_all();
    }

    auto assign(size_type count, const_reference value) -> void {
        clear();
        for (size_type i = 0; i < count; i++) {
            push_back(value);
        }
    }

    template<std::input_iterator It>
    auto assign(It first, It last) -> void {
        clear();
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    auto assign(std::initializer_list<value_type> initializer_list) {
        clear();
        for (const auto& el: initializer_list) {
            push_back(el);
        }
    }

    [[nodiscard]] auto get_allocator() const noexcept(std::is_nothrow_default_constructible_v<allocator_type>)
        -> allocator_type {
        return m_pair.first();
    }

    [[nodiscard]] auto operator==(const array& other) const noexcept -> bool {
        if (size() != other.size()) {
            return false;
        }

        for (size_type i = 0; i < size(); i++) {
            if (m_pair.second[i] != other[i]) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] auto operator!=(const array& other) const noexcept -> bool {
        return !(*this == other);
    }

    [[nodiscard]] auto at(size_type index) noexcept -> optional_ref<value_type> {
        if (index < size()) {
            return m_pair.second[index];
        } else {
            return {};
        }
    }

    [[nodiscard]] auto at(size_type index) const noexcept -> optional_ref<const value_type> {
        if (index < size()) {
            return m_pair.second[index];
        } else {
            return {};
        }
    }

    [[nodiscard]] auto operator[](size_type index) noexcept -> value_type& {
        return m_pair.second[index];
    }

    [[nodiscard]] auto operator[](size_type index) const noexcept -> const_reference {
        return m_pair.second[index];
    }

    [[nodiscard]] auto front() noexcept -> optional_ref<value_type> {
        if (size() > 0) {
            return m_pair.second[0];
        } else {
            return {};
        }
    }

    [[nodiscard]] auto front() const noexcept -> optional_ref<const value_type> {
        if (size() > 0) {
            return m_pair.second[0];
        } else {
            return {};
        }
    }

    [[nodiscard]] auto back() noexcept -> optional_ref<value_type> {
        if (size() > 0) {
            return m_pair.second[size() - 1];
        } else {
            return {};
        }
    }

    [[nodiscard]] auto back() const noexcept -> optional_ref<const value_type> {
        if (size() > 0) {
            return m_pair.second[size() - 1];
        } else {
            return {};
        }
    }

    [[nodiscard]] auto data() noexcept -> pointer {
        return m_pair.second;
    }

    [[nodiscard]] auto data() const noexcept -> const_pointer {
        return m_pair.second;
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return size() == 0;
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return m_size;
    }

    [[nodiscard]] auto capacity() const noexcept -> size_type {
        return m_capacity;
    }

    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        return std::numeric_limits<size_type>::max();
    }

    template<typename value_typey>
    class raw_iterator {
    public:
        using iterator_category = std::contiguous_iterator_tag;
        using value_type = value_typey;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = difference_type;

        constexpr raw_iterator() noexcept = default;
        constexpr raw_iterator(pointer p) noexcept : m_pointer{p} {}

        constexpr raw_iterator(const raw_iterator&) noexcept = default;

        constexpr auto operator*() const noexcept -> reference {
            return *m_pointer;
        }

        constexpr auto operator->() const noexcept -> pointer {
            return m_pointer;
        }

        constexpr auto operator++() noexcept -> raw_iterator& {
            ++m_pointer;
            return *this;
        }

        constexpr auto operator++(int) noexcept -> raw_iterator {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        constexpr auto operator--() noexcept -> raw_iterator& {
            --m_pointer;
            return *this;
        }

        constexpr auto operator--(int) noexcept -> raw_iterator {
            auto temp = *this;
            --(*this);
            return temp;
        }

        constexpr auto operator+=(difference_type n) noexcept -> raw_iterator& {
            m_pointer += n;
            return *this;
        }

        constexpr friend auto operator+(const raw_iterator& it, difference_type n) noexcept -> raw_iterator {
            return it.m_pointer + n;
        }

        constexpr friend auto operator+(difference_type n, const raw_iterator& it) noexcept -> raw_iterator {
            return n + it.m_pointer;
        }

        constexpr auto operator-=(difference_type n) noexcept -> raw_iterator& {
            m_pointer -= n;
            return *this;
        }

        constexpr friend auto operator-(const raw_iterator& it, difference_type n) noexcept -> raw_iterator {
            return it.m_pointer - n;
        }

        constexpr friend auto operator-(difference_type n, const raw_iterator& it) noexcept -> raw_iterator {
            return n - it.m_pointer;
        }

        constexpr friend auto operator-(const raw_iterator& a, const raw_iterator& b) noexcept -> difference_type {
            return a.m_pointer - b.m_pointer;
        }

        constexpr auto operator[](difference_type n) const noexcept -> value_typey& {
            return *(*this + n);
        }

        constexpr friend auto operator<(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return b - a > 0;
        }

        constexpr friend auto operator>(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return b < a;
        }

        constexpr friend auto operator<=(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return !(a < b);
        }

        constexpr friend auto operator>=(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return !(a > b);
        }

        constexpr friend auto operator==(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return a.m_pointer == b.m_pointer;
        }

        constexpr friend auto operator!=(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return a.m_pointer != b.m_pointer;
        }

    private:
        pointer m_pointer{nullptr};
    };// class raw_iterator

    using iterator = raw_iterator<value_type>;
    using const_iterator = raw_iterator<const value_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(std::contiguous_iterator<iterator>);
    static_assert(std::contiguous_iterator<const_iterator>);

    [[nodiscard]] auto begin() noexcept -> iterator {
        return empty() ? end() : m_pair.second;
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return empty() ? end() : m_pair.second;
    }
    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return empty() ? end() : m_pair.second;
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return m_pair.second + m_size;
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return m_pair.second + m_size;
    }

    [[nodiscard]] auto cend() const noexcept -> const_iterator {
        return m_pair.second + m_size;
    }

    [[nodiscard]] auto rbegin() noexcept -> reverse_iterator {
        return std::make_reverse_iterator(end());
    }

    [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator {
        return std::make_reverse_iterator(end());
    }

    [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator {
        return std::make_reverse_iterator(cend());
    }

    [[nodiscard]] auto rend() noexcept -> reverse_iterator {
        return std::make_reverse_iterator(begin());
    }

    [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator {
        return std::make_reverse_iterator(begin());
    }

    [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator {
        return std::make_reverse_iterator(cbegin());
    }

    auto reserve(size_type new_capacity) -> void {
        if (new_capacity <= capacity()) {
            return;
        }

        auto data = m_pair.second;
        m_pair.second = allocate(new_capacity);
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, std::move(data[i]));
        }
        destruct_and_deallocate(data, size(), m_capacity);
        m_capacity = new_capacity;
    }

    auto shrink_to_fit() -> void {
        if (size() == capacity()) {
            return;
        }

        const auto new_capacity = size();
        auto data = m_pair.second;
        m_pair.second = allocate(new_capacity);
        for (size_type i = 0; i < size(); i++) {
            std::construct_at(m_pair.second + i, std::move(data[i]));
        }
        destruct_and_deallocate(data, size(), m_capacity);
        m_capacity = new_capacity;
    }

    /*auto insert(const_iterator pos, const_reference value) -> iterator;
    auto insert(const_iterator pos, value_type&& value) -> iterator;
    auto insert(const_iterator pos, size_type count, const_reference value) -> iterator;
    auto insert(const_iterator pos, std::initializer_list<value_type> initializer_list) -> iterator;
    auto insert_range(const_iterator pos, R&& range) -> iterator;

    template<typename... Args>
    auto emplace(const_iterator pos, Args&&... args) -> iterator {
        return m_pair.second.emplace(pos, std::forward<Args>(args)...);
    }*/

    template<typename... Args>
    auto emplace_back(Args&&... args) -> value_type& {
        grow_if_needed();
        std::construct_at(m_pair.second + m_size++, std::forward<Args>(args)...);
        return m_pair.second[size() - 1];
    }

    auto push_back(const_reference value) -> void {
        grow_if_needed();
        std::construct_at(m_pair.second + m_size++, value);
    }

    auto push_back(value_type&& value) -> void {
        grow_if_needed();
        std::construct_at(m_pair.second + m_size++, std::move(value));
    }

    auto clear() -> void {
        destroy(m_pair.second, size());
        m_size = 0;
    }

    /*auto erase(const_iterator pos) -> iterator;
    auto erase(const_iterator first, const_iterator last) -> iterator;*/

    auto pop_back() -> void {
        if (empty()) {
            return;
        }

        destroy(m_pair.second + size() - 1, 1);
        m_size--;
    }

    [[nodiscard]] auto pop() -> std::optional<value_type> {
        if (empty()) {
            return std::nullopt;
        }

        auto last = std::move(m_pair.second[size() - 1]);
        destroy(m_pair.second + size() - 1, 1);
        m_size--;
        return last;
    }

    auto resize(size_type new_size) -> void {
        resize(new_size, value_type{});
    }

    auto resize(size_type new_size, const_reference value) -> void {
        if (new_size == size()) {
            return;
        }

        if (new_size > size()) {
            if (new_size <= capacity()) {
                // no reallocation needed
                for (auto i = size(); i < new_size; i++) {
                    std::construct_at(m_pair.second + i, value);
                }
                m_size = new_size;
            } else {
                // reallocation required
                auto data = m_pair.second;
                m_pair.second = allocate(new_size);
                for (size_type i = 0; i < size(); i++) {
                    std::construct_at(m_pair.second + i, std::move(data[i]));
                }
                for (auto i = size(); i < new_size; i++) {
                    std::construct_at(m_pair.second + i, value);
                }
                destruct_and_deallocate(data, m_size, m_capacity);
                m_capacity = m_size = new_size;
            }
        } else {
            destroy(m_pair.second + new_size, size() - new_size);
            m_size = new_size;
        }
    }

    auto swap(array& other) -> void {
        m_pair.second = std::exchange(other.m_pair.second, m_pair.second);
        m_size = std::exchange(other.m_size, m_size);
        m_capacity = std::exchange(other.m_capacity, m_capacity);
    }

private:
    auto grow_if_needed() -> void {
        if (size() == capacity()) {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
    }

    [[nodiscard]] auto allocate(size_type count) -> pointer {
        return std::allocator_traits<allocator_type>::allocate(m_pair.first(), count);
    }

    auto deallocate(pointer pointer, size_type count) -> void {
        if (pointer != nullptr && count > 0) {
            std::allocator_traits<allocator_type>::deallocate(m_pair.first(), pointer, count);
        }
    }

    auto destroy(pointer pointer, size_type count) -> void {
        if (pointer != nullptr) {
            std::destroy_n(pointer, count);
        }
    }

    auto destruct_and_deallocate_all() -> void {
        destruct_and_deallocate(m_pair.second, size(), capacity());
        m_pair.second = nullptr;
    }

    auto destruct_and_deallocate(pointer pointer, size_type obj_count, size_type size) -> void {
        if (pointer != nullptr) {
            destroy(pointer, obj_count);
            deallocate(pointer, size);
        }
    }

    compressed_pair<allocator_type, pointer> m_pair;
    size_type m_size{}, m_capacity{};
};
}// namespace serpp::json::detail

#endif// #ifndef SERPP_ARRAY_HPP
