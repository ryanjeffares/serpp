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

#ifndef SERPP_OPTIONAL_REF_HPP
#define SERPP_OPTIONAL_REF_HPP

namespace serpp::json {
template<typename T>
class optional_ref
{
public:
    optional_ref() = default;

    /* implicit */ optional_ref(T& value) : m_value{&value} {
    }

    [[nodiscard]] auto operator=(const optional_ref& other) noexcept -> optional_ref& {
        if (this != &other) {
            m_value = other.m_value;
        }

        return *this;
    }

    [[nodiscard]] auto has_value() const noexcept -> bool {
        return m_value != nullptr;
    }

    [[nodiscard]] operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] auto get() const noexcept -> T& {
        return *m_value;
    }

    [[nodiscard]] operator T&() const noexcept {
        return get();
    }

    [[nodiscard]] auto operator*() const noexcept -> T& {
        return get();
    }

    [[nodiscard]] auto operator->() const noexcept -> T* {
        return m_value;
    }

private:
    T* m_value;
};
} // namespace serpp::json

#endif // #ifndef SERPP_OPTIONAL_REF_HPP
