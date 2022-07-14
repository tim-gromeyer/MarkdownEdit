#ifndef COMMON_H
#define COMMON_H

#include <QMap>
#include <QVariant>

#define RECENT_OPENED_LIST_LENGTH 7

#define MD_UNDERLINE true

#define SPELLCHECK true  // Not avaiable on android and webassembly

QMap<QString, QVariant> getLanguageMap();
void setLanguageMap(const QMap<QString, QVariant> &m);
void setMapAttribute(const QString &, const QVariant &);
const QString mapAttribute(const QString &);
bool mapContains(const QString &);

QString currDir();
void setCurrDir(const QString &);

const bool isDarkMode();

namespace common {
const QString homeDict();
const QStringList languages();
}; // namespace common

#if !SPELLCHECK
#define NO_SPELLCHECK
#endif

#endif // COMMON_H
