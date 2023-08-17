#pragma once

#include <QDialog>

class QDialogButtonBox;
class QLineEdit;

class MarkdownDialog : public QDialog
{
    Q_OBJECT

public:
    enum MarkdownType { Image, Link };

    explicit MarkdownDialog(MarkdownType type, QWidget *parent = nullptr);

    QString getMarkdown();

private:
    MarkdownType markdownType;
    QLineEdit *linkLineEdit;
    QLineEdit *textLineEdit;
    QLineEdit *titleLineEdit;
    QDialogButtonBox *buttonBox;
};
