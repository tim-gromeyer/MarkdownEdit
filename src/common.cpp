#include <QString>
#include <QVariant>
#include <QPalette>


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

void setMapAttribute(const QString &name, const QVariant &value)
{
    languages_map[name] = value;
}


QString currDir() {
    return currentDir;
}

void setCurrDir(const QString &newDir)
{
    currentDir = newDir;
}

bool isDarkMode()
{
    static const QColor back = QPalette().base().color();
    static int r, g, b, a;
    back.getRgb(&r, &g, &b, &a);

    static const bool dark = ((r + g + b + a) / 4) < 127;

    return dark;
}
