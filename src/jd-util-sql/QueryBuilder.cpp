#include "QueryBuilder.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include <jd-util/Functional.h>

#include "DatabaseUtil.h"

namespace JD {
namespace Util {
namespace Database {

WhereConditionBuilder::WhereConditionBuilder(const QString &defaultTable, const Boolean boolean)
	: m_defaultTable(defaultTable), m_boolean(boolean) {}

WhereConditionBuilder &WhereConditionBuilder::add(const QString &field, const Operator op, const QVariant &value)
{
	return add(m_defaultTable, field, op, value);
}
WhereConditionBuilder &WhereConditionBuilder::add(const QString &table, const QString &field, const Operator op, const QVariant &value)
{
	m_conditions.append(Condition{table, field, op, value});
	return *this;
}

WhereConditionBuilder &WhereConditionBuilder::add(const WhereConditionBuilder &other)
{
	m_subBuilders.append(other);
	return *this;
}

bool WhereConditionBuilder::isEmpty() const
{
	return m_conditions.isEmpty() && m_subBuilders.isEmpty();
}

QPair<QString, QVector<QVariant> > WhereConditionBuilder::build() const
{
	QStringList conditions;
	QVector<QVariant> values;

	for (const Condition &cond : m_conditions) {
		QString field = cond.table + QLatin1Char('.') + cond.field;
		switch (cond.op) {
		case Operator::Equal: conditions.append(field + QStringLiteral(" = ?")); break;
		case Operator::NotEqual: conditions.append(field + QStringLiteral(" <> ?")); break;
		case Operator::LessThan: conditions.append(field + QStringLiteral(" < ?")); break;
		case Operator::GreaterThan: conditions.append(field + QStringLiteral(" > ?")); break;
		case Operator::LessThanEqual: conditions.append(field + QStringLiteral(" <= ?")); break;
		case Operator::GreaterThanEqual: conditions.append(field + QStringLiteral(" >= ?")); break;
		case Operator::Contains: conditions.append(field + QStringLiteral(" LIKE CONCAT('%', ?, '%')")); break;
		}

		values.append(cond.value);
	}

	for (const WhereConditionBuilder &sub : m_subBuilders) {
		const auto built = sub.build();
		conditions.append(QLatin1Char('(') + built.first + QLatin1Char(')'));
		values.append(built.second);
	}

	return qMakePair(conditions.join(m_boolean == Boolean::And ? QStringLiteral(" AND ") : QStringLiteral(" OR ")), values);
}

SelectBuilder::SelectBuilder(const QString &table)
	: m_mainTable(table), m_where(table, Boolean::And), m_having(table, Boolean::And) {}

SelectBuilder &SelectBuilder::onlyFields(const QVector<QString> &fields)
{
	m_fields = fields;
	return *this;
}

SelectBuilder &SelectBuilder::where(const QString &field, const Operator op, const QVariant &value)
{
	m_where.add(field, op, value);
	return *this;
}
SelectBuilder &SelectBuilder::where(const WhereConditionBuilder &where)
{
	m_where.add(where);
	return *this;
}
WhereConditionBuilder &SelectBuilder::where()
{
	return m_where;
}

SelectBuilder &SelectBuilder::order(const QVector<QString> &fields, const Qt::SortOrder dir)
{
	m_order = qMakePair(fields, dir);
	return *this;
}
SelectBuilder &SelectBuilder::groupBy(const QString &field)
{
	m_groupBy = field;
	return *this;
}
SelectBuilder &SelectBuilder::having(const QString &field, const Operator op, const QVariant &value)
{
	m_having.add(field, op, value);
	return *this;
}
SelectBuilder &SelectBuilder::having(const WhereConditionBuilder &where)
{
	m_having.add(where);
	return *this;
}

SelectBuilder &SelectBuilder::join(const JoinType type, const QString &ourField, const QString &otherTable, const QString &otherField)
{
	return join(type, m_mainTable, ourField, otherTable, otherField);
}
SelectBuilder &SelectBuilder::join(const JoinType type, const QString &tableA, const QString &fieldA, const QString &tableB, const QString &fieldB)
{
	m_joins.append(Join{type, tableA, fieldA, tableB, fieldB});
	return *this;
}

QPair<QString, QVector<QVariant>> SelectBuilder::buildRaw(QSqlDatabase &db)
{
	auto fullField = [this](const QString &field) {
		if (field.contains(QLatin1Char('.')) || field.contains(QLatin1Char('('))) {
			return field;
		} else {
			return m_mainTable + QLatin1Char('.') + field;
		}
	};

	QString str = QStringLiteral("SELECT");
	QVector<QVariant> values;

	if (m_fields.isEmpty()) {
		str += QLatin1Char(' ') + m_mainTable + QStringLiteral(".*");
	} else {
		QStringList fields;
		for (const QString &field : m_fields) {
			fields.append(fullField(field));
		}
		str += fields.join(QLatin1Char(','));
	}

	str += QStringLiteral(" FROM ") + m_mainTable;

	if (!m_where.isEmpty()) {
		const auto built = m_where.build();
		str += QStringLiteral(" WHERE ") + built.first;
		values.append(built.second);
	}

	for (const Join &join : m_joins) {
		switch (join.type) {
		case JoinType::InnerJoin: str += QStringLiteral(" INNER JOIN"); break;
		case JoinType::LeftOuterJoin: str += QStringLiteral(" LEFT OUTER JOIN"); break;
		case JoinType::FullJoin: str += QStringLiteral(" FULL JOIN"); break;
		}
		str + QStringLiteral(" %1 ON %3.%4 = %1.%2").arg(join.tableA, join.fieldA, join.tableB, join.fieldB);
	}

	if (!m_groupBy.isEmpty()) {
		str += QStringLiteral(" GROUP BY ") + fullField(m_groupBy);

		if (!m_having.isEmpty()) {
			const auto built = m_having.build();
			str += QStringLiteral(" HAVING (") + built.first + QLatin1Char(')');
			values.append(built.second);
		}
	}

	if (!m_order.first.isEmpty()) {
		str += QStringLiteral(" ORDER BY ") + Functional::collection(m_order.first).map(fullField).join(QLatin1Char(','));
		if (m_order.second == Qt::AscendingOrder) {
			str += QStringLiteral(" ASC");
		} else {
			str += QStringLiteral(" DESC");
		}
	}

	return qMakePair(str, values);
}

QSqlQuery SelectBuilder::build(QSqlDatabase &db)
{
	const auto built = buildRaw(db);
	QSqlQuery q = prepare(built.first, db);
	for (const QVariant &val : built.second) {
		q.addBindValue(val);
	}
	return q;
}

}
}
}
