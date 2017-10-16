#pragma once

#include <QString>
#include <QVector>
#include <QVariant>

class QSqlQuery;
class QSqlDatabase;

namespace JD {
namespace Util {
namespace Database {

enum class Boolean
{
	And,
	Or
};
enum class Operator
{
	Equal,
	NotEqual,
	LessThan,
	GreaterThan,
	LessThanEqual,
	GreaterThanEqual,
	Contains
};
enum class JoinType
{
	InnerJoin,
	LeftOuterJoin,
	FullJoin
};

class WhereConditionBuilder
{
public:
	explicit WhereConditionBuilder(const QString &defaultTable, const Boolean boolean);
	WhereConditionBuilder() {}

	WhereConditionBuilder &add(const QString &field, const Operator op, const QVariant &value);
	WhereConditionBuilder &add(const QString &table, const QString &field, const Operator op, const QVariant &value);
	WhereConditionBuilder &add(const WhereConditionBuilder &other);

	bool isEmpty() const;

	QPair<QString, QVector<QVariant>> build() const;

private:
	QString m_defaultTable;
	Boolean m_boolean;

	struct Condition
	{
		QString table;
		QString field;
		Operator op;
		QVariant value;
	};
	QVector<Condition> m_conditions;
	QVector<WhereConditionBuilder> m_subBuilders;
};

class SelectBuilder
{
public:
	explicit SelectBuilder(const QString &table);

	SelectBuilder &onlyFields(const QVector<QString> &fields);
	SelectBuilder &where(const QString &field, const Operator op, const QVariant &value);
	SelectBuilder &where(const WhereConditionBuilder &where);
	WhereConditionBuilder &where();

	SelectBuilder &order(const QVector<QString> &fields, const Qt::SortOrder dir);
	SelectBuilder &groupBy(const QString &field);
	SelectBuilder &having(const QString &field, const Operator op, const QVariant &value);
	SelectBuilder &having(const WhereConditionBuilder &where);

	SelectBuilder &join(const JoinType type, const QString &ourField, const QString &otherTable, const QString &otherField);
	SelectBuilder &join(const JoinType type, const QString &tableA, const QString &fieldA,
						const QString &tableB, const QString &fieldB);

	QPair<QString, QVector<QVariant>> buildRaw(QSqlDatabase &db);
	QSqlQuery build(QSqlDatabase &db);

private:
	QString m_mainTable;
	QVector<QString> m_fields;
	WhereConditionBuilder m_where;
	QPair<QVector<QString>, Qt::SortOrder> m_order;
	QString m_groupBy;
	WhereConditionBuilder m_having;

	struct Join
	{
		JoinType type;
		QString tableA;
		QString fieldA;
		QString tableB;
		QString fieldB;
	};
	QVector<Join> m_joins;
};

}
}
}
