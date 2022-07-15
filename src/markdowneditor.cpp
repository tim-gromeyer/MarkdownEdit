#include "markdowneditor.h"
#include "qdir.h"
#include "qlocale.h"
#include "qmessagebox.h"
#include "spellchecker.h"
#include "common.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMimeData>
#include <QImageReader>


MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QMarkdownTextEdit(parent, false)
{
    checker = new SpellChecker(new TextEditProxyT(this));
    connect(checker, &SpellChecker::languageChanged,
            this, &MarkdownEditor::onLanguageChanged);

    setAcceptDrops(true);
}

void MarkdownEditor::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MarkdownEditor::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MarkdownEditor::dropEvent(QDropEvent *event)
{
    const QMimeData *data = event->mimeData();

    if (data->hasUrls()) {
        const QUrl file = data->urls().at(0);
        if (!QImageReader::imageFormat(file.toLocalFile()).isEmpty()) {
            QTextCursor c = cursorForPosition(event->pos());
            c.insertText(
                        QStringLiteral("![%1](%2)").arg(file.fileName(),
                                                        QLatin1String(file.toEncoded())));
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

bool MarkdownEditor::setLanguage(const QString &lang)
{
    if (checker->getLanguageList().contains(lang))
        return checker->setLanguage(lang);
    else
        return false;
}

QString MarkdownEditor::filePath()
{
    if (getDir() == QLatin1Char('.'))
        return getFileName() + QLatin1String("[*]");
    else {
        QFileInfo info(fileName);
        return QStringLiteral("%1[*] (%2)").arg(info.fileName(),
                                                info.path()).replace(common::homeDict(), QChar('~'));
    }
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

    for (const QString &lang : common::languages()) {
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

    if (fileName == QLatin1String(":/default.md") && !mapContains(fileName))
        setMapAttribute(fileName, QLatin1String("en-US"));

    if (checker) {
        checker->clearDirtyBlocks();
        checker->setDocument(nullptr);
        if (mapContains(fileName))
            checker->setLanguage(mapAttribute(fileName));
        else
            if (checker->getLanguage().isEmpty())
                if (!checker->setLanguage(QLocale::system().name()))
                    checker->setLanguage(QLatin1String("en-US"));
    }

    QMarkdownTextEdit::setPlainText(t);

    checker->setDocument(document());

    document()->setModified(false);
}

MarkdownEditor::~MarkdownEditor()
{
    // delete checker;
}
