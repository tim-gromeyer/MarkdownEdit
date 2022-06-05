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
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor();

    void setText(const QString &, const QString &fileName = QLatin1String());

    bool setLanguage(const QString &);

    inline SpellChecker* getChecker() { return checker; };

    void changeSpelling(const bool &);

signals:
    void languageChanged(const QString &lang = QLatin1String());

public slots:
    void showMarkdownSyntax();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

    void onLanguageChanged(const QString &);

private:
    const QString textUnderCursor() const;

private:
    QCompleter *c = nullptr;
    SpellChecker *checker = nullptr;

    QString fileName = QString::fromLatin1("");
};

#endif // MARKDOWNEDITOR_H
