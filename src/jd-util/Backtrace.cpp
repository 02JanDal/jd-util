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

	std::size_t functionNameSize = 256;
	char *functionName = new char[functionNameSize];

	QVector<QString> out;
	for (int i = 1; i < addressLength; ++i) {
		char *beginName = 0,
				*beginOffset = 0,
				*endOffset = 0;
		for (char *p = symbols[i]; *p; ++p) {
			if (*p == '(') {
				beginName = p;
			} else if (*p == '+') {
				beginOffset = p;
			} else if (*p == ')') {
				endOffset = p;
				break;
			}
		}

		if (beginName && beginOffset && endOffset && beginName < beginOffset) {
			*beginName++ = '\0';
			*beginOffset++ = '\0';
			*endOffset++ = '\0';

			int status;
			char *demangled = abi::__cxa_demangle(beginName, functionName, &functionNameSize, &status);

			if (status == 0) {
				out.append(QString(demangled) + '+' + QString(beginOffset));
			} else {
				out.append(QString(beginName) + "()+" + QString(beginOffset));
			}
		} else {
			out.append(symbols[i]);
		}
	}

	free(symbols);

	return out;
QT_WARNING_POP
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
