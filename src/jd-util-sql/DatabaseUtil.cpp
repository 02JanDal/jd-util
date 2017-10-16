#include "DatabaseUtil.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlField>
#include <QVector>
#include <QVariant>
#include <QDebug>
#include <iostream>

#include "jd-util/TermUtil.h"

namespace JD {
namespace Util {
namespace Database {

QSqlQuery exec(const QSqlQuery &q)
{
	QSqlQuery query = q;
	if (!q.isActive()) {
		query.exec();
	}
	if (query.lastError().isValid()) {
		throw QueryException(query.lastError().text());
	}
	return query;
}
QSqlQuery exec(const QString &query)
{
	return exec(QSqlQuery(query));
}

QVector<QVariant> execOne(const QSqlQuery &q)
{
	QSqlQuery query = exec(q);
	Q_ASSERT_X(query.isSelect(), "JD::Util::Database::execOne", "Only SELECT queries are supported");
	if (!query.next()) {
		throw DoesntExistException();
	}
	const int columns = query.record().count();
	QVector<QVariant> fields(columns);
	for (int i = 0; i < columns; ++i) {
		fields[i] = query.value(i);
	}
	return fields;
}
QVector<QVariant> execOne(const QString &query)
{
	return execOne(QSqlQuery(query));
}

TransactionLocker::TransactionLocker(QSqlDatabase db)
	: m_db(db)
{
	begin();
}

TransactionLocker::~TransactionLocker()
{
	if (!m_finished) {
		rollback();
	}
}

void TransactionLocker::begin()
{
	if (!m_finished) {
		throw TransactionException(QStringLiteral("Already in a transaction"));
	}
	if (!m_db.transaction()) {
		throw TransactionException(QStringLiteral("Unable to begin transaction"));
	}
	m_finished = false;
}
void TransactionLocker::commit()
{
	if (m_finished) {
		throw TransactionException(QStringLiteral("Transaction already completed"));
	}
	if (!m_db.commit()) {
		throw TransactionException(QStringLiteral("Unable to commit transaction"));
	}
	m_finished = true;
}
void TransactionLocker::rollback()
{
	if (m_finished) {
		throw TransactionException(QStringLiteral("Transaction already completed"));
	}
	if (!m_db.rollback()) {
		throw TransactionException(QStringLiteral("Unable to roll back transaction"));
	}
	m_finished = true;
}

QSqlQuery prepare(const QString &string, QSqlDatabase db)
{
	QSqlQuery query(db);
	if (!query.prepare(string)) {
		throw PrepareException(QStringLiteral("Unable to prepare query '%1': %2").arg(string, query.lastError().text()));
	}
	return query;
}

Dialect::Dialect(QSqlDatabase db)
	: m_db(db) {}

QString Dialect::idField(const IntSize size, const QString &name) const
{
	QString intPrefix;
	switch (size) {
	case Small: intPrefix = QStringLiteral("SMALL"); break;
	case Default: intPrefix = QString(); break;
	case Big: intPrefix = QStringLiteral("BIG"); break;
	}

	switch (m_db.driver()->dbmsType()) {
	case QSqlDriver::UnknownDbms: throw NotImplementedException();
	case QSqlDriver::MSSqlServer: throw NotImplementedException();
	case QSqlDriver::MySqlServer: return QStringLiteral("%1 UNSIGNED %2INT NOT NULL AUTO_INCREMENT, PRIMARY KEY(%1)") % name % intPrefix;
	case QSqlDriver::PostgreSQL: return QStringLiteral("%1 %2SERIAL NOT NULL PRIMARY KEY") % name % intPrefix;
	case QSqlDriver::Oracle: throw NotImplementedException();
	case QSqlDriver::Sybase: throw NotImplementedException();
	case QSqlDriver::SQLite: return QStringLiteral("%1 INTEGER PRIMARY KEY") % name;
	case QSqlDriver::Interbase: throw NotImplementedException();
	case QSqlDriver::DB2: throw NotImplementedException();
	}
}

void dump(QSqlQuery q)
{
	q = exec(q);

	QVector<QVector<QString>> tbl;
	QVector<QString> headerRow;
	QVector<int> ratios;

	while (q.next()) {
		QVector<QString> row;
		for (int i = 0; q.record().field(i).isValid(); ++i) {
			if (tbl.isEmpty()) {
				headerRow.append(q.record().fieldName(i));
				if (q.record().fieldName(i) == QStringLiteral("id")) {
					ratios.append(1);
				} else {
					ratios.append(5);
				}
			}
			row.append(q.value(i).toString());
		}

		if (tbl.isEmpty()) {
			tbl.append(headerRow);
		}
		tbl.append(row);
	}

	std::cout << Term::table(tbl, ratios, Term::currentWidth()) << std::endl;
}

}
}
}
