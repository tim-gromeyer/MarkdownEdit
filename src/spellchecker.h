#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>
#include <QTextBrowser>

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
class SpellChecker : public MarkdownHighlighter
#else
class SpellChecker : public QSyntaxHighlighter
#endif
{
    Q_OBJECT
public:
    explicit SpellChecker(TextEditProxy *parent, const QString &lang = QLatin1String());
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

    TextEditProxy *textEdit;

    QString encodeLanguageString(const QString &langString);

    enchant::Dict *speller = nullptr;

#ifdef CHECK_MARKDOWN
    QStringList markdownCharachters = {QStringLiteral("##"), QStringLiteral("###"), QStringLiteral("####"), QStringLiteral("#####"), QStringLiteral("######")};
#endif
};


#endif // SPELLCHECKER_H
