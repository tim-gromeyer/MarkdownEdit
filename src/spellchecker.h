#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

#include "markdownhighlighter.h"


QT_BEGIN_NAMESPACE
namespace enchant { class Dict; };
QT_END_NAMESPACE


class SpellChecker : public MarkdownHighlighter
{
    Q_OBJECT
public:
    explicit SpellChecker(QPlainTextEdit *parent, const QString &lang = QLatin1String());
    ~SpellChecker();

    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

    Q_REQUIRED_RESULT const QStringList getLanguageList();

    inline QString getLanguage() { return language; };

    Q_REQUIRED_RESULT bool isCorrect(const QString &);
    Q_REQUIRED_RESULT QStringList getSuggestion(const QString &);
    void addWort(const QString &);
    void ignoreWord(const QString &);

    inline bool isMarkdownHighlightingEnabled() { return markdownhig; };
    inline bool isSpellCheckingEnabled() { return spellingEnabled; };

public slots:
    void setMarkdownHighlightingEnabled(const bool &enabled);
    void setSpellCheckingEnabled(const bool &enabled);
    bool setLanguage(const QString &);

    void checkSpelling(const QString &);

signals:
    void languageChanged(const QString &lang = QLatin1String());

private slots:
    void slotAddWord();
    void slotIgnoreWord();
    void slotSetLanguage(const bool &);
    void slotReplaceWord();
    void slotShowContextMenu(const QPoint &pos);

private:
    bool spellingEnabled = true;
    bool markdownhig = true;

    void showContextMenu(QMenu *menu, const QPoint &pos, int wordpos);

    QTextCharFormat spellFormat;

    QString language;
    void replaceWord(const int &wordPos, const QString &newWord);

    QPlainTextEdit *textEdit;

    QString encodeLanguageString(const QString &langString);

    enchant::Dict *speller = nullptr;

    QStringList markdownCharachters = {QStringLiteral("##"), QStringLiteral("###"), QStringLiteral("####"), QStringLiteral("#####"), QStringLiteral("######")};
};

#endif // SPELLCHECKER_H
