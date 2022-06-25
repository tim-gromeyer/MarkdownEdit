#include <QString>
#include <QVariant>


QMap<QString, QVariant> languages_map;
QString currentDir;


QMap<QString, QVariant> getLanguageMap()
{
    return languages_map;
}

void setLanguageMap(const QMap<QString, QVariant> &m)
{
    languages_map = m;
}

QString currDir() {
    return currentDir;
}

void setCurrDir(const QString &newDir)
{
    currentDir = newDir;
}
