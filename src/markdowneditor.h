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
    inline auto getPath() -> QString { return info.filePath(); };

    auto getDir() -> QString;
    auto getFileName() -> QString;

    auto filePath() -> QString; // aka window title

    bool bigFile = false;

Q_SIGNALS:
    void openFile(const QString);

protected:
    void dragEnterEvent(QDragEnterEvent *) override;
    void dragMoveEvent(QDragMoveEvent *) override;
    void dropEvent(QDropEvent *) override;

private Q_SLOTS:
    void onLanguageChanged(const QString &);

private:
    SpellChecker *checker = nullptr;

    QFileInfo info;
};

#endif // MARKDOWNEDITOR_H
