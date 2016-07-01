#pragma once

#include <qcompilerdetection.h>

#if __cpp_attributes && __has_cpp_attribute(fallthrough)
#define DECL_FALLTHROUGH [[fallthrough]]
#elif defined(Q_CC_CLANG)
#define DECL_FALLTHROUGH [[clang::fallthrough]]
#else
#define DECL_FALLTHROUGH
#endif
