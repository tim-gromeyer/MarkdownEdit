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

#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include "QFileInfo"

#include "qmarkdowntextedit.h"

QT_BEGIN_NAMESPACE
class SpellChecker;
QT_END_NAMESPACE

class MarkdownEditor : public QMarkdownTextEdit
{
    Q_OBJECT
    Q_PROPERTY(bool autoSaveEnabled MEMBER m_autoSaveEnabled FINAL)

public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor() override;

    void setText(const QByteArray &,
                 const QString &newFile = QLatin1String(),
                 const bool setLangugae = true);

    auto setLanguage(const QString & = QLatin1String()) -> bool;

    inline auto getChecker() -> SpellChecker * { return checker; };

    void changeSpelling(const bool);

    inline void setFile(const QString &file) { info.setFile(file); };
    inline void setFile(const QFileInfo &i) { info = i; };
    inline auto getPath() -> QString { return info.filePath(); };

    auto getDir() -> QString;
    auto getFileName() -> QString;

    auto filePath() -> QString; // aka window title

    inline void setAutoSaveEnabled(bool save)
    {
        m_autoSaveEnabled = save;
        onModificationChanged(document()->isModified());
    }
    inline bool autoSaveEnabled() { return m_autoSaveEnabled; }

    void deleteAutoSaveFile();

public Q_SLOTS:
    void autoSave();

Q_SIGNALS:
    void openFile(const QString);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

    void openUrl(const QString & /**/) override{};

private Q_SLOTS:
    void onLanguageChanged(const QString &);

    // Auto-save related stuff
    void onModificationChanged(bool modified);

private:
    void autoSaveFile(const QString &content);

    SpellChecker *checker = nullptr;

    QTimer *autoSaveTimer = nullptr;
    bool m_autoSaveEnabled = true;

    QFileInfo info;
};

#endif // MARKDOWNEDITOR_H
