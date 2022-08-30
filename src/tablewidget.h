#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTableWidget;
QT_END_NAMESPACE


class TableDialog : public QDialog
{
    Q_OBJECT
public:
    TableDialog(QWidget *parent = nullptr);
    ~TableDialog() override;

    auto markdownTable() const -> QString;

private Q_SLOTS:
    void onContextMenuRequest(const QPoint p);

    void handleRow();
    void handleColumn();

private:
    QTableWidget *table;
};

#endif // TABLEWIDGET_H
