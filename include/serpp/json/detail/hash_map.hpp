/*
 * Copyright © 2025 Ryan Jeffares
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SERPP_HASH_MAP_HPP
#define SERPP_HASH_MAP_HPP

#include "array.hpp"
#include "compressed_pair.hpp"
#include "serpp/optional_ref.hpp"

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>

namespace serpp::json::detail {
template<
    typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>,
    typename KeyContainer = array<Key>, typename ValueContainer = array<Value>>
class hash_map {
public:
    using key_container_type = KeyContainer;
    using value_container_type = ValueContainer;
    using key_type = Key;
    using mapped_type = Value;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using value_type = std::pair<key_type, mapped_type>;
    using reference = std::pair<const key_type&, mapped_type&>;
    using const_reference = std::pair<const key_type&, const mapped_type&>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    hash_map() = default;

    SERPP_DECLARE_DEFAULT_COPYABLE(hash_map)

    hash_map(hash_map&& other) noexcept
        : m_containers{std::move(other.m_containers)}
        , m_hash_key_equal_pair{std::move(other.m_hash_key_equal_pair)} {
        m_size = std::exchange(other.m_size, m_size);
    }

    auto operator=(hash_map&& other) noexcept -> hash_map& {
        if (this != &other) {
            m_containers = std::move(other.m_containers);
            m_hash_key_equal_pair = std::move(other.m_hash_key_equal_pair);
            m_size = std::exchange(other.m_size, 0);
        }

        return *this;
    }

    [[nodiscard]] auto operator==(const hash_map& other) const noexcept -> bool {
        return m_containers.keys == other.m_containers.keys && m_containers.values == other.m_containers.values;
    }

    [[nodiscard]] auto at(const key_type& key) noexcept -> optional_ref<value_type> {
        const auto index = get_index(key);
        while (m_containers.keys[index] != nullptr) {

        }
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return size() == 0;
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return m_size;
    }

    [[nodiscard]] auto capacity() const noexcept -> size_type {
        return m_containers.keys.size();
    }

    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        return std::numeric_limits<size_type>::max();
    }

private:
    static constexpr float s_load_factor = 0.75f;
    static constexpr size_type s_default_capacity = 16;

    [[nodiscard]] auto get_index(const key_type& key) const noexcept -> size_type {
        return m_hash_key_equal_pair.first()(key) % capacity();
    }

    struct {
        key_container_type keys;
        value_container_type values;
    } m_containers;

    compressed_pair<hasher, key_equal> m_hash_key_equal_pair{zero_then_variadic_args{}};
    size_type m_size{};
};// class hash_map
}// namespace serpp::json::detail

#endif// #ifndef SERPP_HASH_MAP_HPP
