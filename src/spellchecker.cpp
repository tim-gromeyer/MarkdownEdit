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

#include "spellchecker.h"
#include "settings.h"
#include "utils.h"

#include <QActionGroup>
#include <QDataStream>
#include <QDebug> // needed for qWaring()
#include <QFile>
#include <QLocale>
#include <QMenu>
#include <QPlainTextEdit>
#include <QRunnable>
#include <QStandardPaths>

#ifndef NO_SPELLCHECK
#include <nuspell/dictionary.hxx>
#include <nuspell/finder.hxx>
#endif

using std::as_const;

static const QString userDictDir = QStandardPaths::writableLocation(
    QStandardPaths::AppLocalDataLocation);

struct Language
{
    std::string dir;
    QString name;
    QString encodedName;

#ifndef NO_SPELLCHECK
    nuspell::v5::Dictionary *dict = nullptr;
    int instanceCount = 0;
#endif
};
QHash<QString, Language> langPathMap;

// We use sliced with Qt 6 (it's faster)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define SUBSTR(text, pos, len) text.mid(pos, len)
#else
#define SUBSTR(text, pos, len) text.sliced(pos, len)
#endif

// NOTE: Language format: de_DE, en_US
SpellChecker::SpellChecker(QPlainTextEdit *parent, const QString &lang)
    : MarkdownHighlighter{parent->document()}
    , textEdit(parent)
{
    applyCustomStyle();

    if (!lang.isEmpty())
        setLanguage(lang);

#ifndef NO_SPELLCHECK
    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit,
            &QPlainTextEdit::customContextMenuRequested,
            this,
            &SpellChecker::showContextMenu);
#endif
    connect(this,
            &SpellChecker::languageChanged,
            this,
            &QSyntaxHighlighter::rehighlight,
            Qt::QueuedConnection);
}

void SpellChecker::applyCustomStyle()
{
    _formats[CodeBlock].setBackground(QColor(51, 51, 51));
    _formats[InlineCodeBlock].setForeground(QColor(52, 101, 164));

    if (settings::isDarkMode()) {
        QPalette p;
        QColor windowColor = p.window().color();
        int windowLightness = windowColor.lightness();

        QColor masked = p.dark().color();
        if (masked.lightness() < windowLightness) {
            masked = windowColor.lighter(255 - windowLightness);
        }

        _formats[List].setForeground(
            _formats[List].foreground().color().lighter(255 - windowLightness));
        _formats[BlockQuote].setForeground(
            _formats[BlockQuote].foreground().color().lighter(255 - windowLightness));
        _formats[MaskedSyntax].setForeground(masked);
        _formats[Link].setForeground(p.link());
    }
}

void SpellChecker::highlightBlock(const QString &text)
{
    if (markdownhig)
        MarkdownHighlighter::highlightBlock(text);

    if (spellingEnabled && !languages.isEmpty())
        checkSpelling(text);
}

void SpellChecker::checkSpelling(const StringView &text)
{
    if (!spellerLoaded() || !spellingEnabled)
        return;

#ifdef NO_SPELLCHECK
    return;
#endif

    QStringList wordList;
    QString word;

    if (isCodeBlock(currentBlockState()))
        return;

    const auto textLength = text.length();

    for (int i = 0; i < textLength; ++i) {
        const QChar c = text[i];
        const bool isLetterOrNumber = c.isLetterOrNumber();

        if (isPosInACodeSpan(currentBlock().blockNumber(), i)) {
            i = getSpanRange(RangeType::CodeSpan, currentBlock().blockNumber(), i).second - 1;
            continue;
        }

        if (isPosInALink(currentBlock().blockNumber(), i)) {
            i = getSpanRange(RangeType::Link, currentBlock().blockNumber(), i).second - 1;
            continue;
        }

        // If last character and letter or number
        if (i == textLength - 1 && isLetterOrNumber) {
            word.append(c);
            wordList.append(word);
            word.clear();
        }
        // If is letter or number append
        else if (isLetterOrNumber)
            word.append(c);
        // Else append the word and reset it.
        else {
            wordList.append(word);
            word.clear();
        }
    }

    qsizetype index = 0;

    for (const QString &word_ : as_const(wordList)) {
        index = text.indexOf(word_, index);

        if (!isCorrect(word_)) {
            QTextCharFormat fmt = QSyntaxHighlighter::format((int) index);
            fmt.setFontUnderline(true);
            fmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            fmt.setUnderlineColor(QColor(255, 0, 0)); // Red
            setFormat((int) index, word_.length(), fmt);
        }
        index += word_.length();
    }
}

auto SpellChecker::isCorrect(const QString &word) const -> bool
{
#ifdef NO_SPELLCHECK
    Q_UNUSED(word);
    return true;
#else
    if (!spellerLoaded() || !spellingEnabled)
        return true;

    if (word.length() < 2)
        return true;
    if (wordList.contains(word) || sessionWordList.contains(word))
        return true;

    std::string wordString = word.toStdString();
    bool spelledCorrectly = false;

    for (const QString &lang : languages) {
        if (langPathMap[lang].dict->spell(wordString)) {
            spelledCorrectly = true;
            break;
        }
    }

    return spelledCorrectly;
#endif
}

auto SpellChecker::setLanguage(const QString &lang) -> bool
{
#ifdef NO_SPELLCHECK
    Q_UNUSED(lang);
    return false;
#else
    if (languages.contains(lang) && !lang.isEmpty())
        return true;

    if (lang.isEmpty())
        return setLanguage(QLocale::system().name());

    Language &dic = langPathMap[lang];
    if (dic.dir.empty()) {
        dic = langPathMap[lang + L1("_frami")];
        if (dic.dir.empty())
            return false;
    } else if (dic.dict) {
        ++dic.instanceCount;
        languages.append(lang);
        return true;
    }

    try {
        dic.dict = new nuspell::Dictionary();
        dic.dict->load_aff_dic(dic.dir);
        ++dic.instanceCount;
        languages.append(lang);
    } catch (const nuspell::Dictionary_Loading_Error &e) {
        qWarning() << "Failed to load dictionary: " << e.what();
        return false;
    }

    loadUserDict();

    Q_EMIT languageChanged(lang);

    return true;
#endif
}

QStringList SpellChecker::getLanguages() const
{
    return languages;
}

void SpellChecker::populateLangMap()
{
#ifndef NO_SPELLCHECK
    if (!langPathMap.isEmpty())
        return;

    static const auto dicts = nuspell::search_default_dirs_for_dicts();

    if (dicts.empty()) {
        qWarning() << "No language dict found";
    }

    struct Language l;

    for (const auto &dic : dicts) {
        l.dir = dic.string();
        l.name = QString::fromStdString(dic.stem().string());
        l.encodedName = encodeLanguageString(l.name);
        langPathMap[l.name] = l;
    }
#endif
}

auto SpellChecker::getLanguageList() -> const QStringList
{
#ifdef NO_SPELLCHECK
    return QStringList();
#else
    if (langPathMap.isEmpty())
        populateLangMap();

    return langPathMap.keys();
#endif
}

auto SpellChecker::getSuggestion(const QString &word, const QString &lang) const -> QStringList
{
#ifdef NO_SPELLCHECK
    Q_UNUSED(word);
    return {};
#else
    QStringList list = {}; // Fix warning

    if (!langPathMap[lang].dict || languages.isEmpty())
        return {};

    std::vector<std::string> suggestions;
    langPathMap[lang].dict->suggest(word.toStdString(), suggestions);

    list.reserve((int) suggestions.size());

    for (std::string &suggestion : suggestions) {
        list.append(QString::fromStdString(suggestion));
    }

    return list;
#endif
}

auto SpellChecker::getWord(const QTextBlock &block, const int pos) -> QString
{
    if (MarkdownHighlighter::isPosInACodeSpan(block.blockNumber(), pos)
        || MarkdownHighlighter::isPosInALink(block.blockNumber(), pos)
        || MarkdownHighlighter::isCodeBlock(block.userState()))
        return QLatin1String();

    QString word;
    const StringView text = block.text();

    for (auto i = 0; i < text.length(); ++i) {
        const QChar c = text[i];

        if (c.isLetterOrNumber())
            word.append(c);
        else if ((c == u',' || c == u'.') && i > 0 && i < text.length() - 1
                 && text[i - 1].isNumber() && text[i + 1].isNumber())
            word.append(c);
        else if (pos <= i)
            return word;
        else
            word.clear();
    }

    return word;
}

void SpellChecker::showContextMenu(const QPoint pos)
{
    QMenu *menu = textEdit->createStandardContextMenu();
    QTextCursor c = textEdit->cursorForPosition(pos);

    const auto wordPos = c.position();

    QAction *insertPos = menu->actions().at(0);

    if (spellerLoaded() && spellingEnabled) {
        c.clearSelection();

        const QString word = getWord(c.block(), c.positionInBlock());

        if (!isCorrect(word)) {
            for (const QString &lang : languages) {
                const QStringList suggestions = getSuggestion(word, lang);
                if (!suggestions.isEmpty()) {
                    QMenu *langMenu = new QMenu(langPathMap[lang].encodedName);
                    for (int i = 0; i < suggestions.length(); i++) {
                        auto *action = new QAction(suggestions.at(i), langMenu);
                        action->setProperty("wordPos", wordPos);
                        action->setProperty("suggestion", suggestions.at(i));
                        connect(action, &QAction::triggered, this, &SpellChecker::slotReplaceWord);
                        langMenu->insertAction(insertPos, action);
                    }
                    menu->insertMenu(insertPos, langMenu);
                }
            }
            if (!languages.isEmpty()) {
                menu->insertSeparator(insertPos);
            }

            auto *addAction = new QAction(tr("Add \"%1\" to dictionary").arg(word), menu);
            addAction->setData(word);
            connect(addAction, &QAction::triggered, this, &SpellChecker::slotAddWord);
            menu->insertAction(insertPos, addAction);

            auto *ignoreAction = new QAction(tr("Ignore \"%1\"").arg(word), menu);
            ignoreAction->setData(word);
            connect(ignoreAction, &QAction::triggered, this, &SpellChecker::slotIgnoreWord);
            menu->insertAction(insertPos, ignoreAction);
            menu->insertSeparator(insertPos);
        }
    }

    auto *languagesMenu = new QMenu(tr("Languages"), menu);

    QMap<QString, Language> sorted;
    for (const Language &lang : as_const(langPathMap))
        sorted[lang.encodedName] = lang;

    for (const Language &lang : as_const(sorted)) {
        auto *action = new QAction(lang.encodedName, languagesMenu);
        action->setData(lang.name);
        action->setCheckable(true);
        action->setChecked(languages.contains(lang.name));
        connect(action, &QAction::triggered, this, &SpellChecker::slotSetLanguage);
        languagesMenu->addAction(action);
    }

    menu->insertMenu(insertPos, languagesMenu);
    menu->insertSeparator(insertPos);

    menu->exec(textEdit->mapToGlobal(pos));

    menu->deleteLater();
    delete menu;
}

auto SpellChecker::encodeLanguageString(const QString &langString) -> QString
{
    static QHash<QString, QString> codeNameMap;

    if (!codeNameMap.contains(langString)) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        const QLocale l(langString);
        codeNameMap[langString] = QStringLiteral("%1 (%2)").arg(l.nativeLanguageName(),
                                                                l.nativeTerritoryName());
#else
        const QLocale l(langString);
        codeNameMap[langString] = QStringLiteral("%1 (%2)").arg(l.nativeLanguageName(),
                                                                l.nativeCountryName());
#endif
    }

    return codeNameMap[langString];
}

void SpellChecker::slotAddWord()
{
    auto *a = qobject_cast<QAction *>(sender());
    addWord(a->data().toString());

    rehighlight();
}

void SpellChecker::slotIgnoreWord()
{
    auto *a = qobject_cast<QAction *>(sender());
    ignoreWord(a->data().toString());

    rehighlight();
}

void SpellChecker::slotReplaceWord()
{
    QObject *action = sender();
    replaceWord(action->property("wordPos").toInt(), action->property("suggestion").toString());
}

void SpellChecker::slotSetLanguage(const bool checked)
{
    auto *action = qobject_cast<QAction *>(sender());
    const QString lang = action->data().toString();

    if (!checked) // deselected
    {
#ifndef NO_SPELLCHECK
        languages.removeAll(lang);
        Language &language = langPathMap[lang];
        --language.instanceCount;

        if (language.instanceCount == 0) {
            qDebug() << __func__ << "Deleting dict:" << language.encodedName;
            delete language.dict;
            language.dict = nullptr;
        }

        rehighlight();
        return;
#endif
    }

    threading::runFunction([this, lang] { setLanguage(lang); });
}

void SpellChecker::addWord(const QString &word)
{
    wordList.append(word);

    saveUserDict();
}

void SpellChecker::ignoreWord(const QString &word)
{
    sessionWordList.append(word);
}

void SpellChecker::replaceWord(const int wordPos, const QString &newWord)
{
    QTextCursor c = textEdit->textCursor();
    c.clearSelection();
    c.setPosition(wordPos);
    c.select(QTextCursor::WordUnderCursor);
    c.insertText(newWord);
}

void SpellChecker::setMarkdownHighlightingEnabled(const bool enabled)
{
    if ((enabled && !markdownhig) || (!enabled && markdownhig)) {
        markdownhig = enabled;

        QSyntaxHighlighter::rehighlight();
    }
}

void SpellChecker::setSpellCheckingEnabled(const bool enabled)
{
    if ((enabled && !spellingEnabled) || (!enabled && spellingEnabled)) {
        spellingEnabled = enabled;

        QSyntaxHighlighter::rehighlight();
    }
}

bool SpellChecker::saveUserDict()
{
    QFile f(userDictDir + STR("/user-dict.dic"));
    if (f.open(QIODevice::WriteOnly)) {
        QDataStream out(&f);
        out << wordList;
    }
    f.close();
    return true;
}

void SpellChecker::loadUserDict()
{
    wordList.clear();

    QFile f(userDictDir + STR("/user-dict.dic"));
    if (!f.open(QIODevice::ReadOnly))
        return;

    QDataStream in(&f);
    in >> wordList;
}

bool SpellChecker::spellerLoaded() const
{
#ifdef NO_SPELLCHECK
    return false;
#else
    for (const QString &lang : languages) {
        if (langPathMap[lang].dict) {
            return true;
        }
    }

    return true;
#endif
}

SpellChecker::~SpellChecker()
{
#ifndef NO_SPELLCHECK
    for (const QString &lang : languages) {
        Language &language = langPathMap[lang];
        --language.instanceCount;

        if (language.instanceCount == 0) {
            qDebug() << __func__ << "Deleting dict:" << language.encodedName;
            delete language.dict;
            language.dict = nullptr;
        }
    }
#endif
};
