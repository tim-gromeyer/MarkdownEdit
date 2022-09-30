#include "tableofcontents.h"
#include "markdownparser.h"
#include "settings.h"

#include <utility>

#include "QGuiApplication"
#include "QHBoxLayout"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QMetaObject>
#include <QTextDocument>

using StringPair = QPair<QString, QString>;
using StringPairList = QList<StringPair>;


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
    StringPairList selected;
    QString out;

    auto count = list->count();

    for (int i = 0; i < count; ++i) {
        auto *item = list->item(i);

        if (item->checkState() == Qt::Unchecked) continue;

        selected.append(StringPair(item->text(), item->data(14).toString()));
    }

    const bool numberList = box->isChecked();

    for (int i = 0; i < selected.size(); ++i) {
        auto &pair = selected[i];

        if (numberList)
            out.append(STR("%1. ").arg(QString::number(i +1)));
        else
            out.append(L1("- "));

        out.append(STR("[%2](#%3)\n").arg(pair.first, pair.second));
    }

    return out;
}

void TableOfContents::parseText()
{
    // Show wait cursor
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // Separate by line
    QStringList list = in.split(u'\n');

    StringPairList headings;

    QTextDocument doc;

    // Loop through each line
    for (const QString &line : list) {
        // If the line doesn't start with # move to the next line
        if (!line.startsWith(u'#')) continue;

        const QString html = Parser::heading2HTML(line);
        doc.setHtml(html);

        QString text = doc.toPlainText().trimmed();
        if (text.endsWith(u':'))
            text.remove(text.size() -1, 1);

        // (?:id="([^"]*)")
        static const QRegularExpression id(STR("(?:id=\"([^\"]*)\")"));
        const QStringList matches = id.match(html).capturedTexts();
        if (matches.isEmpty()) continue;

        const QString &link = matches.last();

        headings.append(StringPair(text, link));
    }

    for (const auto &heading : qAsConst(headings)) {
        auto *item = new QListWidgetItem(heading.first, this->list);
        item->setCheckState(Qt::Unchecked);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setData(14, heading.second);
        this->list->addItem(item);
    }

    // Restore the normal cursor
    QGuiApplication::restoreOverrideCursor();
}
