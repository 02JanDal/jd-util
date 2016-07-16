#pragma once

class QString;

namespace JD {
namespace Util {
namespace Introspection {
namespace detail {
QString extractClass(const char *func);
}
}
}
}

#define INTROSPECTION_X(classname) \
	public: \
	static QString staticClassName() { return QStringLiteral(#classname); } \
	QT_WARNING_PUSH \
	QT_WARNING_DISABLE_CLANG("-Winconsistent-missing-override") \
	virtual QString className() const { return staticClassName(); } \
	QT_WARNING_POP \
	private:

#define INTROSPECTION \
	public: \
	static QString staticClassName() { return ::JD::Util::Introspection::detail::extractClass(__PRETTY_FUNCTION__); } \
	QT_WARNING_PUSH \
	QT_WARNING_DISABLE_CLANG("-Winconsistent-missing-override") \
	virtual QString className() const { return staticClassName(); } \
	QT_WARNING_POP \
	private:
