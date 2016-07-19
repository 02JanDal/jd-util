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

#include <QObject>

namespace JD {
namespace Util {
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

template <typename Container>
void mergeAssociative(Container &to, const Container &from)
{
	for (auto it = from.cbegin(); it != from.cend(); ++it) {
		to[it.key()] = it.value();
	}
}
template <typename Container>
Container mergeAssociative(const Container &to, const Container &from)
{
	Container out = to;
	mergeAssociative<Container>(out, from);
	return out;
}

template <typename Object, typename Getter, typename Signal, typename Func>
void applyProperty(Object *obj, Getter getter, Signal signal, QObject *receiveContext, Func func)
{
	func((obj->*getter)());
	QObject::connect(obj, signal, receiveContext, func);
}

}
}
