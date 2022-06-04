#include "markdowneditor.h"

#include <QAbstractItemView>
#include <QScrollBar>
#include <QDialog>
#include <QHBoxLayout>
#include <QFile>


MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QMarkdownTextEdit(parent, false)
{
}

void MarkdownEditor::showMarkdownSyntax()
{
    QDialog d(this);
    QHBoxLayout l(&d);
    d.setLayout(&l);
    l.setContentsMargins(0, 0, 0, 0);
    d.resize(size());

    QMarkdownTextEdit e(&d, true);


    QFile f(QStringLiteral(":/syntax_%1.md").arg(checker->getLanguage().split("_").at(0)), &d);
    if (!f.open(QIODevice::ReadOnly)) {
        f.setFileName(QStringLiteral(":/syntax_en.md"));
        f.open(QIODevice::ReadOnly);
    }
    e.setPlainText(f.readAll());
    l.addWidget(&e);

    d.exec();
}

void MarkdownEditor::setText(const QString &t)
{
    if (checker)
        checker->clearDirtyBlocks();

    QMarkdownTextEdit::setPlainText(t);
    document()->setModified(false);
}

void MarkdownEditor::setChecker(SpellChecker* &c)
{
    checker = c;
}

void MarkdownEditor::setCompleter(QCompleter *completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    connect(c, QOverload<const QString &>::of(&QCompleter::activated),
            this, &MarkdownEditor::insertCompletion);
}

void MarkdownEditor::insertCompletion(const QString &completion)
{
    QTextCursor tc = textCursor();
    const int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

const QString MarkdownEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void MarkdownEditor::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QMarkdownTextEdit::focusInEvent(e);
}

void MarkdownEditor::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QMarkdownTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}

MarkdownEditor::~MarkdownEditor()
{
    delete c;
}
