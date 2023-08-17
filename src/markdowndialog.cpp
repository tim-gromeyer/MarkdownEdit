#include "markdowndialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

MarkdownDialog::MarkdownDialog(MarkdownType type, QWidget *parent)
    : QDialog(parent)
    , markdownType(type)
{
    if (markdownType == Image) {
        setWindowTitle(tr("Markdown Image Dialog"));
    } else if (markdownType == Link) {
        setWindowTitle(tr("Markdown Link Dialog"));
    }

    linkLineEdit = new QLineEdit(this);
    textLineEdit = new QLineEdit(this);
    titleLineEdit = new QLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &MarkdownDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MarkdownDialog::reject);

    auto *layout = new QFormLayout;
    layout->addRow(tr("Link:"), linkLineEdit);
    layout->addRow(tr("Text:"), textLineEdit);

    if (markdownType == Image) {
        layout->addRow(tr("Title:"), titleLineEdit);
    } else {
        delete titleLineEdit;
    }

    layout->addRow(buttonBox);

    setLayout(layout);
}

QString MarkdownDialog::getMarkdown()
{
    if (exec() != Accepted) {
        return {};
    }

    QString link = linkLineEdit->text();
    QString text = textLineEdit->text();

    if (markdownType == Image) {
        QString title = titleLineEdit->text();
        QString markdown = QStringLiteral("![%1](%2").arg(text, link);
        if (!title.isEmpty())
            markdown.append(QStringView(u" \"") + title + u'"');
        return markdown + u')';
    } else {
        return QString::fromLatin1("[%1](%2)").arg(text, link);
    }
}
