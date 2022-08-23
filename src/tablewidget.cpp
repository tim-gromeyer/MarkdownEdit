#include "tablewidget.h"

#include <QDialogButtonBox>
#include <QTableWidget>
#include <QVBoxLayout>


TableWidget::TableWidget(QWidget *parent)
    : QDialog(parent)
{
    table = new QTableWidget(3, 2, this);

    auto *l = new QVBoxLayout(this);
    l->addWidget(table, 1);
    setLayout(l);

    auto *box = new
            QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Abort,
                             Qt::Vertical, this);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    l->addWidget(box);
}

auto TableWidget::markdownTable() -> QString
{
    return QLatin1String();
}
TableWidget::~TableWidget()
{
}
