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

#include "Backtrace.h"

#include <QVector>
#include <iostream>

#if defined(Q_OS_UNIX)
#include <execinfo.h>
#include <cxxabi.h>
#endif

namespace JD {
namespace Util {

QVector<QString> backtrace(const int maxFrames)
{
#if defined(Q_OS_UNIX)
QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wvla")
QT_WARNING_DISABLE_CLANG("-Wvla-extension")
	void *addressList[maxFrames+1];
	int addressLength = ::backtrace(addressList, sizeof(addressList)/sizeof(void*));
	if (addressLength == 0) {
		return {};
	}

	char **symbols = ::backtrace_symbols(addressList, addressLength);

	QVector<QString> out;
	for (int i = 1; i < addressLength; ++i) {
		const QString symbol = QString::fromLocal8Bit(symbols[i]);
#if defined(Q_OS_OSX)
		const QVector<QStringRef> parts = symbol.splitRef(QLatin1Char(' '), QString::SkipEmptyParts);
		int status;
		char *demangled = abi::__cxa_demangle(parts.at(3).toLocal8Bit().constData(), 0, 0, &status);
		if (status == 0) {
			out.append(QString::fromLocal8Bit(demangled) + QStringLiteral(" + ") + parts.at(5).toString());
			free(demangled);
		} else {
			out.append(parts.at(3).toString() + QStringLiteral(" + ") + parts.at(5).toString());
		}
#else
		const int beginName = symbol.indexOf(QLatin1Char('(')),
				beginOffset = symbol.indexOf(QLatin1Char('+')),
				endOffset = symbol.indexOf(QLatin1Char(')'));
		/*for (char *p = symbols[i]; *p; ++p) {
			if (*p == '(') {
				beginName = p;
			} else if (*p == '+') {
				beginOffset = p;
			} else if (*p == ')') {
				endOffset = p;
				break;
			}
		}*/

		if (beginName != -1 && beginOffset != -1 && endOffset != -1 && beginName < beginOffset) {
		//if (beginName && beginOffset && endOffset && beginName < beginOffset) {
			//*beginName++ = '\0';
			//*beginOffset++ = '\0';
			//*endOffset++ = '\0';

			/*int status;
			char *demangled = abi::__cxa_demangle(beginName, 0, 0, &status);

			if (status == 0) {
				out.append(QString::fromLocal8Bit(demangled) + QLatin1Char('+') + QString::fromLocal8Bit(beginOffset));
			} else {
				out.append(QString::fromLocal8Bit(beginName) + QStringLiteral("()+") + QString::fromLocal8Bit(beginOffset));
			}
			free(demangled);*/
		} else {
			out.append(QString::fromLocal8Bit(symbols[i]));
		}
#endif
	}

	free(symbols);
QT_WARNING_POP

	return out;
#else
	return QVector<QString>();
#endif
}

void dumpBacktrace(const int offset, const int maxFrames)
{
	const QVector<QString> frames = backtrace(maxFrames + 1 + offset).mid(1 + offset);
	for (const QString &frame : frames) {
		std::cerr << '\t' << frame.toLocal8Bit().constData() << '\n';
	}
}

}
}
