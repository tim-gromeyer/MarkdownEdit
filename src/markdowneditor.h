#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include "qmarkdowntextedit.h"

QT_BEGIN_NAMESPACE
class SpellChecker;
QT_END_NAMESPACE


class MarkdownEditor : public QMarkdownTextEdit
{
    Q_OBJECT
public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor();

    void setText(const QString &, const QString &newFile = QLatin1String(), const bool setLangugae = true);

    bool setLanguage(const QString & = QLatin1String());

    inline SpellChecker* getChecker() { return checker; };

    void changeSpelling(const bool &);

    inline void setFile(const QString &file) { fileName = file; };
    QString getDir();
    QString getFileName();
    inline QString getPath() { return fileName; };

    QString filePath();

protected:
    void dragEnterEvent(QDragEnterEvent *) override;
    void dragMoveEvent(QDragMoveEvent *) override;
    void dropEvent(QDropEvent *) override;

public slots:
    static void showMarkdownSyntax();

private slots:
    void onLanguageChanged(const QString &);

private:
    SpellChecker *checker = nullptr;

    QString fileName = QString::fromLatin1("");
};

#endif // MARKDOWNEDITOR_H
