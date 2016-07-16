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

class QString;

namespace JD {
namespace Util {
namespace Introspection {
namespace detail {
QString extractClass(const char *func);
}
}
}
}

#define INTROSPECTION_X(classname) \
	public: \
	static QString staticClassName() { return QStringLiteral(#classname); } \
	QT_WARNING_PUSH \
	QT_WARNING_DISABLE_CLANG("-Winconsistent-missing-override") \
	virtual QString className() const { return staticClassName(); } \
	QT_WARNING_POP \
	private:

#define INTROSPECTION \
	public: \
	static QString staticClassName() { return ::JD::Util::Introspection::detail::extractClass(__PRETTY_FUNCTION__); } \
	QT_WARNING_PUSH \
	QT_WARNING_DISABLE_CLANG("-Winconsistent-missing-override") \
	virtual QString className() const { return staticClassName(); } \
	QT_WARNING_POP \
	private:
