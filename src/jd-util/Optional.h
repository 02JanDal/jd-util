/* Copyright 2016 Jan Dalheimer <jan@dalheimer.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <utility>

// TODO replace by std::optional once it's been released

namespace JD {
namespace Util {

template <typename T>
class Optional
{
	T m_value;
	const bool m_valid = false;

public:
	constexpr Optional(T &&v) : m_value(std::forward<T>(v)), m_valid(true) {}
	constexpr explicit Optional() : m_valid(false) {}

	constexpr operator bool() const { return m_valid; }

	constexpr const T *operator->() const { return &m_value; }
	constexpr T *operator->() { return &m_value; }

	constexpr const T &operator*() const & { return m_value; }
	constexpr T &operator*() & { return m_value; }

	constexpr const T &&operator*() const && { return m_value; }
	constexpr T &&operator*() && { return m_value; }

	constexpr const T value() const { return m_value; }
};

}
}
