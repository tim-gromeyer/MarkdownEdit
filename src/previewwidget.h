/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 Tim Gromeyer <sakul8826@gmail.com>.
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


#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QTextBrowser>


class PreviewWidget : public QTextBrowser
{
    Q_OBJECT
public:
    PreviewWidget(QWidget *parent = nullptr);
    ~PreviewWidget() override = default;

    void scrollToHeader(QString name);

    inline void setSearchPaths(const QStringList &searchPaths) { QTextBrowser::setSearchPaths(searchPaths); };
    inline auto searchPaths() -> QStringList { return QTextBrowser::searchPaths(); };
    inline void appenSearchPath(const QString &path) { QTextBrowser::setSearchPaths(QTextBrowser::searchPaths() << path); };

    inline void setLoadImages(const bool ok) { load = ok; };
    inline auto loadImages() -> bool { return load; };

    auto loadResource(int type, const QUrl &name) -> QVariant override;

public Q_SLOTS:
    void setHtml(const QString &);
    void openUrl(const QUrl &);

private:
    bool load = true;
};

#endif // PREVIEWWIDGET_H
