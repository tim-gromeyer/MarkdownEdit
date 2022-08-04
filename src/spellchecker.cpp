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


#include "spellchecker.h"

#include <QLocale>
#include <QMenu>
#include <QActionGroup>
#include <QDebug> // needed for qWaring()

#ifndef NO_SPELLCHECK
#include <enchant++.h>
#endif

// Needed for wasm
#ifndef QStringViewLiteral
# define QStringViewLiteral(str) QStringView(QT_UNICODE_LITERAL(str))
#endif

// We use sliced with Qt 6
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #define SUBSTR(text, pos, len) QStringView(text).mid(pos, len)
#else
    #define SUBSTR(text, pos, len) QStringView(text).sliced(pos, len)
#endif


#ifndef NO_SPELLCHECK
static void dict_describe_cb(const char* const lang_tag,
                             const char* const /*provider_name*/,
                             const char* const /*provider_desc*/,
                             const char* const /*provider_file*/,
                             void* user_data)
{
    if (strlen(lang_tag) <= 5) {
        QStringList *languages = static_cast<QStringList*>(user_data);
        languages->append(QString::fromLatin1(lang_tag));
    }
}

static enchant::Broker* get_enchant_broker() {
    static enchant::Broker broker;
    return &broker;
}
#endif

// NOTE: Language format: de_DE, en_US
SpellChecker::SpellChecker(TextEditProxy *parent, const QString &lang)
    : SpellCheckerBaseClass{parent->document()},
    textEdit(parent)
{
    Q_ASSERT(parent);

    if (!lang.isEmpty())
        setLanguage(lang);

#ifndef NO_SPELLCHECK
    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit, &TextEditProxy::customContextMenuRequested,
            this, &SpellChecker::showContextMenu);
#endif
}

void SpellChecker::highlightBlock(const QString &text)
{
#ifdef CHECK_MARKDOWN
    if (markdownhig)
        MarkdownHighlighter::highlightMarkdown(text);
#endif

    if (spellingEnabled)
        checkSpelling(text);
}

void SpellChecker::checkSpelling(const QString &text)
{
    if (!speller || !spellingEnabled) return;

#ifdef CHECK_MARKDOWN
    QStringList wordList;
    QString word;

    const int currentBlockNumber = currentBlock().blockNumber();

    if (isCodeBlock(currentBlockState())) return;

    const int textLength = text.length();

    for (int i = 0; i < textLength; i++) {
        const QChar c = text[i];
        const bool isLetterOrNumber = c.isLetterOrNumber();

        if (isPosInACodeSpan(currentBlockNumber, i))
                continue;

        if (c == QLatin1Char('h')) {
            if (textLength -i >= 10) { // http 4; :// 7; * >1; .de 10
                if (SUBSTR(text, i, 4) == QStringViewLiteral("http")) {
                    const int last = i;

                    i = text.indexOf(QChar(')'), i);

                    if (i == -1) {
                        i = text.indexOf(QChar(' '), last);

                        if (i == -1)
                            break;
                    }
                }
            }
        }

        if (i == textLength -1 && isLetterOrNumber) {
            word.append(c);
            wordList.append(word);
            word.clear();
        }
        else if (isLetterOrNumber)
                word.append(c);
        else {
            wordList.append(word);
            word.clear();
        }
    }
#else
#if QT_VERSION > QT_VERSION_CHECK(5, 14, 0)
    const QStringList wordList = text.split(expr, Qt::SkipEmptyParts);
#else
    const QStringList wordList = text.split(expr, QString::SkipEmptyParts);
#endif
#endif

    int index = 0;

    for (const QString &word_ : qAsConst(wordList)) {
        index = text.indexOf(word_, index); // is still faster

        if (!isCorrect(word_)) {
            QTextCharFormat fmt = QSyntaxHighlighter::format(index);
            fmt.setFontUnderline(true);
            fmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            fmt.setUnderlineColor(Qt::red);
            setFormat(index, word_.length(), fmt);
        }
        index += word_.length();
    }
}

bool SpellChecker::isCorrect(const QString &word) {
#ifdef NO_SPELLCHECK
    Q_UNUSED(word);
    return true;
#else
    if (!speller || !spellingEnabled) return true;

    if (word.length() < 2) return true;

    try {
        return speller->check(word.toStdString());
    }
    catch (const enchant::Exception &e){
        qWarning() << e.what();
        return true;
    }
#endif
}

bool SpellChecker::setLanguage(const QString &lang)
{
#ifdef NO_SPELLCHECK
    Q_UNUSED(lang);
    return false;
#else
    if (lang == language && !lang.isEmpty()) return true;

    if(lang.isEmpty())
        return setLanguage(QLocale::system().name());

    std::string newLang = lang.toStdString();

    enchant::Broker *broker = get_enchant_broker();
        if (!broker->dict_exists(newLang))
            return false;

    // Request dictionary
    try {
        speller.reset(broker->request_dict(newLang));
        language = lang;
        emit languageChanged(language);
    } catch(enchant::Exception& e) {
        qWarning() << "Failed to load dictionary: " << e.what();
        language.clear();
        return false;
    }

    QSyntaxHighlighter::rehighlight();

    return true;
#endif
}

QString SpellChecker::getLanguage()
{
#ifndef NO_SPELLCHECK
    if (speller)
        return QString::fromStdString(speller->get_lang());
    else
        return QLatin1String();
#else
    return QLatin1String();
#endif
}

void SpellChecker::addWort(const QString &word)
{
#ifdef NO_SPELLCHECK
    Q_UNUSED(word);
    return;
#else
    if (!speller) return;

    speller->add(word.toStdString());
    checkSpelling(word);
#endif
}

const QStringList SpellChecker::getLanguageList()
{
#ifdef NO_SPELLCHECK
    return QStringList();
#else
    enchant::Broker *broker = get_enchant_broker();
    QStringList languages;
    broker->list_dicts(dict_describe_cb, &languages);
    std::sort(languages.begin(), languages.end());
    if (languages.isEmpty())
        qWarning() << "No language dict found";

    languages.removeDuplicates();

    return languages;
#endif
}

QStringList SpellChecker::getSuggestion(const QString &word)
{
    QStringList list;
#ifdef NO_SPELLCHECK
    Q_UNUSED(word);
#else
    if(speller){
        std::vector<std::string> suggestions;
        speller->suggest(word.toUtf8().data(), suggestions);
        for(std::string &suggestion : suggestions){
            list.append(QString::fromStdString(suggestion));
        }
    }
#endif
    return list;
}

#ifdef CHECK_MARKDOWN
QString SpellChecker::getWord(const QTextBlock &block, const int &pos)
{
    if (MarkdownHighlighter::isPosInACodeSpan(block.blockNumber(), pos)
        || MarkdownHighlighter::isCodeBlock(block.userState()))
        return QLatin1String();

    QString word;
    const QString text = block.text();

    bool isLink = false;

    const int textLength = text.length();

    for (int i = 0; i < textLength; i++) {
        const QChar c = text[i];
        const bool isLetterOrNumber = c.isLetterOrNumber();

        if (c == QLatin1Char('h')) {
            if (SUBSTR(text, i, 4) == QStringViewLiteral("http")) {
                if (text.indexOf(QChar(QChar::Space), i) > pos)
                    return QLatin1String();
                else if (text.indexOf(QChar(')'), i) > pos)
                    return QLatin1String();

                isLink = true;
            }
        }

        if (isLink) {
            if (c.isSpace() || c == QLatin1Char(')')) {
                isLink = false;
            }
        }
        else if (i == textLength -1 && isLetterOrNumber) {
            word.append(c);
            return word;
        }
        else if (isLetterOrNumber)
            word.append(c);
        else {
            if (pos <= i)
                return word;
            else
                word.clear();
        }
    }

    return QLatin1String();
}
#endif

void SpellChecker::showContextMenu(const QPoint &pos)
{
    QMenu *menu = textEdit->createStandardContextMenu();
    QTextCursor c = textEdit->cursorForPosition(pos);

    const int wordPos = c.position();

    QAction *insertPos = menu->actions().at(0);

    if(speller && spellingEnabled){
        c.clearSelection();

#ifdef CHECK_MARKDOWN
        const QString word = getWord(c.block(), c.positionInBlock());
#else
        c.select(QTextCursor::WordUnderCursor);
        const QString word = c.selectedText();
#endif

        if(!isCorrect(word)) {
            const QStringList suggestions = getSuggestion(word);
            if(!suggestions.isEmpty()) {
                for(int i = 0, n = qMin(10, suggestions.length()); i < n; ++i) {
                    QAction* action = new QAction(suggestions.at(i), menu);
                    action->setProperty("wordPos", wordPos);
                    action->setProperty("suggestion", suggestions.at(i));
                    connect(action, &QAction::triggered, this, &SpellChecker::slotReplaceWord);
                    menu->insertAction(insertPos, action);
                }
                if(suggestions.length() > 10) {
                    QMenu* moreMenu = new QMenu(tr("More..."), menu);
                    for(int i = 10, n = suggestions.length(); i < n; ++i){
                        QAction* action = new QAction(suggestions.at(i), moreMenu);
                        action->setProperty("wordPos", wordPos);
                        action->setProperty("suggestion", suggestions.at(i));
                        connect(action, &QAction::triggered, this, &SpellChecker::slotReplaceWord);
                        moreMenu->addAction(action);
                    }
                    menu->insertMenu(insertPos, moreMenu);
                }
                menu->insertSeparator(insertPos);
            }

            QAction* addAction = new QAction(tr("Add \"%1\" to dictionary").arg(word), menu);
            addAction->setData(word);
            connect(addAction, &QAction::triggered, this, &SpellChecker::slotAddWord);
            menu->insertAction(insertPos, addAction);

            QAction* ignoreAction = new QAction(tr("Ignore \"%1\"").arg(word), menu);
            ignoreAction->setData(word);
            connect(ignoreAction, &QAction::triggered, this, &SpellChecker::slotIgnoreWord);
            menu->insertAction(insertPos, ignoreAction);
            menu->insertSeparator(insertPos);
        }

        QMenu *languagesMenu = new QMenu(tr("Languages"), menu);
        QScopedPointer<QActionGroup> actionGroup(new QActionGroup(languagesMenu)); // Fix warning
        for (const QString &lang : getLanguageList()) {
            QAction* action = new QAction(encodeLanguageString(lang), languagesMenu);
            action->setData(lang);
            action->setCheckable(true);
            action->setChecked(lang == language);
            connect(action, &QAction::triggered, this, &SpellChecker::slotSetLanguage);
            languagesMenu->addAction(action);
            actionGroup->addAction(action);
        }

        menu->insertMenu(insertPos, languagesMenu);
        menu->insertSeparator(insertPos);
    }

    menu->exec(textEdit->mapToGlobal(pos));
    menu->deleteLater();
    delete menu;
}

QString SpellChecker::encodeLanguageString(const QString &langString)
{
    if (!langMap.contains(langString)) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        const QLocale l(langString);
        langMap[langString] = QStringLiteral("%1 (%2)").arg(l.nativeLanguageName(), l.nativeTerritoryName());
#else
        const QLocale l(langString);
        langMap[langString] = QStringLiteral("%1 (%2)").arg(l.nativeLanguageName(), l.nativeCountryName());
#endif
    }

    return langMap[langString];
}

void SpellChecker::slotAddWord()
{
    QAction *a = qobject_cast<QAction*>(sender());
    addWort(a->data().toString());

    rehighlight();
}

void SpellChecker::slotIgnoreWord()
{
    QAction *a = qobject_cast<QAction*>(sender());
    ignoreWord(a->data().toString());
}

void SpellChecker::slotReplaceWord()
{
    QObject* action = sender();
    replaceWord(action->property("wordPos").toInt(),
                action->property("suggestion").toString());
}

void SpellChecker::slotSetLanguage(const bool &checked)
{
    if (!checked) return;

    QAction* action = qobject_cast<QAction*>(sender());
    const QString lang = action->data().toString();

    if(!setLanguage(lang))
        action->setChecked(false);
        language.clear();
}

void SpellChecker::ignoreWord(const QString &word)
{
#ifdef NO_SPELLCHECK
    Q_UNUSED(word);
#else
    if (!speller) return;

    speller->add_to_session(word.toUtf8().data());
    rehighlight();
#endif
}

void SpellChecker::replaceWord(const int &wordPos, const QString &newWord)
{
    QTextCursor c = textEdit->textCursor();
    c.clearSelection();
    c.setPosition(wordPos);
    c.select(QTextCursor::WordUnderCursor);
    c.insertText(newWord);
}

#ifdef CHECK_MARKDOWN
void SpellChecker::setMarkdownHighlightingEnabled(const bool &enabled)
{
    if ((enabled && !markdownhig) || (!enabled && markdownhig)) {
        markdownhig = enabled;

        QSyntaxHighlighter::rehighlight();
    }
}
#endif

// FIXME: Code is spell checked when the markdownhighlighter is disabled
void SpellChecker::setSpellCheckingEnabled(const bool &enabled)
{
    if ((enabled && !spellingEnabled) || (!enabled && spellingEnabled)) {
        spellingEnabled = enabled;

        QSyntaxHighlighter::rehighlight();
    }
}

SpellChecker::~SpellChecker()
{
}
