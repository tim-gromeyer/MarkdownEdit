#include "spellchecker.h"
#include "common.h"

#ifdef CHECK_MARKDOWN
#include "markdownhighlighter.h"
#endif

#include <QLocale>
#include <QtDebug>
#include <QMenu>
#include <QRegularExpression>
#include <QActionGroup>

#ifndef NO_SPELLCHECK
#include <enchant++.h>
#endif

#ifndef CHECK_MARKDOWN
static const QRegularExpression expr(QStringLiteral("\\W+"));
#endif


#ifndef NO_SPELLCHECK
static void dict_describe_cb(const char* const lang_tag,
                             const char* const /*provider_name*/,
                             const char* const /*provider_desc*/,
                             const char* const /*provider_file*/,
                             void* user_data)
{
    QStringList *languages = static_cast<QStringList*>(user_data);
    languages->append(lang_tag);
}

static enchant::Broker* get_enchant_broker() {
    static enchant::Broker broker;
    return &broker;
}
#endif

#ifdef CHECK_MARKDOWN
SpellChecker::SpellChecker(TextEditProxy *parent, const QString &lang)
    : MarkdownHighlighter{parent->document()},
    textEdit(parent)
#else
SpellChecker::SpellChecker(TextEditProxy *parent, const QString &lang)
    : QSyntaxHighlighter{parent->document()},
    textEdit(parent)
#endif
{
    setLanguage(lang);

    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit, &TextEditProxy::customContextMenuRequested,
            this, &SpellChecker::slotShowContextMenu);
}

void SpellChecker::highlightBlock(const QString &text)
{
#ifdef CHECK_MARKDOWN
    if (markdownhig)
        MarkdownHighlighter::highlightBlock(text);
#endif

    if (spellingEnabled)
        checkSpelling(text);
}

void SpellChecker::checkSpelling(const QString &text)
{
    if (!spellingEnabled) return;

#ifdef CHECK_MARKDOWN
    QStringList wordList;
    QString word;

    bool isLink = false;
    const int currentBlockNumber = currentBlock().blockNumber();

    if (isCodeBlock(currentBlockState()))
        return;

    const int textLength = text.length();

    for (int i = 0; i < textLength; i++) {
        const QChar &c = text.at(i);
        const bool isLetterOrNumber = c.isLetterOrNumber();

        if (isPosInACodeSpan(currentBlockNumber, i))
            continue;

        if (c == QLatin1Char('('))
            if (text.mid(i +1, 4) == QStringLiteral("http"))
                isLink = true;

        if (isLink) {
            if (c.isSpace() || c == QLatin1Char(')')) {
                isLink = false;
            }
        }
        else if (i == textLength -1 && isLetterOrNumber) {
            word.append(c);
            wordList.append(word);
            word.clear();
        }
        else if (isLetterOrNumber)
                word.append(c);
        else if (c == QLatin1Char('_') || c == QLatin1Char('-')) {
            if (i +1 < textLength) {
                if (text.at(i +1).isLetter())
                    word.append(c);
                }
        }
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

    for (const QString &word_ : wordList) {
        index = text.indexOf(word_, index);

        if (!isCorrect(word_)) {
            QTextCharFormat fmt = format(index);
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

#ifdef CHECK_MARKDOWN
    if (markdownCharachters.contains(word)) return true;
#endif

    try {
        return speller->check(word.toUtf8().data());
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
    delete speller;
    speller = nullptr;
    language = lang;

    // Determine language from system locale
    if(lang.isEmpty()) {
        language = QLocale::system().name();
        if(language.toLower() == "c" || language.isEmpty()) {
            qWarning() << "Cannot use system locale " << language;
            language = QLatin1String();
            return false;
        }
    }

    // Request dictionary
    try {
        speller = get_enchant_broker()->request_dict(language.toStdString());
        emit languageChanged(lang);
    } catch(enchant::Exception& e) {
        qWarning() << "Failed to load dictionary: " << e.what();
        language = QLatin1String();
        return false;
    }

#ifdef CHECK_MARKDOWN
    const bool wasMarkdownHigh = markdownhig;
    rehighlight();
    markdownhig = wasMarkdownHigh;
#else
    rehighlight();
#endif

    return true;
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
    return languages;
#endif
}

QStringList SpellChecker::getSuggestion(const QString& word)
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

void SpellChecker::showContextMenu(QMenu* menu, const QPoint& pos, int wordPos)
{
    QAction *insertPos = menu->actions().at(0);

    if(speller && spellingEnabled){
        QTextCursor c = textEdit->textCursor();
        c.setPosition(wordPos);
        c.select(QTextCursor::WordUnderCursor);
        const QString word = c.selectedText();

        if(!isPosInACodeSpan(c.blockNumber(), c.positionInBlock() -1)
            && !isCorrect(word)) {
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
        QActionGroup *actionGroup = new QActionGroup(languagesMenu);
        for (const QString &lang : getLanguageList()) {
            QAction* action = new QAction(encodeLanguageString(lang), languagesMenu);
            action->setData(lang);
            action->setCheckable(true);
            action->setChecked(lang == getLanguage());
            connect(action, &QAction::triggered, this, &SpellChecker::slotSetLanguage);
            languagesMenu->addAction(action);
            actionGroup->addAction(action);
        }

        menu->insertMenu(insertPos, languagesMenu);
        menu->insertSeparator(insertPos);
    }

    menu->exec(pos);
    menu->deleteLater();
    delete menu;
}

QString SpellChecker::encodeLanguageString(const QString &lang)
{
    if (langMap.contains(lang))
        return langMap[lang].toString();

#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    const QLocale l(lang);
    return QStringLiteral("%1 (%2)").arg(l.nativeLanguageName(), l.nativeTerritoryName());
#else
    const QLocale l(lang);
    return QStringLiteral("%1 (%2)").arg(l.nativeLanguageName(), l.nativeCountryName());
#endif
}

void SpellChecker::slotAddWord()
{
    QAction *a = qobject_cast<QAction*>(sender());
    addWort(a->data().toString());
}

void SpellChecker::slotIgnoreWord()
{
    QAction *a = qobject_cast<QAction*>(sender());
    ignoreWord(a->data().toString());
}

void SpellChecker::slotReplaceWord()
{
    QAction* action = qobject_cast<QAction*>(sender());
    replaceWord(action->property("wordPos").toInt(),
                action->property("suggestion").toString());
}

void SpellChecker::slotSetLanguage(const bool &checked)
{
    if(checked) {
        QAction* action = qobject_cast<QAction*>(sender());
        language = action->data().toString();
        if(!setLanguage(language)){
            action->setChecked(false);
            language = QLatin1String();
        }
    }
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

void SpellChecker::slotShowContextMenu(const QPoint &pos)
{
    const QPoint globalPos = textEdit->mapToGlobal(pos);
    QMenu *menu = textEdit->createStandardContextMenu();
    const int wordPos = textEdit->cursorForPosition(pos).position();
    showContextMenu(menu, globalPos, wordPos);
}

#ifdef CHECK_MARKDOWN
void SpellChecker::setMarkdownHighlightingEnabled(const bool &enabled)
{
    markdownhig = enabled;

    rehighlight();
}
#endif

void SpellChecker::setSpellCheckingEnabled(const bool &enabled)
{
    spellingEnabled = enabled;

    rehighlight();

    if (enabled)
        return;

    setDocument(Q_NULLPTR);
    setDocument(textEdit->document());
}


SpellChecker::~SpellChecker()
{
#ifndef NO_SPELLCHECK
    delete speller;
#endif
}
