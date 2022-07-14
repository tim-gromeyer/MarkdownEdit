#include <QString>
#include <QVariant>
#include <QPalette>
#include <QDir>
#include <QLocale>


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

const QString mapAttribute(const QString &s)
{
    return languages_map.value(s, QLatin1String()
                               ).toString();
}

bool mapContains(const QString &s)
{
    return languages_map.contains(s);
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

namespace common {
const QString homeDict()
{
    static const QString homeDirectory = QDir::homePath();
    return homeDirectory;
}

const QStringList languages()
{
    return QLocale::system().uiLanguages();
}
}
