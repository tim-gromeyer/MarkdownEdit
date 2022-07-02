#include "markdowneditor.h"
#include "qdir.h"
#include "qlocale.h"
#include "qmessagebox.h"
#include "spellchecker.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QFile>
#include <QFileInfo>


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
    QDialog d;
    QHBoxLayout l(&d);
    d.setLayout(&l);
    l.setContentsMargins(0, 0, 0, 0);

    MarkdownEditor e(&d);

    QString file = QLatin1String(":/syntax_en.md");
    QString language = QLatin1String("en_US");

    for (const QString &lang : QLocale::system().uiLanguages()) {
        if (QFile::exists(QStringLiteral(":/syntax_%1.md").arg(lang))) {
            file = QStringLiteral(":/syntax_%1.md").arg(lang);
            language = lang;
            break;
        }
    }

    QFile f(file, &d);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(&d, tr("Couldn't open file"),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(file), f.errorString()));
    }
    e.setText(f.readAll());
    e.setReadOnly(true);
    if (!e.setLanguage(language))
        e.setLanguage(QLocale::system().name());
    l.addWidget(&e);

    d.showMaximized();
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
