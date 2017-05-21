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

#include "Util.h"

#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

namespace JD {
namespace Util {

QString regexReplace(const QString &string, const QRegularExpression &regex, const QString &replacement)
{
	return QString(string).replace(regex, replacement);
}

QString detail::regexReplaceImplString(const QString &string, const QRegularExpression &regex,
									   const std::function<QString(QString)> &func)
{
	return regexReplaceImplRegex(string, regex, [func](const QRegularExpressionMatch &match) { return func(match.captured()); });
}
QString detail::regexReplaceImplRegex(const QString &string, const QRegularExpression &regex,
								 const std::function<QString(QRegularExpressionMatch)> &func)
{
	QString result = string;

	int offset = 0;
	QRegularExpressionMatchIterator iterator = regex.globalMatch(result);
	while (iterator.hasNext()) {
		const QRegularExpressionMatch match = iterator.next();
		const QString replacement = func(match);
		result = result.replace(match.capturedStart() + offset, match.capturedLength(), replacement);
		offset += (replacement.length() - match.capturedLength());
	}

	return result;
}

}
}
