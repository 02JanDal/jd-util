/* Copyright (c) 2005-2017 David Heinemeier Hansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This code is a port of the ActiveSupport::Inflector code from the Ruby on
 * Rails project, and thus subject to the above license. The code was ported by
 * Jan Dalheimer in 2017. The full original license can be found here:
 *
 *   https://github.com/rails/rails/blob/37d956f/activesupport/MIT-LICENSE
 *
 */

#include "Inflector.h"

#include <QRegularExpression>
#include <QVector>
#include <QSet>
#include <QDebug>
#include <cmath>

#include "functional/Collection.h"
#include "Util.h"

namespace JD {
namespace Util {

struct InflectorData
{
	using Rule = QPair<QRegularExpression, QString>;
	using Rules = QVector<Rule>;

	QVector<QString> acronyms;
	QVector<QString> acronymsLower;
	QString acronymString;
	QSet<QString> uncountable;
	Rules plurals;
	Rules singulars;

	QString applyInflections(const QString &word, const Rules &rules)
	{
		if (word.isEmpty() || uncountable.contains(word)) {
			return word;
		} else {
			for (auto it = rules.begin(); it != rules.end(); ++it) {
				const Rule &rule = *it;
				if (rule.first.match(word).hasMatch()) {
					return QString(word).replace(rule.first, rule.second);
				}
			}
			return word;
		}
	}
};

QHash<QString, InflectorData *> Inflector::m_locales;

Inflector::Inflector(const QString &locale)
{
	if (!m_locales.contains(locale)) {
		m_locales.insert(locale, new InflectorData);
	}
	m_data = m_locales.value(locale);
}

QString Inflector::pluralize(const QString &word) const
{
	return m_data->applyInflections(word, m_data->plurals);
}
QString Inflector::singularize(const QString &word) const
{
	return m_data->applyInflections(word, m_data->singulars);
}

inline QString capitalize(const QString &str)
{
	if (str.isEmpty()) {
		return str;
	} else if (str.size() == 1) {
		return str.toUpper();
	} else {
		return str.at(0).toUpper() + str.mid(1).toLower();
	}
}
QString Inflector::camelize(const QString &word, const bool upperFirstLetter) const
{
	static const QRegularExpression expA(QStringLiteral("^[a-z\\d]*"));
	static const QRegularExpression expC(QStringLiteral("(?:_|(/))([a-z\\d]*)"));

	QString string = word;
	if (upperFirstLetter) {
		string = regexReplace(string, expA, [this](const QString &str) {
			if (m_data->acronymsLower.contains(str.toLower())) {
				return m_data->acronyms.at(m_data->acronymsLower.indexOf(str.toLower()));
			} else {
				return capitalize(str);
			}
		});
	} else {
		const QRegularExpression expB(QStringLiteral("(?:") + m_data->acronymString + QStringLiteral("(?=\\b|[A-Z_])|\\w)"));
		string = regexReplace(string, expB, [](const QString &str) { return str.toLower(); });
	}

	string = regexReplace(string, expC, [this](const QRegularExpressionMatch &match) {
		const int acronymIndex = m_data->acronymsLower.indexOf(match.captured(2).toLower());
		if (acronymIndex != -1) {
			return match.captured(1) + m_data->acronyms.at(acronymIndex);
		} else {
			return match.captured(1) + capitalize(match.captured(2));
		}
	});

	return string.replace(QLatin1Char('/'), QStringLiteral("::"));
}
QString Inflector::underscore(const QString &word) const
{
	static const QRegularExpression expA(QStringLiteral("([A-Z-]|::)"));
	if (!word.contains(expA)) {
		return word;
	}

	QString string = word;
	string = string.replace(QStringLiteral("::"), QStringLiteral("/"));

	const QRegularExpression expB(QStringLiteral("(?:([A-Za-z\\d])|\\b)(") + m_data->acronymString + QStringLiteral(")(?=\\b|[^a-z])"));
	string = regexReplace(string, expB, [](const QRegularExpressionMatch &match) {
		return (match.captured(1).isEmpty() ? QStringLiteral("") : (match.captured(1) + QLatin1Char('_'))) + match.captured(2);
	});

	static const QRegularExpression expC(QStringLiteral("([A-Z\\d]+)([A-Z][a-z])"));
	static const QRegularExpression expD(QStringLiteral("([a-z\\d])([A-Z])"));
	string = string
			.replace(expC, QStringLiteral("\\1_\\2"))
			.replace(expD, QStringLiteral("\\1_\\2"))
			.replace(QLatin1Char('-'), QLatin1Char('_'))
			.toLower();

	return string;
}

QString Inflector::tableize(const QString &className) const
{
	return pluralize(underscore(className));
}
QString Inflector::classify(const QString &tableName) const
{
	const int point = tableName.indexOf(QLatin1Char('.'));
	return camelize(singularize(point == -1 ? tableName : tableName.mid(point+1)));
}
QString Inflector::foreignKey(const QString &className) const
{
	return underscore(className) + QStringLiteral("_id");
}

QString Inflector::ordinal(const int number) const
{
	const int abs = std::abs(double(number));
	const int modulo100 = abs % 100;
	const int modulo10 = abs % 10;
	if (modulo100 == 11 || modulo100 == 12 || modulo100 == 13) {
		return QStringLiteral("th");
	} else if (modulo10 == 1) {
		return QStringLiteral("st");
	} else if (modulo10 == 2) {
		return QStringLiteral("nd");
	} else if (modulo10 == 3) {
		return QStringLiteral("rd");
	} else {
		return QStringLiteral("th");
	}
}

void Inflector::registerAcronym(const QString &word)
{
	m_data->acronyms.append(word);
	m_data->acronymsLower.append(word.toLower());
	m_data->acronymString = Functional::collection(m_data->acronyms).join(QLatin1Char('|'));
}
void Inflector::registerPlural(const QString &rule, const QString &replacement, const bool caseInsensitive)
{
	m_data->plurals.prepend(qMakePair(QRegularExpression(rule,
														caseInsensitive ?
															QRegularExpression::CaseInsensitiveOption
														  : QRegularExpression::NoPatternOption),
									 replacement));
}
void Inflector::registerSingular(const QString &rule, const QString &replacement, const bool caseInsensitive)
{
	m_data->singulars.prepend(qMakePair(QRegularExpression(rule,
														  caseInsensitive ?
															  QRegularExpression::CaseInsensitiveOption
															: QRegularExpression::NoPatternOption),
									   replacement));
}
void Inflector::registerIrregular(const QString &singular, const QString &plural)
{
	const QChar s0 = singular.at(0);
	const QString srest = singular.mid(1);

	const QChar p0 = plural.at(0);
	const QString prest = plural.mid(1);

	if (s0.toUpper() == p0.toUpper()) {
		const QString templ = QStringLiteral("(%1)%2$");
		registerPlural(templ.arg(s0, srest), QStringLiteral("\\1") + prest);
		registerPlural(templ.arg(p0, prest), QStringLiteral("\\1") + prest);
		registerSingular(templ.arg(s0, srest), QStringLiteral("\\1") + srest);
		registerSingular(templ.arg(p0, prest), QStringLiteral("\\1") + srest);
	} else {
		const QString templ = QStringLiteral("%1(?i)%2$");

		registerPlural(templ.arg(s0.toUpper(), srest), p0.toUpper() + prest, false);
		registerPlural(templ.arg(s0.toLower(), srest), p0.toLower() + prest, false);
		registerPlural(templ.arg(p0.toUpper(), srest), p0.toUpper() + prest, false);
		registerPlural(templ.arg(p0.toLower(), srest), p0.toLower() + prest, false);

		registerPlural(templ.arg(s0.toUpper(), srest), s0.toUpper() + srest, false);
		registerPlural(templ.arg(s0.toLower(), srest), s0.toLower() + srest, false);
		registerPlural(templ.arg(p0.toUpper(), srest), s0.toUpper() + srest, false);
		registerPlural(templ.arg(p0.toLower(), srest), s0.toLower() + srest, false);
	}
}
void Inflector::registerUncountable(const QString &word)
{
	m_data->uncountable.insert(word);
}

void Inflector::optimize()
{
	for (auto it = m_data->plurals.begin(); it != m_data->plurals.end(); ++it) {
		(*it).first.optimize();
	}
	for (auto it = m_data->singulars.begin(); it != m_data->singulars.end(); ++it) {
		(*it).first.optimize();
	}
}

void Inflector::clear()
{
	m_data->acronyms.clear();
	m_data->acronymsLower.clear();
	m_data->acronymString.clear();
	m_data->plurals.clear();
	m_data->singulars.clear();
	m_data->uncountable.clear();
}
bool Inflector::isEmpty() const
{
	return m_data->acronyms.isEmpty() && m_data->plurals.isEmpty() && m_data->singulars.isEmpty() && m_data->uncountable.isEmpty();
}

void Inflector::registerEN()
{
	if (hasLocale(QStringLiteral("en")) && !Inflector(QStringLiteral("en")).isEmpty()) {
		return;
	}

	Inflector inf(QStringLiteral("en"));
	inf.registerPlural(QStringLiteral("$"), QStringLiteral("s"));
	inf.registerPlural(QStringLiteral("s$"), QStringLiteral("s"));
	inf.registerPlural(QStringLiteral("^(ax|test)is$"), QStringLiteral("\\1es"));
	inf.registerPlural(QStringLiteral("(octop|vir)us$"), QStringLiteral("\\1i"));
	inf.registerPlural(QStringLiteral("(octop|vir)i$"), QStringLiteral("\\1i"));
	inf.registerPlural(QStringLiteral("(alias|status)$"), QStringLiteral("\\1es"));
	inf.registerPlural(QStringLiteral("(bu)s$"), QStringLiteral("\\1ses"));
	inf.registerPlural(QStringLiteral("(buffal|tomat)o$"), QStringLiteral("\\1oes"));
	inf.registerPlural(QStringLiteral("([ti])um$"), QStringLiteral("\\1a"));
	inf.registerPlural(QStringLiteral("([ti])a$"), QStringLiteral("\\1a"));
	inf.registerPlural(QStringLiteral("sis$"), QStringLiteral("ses"));
	inf.registerPlural(QStringLiteral("(?:([^f])fe|([lr])f)$"), QStringLiteral("\\1\\2ves"));
	inf.registerPlural(QStringLiteral("(hive)$"), QStringLiteral("\\1s"));
	inf.registerPlural(QStringLiteral("([^aeiouy]|qu)y$"), QStringLiteral("\\1ies"));
	inf.registerPlural(QStringLiteral("(x|ch|ss|sh)$"), QStringLiteral("\\1es"));
	inf.registerPlural(QStringLiteral("(matr|vert|ind)(?:ix|ex)$"), QStringLiteral("\\1ices"));
	inf.registerPlural(QStringLiteral("^(m|l)ouse$"), QStringLiteral("\\1ice"));
	inf.registerPlural(QStringLiteral("^(m|l)ice$"), QStringLiteral("\\1ice"));
	inf.registerPlural(QStringLiteral("^(ox)$"), QStringLiteral("\\1en"));
	inf.registerPlural(QStringLiteral("^(oxen)$"), QStringLiteral("\\1"));
	inf.registerPlural(QStringLiteral("(quiz)$"), QStringLiteral("\\1zes"));

	inf.registerSingular(QStringLiteral("s$"), QStringLiteral(""));
	inf.registerSingular(QStringLiteral("(ss)$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("(n)ews$"), QStringLiteral("\\1ews"));
	inf.registerSingular(QStringLiteral("([ti])a$"), QStringLiteral("\\1um"));
	inf.registerSingular(QStringLiteral("((a)naly|(b)a|(d)iagno|(p)arenthe|(p)rogno|(s)ynop|(t)he)(sis|ses)$"), QStringLiteral("\\1sis"));
	inf.registerSingular(QStringLiteral("(^analy)(sis|ses)$"), QStringLiteral("\\1sis"));
	inf.registerSingular(QStringLiteral("([^f])ves$"), QStringLiteral("\\1fe"));
	inf.registerSingular(QStringLiteral("(hive)s$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("(tive)s$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("([lr])ves$"), QStringLiteral("\\1f"));
	inf.registerSingular(QStringLiteral("([^aeiouy]|qu)ies$"), QStringLiteral("\\1y"));
	inf.registerSingular(QStringLiteral("(s)eries$"), QStringLiteral("\\1eries"));
	inf.registerSingular(QStringLiteral("(m)ovies$"), QStringLiteral("\\1ovie"));
	inf.registerSingular(QStringLiteral("(x|ch|ss|sh)es$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("^(m|l)ice$"), QStringLiteral("\\1ouse"));
	inf.registerSingular(QStringLiteral("(bus)(es)?$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("(o)es$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("(shoe)s$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("(cris|test)(is|es)$"), QStringLiteral("\\1is"));
	inf.registerSingular(QStringLiteral("^(a)x[ie]s$"), QStringLiteral("\\1xis"));
	inf.registerSingular(QStringLiteral("(octop|vir)(us|i)$"), QStringLiteral("\\1us"));
	inf.registerSingular(QStringLiteral("(alias|status)(es)?$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("^(ox)en"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("(vert|ind)ices$"), QStringLiteral("\\1ex"));
	inf.registerSingular(QStringLiteral("(matr)ices$"), QStringLiteral("\\1ix"));
	inf.registerSingular(QStringLiteral("(quiz)zes$"), QStringLiteral("\\1"));
	inf.registerSingular(QStringLiteral("(database)s$"), QStringLiteral("\\1"));

	inf.registerIrregular(QStringLiteral("person"), QStringLiteral("people"));
	inf.registerIrregular(QStringLiteral("man"), QStringLiteral("men"));
	inf.registerIrregular(QStringLiteral("child"), QStringLiteral("children"));
	inf.registerIrregular(QStringLiteral("sex"), QStringLiteral("sexes"));
	inf.registerIrregular(QStringLiteral("move"), QStringLiteral("moves"));
	inf.registerIrregular(QStringLiteral("zombie"), QStringLiteral("zombies"));

	inf.registerUncountable(QStringLiteral("equipment"));
	inf.registerUncountable(QStringLiteral("information"));
	inf.registerUncountable(QStringLiteral("rice"));
	inf.registerUncountable(QStringLiteral("money"));
	inf.registerUncountable(QStringLiteral("species"));
	inf.registerUncountable(QStringLiteral("series"));
	inf.registerUncountable(QStringLiteral("fish"));
	inf.registerUncountable(QStringLiteral("sheep"));
	inf.registerUncountable(QStringLiteral("jeans"));
	inf.registerUncountable(QStringLiteral("police"));

	inf.registerAcronym(QStringLiteral("API"));
	inf.registerAcronym(QStringLiteral("HTML"));
	inf.registerAcronym(QStringLiteral("HTTP"));
	inf.registerAcronym(QStringLiteral("REST"));
	inf.registerAcronym(QStringLiteral("PhD"));
	inf.registerAcronym(QStringLiteral("SSL"));
	inf.registerAcronym(QStringLiteral("GIS"));

	inf.optimize();
}

bool Inflector::hasLocale(const QString &locale)
{
	return m_locales.contains(locale);
}

}
}
