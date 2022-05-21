#include "spellchecker.h"
#include "markdownhighlighter.h"

#include <QLocale>
#include <QtDebug>
#include <QMenu>
#include <QRegularExpression>
#include <QActionGroup>

#include <enchant++.h>


static const QRegularExpression expr("\\W+");

static void dict_describe_cb(const char* const lang_tag,
                             const char* const /*provider_name*/,
                             const char* const /*provider_desc*/,
                             const char* const /*provider_file*/,
                             void* user_data)
{
    QStringList* languages = static_cast<QStringList*>(user_data);
    languages->append(lang_tag);
}

static enchant::Broker* get_enchant_broker() {
    static enchant::Broker broker;
    return &broker;
}

SpellChecker::SpellChecker(QPlainTextEdit *parent, const QString &lang)
    : MarkdownHighlighter{parent->document()},
    textEdit(parent)
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
    if (markdownhig)
        MarkdownHighlighter::highlightBlock(text);

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
    qDebug() << wordList;
    int index = 0;

    for (const QString &word : wordList) {
        qDebug() << word << isCorrect(word);
        index = text.indexOf(word, index);

        if (!isCorrect(word)) {
            QTextCharFormat fmt = QSyntaxHighlighter::format(index + 1);
            fmt.merge(spellFormat);
            setFormat(index, word.length(), fmt);
        }
        index += word.length();
    }
}

bool SpellChecker::isCorrect(const QString &word) {
    if (!speller || !spellingEnabled) return true;

    if (markdownCharachters.contains(word) || word.length() < 2) return true;

    try {
        return speller->check(word.toUtf8().data());
    }
    catch (const enchant::Exception &e){
        qDebug() << e.what();
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
    } catch(enchant::Exception& e) {
        qWarning() << "Failed to load dictionary: " << e.what();
        language = QLatin1String();
        return false;
    }

    const bool wasMarkdownHigh = markdownhig;
    rehighlight();
    markdownhig = wasMarkdownHigh;

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
    enchant::Broker * broker = get_enchant_broker();
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
        for(std::size_t i = 0, n = suggestions.size(); i < n; ++i){
            list.append(QString::fromUtf8(suggestions[i].c_str()));
        }
    }
    return list;
}

void SpellChecker::showContextMenu(QMenu* menu, const QPoint& pos, int wordPos)
{
    QAction* insertPos = menu->actions().at(0);
    if(speller && spellingEnabled){
        QTextCursor c = textEdit->textCursor();
        c.setPosition(wordPos);
        c.select(QTextCursor::WordUnderCursor);
        const QString word = c.selectedText();

        if(!isCorrect(word)) {
            QStringList suggestions = getSuggestion(word);
            if(!suggestions.isEmpty()){
                for(int i = 0, n = qMin(10, suggestions.length()); i < n; ++i) {
                    QAction* action = new QAction(suggestions[i], menu);
                    action->setProperty("wordPos", wordPos);
                    action->setProperty("word", word);
                    action->setProperty("suggestion", suggestions[i]);
                    connect(action, &QAction::triggered, this, &SpellChecker::slotReplaceWord);
                    menu->insertAction(insertPos, action);
                }
                if(suggestions.length() > 10) {
                    QMenu* moreMenu = new QMenu(menu);
                    for(int i = 10, n = suggestions.length(); i < n; ++i){
                        QAction* action = new QAction(suggestions[i], moreMenu);
                        action->setProperty("wordPos", wordPos);
                        action->setProperty("word", word);
                        action->setProperty("suggestion", suggestions[i]);
                        connect(action, &QAction::triggered, this, &SpellChecker::slotReplaceWord);
                        moreMenu->addAction(action);
                    }
                    QAction* action = new QAction(tr("More..."), menu);
                    menu->insertAction(insertPos, action);
                    action->setMenu(moreMenu);
                }
                menu->insertSeparator(insertPos);
            }

            QAction* addAction = new QAction(tr("Add \"%1\" to dictionary").arg(word), menu);
            addAction->setData(word);
            addAction->setProperty("wordPos", wordPos);
            connect(addAction, &QAction::triggered, this, &SpellChecker::slotAddWord);
            menu->insertAction(insertPos, addAction);

            QAction* ignoreAction = new QAction(tr("Ignore \"%1\"").arg(word), menu);
            ignoreAction->setData(word);
            ignoreAction->setProperty("wordPos", wordPos);
            connect(ignoreAction, &QAction::triggered, this, &SpellChecker::slotIgnoreWord);
            menu->insertAction(insertPos, ignoreAction);
            menu->insertSeparator(insertPos);
        }

        QMenu* languagesMenu = new QMenu(tr("Languages"), menu);
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
    const QString word = a->data().toString();
    addWort(word);
}

void SpellChecker::slotIgnoreWord()
{
    const QString word = qobject_cast<QAction*>(sender())->data().toString();
    ignoreWord(word);
}

void SpellChecker::slotReplaceWord()
{
    QAction* action = qobject_cast<QAction*>(sender());
    replaceWord(action->property("wordPos").toInt(),
                action->property("word").toString(),
                action->property("suggestion").toString());
}

void SpellChecker::slotSetLanguage(const bool &checked)
{
    if(checked) {
        QAction* action = qobject_cast<QAction*>(QObject::sender());
        QString lang = action->data().toString();
        if(!setLanguage(lang)){
            action->setChecked(false);
            lang = QLatin1String();
        }
    }
}

void SpellChecker::ignoreWord(const QString &word)
{
    if (!speller) return;

    speller->add_to_session(word.toUtf8().data());
    checkSpelling(word);
}

void SpellChecker::replaceWord(const int &wordPos, const QString &word, const QString &newWord)
{
    QTextCursor c = textEdit->textCursor();
    c.setPosition(wordPos);
    c.selectionStart();
    c.setPosition(wordPos + word.length());
    c.selectionEnd();
    c.insertText(newWord);
}

void SpellChecker::slotShowContextMenu(const QPoint &pos)
{
    QPoint globalPos = textEdit->mapToGlobal(pos);
    QMenu* menu = textEdit->createStandardContextMenu();
    int wordPos = textEdit->cursorForPosition(pos).position();
    showContextMenu(menu, globalPos, wordPos);
}

void SpellChecker::setMarkdownHighlightingEnabled(const bool &enabled)
{
    markdownhig = enabled;

    rehighlight();
    //highlightBlock(textEdit->toPlainText());
}

void SpellChecker::setSpellCheckingEnabled(const bool &enabled)
{
    spellingEnabled = enabled;

    checkSpelling(textEdit->toPlainText());
}

SpellChecker::~SpellChecker()
{
    delete speller;
}
