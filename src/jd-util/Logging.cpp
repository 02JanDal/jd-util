#include "Logging.h"

#include <iostream>

#include "TermUtil.h"
#include "Backtrace.h"

namespace JD {
namespace Util {

static QString cleanMessagePath(const QString &path)
{
	if (path.contains('/')) {
		return path.mid(path.lastIndexOf('/') + 1);
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

	if (ctxt.category && QString(ctxt.category) != "default") {
		msg += QLatin1Char('[') + Term::fg(Term::Green, ctxt.category) + QLatin1Char(']');
	}
	if (ctxt.file) {
		msg += QLatin1Char('[') + cleanMessagePath(ctxt.file) + QLatin1Char(':') + QString::number(ctxt.line) + QLatin1Char(']');
	}
	/*if (ctxt.function) {
		msg += QLatin1Char('[') + cleanMessageFunction(ctxt.function) + QLatin1Char(']');
	}*/

	msg += QLatin1Char(' ') + Term::style(Term::Bold, message);

	switch (type) {
	case QtInfoMsg:     msg.prepend(QLatin1Char('[') + Term::fg(Term::Cyan,   "INFO"    ) + QLatin1Char(']')); break;
	case QtDebugMsg:    msg.prepend(QLatin1Char('[') + Term::fg(Term::Blue,   "DEBUG"   ) + QLatin1Char(']')); break;
	case QtWarningMsg:  msg.prepend(QLatin1Char('[') + Term::fg(Term::Yellow, "WARNING" ) + QLatin1Char(']')); break;
	case QtCriticalMsg: msg.prepend(QLatin1Char('[') + Term::fg(Term::Red,    "CRITICAL") + QLatin1Char(']')); break;
	case QtFatalMsg:    msg.prepend(QLatin1Char('[') + Term::fg(Term::Red,    "FATAL"   ) + QLatin1Char(']')); break;
	}

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
