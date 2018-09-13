#ifndef SLATEGLOBAL_H
#define SLATEGLOBAL_H

#include <QtGlobal>

#if defined(SLATE_LIBRARY)
#define SLATE_EXPORT Q_DECL_EXPORT
#else
#define SLATE_EXPORT Q_DECL_IMPORT
#endif

#endif // SLATEGLOBAL_H
