#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include "qmarkdowntextedit.h"
#include <QWidget>
#include <QCompleter>


class MarkdownEditor : public QMarkdownTextEdit
{
    Q_OBJECT
public:
    MarkdownEditor(QWidget *parent);
    ~MarkdownEditor();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *c = nullptr;
};

#endif // MARKDOWNEDITOR_H
