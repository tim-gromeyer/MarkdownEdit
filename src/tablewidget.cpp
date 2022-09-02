#include "tablewidget.h"

#include <QDialogButtonBox>
#include <QList>
#include <QMenu>
#include <QTableWidget>
#include <QVBoxLayout>


TableDialog::TableDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Insert table"));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &TableDialog::onContextMenuRequest);

    table = new QTableWidget(3, 2, this);

    auto *l = new QVBoxLayout(this);
    l->addWidget(table, 1);
    setLayout(l);

    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Abort,
                                     Qt::Horizontal, this);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    l->addWidget(box);
}

auto TableDialog::markdownTable() const -> QString
{
#define L(str) QLatin1String(str)
#define S(str) QStringLiteral(str)

    QString out;

    const auto rowCount = table->rowCount();
    const auto columnCount = table->columnCount();

    QList<int> biggest;
    biggest.reserve(columnCount);

    // Find the longest text in each column
    for (int i = 0; i < columnCount; i++) {
        int size = 0;

        for (int row = 0; row < rowCount; row++) {

            if (!table->item(row, i))
                continue;

            const QString text = table->item(row, i)->text();

            if (text.size() > size)
                size = text.size();
        }
        biggest.append(size);
    }

    int size = 0;
    // Calculate size for the output string
    for (int i = 0; i < columnCount; i++) {
        size += (biggest[i] +4) * (rowCount +1); // +1 because of the extra row which marks the headers
    }
    out.reserve(size);

    for (int row = 0; row < rowCount; row++) {
        for (int column = 0; column < columnCount; column++) {
            out.append(L("| "));
            if (!table->item(row, column)) {
                out.append(S(" ").repeated(biggest[column] +1));
                continue;
            }

            const QString text = table->item(row, column)->text();
            out.append(text);
            out.append(S(" ").repeated(biggest[column] - text.size() +1));

        }
        out.append(u'|');
        out.append(u'\n');
        if (row != 0) continue;

        for (int column = 0; column < columnCount; column++) {
            out.append(u'|');
            out.append(S("-").repeated(biggest[column] +2));
            // out.append(u' ');
        }
        out.append(u'|');
        out.append(u'\n');
    }
    // Expected output:
    /*
     * | 1              |       |
     * |----------------|-------|
     * | Test           | Hello |
     * | This is a test |       |
    */

    out.squeeze();
    return out;
}

void TableDialog::onContextMenuRequest(const QPoint p)
{
#define S(str) QStringLiteral(str)
    const QPoint pos = mapToGlobal(p);
    setProperty("menuPos", pos);

    auto *menu = new QMenu(this);

    // Handle rows
    auto *aInsertRowAbove = new QAction(tr("Add row above"), menu);
    auto *aInsertRowBelow = new QAction(tr("Add row below"), menu);
    auto *aRemoveRow = new QAction(tr("Remove row"), menu);

    aInsertRowAbove->setIcon(QIcon::fromTheme(S("edit-table-insert-row-above"), QIcon(S(":/icons/edit-table-insert-row-above.svg"))));
    aInsertRowBelow->setIcon(QIcon::fromTheme(S("edit-table-insert-row-below"), QIcon(S(":/icons/edit-table-insert-row-below.svg"))));
    aRemoveRow->setIcon(QIcon::fromTheme(S("edit-table-delete-row"), QIcon(S(":/icons/edit-table-delete-row.svg"))));

    aInsertRowAbove->setData(QStringLiteral("above"));
    aInsertRowBelow->setData(QStringLiteral("below"));
    aRemoveRow->setData(QStringLiteral("remove"));

    connect(aInsertRowAbove, &QAction::triggered, this, &TableDialog::handleRow);
    connect(aInsertRowBelow, &QAction::triggered, this, &TableDialog::handleRow);
    connect(aRemoveRow, &QAction::triggered, this, &TableDialog::handleRow);

    menu->addAction(aInsertRowAbove);
    menu->addAction(aInsertRowBelow);
    menu->addAction(aRemoveRow);

    // Handle columns
    auto *aInsertColumnAbove = new QAction(tr("Add column to the left"), menu);
    auto *aInsertColumnBelow = new QAction(tr("Add column to the right"), menu);
    auto *aRemoveColumn = new QAction(tr("Remove column"), menu);

    aInsertColumnAbove->setIcon(QIcon::fromTheme(S("edit-table-insert-column-left"), QIcon(S(":/icons/edit-table-insert-column-left.svg"))));
    aInsertColumnBelow->setIcon(QIcon::fromTheme(S("edit-table-insert-column-right"), QIcon(S(":/icons/edit-table-insert-column-right.svg"))));
    aRemoveColumn->setIcon(QIcon::fromTheme(S("edit-table-delete-column"), QIcon(S(":/icons/edit-table-delete-column.svg"))));

    aInsertColumnAbove->setData(QStringLiteral("above"));
    aInsertColumnBelow->setData(QStringLiteral("below"));
    aRemoveColumn->setData(QStringLiteral("remove"));

    connect(aInsertColumnAbove, &QAction::triggered, this, &TableDialog::handleColumn);
    connect(aInsertColumnBelow, &QAction::triggered, this, &TableDialog::handleColumn);
    connect(aRemoveColumn, &QAction::triggered, this, &TableDialog::handleColumn);

    menu->addSeparator();
    menu->addAction(aInsertColumnAbove);
    menu->addAction(aInsertColumnBelow);
    menu->addAction(aRemoveColumn);

    menu->exec(pos);

    menu->deleteLater();
    delete menu;
}

void TableDialog::handleRow()
{
    auto *a = qobject_cast<QAction*>(sender());

    const QString s = a->data().toString();

    if (s == QLatin1String("above")) {
        int i = table->currentRow() -1;
        if (i == -1) i++;
        table->insertRow(i);
    } else if (s == QLatin1String("below"))
        table->insertRow(table->currentRow() +1);
    else
        table->removeRow(table->currentRow());
}

void TableDialog::handleColumn()
{
    auto *a = qobject_cast<QAction*>(sender());

    const QString s = a->data().toString();

    if (s == QLatin1String("above")) {
        int i = table->currentColumn() -1;
        if (i == -1) i++;
        table->insertColumn(i);
    } else if (s == QLatin1String("below"))
        table->insertColumn(table->currentColumn() +1);
    else
        table->removeColumn(table->currentColumn());
}

TableDialog::~TableDialog() = default;