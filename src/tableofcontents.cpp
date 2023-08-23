#include "tableofcontents.h"
#include "markdownparser.h"
#include "settings.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QListWidget>

using StringPair = QPair<QString, QString>;
using StringPairList = QList<StringPair>;
using std::as_const;

TableOfContents::TableOfContents(const QString &text, QWidget *parent)
    : QDialog(parent)
    , list(new QListWidget(this))
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    setWindowTitle(tr("Insert table of contents"));

    auto *l = new QVBoxLayout(this);

    list->setDragDropMode(QListWidget::InternalMove);

    box = new QCheckBox(tr("Number list"), this);

    auto *bb = new QDialogButtonBox(Qt::Horizontal, this);
    bb->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
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
    selected.reserve(count);

    for (int i = 0; i < count; ++i) {
        auto *item = list->item(i);

        if (item->checkState() == Qt::Checked)
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

    QString lines;

    StringPairList headings;

    // Loop through each line
    for (const QString &line : list) {
        // If the line doesn't start with # move to the next line
        if (!line.startsWith(u'#'))
            continue;

        lines.append(line);
        lines.append(u'\n');
    }

    // Convert the markdown headings to HTML
    lines = QString::fromStdString(Parser::heading2HTML(lines));

    for (const QString &line : lines.split(u'\n')) {
        QString id;
        QString text;

        static const QRegularExpression idRegex(
            QStringLiteral("<h[1-6][^>]*id\\s*=\\s*[\"']([^\"']*)[\"'][^>]*>"));
        QRegularExpressionMatch idMatch = idRegex.match(line);
        if (idMatch.hasMatch()) {
            id = idMatch.captured(1);
        }

        bool inAttribute = false;
        bool inTag = false;

        for (char c : line.toStdString()) {
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

            if (!inTag)
                text.append(QChar::fromLatin1(c));
        }

        if (!text.isEmpty() && !id.isEmpty())
            headings.append(StringPair(text, id));
    }

    for (const auto &heading : as_const(headings)) {
        auto *item = new QListWidgetItem(heading.first, this->list);
        item->setCheckState(Qt::Unchecked);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setData(14, heading.second);
        this->list->addItem(item);
    }
}
