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

#include "Logging.h"

#include <iostream>

#include "TermUtil.h"
#include "Backtrace.h"

namespace JD {
namespace Util {

static QString cleanMessagePath(const QString &path)
{
	if (path.contains(QLatin1Char('/'))) {
		return path.mid(path.lastIndexOf(QLatin1Char('/')) + 1);
	} else {
		return path;
	}
}
/*static QString cleanMessageFunction(const QString &func)
{
	const QString withoutReturn = func.mid(func.indexOf(' ') + 1);
	return withoutReturn.left(withoutReturn.indexOf('('));
}*/

static void messageHandler(const QtMsgType type, const QMessageLogContext &ctxt, const QString &message)
{
	QString msg;

	if (ctxt.category && QString::fromLatin1(ctxt.category) != QStringLiteral("default")) {
		msg += QLatin1Char('[') + Term::fg(Term::Green, QString::fromLatin1(ctxt.category)) + QLatin1Char(']');
	}
	if (ctxt.file) {
		msg += QLatin1Char('[') + cleanMessagePath(QString::fromLatin1(ctxt.file)) + QLatin1Char(':') + QString::number(ctxt.line) + QLatin1Char(']');
	}
	/*if (ctxt.function) {
		msg += QLatin1Char('[') + cleanMessageFunction(ctxt.function) + QLatin1Char(']');
	}*/

	msg += QLatin1Char(' ') + Term::style(Term::Bold, message);

	msg.prepend(QLatin1Char(']'));
	switch (type) {
	case QtInfoMsg:     msg.prepend(Term::fg(Term::Cyan,   QStringLiteral("INFO")    )); break;
	case QtDebugMsg:    msg.prepend(Term::fg(Term::Blue,   QStringLiteral("DEBUG")   )); break;
	case QtWarningMsg:  msg.prepend(Term::fg(Term::Yellow, QStringLiteral("WARNING") )); break;
	case QtCriticalMsg: msg.prepend(Term::fg(Term::Red,    QStringLiteral("CRITICAL"))); break;
	case QtFatalMsg:    msg.prepend(Term::fg(Term::Red,    QStringLiteral("FATAL")   )); break;
	}
	msg.prepend(QLatin1Char('['));

	if (type == QtInfoMsg) {
		std::cout << msg.toLocal8Bit().constData() << '\n';
	} else {
		std::cerr << msg.toLocal8Bit().constData() << std::endl;
	}

	if (type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg) {
		dumpBacktrace(3, 15);
	}
}

void installLogFormatter()
{
	qInstallMessageHandler(messageHandler);
}

}
}
