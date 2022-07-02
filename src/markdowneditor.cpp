#include "markdowneditor.h"
#include "spellchecker.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QDebug>


MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QMarkdownTextEdit(parent, false)
{
    checker = new SpellChecker(new TextEditProxyT(this));
    connect(checker, &SpellChecker::languageChanged,
            this, &MarkdownEditor::onLanguageChanged);
}

bool MarkdownEditor::setLanguage(const QString &lang)
{
    if (checker->getLanguageList().contains(lang))
        return checker->setLanguage(lang);
    else
        return false;
}

QString MarkdownEditor::getFileName()
{
    return QFileInfo(fileName).fileName();
}

QString MarkdownEditor::getDir()
{
    return QFileInfo(fileName).path();
}

void MarkdownEditor::changeSpelling(const bool &c)
{
    checker->setSpellCheckingEnabled(c);
}

void MarkdownEditor::onLanguageChanged(const QString &l)
{
    if (!fileName.isEmpty()) {
        setMapAttribute(fileName, l);
    }
}

void MarkdownEditor::showMarkdownSyntax()
{
    QDialog d(this);
    QHBoxLayout l(&d);
    d.setLayout(&l);
    l.setContentsMargins(0, 0, 0, 0);
    d.resize(size());

    QMarkdownTextEdit e(&d, true);

    QFile f(QStringLiteral(":/syntax_%1.md").arg(checker->getLanguage().split("_").at(0)), &d);
    if (!f.open(QIODevice::ReadOnly)) {
        f.setFileName(QStringLiteral(":/syntax_en.md"));
        f.open(QIODevice::ReadOnly);
    }
    e.setPlainText(f.readAll());
    e.setReadOnly(true);
    l.addWidget(&e);

    d.exec();
}

void MarkdownEditor::setText(const QString &t, const QString &newFile)
{
    if (!newFile.isEmpty())
        fileName = newFile;

    QMap<QString, QVariant> map = getLanguageMap();

    if (fileName == QStringLiteral(":/default.md") && !map.contains(fileName))
        map[fileName] = QLatin1String("en_US");

    if (checker) {
        checker->clearDirtyBlocks();
        checker->setDocument(nullptr);
        if (map.contains(fileName))
            checker->setLanguage(map[fileName].toString());
        else
            map[fileName] = checker->getLanguage();
    }

    setLanguageMap(map);

    QMarkdownTextEdit::setPlainText(t);

    if (checker)
        checker->setDocument(document());

    document()->setModified(false);
}

MarkdownEditor::~MarkdownEditor()
{
    // delete checker;
}
