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

namespace detail
{
class RangeIterator
{
	int m_value;
public:
	explicit RangeIterator(const int value) : m_value(value) {}

	int operator*() const { return m_value; }
	RangeIterator &operator++() { ++m_value; return *this; }
	bool operator==(const RangeIterator &other) { return m_value == other.m_value; }
	bool operator!=(const RangeIterator &other) { return m_value != other.m_value; }
};
class RangeContainer
{
	int m_start, m_end;
public:
	explicit RangeContainer(const int start, const int end) : m_start(start), m_end(end) {}

	RangeIterator begin() { return RangeIterator(m_start); }
	RangeIterator end() { return RangeIterator(m_end); }
};
}

inline detail::RangeContainer range(const int start, const int end)
{
	return detail::RangeContainer(start, end);
}
inline detail::RangeContainer range(const int end)
{
	return detail::RangeContainer(0, end);
}
