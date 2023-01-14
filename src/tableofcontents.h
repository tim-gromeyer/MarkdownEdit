#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QListWidget;
class QCheckBox;
QT_END_NAMESPACE

class TableOfContents : public QDialog
{
    Q_OBJECT
public:
    explicit TableOfContents(QString text, QWidget *parent = nullptr);

    auto markdownTOC() -> QString;

private Q_SLOTS:
    void parseText();

    // void selectionChanged();

private:
    QString in;

    QListWidget *list;

    QCheckBox *box;
};

#endif // TABLEOFCONTENTS_H
