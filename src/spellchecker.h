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
class QPlainTextEdit;
QT_END_NAMESPACE

// We use sliced QStringView 5.14 (it's faster)
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    #define STRINGVIEW QString
#else
    #define STRINGVIEW QStringView
#endif

class SpellChecker : public SpellCheckerBaseClass
{
    Q_OBJECT
public:
    explicit SpellChecker(QPlainTextEdit *parent, const QString &lang = QLatin1String());
    ~SpellChecker() override;

    auto setLanguage(const QString &) -> bool;
    auto getLanguage() -> QString;

#ifdef CHECK_MARKDOWN
    void setMarkdownHighlightingEnabled(const bool);
    inline auto isMarkdownHighlightingEnabled() -> bool { return markdownhig; };
#endif

    void setSpellCheckingEnabled(const bool);
    inline auto isSpellCheckingEnabled() -> bool { return spellingEnabled && speller; };

    Q_REQUIRED_RESULT static auto getLanguageList() -> const QStringList;

    void highlightBlock(const QString &text) override;

    Q_REQUIRED_RESULT auto isCorrect(const QString &word) -> bool;

    Q_REQUIRED_RESULT auto getSuggestion(const QString &) -> QStringList;

    void addWort(const QString &);

    void ignoreWord(const QString &);

public Q_SLOTS:
    void checkSpelling(const STRINGVIEW &);

Q_SIGNALS:
    void languageChanged(const QString &lang = QLatin1String());

private Q_SLOTS:
    void slotAddWord();

    void slotIgnoreWord();

    void slotSetLanguage(const bool);

    void slotReplaceWord();

private:
    bool spellingEnabled = true;
#ifdef CHECK_MARKDOWN
    bool markdownhig = true;
    auto getWord(const QTextBlock &, const int) -> QString;
#endif

    void showContextMenu(const QPoint);

    QString language;
    void replaceWord(const int wordPos, const QString &newWord);

    QPlainTextEdit *textEdit = nullptr;

    auto encodeLanguageString(const QString &langString) -> QString;

    enchant::Dict *speller = nullptr;

    QHash<QString, QString> langMap;
};

#endif // SPELLCHECKER_H
