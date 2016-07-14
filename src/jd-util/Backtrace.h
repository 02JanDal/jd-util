#pragma once

#include <QString>

namespace JD {
namespace Util {
QVector<QString>  backtrace(const int maxFrames = 63);
void dumpBacktrace(const int offset = 0, const int maxFrames = 63);
}
}
