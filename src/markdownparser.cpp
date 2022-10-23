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


#include <QByteArray>
#include <QRegularExpression>

#include <qdebug.h>

#include "html2md.h"

#include "common.h" // needed for MD_UNDERLINE
#include "markdownparser.h"
#include "md4c-html.h"

/* Global options. */
#if MD_UNDERLINE
static unsigned parser_flags = MD_FLAG_UNDERLINE;
#else
static unsigned parser_flags = 0;
#endif

const QByteArray templateArray = QByteArrayLiteral("<!DOCTYPE html>\n"
                                                    "<html>\n"
                                                    "<head>\n"
                                                    "</head>\n"
                                                    "<body class=\"preview\">\n");


void captureHtmlFragment(const MD_CHAR* data, const MD_SIZE data_size, void* userData) {
    auto *array = static_cast<QByteArray*>(userData);

    array->append(data, data_size);
}

auto Parser::toHtml(const QString &in, const int dia) -> QString
{
    if (dia == GitHub)
        parser_flags |= MD_DIALECT_GITHUB;
    else
        parser_flags |= MD_DIALECT_COMMONMARK;

    const QByteArray array = in.toUtf8(); // Use UTF-8 for better support
    QByteArray out = templateArray;
    out.reserve(array.size() *1.28 + 115);

    md_html(array.constData(), array.size(), &captureHtmlFragment, &out,
            parser_flags, MD_HTML_FLAG_SKIP_UTF8_BOM | MD_HTML_FLAG_DEBUG);

    out.append("</body>\n"
               "</html>\n");
    out.squeeze();

    return QString::fromUtf8(out);
}

auto Parser::heading2HTML(const QString &in) -> QString
{
    const QByteArray array = in.toUtf8(); // Use UTF-8 for better support

    QByteArray out;

    md_html(array.constData(), array.size(), &captureHtmlFragment, &out,
            parser_flags, MD_HTML_FLAG_SKIP_UTF8_BOM);

    return QString::fromUtf8(out);
}

auto Parser::toMarkdown(const QString &in) -> QString
{
    auto html = in.toStdString();

    html2md::Converter c(html);
    auto md = c.Convert2Md();

    qDebug() << c.ok();

    // auto md = html2md::Convert(html);

    return QString::fromStdString(md);
}
