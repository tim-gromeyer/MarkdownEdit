#include <QFile>
#include <QTextStream>


#include "autosave.h"

AutoSave::AutoSave(QWidget *parent)
{
    _timer = new QTimer(parent);
}

AutoSave::AutoSave(QString file, QWidget *parent)
{
    _timer = new QTimer(parent);
    setFile(file);
}

void AutoSave::setSaveIntervall(int msec)
{
    _timer->setInterval(msec);
}

bool AutoSave::saveFile()
{
    if (!_file.endsWith(".autosave"))
        _file.append(".autosave");

    QFile f(_file);

    if (!f.exists())
        return false;

    if (!f.open(QFile::WriteOnly | QFile::Text)) {
        _error = f.errorString();
        return false;
    }

    QTextStream out(&f);
    out << _doc->toPlainText();
    return true;
}

bool AutoSave::saveFile(QTextDocument *doc, QString file)
{
    if (!file.endsWith(".autosave"))
        file.append(".autosave");

    QFile f(file);

    if (!f.exists())
        return false;

    if (!f.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }

    QTextStream out(&f);
    out << doc->toPlainText();
    return true;
}
