#include "MultiLevelModel.h"

#include <QAbstractProxyModel>
#include <QIdentityProxyModel>

#include "Util.h"

namespace JD {
namespace Util {

MultiLevelModel::MultiLevelModel(QAbstractItemModel *source)
	: m_source(source), m_model(new QIdentityProxyModel)
{
	m_model->setSourceModel(m_source);
	m_proxies.append(m_model);
}
MultiLevelModel::~MultiLevelModel()
{
	delete m_model;
}

MultiLevelModel &MultiLevelModel::append(QAbstractProxyModel *proxy)
{
	m_model->setSourceModel(proxy);
	proxy->setSourceModel(m_proxies.last());
	m_proxies.append(proxy);
	return *this;
}
MultiLevelModel &MultiLevelModel::prepend(QAbstractProxyModel *proxy)
{
	proxy->setSourceModel(m_source);
	m_proxies.first()->setSourceModel(proxy);
	m_proxies.prepend(proxy);
	return *this;
}

QAbstractItemModel *MultiLevelModel::result() const
{
	return m_model;
}

QModelIndex MultiLevelModel::mapToSource(const QModelIndex &viewIndex) const
{
	QModelIndex index = viewIndex;
	for (QAbstractProxyModel *proxy : reverse(m_proxies)) {
		index = proxy->mapToSource(index);
	}
	return index;
}
QModelIndex MultiLevelModel::mapFromSource(const QModelIndex &sourceIndex) const
{
	QModelIndex index = sourceIndex;
	for (QAbstractProxyModel *proxy : m_proxies) {
		index = proxy->mapFromSource(index);
	}
	return index;
}

}
}
