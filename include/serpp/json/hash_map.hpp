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
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SERPP_HASH_MAP_HPP
#define SERPP_HASH_MAP_HPP

#include "optional_ref.hpp"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace serpp::json::detail {
template<typename ValueType>
class hash_map {
private:
    struct bucket {
        std::pair<std::string, ValueType> pair{};
        std::size_t hash{};
        std::size_t distance{};
        bool occupied{};

        [[nodiscard]] auto operator==(const bucket&) const noexcept -> bool = default;
    };

    struct hash {
        [[nodiscard]] auto operator()(const std::string& s) const noexcept -> std::size_t {
            return std::hash<std::string>{}(s);
        }

        [[nodiscard]] auto operator()(std::string_view s) const noexcept -> std::size_t {
            return std::hash<std::string_view>{}(s);
        }
    };

    struct key_equal {
        [[nodiscard]] auto operator()(const std::string& s1, const std::string& s2) const noexcept -> bool {
            return s1 == s2;
        }

        [[nodiscard]] auto operator()(std::string_view s1, const std::string& s2) const noexcept -> bool {
            return s1 == s2;
        }

        [[nodiscard]] auto operator()(const std::string& s1, std::string_view s2) const noexcept -> bool {
            return s1 == s2;
        }

        [[nodiscard]] auto operator()(std::string_view s1, std::string_view s2) const noexcept -> bool {
            return s1 == s2;
        }
    };

public:
    template<typename T>
    class raw_iterator final {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;

        raw_iterator(pointer it, pointer end)
            : m_pointer{ it }, m_end{ end } {}

        auto operator*() const noexcept -> const std::pair<std::string, ValueType>& {
            return m_pointer->pair;
        }

        auto operator->() const noexcept -> const std::pair<std::string, ValueType>* {
            return &m_pointer->pair;
        }

        auto value() noexcept -> ValueType& {
            return m_pointer->pair.second;
        }

        auto value() const noexcept -> const ValueType& {
            return m_pointer->pair.second;
        }

        auto operator++() noexcept -> raw_iterator& {
            do {
                m_pointer++;
            } while (m_pointer != m_end && !m_pointer->occupied);
            return *this;
        }

        auto operator++(int) noexcept -> raw_iterator {
            auto temp = *this;
            do {
                ++(*this);
            } while (m_pointer != m_end && !m_pointer->occupied);
            return temp;
        }

        friend auto operator==(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return a.m_pointer == b.m_pointer;
        }

        friend auto operator!=(const raw_iterator& a, const raw_iterator& b) noexcept -> bool {
            return a.m_pointer != b.m_pointer;
        }

    private:
        pointer m_pointer, m_end;
    };

    using iterator = raw_iterator<bucket>;
    using const_iterator = raw_iterator<const bucket>;

    hash_map() : m_buckets{ s_initial_capacity } {

    }

    [[nodiscard]] auto operator==(const hash_map& other) const noexcept {
        return m_buckets == other.m_buckets;
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return size() == 0;
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t {
        return m_size;
    }

    [[nodiscard]] auto capacity() const noexcept -> std::size_t {
        return m_buckets.size();
    }

    [[nodiscard]] auto insert(const std::string& key, const ValueType& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            return { it, false };
        }

        return insert({
            .pair = { key , value },
            .hash = hash,
            .occupied = true,
        });
    }

    [[nodiscard]] auto insert(const std::string& key, ValueType&& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            return { it, false };
        }

        return insert({
            .pair = { key , std::move(value) },
            .hash = hash,
            .occupied = true,
        });
    }

    [[nodiscard]] auto insert(std::string&& key, const ValueType& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            return { it, false };
        }

        return insert({
            .pair = { std::move(key), value },
            .hash = hash,
            .occupied = true,
        });
    }

    [[nodiscard]] auto insert(std::string&& key, ValueType&& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            return { it, false };
        }

        return insert({
            .pair = { std::move(key), std::move(value) },
            .hash = hash,
            .occupied = true,
        });
    }

    auto insert_or_assign(std::string_view key, const ValueType& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            it.value() = value;
            return { it, false };
        } else {
            return insert({
                .pair = { std::string{ key }, value },
                .hash = hash,
                .occupied = true,
            });
        }
    }

    auto insert_or_assign(std::string_view key, ValueType&& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            it.value() = std::move(value);
            return { it, false };
        } else {
            return insert({
                .pair = { std::string{ key }, std::move(value) },
                .hash = hash,
                .occupied = true,
            });
        }
    }

    auto insert_or_assign(std::string&& key, const ValueType& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            it.value() = value;
            return { it, false };
        } else {
            return insert({
                .pair = { std::move(key), value },
                .hash = hash,
                .occupied = true,
            });
        }
    }

    auto insert_or_assign(std::string&& key, ValueType&& value) noexcept -> std::pair<iterator, bool> {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            it.value() = std::move(value);
            return { it, false };
        } else {
            return insert({
                .pair = { std::move(key), std::move(value) },
                .hash = hash,
                .occupied = true,
            });
        }
    }

    [[nodiscard]] auto operator[](const std::string& key) noexcept -> ValueType& {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            return it->second;
        } else {
            return insert({
                .pair = { key , {} },
                .hash = hash,
                .occupied = true,
            }).first->second;
        }
    }

    [[nodiscard]] auto operator[](std::string&& key) noexcept -> ValueType& {
        const auto hash = m_hasher(key);
        auto it = find(key, hash);
        if (it != end()) {
            return it.value();
        } else {
            return insert({
                .pair = { std::move(key), {} },
                .hash = hash,
                .occupied = true,
            }).first.value();
        }
    }

    [[nodiscard]] auto at(std::string_view key) noexcept -> optional_ref<ValueType> {
        auto it = find(key);
        return it == end() ? optional_ref<ValueType>{} : optional_ref<const ValueType>{ it.value() };
    }

    [[nodiscard]] auto at(std::string_view key) const noexcept -> optional_ref<const ValueType> {
        const auto it = find(key);
        return it == end() ? optional_ref<const ValueType>{} : optional_ref<const ValueType>{ it.value() };
    }

    [[nodiscard]] auto find(std::string_view key) noexcept -> iterator {
        const auto hash = m_hasher(key);
        return find(key, hash);
    }

    [[nodiscard]] auto find(std::string_view key) const noexcept -> const_iterator {
        const auto hash = m_hasher(key);
        return find(key, hash);
    }

    [[nodiscard]] auto contains(std::string_view key) const noexcept -> bool {
        return find(key) != end();
    }

    [[nodiscard]] auto remove(std::string_view key) noexcept -> bool {
        auto index = m_hasher(key) % capacity();

        while (m_buckets[index].occupied) {
            auto& entry = m_buckets[index];
            if (m_key_equal(entry.pair.first, key)) {
                entry = {};
                index = (index + 1) % capacity();
                while (m_buckets[index].occupied && m_buckets[index].distance > 0) {
                    std::swap(entry, m_buckets[index]);
                    index = (index + 1) % capacity();
                }
                m_size--;
                return true;
            }
            index = (index + 1) % capacity();
        }

        return false;
    }

    auto clear() noexcept -> void {
        std::ranges::fill(m_buckets, {});
        m_size = 0;
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        if (empty()) {
            return end();
        }

        iterator it{
            m_buckets.data(),
            m_buckets.data() + m_buckets.size(),
        };

        if (!it->occupied) {
            it++;
        }

        return it;
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        if (empty()) {
            return end();
        }

        const_iterator it{
            m_buckets.data(),
            m_buckets.data() + m_buckets.size(),
        };

        if (!it->occupied) {
            it++;
        }

        return it;
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return iterator{
            m_buckets.data() + m_buckets.size(),
            m_buckets.data() + m_buckets.size(),
        };
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return const_iterator{
            m_buckets.data() + m_buckets.size(),
            m_buckets.data() + m_buckets.size(),
        };
    }

    [[nodiscard]] auto cend() const noexcept -> const_iterator {
        return end();
    }

private:
    [[nodiscard]] auto insert(bucket&& entry) noexcept -> std::pair<iterator, bool> {
        check_load();
        auto index = entry.hash % capacity();
        std::size_t inserted_index{};
        bool inserted = false;

        while (m_buckets[index].occupied) {
            auto& old = m_buckets[index];
            if (entry.distance > old.distance) {
                std::swap(entry, old);

                if (!inserted) {
                    inserted = true;
                    inserted_index = index;
                }
            }

            entry.distance++;
            index = (index + 1) % capacity();
        }

        m_buckets[index] = std::move(entry);
        m_size++;

        if (!inserted) {
            inserted_index = index;
        }

        return { iterator{ m_buckets.data() + inserted_index, m_buckets.data() + m_buckets.size() }, true };
    }

    [[nodiscard]] auto find(std::string_view key, std::size_t hash) noexcept -> iterator {
        auto index = hash % capacity();

        while (m_buckets[index].occupied) {
            const auto& entry = m_buckets[index];
            if (m_key_equal(entry.pair.first, key)) {
                return iterator{
                    m_buckets.data() + index,
                    m_buckets.data() + m_buckets.size()
                };
            }
            index = (index + 1) % capacity();
        }

        return end();
    }

    [[nodiscard]] auto find(std::string_view key, std::size_t hash) const noexcept -> const_iterator {
        auto index = hash % capacity();

        while (m_buckets[index].occupied) {
            const auto& entry = m_buckets[index];
            if (m_key_equal(entry.pair.first, key)) {
                return const_iterator{
                    m_buckets.data() + index,
                    m_buckets.data() + m_buckets.size()
                };
            }
            index = (index + 1) % capacity();
        }

        return end();
    }

    auto check_load() noexcept -> void {
        if (static_cast<float>(size()) / static_cast<float>(capacity()) >= s_grow_factor) {
            grow_and_rehash();
        }
    }

    auto grow_and_rehash() noexcept -> void {
        std::vector<bucket> buckets;
        buckets.reserve(m_size);
        for (auto& bucket : m_buckets) {
            if (bucket.occupied) {
                bucket.distance = 0;
                buckets.emplace_back(std::move(bucket));
            }
        }

        std::ranges::fill(m_buckets, bucket{});
        m_buckets.resize(capacity() * 2);

        for (auto& bucket : buckets) {
            auto index = bucket.hash % capacity();

            while (m_buckets[index].occupied) {
                auto& old = m_buckets[index];

                if (bucket.distance > old.distance) {
                    std::swap(bucket, old);
                }

                bucket.distance++;
                index = (index + 1) % capacity();
            }

            m_buckets[index] = std::move(bucket);
        }
    }

    static constexpr std::size_t s_initial_capacity = 8;
    static constexpr float s_grow_factor = 0.75f;

    hash m_hasher{};
    key_equal m_key_equal{};

    std::size_t m_size{};
    std::vector<bucket> m_buckets{};
};
} // namespace serpp::json::detail

#endif // #ifndef SERPP_HASH_MAP_HPP
