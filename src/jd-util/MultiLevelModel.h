#pragma once

#include <QVector>

class QAbstractItemModel;
class QAbstractProxyModel;
class QIdentityProxyModel;
class QModelIndex;

namespace JD {
namespace Util {

class MultiLevelModel
{
public:
	explicit MultiLevelModel(QAbstractItemModel *source);
	~MultiLevelModel();

	/// Adds a proxy to the end of the chain, closest to the view
	MultiLevelModel &append(QAbstractProxyModel *proxy);
	/// Adds a proxy to the beginning of the chain, closest to the source
	MultiLevelModel &prepend(QAbstractProxyModel *proxy);

	QAbstractItemModel *result() const;

	QModelIndex mapToSource(const QModelIndex &viewIndex) const;
	QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

private:
	MultiLevelModel(const MultiLevelModel &other) = delete;

	QAbstractItemModel *m_source;
	QIdentityProxyModel *m_model;
	QVector<QAbstractProxyModel *> m_proxies;
};

}
}
