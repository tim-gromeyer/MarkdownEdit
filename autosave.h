#ifndef AUTOSAVE_H
#define AUTOSAVE_H

#include <QWidget>
#include <QTimer>
#include <QTextDocument>


class AutoSave
{
public:
    AutoSave(QWidget *parent);
    AutoSave(QString file, QWidget *parent);

    void setFile(QString file)  { _file = file; };
    QString file() { return _file; };

    void setDoc(QTextDocument *doc) { _doc = doc; };
    QTextDocument* doc() { return _doc; };

    void setSaveIntervall(int msec);
    void errorString();

public slots:
    bool saveFile();
    static bool saveFile(QTextDocument *doc, QString file);

private:
    QString _file;
    QString _error;
    QTimer *_timer;
    QTextDocument *_doc = new QTextDocument();

};

#endif // AUTOSAVE_H
