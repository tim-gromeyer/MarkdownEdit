/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 - 2023 Tim Gromeyer <sakul8826@gmail.com>.
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

#include "markdownhighlighter.h"

QT_BEGIN_NAMESPACE
namespace enchant {
class Dict;
};
class QMenu;
template<typename Key, typename T>
class QHash;
class QPlainTextEdit;
namespace nuspell {
inline namespace v5 {
class Dictionary;
}
} // namespace nuspell
QT_END_NAMESPACE

class SpellChecker : public MarkdownHighlighter
{
    Q_OBJECT
public:
    explicit SpellChecker(QPlainTextEdit *parent, const QString &lang = QLatin1String());
    ~SpellChecker() override;

    auto setLanguage(const QString &) -> bool;
    [[nodiscard]] auto getLanguage() const -> QString;

    void setMarkdownHighlightingEnabled(const bool);
    [[nodiscard]] inline auto isMarkdownHighlightingEnabled() const -> bool { return markdownhig; };

    void setSpellCheckingEnabled(const bool);
    [[nodiscard]] inline auto isSpellCheckingEnabled() const -> bool
    {
        return spellingEnabled && speller;
    };

    inline void setHeaderColor(QColor color)
    {
        // set all header colors to the same color
        for (int i = HighlighterState::H1; i <= HighlighterState::H6; ++i) {
            _formats[static_cast<HighlighterState>(i)].setForeground(color);
        }
    }
    [[nodiscard]] inline QColor getHeaderColor() { return _formats[H1].foreground().color(); }

    Q_REQUIRED_RESULT static auto getLanguageList() -> const QStringList;

    void highlightBlock(const QString &text) override;

    Q_REQUIRED_RESULT auto isCorrect(const QString &word) const -> bool;

    Q_REQUIRED_RESULT auto getSuggestion(const QString &) const -> QStringList;

    void addWord(const QString &);

    void ignoreWord(const QString &);

    static void populateLangMap();

    void applyCustomStyle();

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    using StringView = QString;
#else
    using StringView = QStringView;
#endif

public Q_SLOTS:
    void checkSpelling(const StringView &);

Q_SIGNALS:
    void languageChanged(const QString &lang = QLatin1String());

private Q_SLOTS:
    void slotAddWord();

    void slotIgnoreWord();

    void slotSetLanguage(const bool);

    void slotReplaceWord();

private:
    bool spellingEnabled = true;
    bool markdownhig = true;

    auto getWord(const QTextBlock &, const int) -> QString;

    bool saveUserDict();
    void loadUserDict(const QString &);

    void showContextMenu(const QPoint);

    QString language;
    void replaceWord(const int wordPos, const QString &newWord);

    QPlainTextEdit *textEdit = nullptr;

    static auto encodeLanguageString(const QString &langString) -> QString;

    // Spell checker
    nuspell::Dictionary *speller = nullptr;
    QStringList wordList;
    QStringList sessionWordList;
};

#endif // SPELLCHECKER_H
