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

#include "serpp/json/jvalue.hpp"

namespace serpp::json {
jvalue::jvalue(const jobject& object)
    : m_data{object} {}

jvalue::jvalue(jobject&& object) noexcept
    : m_data{std::move(object)} {}

jvalue::jvalue(const jarray& array)
    : m_data{array} {}

jvalue::jvalue(jarray&& array) noexcept
    : m_data{std::move(array)} {}

jvalue::jvalue(bool boolean) noexcept
    : m_data{boolean} {}

jvalue::jvalue(std::nullptr_t) noexcept
    : m_data{nullptr} {}

auto jvalue::operator=(const jobject& object) -> jvalue& {
    m_data.emplace<jobject>(object);
    return *this;
}

auto jvalue::operator=(jobject&& object) noexcept -> jvalue& {
    m_data.emplace<jobject>(std::move(object));
    return *this;
}

auto jvalue::operator=(const jarray& array) -> jvalue& {
    m_data.emplace<jarray>(array);
    return *this;
}

auto jvalue::operator=(jarray&& array) noexcept -> jvalue& {
    m_data.emplace<jarray>(std::move(array));
    return *this;
}

auto jvalue::operator=(bool boolean) noexcept -> jvalue& {
    m_data.emplace<bool>(boolean);
    return *this;
}

auto jvalue::operator=(std::nullptr_t) noexcept -> jvalue& {
    m_data.emplace<std::nullptr_t>(nullptr);
    return *this;
}

auto jvalue::emplace(const jobject& object) -> void {
    m_data.emplace<jobject>(object);
}

auto jvalue::emplace(jobject&& object) noexcept -> void {
    m_data.emplace<jobject>(std::move(object));
}

auto jvalue::emplace(const jarray& array) -> void {
    m_data.emplace<jarray>(array);
}

auto jvalue::emplace(jarray&& array) noexcept -> void {
    m_data.emplace<jarray>(std::move(array));
}

auto jvalue::emplace(bool boolean) noexcept -> void {
    m_data.emplace<bool>(boolean);
}

auto jvalue::emplace(std::nullptr_t) noexcept -> void {
    m_data.emplace<std::nullptr_t>(nullptr);
}

auto jvalue::type() const noexcept -> enum type {
    return static_cast<enum type>(m_data.index());
}

auto jvalue::string() noexcept -> optional_ref<std::string> {
    if (type() == type::string) {
        return std::get<0>(m_data);
    } else {
        return {};
    }
}

auto jvalue::string() const noexcept -> optional_ref<const std::string> {
    if (type() == type::string) {
        return std::get<0>(m_data);
    } else {
        return {};
    }
}

auto jvalue::number() noexcept -> optional_ref<double> {
    if (type() == type::number) {
        return std::get<1>(m_data);
    } else {
        return {};
    }
}

auto jvalue::number() const noexcept -> optional_ref<const double> {
    if (type() == type::number) {
        return std::get<1>(m_data);
    } else {
        return {};
    }
}

auto jvalue::object() noexcept -> optional_ref<jobject> {
    if (type() == type::object) {
        return std::get<2>(m_data);
    } else {
        return {};
    }
}

auto jvalue::object() const noexcept -> optional_ref<const jobject> {
    if (type() == type::object) {
        return std::get<2>(m_data);
    } else {
        return {};
    }
}

auto jvalue::array() noexcept -> optional_ref<jarray> {
    if (type() == type::array) {
        return std::get<3>(m_data);
    } else {
        return {};
    }
}

auto jvalue::array() const noexcept -> optional_ref<const jarray> {
    if (type() == type::array) {
        return std::get<3>(m_data);
    } else {
        return {};
    }
}

auto jvalue::boolean() noexcept -> optional_ref<bool> {
    if (type() == type::boolean) {
        return std::get<4>(m_data);
    } else {
        return {};
    }
}

auto jvalue::boolean() const noexcept -> optional_ref<const bool> {
    if (type() == type::boolean) {
        return std::get<4>(m_data);
    } else {
        return {};
    }
}

auto jvalue::null() noexcept -> optional_ref<std::nullptr_t> {
    if (type() == type::null) {
        return std::get<5>(m_data);
    } else {
        return {};
    }
}

auto jvalue::null() const noexcept -> optional_ref<const std::nullptr_t> {
    if (type() == type::null) {
        return std::get<5>(m_data);
    } else {
        return {};
    }
}

auto jvalue::to_string() const noexcept -> std::string {
    switch (type()) {
        case type::string:
            return std::get<0>(m_data);
        case type::number:
            return std::to_string(std::get<1>(m_data));
        case type::object:
            return "{}"; // TODO
        case type::array:
            return "[]"; // TODO
        case type::boolean:
            return std::get<4>(m_data) ? "true" : "false";
        case type::null:
            return "null";
        default:
            return "";
    }
}
}// namespace serpp::json
