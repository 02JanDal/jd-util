#pragma once

#include <jd-util/Exception.h>

#include <QSqlDatabase>

class QSqlQuery;
class QVariant;

namespace JD {
namespace Util {
namespace Database {

DECLARE_EXCEPTION_X(Database, QStringLiteral("Database error"), ::Exception)
DECLARE_EXCEPTION_X(Query, QString(), DatabaseException)
DECLARE_EXCEPTION_X(Prepare, QString(), DatabaseException)
DECLARE_EXCEPTION_X(Transaction, QString(), DatabaseException)
DECLARE_EXCEPTION_X(DoesntExist, QStringLiteral("Requested record does not exist"), QueryException)

QSqlQuery prepare(const QString &string, QSqlDatabase db = QSqlDatabase::database());
QSqlQuery exec(const QSqlQuery &query);
QSqlQuery exec(const QString &query);
QVector<QVariant> execOne(const QSqlQuery &query);
QVector<QVariant> execOne(const QString &query);

void dump(QSqlQuery q);

class Dialect
{
public:
	explicit Dialect(QSqlDatabase db);

	enum IntSize
	{
		Small,
		Default,
		Big
	};

	QString idField(const IntSize size = Default, const QString &name = QStringLiteral("id")) const;

private:
	QSqlDatabase m_db;
};

class TransactionLocker
{
public:
	explicit TransactionLocker(QSqlDatabase db);
	~TransactionLocker();

	void begin();
	void commit();
	void rollback();

private:
	QSqlDatabase m_db;
	bool m_finished = true;
};

}
}
}
