#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTableWidget;
QT_END_NAMESPACE


class TableWidget : public QDialog
{
    Q_OBJECT
public:
    TableWidget(QWidget *parent = nullptr);
    virtual ~TableWidget() = 0;

    auto markdownTable() -> QString;

private:
    QTableWidget *table;
};

#endif // TABLEWIDGET_H
