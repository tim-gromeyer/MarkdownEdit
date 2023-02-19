#include "tableofcontents.h"
#include "markdownparser.h"
#include "settings.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QListWidget>

#include <thread>

using StringPair = QPair<QString, QString>;
using StringPairList = QList<StringPair>;

TableOfContents::TableOfContents(const QString &text, QWidget *parent)
    : QDialog(parent)
    , list(new QListWidget(this))
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    setWindowTitle(tr("Insert table of contents"));

    auto *l = new QVBoxLayout(this);

    list->setDragDropMode(QListWidget::InternalMove);

    box = new QCheckBox(tr("Number list"), this);

    auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                    Qt::Horizontal,
                                    this);
    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);

    l->addWidget(list, 1);
    l->addWidget(box);
    l->addWidget(bb);
    setLayout(l);

    parseText(text);
    QGuiApplication::restoreOverrideCursor();
}

auto TableOfContents::markdownTOC() -> QString
{
    StringPairList selected;
    QString out;

    auto count = list->count();

    for (int i = 0; i < count; ++i) {
        auto *item = list->item(i);

        if (item->checkState() == Qt::Unchecked)
            continue;

        selected.append(StringPair(item->text(), item->data(14).toString()));
    }

    const bool numberList = box->isChecked();

    for (int i = 0; i < selected.size(); ++i) {
        auto &pair = selected[i];

        if (numberList)
            out.append(STR("%1. ").arg(QString::number(i + 1)));
        else
            out.append(L1("- "));

        out.append(STR("[%2](#%3)\n").arg(pair.first, pair.second));
    }

    return out;
}

void TableOfContents::parseText(const QString &in)
{
    // Separate by line
    QStringList list = in.split(u'\n');

    StringPairList headings;

    // Loop through each line
    for (const QString &line : list) {
        // If the line doesn't start with # move to the next line
        if (!line.startsWith(u'#'))
            continue;

        const std::string html = Parser::heading2HTML(line);

        QString id;
        QString text;

        bool inAttribute = false;
        bool inTag = false;

        for (char c : html) {
            switch (c) {
            case '<':
                inTag = true;
                break;
            case '>':
                inTag = false;
                continue;
            case '"':
                inAttribute = !inAttribute;
                continue;
            case '\n':
                continue;
            default:
                break;
            }

            if (inAttribute)
                id.append(QChar::fromLatin1(c));
            else if (!inTag)
                text.append(QChar::fromLatin1(c));
        }

        headings.append(StringPair(text, id));
    }

    for (const auto &heading : qAsConst(headings)) {
        auto *item = new QListWidgetItem(heading.first, this->list);
        item->setCheckState(Qt::Unchecked);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setData(14, heading.second);
        this->list->addItem(item);
    }
}
