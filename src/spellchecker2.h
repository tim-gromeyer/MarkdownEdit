#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

#ifdef CHECK_MARKDOWN
#include "markdownhighlighter.h"
#endif


QT_BEGIN_NAMESPACE
namespace enchant { class Dict; };
QT_END_NAMESPACE


#ifdef CHECK_MARKDOWN
class SpellChecker : public MarkdownHighlighter
#else
class SpellChecker : public QSyntaxHighlighter
#endif
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

#ifdef CHECK_MARKDOWN
    inline bool isMarkdownHighlightingEnabled() { return markdownhig; };
#endif
    inline bool isSpellCheckingEnabled() { return spellingEnabled; };

public slots:
#ifdef CHECK_MARKDOWN
    void setMarkdownHighlightingEnabled(const bool &enabled);
#endif
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
#ifdef CHECK_MARKDOWN
    bool markdownhig = true;
#endif

    void showContextMenu(QMenu *menu, const QPoint &pos, int wordpos);

    QTextCharFormat spellFormat;

    QString language;
    void replaceWord(const int &wordPos, const QString &newWord);

    QPlainTextEdit *textEdit;

    QString encodeLanguageString(const QString &langString);

    enchant::Dict *speller = nullptr;

#ifdef CHECK_MARKDOWN
    QStringList markdownCharachters = {QStringLiteral("##"), QStringLiteral("###"), QStringLiteral("####"), QStringLiteral("#####"), QStringLiteral("######")};
#endif
};

#endif // SPELLCHECKER_H
