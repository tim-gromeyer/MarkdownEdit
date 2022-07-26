#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "common.h"

#include <QString>

// Namespace
template <typename Key, typename T> class QHash;
class QVariant;

QHash<QString, QVariant> getLanguageMap();
void setLanguageMap(const QHash<QString, QVariant> &m);
void setMapAttribute(const QString &, const QVariant &);
const QString mapAttribute(const QString &);
bool mapContains(const QString &);

QString currDir();
void setCurrDir(const QString &);

namespace settings {
    const bool isDarkMode();
}

namespace common {
const QString homeDict();
const QStringList languages();
}; // namespace common

#if !SPELLCHECK
#define NO_SPELLCHECK
#endif

#endif // SETTINGS_HPP
