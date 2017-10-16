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

#include "TermUtil.h"

#include <QStringList>
#include <QRegularExpression>
#include <QVector>

#include <iostream>
#include <cmath>

#include "Functional.h"
#include "Formatting.h"

#ifdef Q_OS_UNIX
# include <sys/ioctl.h>
# include <cstdio>
# include <unistd.h>
# include <termios.h>
#else
# include <io.h>
# include <windows.h>
# warning Windows support has not yet been implemented. No colors will be shown.
#endif

namespace JD {
namespace Util {
using Functional::collection;
namespace Term {
namespace detail {
#ifdef Q_OS_UNIX
static QString getStyleCode(const Style style)
{
	switch (style) {
	case Term::Bold: return QStringLiteral("\033[1m");
	case Term::Dark: return QStringLiteral("\033[2m");
	case Term::Underline: return QStringLiteral("\033[4m");
	case Term::Blink: return QStringLiteral("\033[5m");
	case Term::Reverse: return QStringLiteral("\033[7m");
	case Term::Concealed: return QStringLiteral("\033[8m");
	}
}
static QString getStyleEndCode(const Style style)
{
	switch (style) {
	case Term::Bold: return QStringLiteral("\033[22m");
	case Term::Dark: return QStringLiteral("\033[22m");
	case Term::Underline: return QStringLiteral("\033[24m");
	case Term::Blink: return QStringLiteral("\033[25m");
	case Term::Reverse: return QStringLiteral("\033[27m");
	case Term::Concealed: return QStringLiteral("\033[38m");
	}
}
static QString getFGColorCode(const Color color)
{
	switch (color) {
	case Term::Grey: return QStringLiteral("\033[30m");
	case Term::Red: return QStringLiteral("\033[31m");
	case Term::Green: return QStringLiteral("\033[32m");
	case Term::Yellow: return QStringLiteral("\033[33m");
	case Term::Blue: return QStringLiteral("\033[34m");
	case Term::Magenta: return QStringLiteral("\033[35m");
	case Term::Cyan: return QStringLiteral("\033[36m");
	case Term::White: return QStringLiteral("\033[37m");
	}
}
static QString getBGColorCode(const Color color)
{
	switch (color) {
	case Term::Grey: return QStringLiteral("\033[40m");
	case Term::Red: return QStringLiteral("\033[41m");
	case Term::Green: return QStringLiteral("\033[42m");
	case Term::Yellow: return QStringLiteral("\033[43m");
	case Term::Blue: return QStringLiteral("\033[44m");
	case Term::Magenta: return QStringLiteral("\033[45m");
	case Term::Cyan: return QStringLiteral("\033[46m");
	case Term::White: return QStringLiteral("\033[47m");
	}
}
static QChar getMoveTypeCode(const MoveType type)
{
	switch (type) {
	case JD::Util::Term::Up: return QLatin1Char('A');
	case JD::Util::Term::Down: return QLatin1Char('B');
	case JD::Util::Term::Left: return QLatin1Char('D');
	case JD::Util::Term::Right: return QLatin1Char('C');
	case JD::Util::Term::LineDown: return QLatin1Char('E');
	case JD::Util::Term::LineUp: return QLatin1Char('F');
	}
}
// http://stackoverflow.com/a/1455007/953222
static void setStdinEcho(const bool enable)
{
	struct termios tty;
	tcgetattr(STDIN_FILENO, &tty);

	QT_WARNING_PUSH
	QT_WARNING_DISABLE_GCC("-Wsign-conversion")
	QT_WARNING_DISABLE_CLANG("-Wsign-conversion")
	if (!enable) {
		tty.c_lflag &= ~ECHO;
	} else {
		tty.c_lflag |= ECHO;
	}
	QT_WARNING_POP

	tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}
#elif defined(Q_OS_WIN)
// http://stackoverflow.com/a/1455007/953222
static void setStdinEcho(const bool enable)
{
	// WARNING this has not been tested
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);

	if(!enable) {
		mode &= ~ENABLE_ECHO_INPUT;
	} else {
		mode |= ENABLE_ECHO_INPUT;
	}

	SetConsoleMode(hStdin, mode );
}
#endif
}

QString style(const Style style, const QString &in)
{
	if (!isTty()) {
		return in;
	}
#ifdef Q_OS_WIN
	return in;
#else
	if (in.isEmpty()) {
		return detail::getStyleCode(style);
	} else {
		return detail::getStyleCode(style) + in + detail::getStyleEndCode(style);
	}
#endif
}
QString fg(const Color color, const QString &in)
{
	if (!isTty()) {
		return in;
	}
#ifdef Q_OS_WIN
	return in;
#else
	if (in.isEmpty()) {
		return detail::getFGColorCode(color);
	} else {
		return detail::getFGColorCode(color) + in + QStringLiteral("\033[39m");
	}
#endif
}
QString bg(const Color color, const QString &in)
{
	if (!isTty()) {
		return in;
	}
#ifdef Q_OS_WIN
	return in;
#else
	if (in.isEmpty()) {
		return detail::getBGColorCode(color);
	} else {
		return detail::getBGColorCode(color) + in + QStringLiteral("\033[39m");
	}
#endif
}
QString reset()
{
	if (!isTty()) {
		return QString();
	}
#ifdef Q_OS_WIN
	return "";
#else
	return QStringLiteral("\033[00m");
#endif
}
QString move(const MoveType type, const int n)
{
	if (!isTty()) {
		return QString();
	}
#ifdef Q_OS_WIN
	return "";
#else
	return QStringLiteral("\033[%1%2") % n % detail::getMoveTypeCode(type);
#endif
}
QString save()
{
	if (!isTty()) {
		return QString();
	}
#ifdef Q_OS_WIN
	return "";
#else
	return QStringLiteral("\033[s");
#endif
}
QString restore()
{
	if (!isTty()) {
		return QString();
	}
#ifdef Q_OS_WIN
	return "";
#else
	return QStringLiteral("\033[u");
#endif
}

bool isTty()
{
#ifdef Q_OS_WIN
	return ::_isatty(_fileno(stdout));
#else
	return ::isatty(fileno(stdout));
#endif
}

int currentWidth()
{
#ifdef Q_OS_UNIX
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
#else
	return 120;
#endif
}

QString wrap(const QString &text, const int maxWidth, const int indent)
{
	static const QRegularExpression breakExpression(QStringLiteral("[^a-zA-Z]"));

	Q_ASSERT(maxWidth > indent);
	const int realWidth = maxWidth - indent;

	// simplest case
	if (text.size() <= realWidth) {
		return text;
	}

	QStringList rows;
	int index = 0;
	while (index < text.size()) {
		QString row = text.mid(index);

		int breakIndex = row.lastIndexOf(QLatin1Char('\n'), realWidth);
		bool breakAtSpace = true;
		if (breakIndex == -1) {
			breakIndex = row.lastIndexOf(QLatin1Char(' '), realWidth);
		}
		if (breakIndex == -1) {
			breakIndex = row.lastIndexOf(breakExpression, realWidth);
			if (breakIndex >= 0) {
				breakAtSpace = row.at(breakIndex).isSpace();
			}
		}
		if (breakIndex <= 0) {
			breakIndex = realWidth;
		}
		row = row.mid(0, breakIndex);
		rows.append(row);

		index += breakAtSpace ? row.size() + 1 : row.size();
	}

	return rows.join(QLatin1Char('\n') + QString(indent, QLatin1Char(' ')));
}

static QVector<QVector<QString>> partitionRow(const QVector<QString> &columns, const QVector<int> &columnSizes)
{
	Q_ASSERT(columns.size() == columnSizes.size());

	QVector<QVector<QString>> wrapped;
	for (int i = 0; i < columns.size(); ++i) {
		// perform wrapping and split at new lines
		wrapped.append(wrap(columns.at(i), columnSizes.at(i)).split(QLatin1Char('\n')).toVector());
	}

	// how many rows to we have at most in a single column?
	const int numRows = Functional::collection(wrapped).mapSize().max();

	// wrapped is a list of columns, but we need a list of rows
	// this is just a "normal" row-major => column-major conversion
	QVector<QVector<QString>> result;
	result.resize(numRows);
	for (int i = 0; i < numRows; ++i) {
		result[i].resize(wrapped.size());
		for (int j = 0; j < wrapped.size(); ++j) {
			// if one cell does not have any more rows we use an empty string
			result[i][j] = (i >= wrapped[j].size()) ? QString() : wrapped.at(j).at(i);
		}
	}

	return result;
}
QString table(const QVector<QVector<QString> > &rows, const QVector<int> &columnSizeRatios, const int maxWidth, const int indent)
{
	Q_ASSERT(maxWidth > indent);
	const int realWidth = maxWidth - indent;

	QVector<int> columnSizes(columnSizeRatios.size());
	for (const QVector<QString> &row : rows) {
		Q_ASSERT(row.size() == columnSizeRatios.size());
		for (int i = 0; i < row.size(); ++i) {
			columnSizes[i] = std::max(columnSizes.at(i), row.at(i).size());
		}
	}

	const int requiredPadding = columnSizes.size() - 1;
	const int totalRequiredColumnSizes = collection(columnSizes).sum() + requiredPadding;
	if (totalRequiredColumnSizes > realWidth) {
		const int totalRatio = collection(columnSizeRatios).sum();
		columnSizes = collection(columnSizeRatios)
				.map([realWidth, totalRatio, requiredPadding](const int ratio) { return std::min(1, int(std::floor(qreal(ratio) / qreal(totalRatio) * qreal(realWidth-requiredPadding)))); });

		// due to the flooring we might end up with some left over space. give it to the last column
		const int newWidth = collection(columnSizes).sum() + requiredPadding;
		columnSizes.last() += realWidth - newWidth;
	}

	return collection(rows)
			.map([columnSizes, indent](const QVector<QString> &row)
	{
		return collection(partitionRow(row, columnSizes))
				.map([columnSizes](const QVector<QString> &partitionedRow)
		{
			QStringList res;
			for (int i = 0; i < partitionedRow.size(); ++i) {
				const int padding = columnSizes.at(i) - partitionedRow.at(i).size();
				res.append(partitionedRow.at(i) + QString(padding, QLatin1Char(' ')));
			}
			return res.join(QLatin1Char(' '));
		})
				.join(QLatin1Char('\n') + QString(indent, QLatin1Char(' ')));
	})
			.join(QLatin1Char('\n') + QString(indent, QLatin1Char(' ')));
}

QString readPassword()
{
	std::string str;
	detail::setStdinEcho(false);
	std::getline(std::cin, str);
	std::cout << '\n';
	detail::setStdinEcho(true);
	return QString::fromStdString(str);
}

bool askBoolean(const QString &question, const bool default_)
{
	while (true) {
		std::cout << question << (default_ ? " [Y/n]: " : " [y/N]: ");
		std::string answer;
		std::cin >> answer;
		if (answer.size() == 0 || answer.at(0) == '\n') {
			return default_;
		} else if (std::tolower(answer.at(0)) == 'y') {
			return true;
		} else if (std::tolower(answer.at(1)) == 'n') {
			return false;
		}
	}
}

QString askString(const QString &question, const QString &default_)
{
	std::cout << question << " [" << default_ << "]: ";
	std::string answer;
	std::cin >> answer;
	return answer.empty() ? default_ : QString::fromStdString(answer);
}

}
}
}
