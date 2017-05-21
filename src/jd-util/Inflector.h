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

#pragma once

#include <QHash>

namespace JD {
namespace Util {

class Inflector
{
public:
	explicit Inflector(const QString &locale = QStringLiteral("en"));

	QString pluralize(const QString &word) const;
	QString singularize(const QString &word) const;

	QString camelize(const QString &word, const bool upperFirstLetter = true) const;
	QString underscore(const QString &word) const;

	QString tableize(const QString &className) const;
	QString classify(const QString &tableName) const;
	QString foreignKey(const QString &className) const;

	QString ordinal(const int number) const;

	void registerAcronym(const QString &word);
	void registerPlural(const QString &rule, const QString &replacement, const bool caseInsensitive = true);
	void registerSingular(const QString &rule, const QString &replacement, const bool caseInsensitive = true);
	void registerIrregular(const QString &singular, const QString &plural);
	void registerUncountable(const QString &word);
	void optimize();
	void clear();
	bool isEmpty() const;

	static void registerEN();
	static bool hasLocale(const QString &locale);

private:
	static QHash<QString, struct InflectorData *> m_locales;
	struct InflectorData *m_data;
};

}
}
