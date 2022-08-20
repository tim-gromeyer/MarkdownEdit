/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 Tim Gromeyer <sakul8825@gmail.com>.
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
public:
    PreviewWidget(QWidget *parent = nullptr);

    void scrollToHeader(QString name);

    inline void setSearchPaths(const QStringList &searchPaths) { QTextBrowser::setSearchPaths(searchPaths); };
    inline QStringList searchPaths() { return QTextBrowser::searchPaths(); };
    inline void appenSearchPath(const QString &path) { QTextBrowser::setSearchPaths(QTextBrowser::searchPaths() << path); };

    inline void setLoadImages(const bool ok) { load = ok; };
    inline bool loadImages() { return load; };

    QVariant loadResource(int type, const QUrl &name) override;

public slots:
    void setHtml(const QString &);

private slots:
    void openUrl(const QUrl &);

private:
    bool load = true;
};

#endif // PREVIEWWIDGET_H
