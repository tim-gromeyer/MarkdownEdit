#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QMap>
#include <QVariant>

#include "common.h"

#ifdef CHECK_MARKDOWN
#include "markdownhighlighter.h"
#endif

QT_BEGIN_NAMESPACE
namespace enchant { class Dict; };
QT_END_NAMESPACE


class TextEditProxy : public QObject {
    Q_OBJECT
public:
    TextEditProxy(QObject* parent = nullptr) : QObject(parent) {}
    virtual QTextCursor textCursor() const = 0;
    virtual QTextDocument* document() const = 0;
    virtual QPoint mapToGlobal(const QPoint& pos) const = 0;
    virtual QMenu* createStandardContextMenu() = 0;
    virtual QTextCursor cursorForPosition(const QPoint& pos) const = 0;
    virtual void setContextMenuPolicy(Qt::ContextMenuPolicy policy) = 0;
    virtual void setTextCursor(const QTextCursor& cursor) = 0;
    virtual Qt::ContextMenuPolicy contextMenuPolicy() const = 0;
    virtual void installEventFilter(QObject* filterObj) = 0;
    virtual void removeEventFilter(QObject* filterObj) = 0;
    virtual void ensureCursorVisible() = 0;

signals:
    void customContextMenuRequested(const QPoint& pos);
    void textChanged();
    void editDestroyed();
};

template<class T>
class TextEditProxyT : public TextEditProxy {
public:
    explicit TextEditProxyT(T* textEdit, QObject* parent = nullptr) : TextEditProxy(parent), m_textEdit(textEdit) {
        connect(textEdit, &T::customContextMenuRequested, this, &TextEditProxy::customContextMenuRequested);
        connect(textEdit, &T::textChanged, this, &TextEditProxy::textChanged);
        connect(textEdit, &T::destroyed, this, &TextEditProxy::editDestroyed);
    }
    QTextCursor textCursor() const override { return m_textEdit->textCursor(); }
    QTextDocument* document() const override { return m_textEdit->document(); }
    QPoint mapToGlobal(const QPoint& pos) const override { return m_textEdit->mapToGlobal(pos); }
    QMenu* createStandardContextMenu() override { return m_textEdit->createStandardContextMenu(); }
    QTextCursor cursorForPosition(const QPoint& pos) const override { return m_textEdit->cursorForPosition(pos); }
    void setContextMenuPolicy(Qt::ContextMenuPolicy policy) override { m_textEdit->setContextMenuPolicy(policy); }
    void setTextCursor(const QTextCursor& cursor) override { m_textEdit->setTextCursor(cursor); }
    Qt::ContextMenuPolicy contextMenuPolicy() const override { return m_textEdit->contextMenuPolicy(); }
    void installEventFilter(QObject* filterObj) override { m_textEdit->installEventFilter(filterObj); }
    void removeEventFilter(QObject* filterObj) override { m_textEdit->removeEventFilter(filterObj); }
    void ensureCursorVisible() override { m_textEdit->ensureCursorVisible(); }

private:
    T* m_textEdit = nullptr;
};

#ifdef CHECK_MARKDOWN
/**
 * @brief SpellChecker for QPlainTextEdit, QTextEdit, QTextBrowser
 */
class SpellChecker : public MarkdownHighlighter
#else
/**
 * @brief SpellChecker for QPlainTextEdit, QTextEdit, QTextBrowser
 */
class SpellChecker : public QSyntaxHighlighter
#endif
{
    Q_OBJECT
public:
    /**
     * @brief SpellChecker initializer.
     * @param parent parent: new new TextEditProxyT(textEdit)
     * @param lang Spell language, if empty or not set QLocale::system().name() is used.
     */
    explicit SpellChecker(TextEditProxy *parent, const QString &lang = QLatin1String());
    ~SpellChecker();

    QMap<QString, QVariant> langMap;

    /**
     * @brief highlightBlock function thats call checkSpelling()
     * @param text text passed to checkSpelling()
     */
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;


    /**
     * @brief getLanguageList returns a list with all avaiable languages
     * @return list with all avaiable languages
     */
    Q_REQUIRED_RESULT const QStringList getLanguageList();

    /**
     * @brief getLanguage returns the current language
     * @return current language
     */
    inline QString getLanguage() { return language; };

    /**
     * @brief isCorrect returns true if @word is correct otherwise false
     * @param word returns if the word is spelled correct
     * @return returns true if @word is correct otherwise false
     */
    Q_REQUIRED_RESULT bool isCorrect(const QString &word);

    /**
     * @brief getSuggestion
     * @return
     */
    Q_REQUIRED_RESULT QStringList getSuggestion(const QString &);

    /**
     * @brief addWort
     */
    void addWort(const QString &);

    /**
     * @brief ignoreWord
     */
    void ignoreWord(const QString &);

#ifdef CHECK_MARKDOWN
    /**
     * @brief isMarkdownHighlightingEnabled
     * @return
     */
    inline bool isMarkdownHighlightingEnabled() { return markdownhig; };
#endif

    /**
     * @brief isSpellCheckingEnabled
     * @return
     */
    inline bool isSpellCheckingEnabled() { return spellingEnabled; };

public slots:
#ifdef CHECK_MARKDOWN
    /**
     * @brief setMarkdownHighlightingEnabled
     * @param enabled
     */
    void setMarkdownHighlightingEnabled(const bool &enabled);
#endif

    /**
     * @brief setSpellCheckingEnabled
     * @param enabled
     */
    void setSpellCheckingEnabled(const bool &enabled);

    /**
     * @brief setLanguage
     * @return
     */
    bool setLanguage(const QString &);

    /**
     * @brief checkSpelling
     */
    void checkSpelling(const QString &);

signals:
    /**
     * @brief languageChanged
     * @param lang
     */
    void languageChanged(const QString &lang = QLatin1String());

private slots:
    void slotAddWord();

    void slotIgnoreWord();

    void slotSetLanguage(const bool &);

    void slotReplaceWord();

private:
    bool spellingEnabled = true;
#ifdef CHECK_MARKDOWN
    bool markdownhig = true;
#endif

    void showContextMenu(const QPoint &);

    QString language;
    void replaceWord(const int &wordPos, const QString &newWord);

    TextEditProxy *textEdit;

    QString encodeLanguageString(const QString &langString);

    enchant::Dict *speller = nullptr;

#ifdef CHECK_MARKDOWN
    QString getWord(const QTextBlock &, const int &);

    QStringList markdownCharachters = {QStringLiteral("##"), QStringLiteral("###"), QStringLiteral("####"), QStringLiteral("#####"), QStringLiteral("######")};
#endif
};


#endif // SPELLCHECKER_H
