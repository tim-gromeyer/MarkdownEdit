#include <QString>
#include <QVariant>


QMap<QString, QVariant> languages_map;

QMap<QString, QVariant> getLanguageMap()
{
    return languages_map;
}

void setLanguageMap(const QMap<QString, QVariant> &m)
{
    languages_map = m;
}
