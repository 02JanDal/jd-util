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
#include <QMetaProperty>
#include <QDebug>
#include <functional>

#include "functional/FunctionTraits.h"
#include "functional/Base.h"

#include "functional/FunctionTraits.h"
#include "functional/Base.h"

namespace JD {
namespace Util {
namespace detail {
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

namespace detail {
template <typename> class PropertyApplier;
// clazy:excludeall=missing-qobject-macro
#define PROPERTYAPPLIER(type) \
class PropertyApplier_##type : public QObject \
{ \
	Q_OBJECT \
	using Function = std::function<void(type)>; \
	Function m_function; \
public: \
	explicit PropertyApplier_##type(Function &&func, QObject *parent = nullptr) : QObject(parent), m_function(std::forward<Function>(func)) {} \
	Q_INVOKABLE void apply(const type &property) { m_function(property); } \
	static QMetaMethod applyMethod() { return staticMetaObject.method(staticMetaObject.indexOfMethod("apply(" #type ")")); } \
}; \
template <> \
class PropertyApplier<type> : public PropertyApplier_##type \
{ \
public: \
	using PropertyApplier_##type::PropertyApplier_##type; \
};
PROPERTYAPPLIER(QVariant)
PROPERTYAPPLIER(QString)
PROPERTYAPPLIER(int)
PROPERTYAPPLIER(bool)
#undef PROPERTYAPPLIER
}

template <typename Object, typename Getter, typename Signal, typename Receiver, typename Func>
void applyProperty(Object *obj, Getter getter, Signal signal, Receiver *receiverContext, Func func)
{
	JD::Util::Functional::static_if<std::is_member_function_pointer<Func>::value>([&](auto f)
	{
		(receiverContext->*f(func))((obj->*getter)());
		QObject::connect(obj, signal, receiverContext, func);
	}).else_([&](auto f)
	{
		f(func)((obj->*getter)());
		QObject::connect(obj, signal, receiverContext, f(func));
	});
}
template <typename Receiver, typename Func>
void applyProperty(QObject *obj, const char *propertyName, Receiver *receiverContext, Func func)
{
	using DirtyType = typename Functional::FunctionTraits<Func>::template Argument<0>::Type;
	using Type = std::remove_cv_t<std::remove_reference_t<DirtyType>>;
	const QMetaObject *meta = obj->metaObject();
	const QMetaProperty property = meta->property(meta->indexOfProperty(propertyName));
	JD::Util::Functional::static_if<std::is_member_function_pointer<Func>::value>([&](auto f)
	{
		(receiverContext->*f(func))(property.read(obj).value<Type>());
		QObject::connect(obj, property.notifySignal(),
						 new detail::PropertyApplier<Type>([=](const Type &v) { (receiverContext->*f(func))(v); }, receiverContext), detail::PropertyApplier<Type>::applyMethod());
	}).else_([&](auto f)
	{
		f(func)(property.read(obj).value<Type>());
		QObject::connect(obj,
						 property.notifySignal(), new detail::PropertyApplier<Type>([=](const Type &v) { f(func)(v); }, receiverContext), detail::PropertyApplier<Type>::applyMethod());
	});
}

namespace detail {
template <typename Container>
class ReverseContainer
{
	const Container m_cont;
public:
	explicit ReverseContainer(Container &&cont) : m_cont(std::forward<Container>(cont)) {}

	auto begin() { return m_cont.rbegin(); }
	auto end() { return m_cont.rend(); }
};
}

template <typename Container>
auto reverse(Container &&container)
{
	return detail::ReverseContainer<Container>(std::forward<Container>(container));
}

namespace detail {
QString regexReplaceImplString(const QString &string, const QRegularExpression &regex, const std::function<QString(QString)> &func);
QString regexReplaceImplRegex(const QString &string, const QRegularExpression &regex, const std::function<QString(QRegularExpressionMatch)> &func);

template <typename Func>
constexpr bool IsFunctionTakingRegularExpressionMatch =
		std::is_same<typename std::decay_t<typename Functional::FunctionTraits<Func>::template Argument<0>::Type>, QRegularExpressionMatch>::value;
}

QString regexReplace(const QString &string, const QRegularExpression &regex, const QString &replacement);
template <typename Func>
QString regexReplace(const QString &string, const QRegularExpression &regex, Func &&func,
					 std::enable_if_t<detail::IsFunctionTakingRegularExpressionMatch<Func>>* = nullptr)
{
	return detail::regexReplaceImplRegex(string, regex, [func](const QRegularExpressionMatch &str) { return func(str); });
}
template <typename Func>
QString regexReplace(const QString &string, const QRegularExpression &regex, Func &&func,
					 std::enable_if_t<!detail::IsFunctionTakingRegularExpressionMatch<Func>>* = nullptr)
{
	return detail::regexReplaceImplString(string, regex, [func](const QString &str) { return func(str); });
}

}
}
