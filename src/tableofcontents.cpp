#include "tableofcontents.h"
#include "settings.h"

#include <utility>

#include "QGuiApplication"
#include "QHBoxLayout"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QMetaObject>


TableOfContents::TableOfContents(QString text, QWidget *parent)
    : QDialog(parent),
      in(std::move(text))
{
    setWindowTitle(tr("Insert table of contents"));

    auto *l = new QVBoxLayout(this);

    list = new QListWidget(this);
    list->setDragDropMode(QListWidget::InternalMove);

    box = new QCheckBox(tr("Number list"), this);

    auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);

    l->addWidget(list, 1);
    l->addWidget(box);
    l->addWidget(bb);
    setLayout(l);

    // Don't block GUI
    QMetaObject::invokeMethod(this, "parseText", Qt::QueuedConnection);
}

auto TableOfContents::markdownTOC() -> QString
{
    QStringList selected;
    QString out;

    auto count = list->count();

    for (int i = 0; i < count; ++i) {
        auto *item = list->item(i);

        if (item->checkState() == Qt::Unchecked) continue;

        selected.append(item->data(0).toString());
    }

    const bool numberList = box->isChecked();

    for (int i = 0; i < selected.size(); ++i) {
        QString heading = selected[i];
        if (heading.startsWith(u'[')) {
            static QRegularExpression text(STR(R"(\[(.*)\])"));
            QRegularExpressionMatch match = text.match(heading);

            const QStringList matches = match.capturedTexts();

            if (!matches.isEmpty())
                heading = matches.last(); // The first match contains []
        }
        QString link = heading.toLower().replace(u' ', u'-').remove(u':').remove(u'.');
        link.prepend(u'#');

        if (numberList)
            out.append(STR("%1. ").arg(QString::number(i +1)));
        else
            out.append(L1("- "));

        out.append(STR("[%2](%3)\n").arg(heading, link));
    }

    return out;
}

void TableOfContents::parseText()
{
    // Show wait cursor
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // Separate by line
    QStringList list = in.split(u'\n');

    // Loop through each line
    for (const QString &line : list) {
        // If the line doesn't start with # move to the next line
        if (!line.startsWith(u'#')) continue;

        // In case it's a heading find the whitespace between the #'s and the text
        auto pos = line.indexOf(u' ');
        // If not found or greater than 6 move to next line
        if (pos == -1 || pos > 6) continue;

        // Get the string from 0 til 1 - 6
        const QString first = line.mid(0, pos);
        // Get the heading number
        auto count = first.count(u'#');

        headings.append(line.mid(++count));
    }

    for (const QString &heading : qAsConst(headings)) {
        auto *item = new QListWidgetItem(heading, this->list);
        item->setCheckState(Qt::Unchecked);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setData(0, heading);
        this->list->addItem(item);
    }

    // Restore the normal cursor
    QGuiApplication::restoreOverrideCursor();
}
