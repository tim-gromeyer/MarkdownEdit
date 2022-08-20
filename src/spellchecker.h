/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 Tim Gromeyer <sakul8825@gmail.com>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/


#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QSyntaxHighlighter>

#ifdef CHECK_MARKDOWN
#include "markdownhighlighter.h"
#define SpellCheckerBaseClass MarkdownHighlighter
#else
#define SpellCheckerBaseClass QSyntaxHighlighter
#endif

QT_BEGIN_NAMESPACE
namespace enchant { class Dict; };
class QMenu;
template <typename Key, typename T> class QHash;
QT_END_NAMESPACE

// We use sliced QStringView 5.14 (it's faster)
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    #define STRING QString
#else
    #define STRING QStringView
#endif


class TextEditProxy : public QObject {
    Q_OBJECT
public:
    TextEditProxy(QObject* parent) : QObject(parent) {}
    virtual QTextCursor textCursor() const = 0;
    virtual QTextDocument* document() const = 0;
    virtual QPoint mapToGlobal(const QPoint& pos) const = 0;
    virtual QMenu* createStandardContextMenu() = 0;
    virtual QTextCursor cursorForPosition(const QPoint& pos) const = 0;
    virtual void setContextMenuPolicy(Qt::ContextMenuPolicy policy) = 0;

signals:
    void customContextMenuRequested(const QPoint& pos);
};

template<class T>
class TextEditProxyT : public TextEditProxy {
public:
    explicit TextEditProxyT(T* textEdit, QObject* parent = nullptr) : TextEditProxy(parent), m_textEdit(textEdit) {
        connect(textEdit, &T::customContextMenuRequested, this, &TextEditProxy::customContextMenuRequested);
    }
    ~TextEditProxyT() {}; // app will crash if u delete m_textEdit;

    // static TextEditProxyT* createTextEditProxy(QPlainTextEdit *textEdit) { return new TextEditProxyT(textEdit, textEdit); };
    QTextCursor textCursor() const override { return m_textEdit->textCursor(); }
    QTextDocument* document() const override { return m_textEdit->document(); }
    QPoint mapToGlobal(const QPoint& pos) const override { return m_textEdit->mapToGlobal(pos); }
    QMenu* createStandardContextMenu() override { return m_textEdit->createStandardContextMenu(); }
    QTextCursor cursorForPosition(const QPoint& pos) const override { return m_textEdit->cursorForPosition(pos); }
    void setContextMenuPolicy(Qt::ContextMenuPolicy policy) override { m_textEdit->setContextMenuPolicy(policy); }

private:
    T* m_textEdit;
};

class SpellChecker : public SpellCheckerBaseClass
{
    Q_OBJECT
public:
    explicit SpellChecker(TextEditProxy *parent, const QString &lang = QLatin1String());
    ~SpellChecker();

    QHash<QString, QString> langMap;

    void highlightBlock(const QString &text) override;


    Q_REQUIRED_RESULT static const QStringList getLanguageList();

    QString getLanguage();

    Q_REQUIRED_RESULT bool isCorrect(const QString &word);

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

    void checkSpelling(const STRING &);

signals:
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

    TextEditProxy *textEdit = nullptr; // Fix warning

    QString encodeLanguageString(const QString &langString);

#ifndef NO_SPELLCHECK
    QScopedPointer<enchant::Dict> speller;
#else
    enchant::Dict *speller = nullptr;
#endif

#ifdef CHECK_MARKDOWN
    QString getWord(const QTextBlock &, const int &);
#endif
};


#endif // SPELLCHECKER_H
