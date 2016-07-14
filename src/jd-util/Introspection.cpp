#include "Introspection.h"

#include <QString>

QString JD::Util::Introspection::detail::extractClass(const char *func)
{
	const QString in = func;
	return in.left(in.lastIndexOf(':')-1).mid(in.lastIndexOf(' ')+1);
}
