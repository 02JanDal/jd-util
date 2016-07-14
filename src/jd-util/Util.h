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
