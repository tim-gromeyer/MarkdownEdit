#include "spellchecker.h"
#include "markdownhighlighter.h"

#include <QLocale>
#include <QtDebug>
#include <QMenu>
#include <QRegularExpression>
#include <QActionGroup>

#include <enchant++.h>


static const QRegularExpression expr(QStringLiteral("\\W+"));

#ifdef CHECK_MARKDOWN
static const QRegularExpression removeLinks(QStringLiteral("(\\(http[s]?:\\/\\/[\\w\\-\\_\\.\\/]*)(\n)?([\\w\\-\\_\\.\\/]*\\))"));
static const QRegularExpression removeCode(QStringLiteral("\\`{1,3}([^\\`]+)\\`{1,3}"));
#endif

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

#ifdef CHECK_MARKDOWN
SpellChecker::SpellChecker(QPlainTextEdit *parent, const QString &lang)
    : MarkdownHighlighter{parent->document()},
    textEdit(parent)
#else
SpellChecker::SpellChecker(QPlainTextEdit *parent, const QString &lang)
    : QSyntaxHighlighter{parent->document()},
    textEdit(parent)
#endif
{
    spellFormat.setFontUnderline(true);
    spellFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    spellFormat.setUnderlineColor(Qt::red);

    setLanguage(lang);

    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit, &QWidget::customContextMenuRequested,
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

#if QT_VERSION > QT_VERSION_CHECK(5, 14, 0)
    const QStringList wordList = text.split(expr, Qt::SkipEmptyParts);
#else
    const QStringList wordList = text.split(expr, QString::SkipEmptyParts);
#endif
    int index = 0;

    for (const QString &word : wordList) {
        index = text.indexOf(word, index);

        if (!isCorrect(word)) {
            QTextCharFormat fmt = format(index);
            fmt.merge(spellFormat);
            setFormat(index, word.length(), fmt);
        }
        index += word.length();
    }
}

bool SpellChecker::isCorrect(const QString &word) {
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
}

bool SpellChecker::setLanguage(const QString &lang)
{
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
}

void SpellChecker::addWort(const QString &word)
{
    if (!speller) return;

    speller->add(word.toStdString());
    checkSpelling(word);
}

const QStringList SpellChecker::getLanguageList()
{
    enchant::Broker *broker = get_enchant_broker();
    QStringList languages;
    broker->list_dicts(dict_describe_cb, &languages);
    std::sort(languages.begin(), languages.end());
    return qAsConst(languages);
}

QStringList SpellChecker::getSuggestion(const QString& word)
{
    QStringList list;
    if(speller){
        std::vector<std::string> suggestions;
        speller->suggest(word.toUtf8().data(), suggestions);
        for(std::string &suggestion : suggestions){
            list.append(QString::fromStdString(suggestion));
        }
    }
    return list;
}

void SpellChecker::showContextMenu(QMenu* menu, const QPoint& pos, int wordPos)
{
    QAction *insertPos = menu->actions().at(0);
    if(speller && spellingEnabled){
        // QTextCharFormat fmt = format(wordPos + 1);
        QTextCursor c = textEdit->textCursor();
        c.setPosition(wordPos);
        c.select(QTextCursor::WordUnderCursor);
        const QString word = c.selectedText();

        if(!isCorrect(word)) {
            QStringList suggestions = getSuggestion(word);
            if(!suggestions.isEmpty()) {
                for(int i = 0, n = qMin(10, suggestions.length()); i < n; ++i) {
                    QAction* action = new QAction(suggestions[i], menu);
                    action->setProperty("wordPos", wordPos);
                    action->setProperty("suggestion", suggestions[i]);
                    connect(action, &QAction::triggered, this, &SpellChecker::slotReplaceWord);
                    menu->insertAction(insertPos, action);
                }
                if(suggestions.length() > 10) {
                    QMenu* moreMenu = new QMenu(tr("More..."), menu);
                    for(int i = 10, n = suggestions.length(); i < n; ++i){
                        QAction* action = new QAction(suggestions[i], moreMenu);
                        action->setProperty("wordPos", wordPos);
                        action->setProperty("suggestion", suggestions[i]);
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
    if (!speller) return;

    speller->add_to_session(word.toUtf8().data());
    checkSpelling(word);
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
    delete speller;
}
