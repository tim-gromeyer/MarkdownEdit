#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include "qmarkdowntextedit.h"
#include "spellchecker.h"

#include <QWidget>
#include <QCompleter>


class MarkdownEditor : public QMarkdownTextEdit
{
    Q_OBJECT
public:
    explicit MarkdownEditor(QWidget *parent);
    ~MarkdownEditor();

    void setCompleter(QCompleter *c);
    void setChecker(SpellChecker* &);

    void setText(const QString &);

public slots:
    void showMarkdownSyntax();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    const QString textUnderCursor() const;

private:
    QCompleter *c = nullptr;
    SpellChecker *checker = nullptr;
};

#endif // MARKDOWNEDITOR_H
