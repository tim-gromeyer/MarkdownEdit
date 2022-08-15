#include "tablewidget.h"

#include <QTableWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>


TableWidget::TableWidget(QWidget *parent)
    : QDialog(parent)
{
    table = new QTableWidget(3, 2, this);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(table, 1);
    setLayout(l);

    QDialogButtonBox *box = new
            QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Abort,
                             Qt::Vertical, this);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    l->addWidget(box);
}

QString TableWidget::markdownTable()
{
    return QLatin1String();
}
