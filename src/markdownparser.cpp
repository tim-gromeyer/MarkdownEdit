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

#include "markdownparser.h"
#include "common.h" // needed for MD_UNDERLINE

#include "html2md.h"
#include "md4c-html.h"

#include <QByteArray>

#include <sstream>

const QByteArray templateArray = QByteArrayLiteral("<!DOCTYPE html>\n"
                                                   "<html>\n"
                                                   "<head>\n"
                                                   "</head>\n"
                                                   "<body class=\"preview\">\n");

void captureHtmlFragment(const MD_CHAR *data, const MD_SIZE data_size, void *userData)
{
    auto *array = static_cast<QByteArray *>(userData);

    array->append(data, (int) data_size);
}

auto Parser::toHtml(const QString &in, const int dia, const int toc_depth) -> QString
{
#if MD_UNDERLINE
    unsigned parser_flags = MD_FLAG_UNDERLINE;
#else
    unsigned parser_flags = 0;
#endif

    MD_TOC_OPTIONS toc;
    toc.depth = 0;
    toc.toc_placeholder = nullptr;

    switch (dia) {
    case Doxygen:
        toc.toc_placeholder = "[TOC]";
        toc.depth = toc_depth;
    case GitHub:
        parser_flags |= MD_DIALECT_GITHUB;
        break;
    case Commonmark:
        parser_flags |= MD_DIALECT_COMMONMARK;
        toc.depth = 0;
        break;
    default:
        break;
    }

    const QByteArray array = in.toLocal8Bit();
    QByteArray out = templateArray;
    out.reserve(array.size() * 1.28 + 115);

    md_html(array.constData(),
            array.size(),
            &captureHtmlFragment,
            &out,
            parser_flags,
            MD_HTML_FLAG_SKIP_UTF8_BOM,
            &toc);

    out.append("</body>\n"
               "</html>\n");
    out.squeeze();

    return QString::fromUtf8(out);
}

void captureHeading(const MD_CHAR *data, const MD_SIZE data_size, void *userData)
{
    auto *ss = (std::stringstream *) userData;
    ss->write(data, data_size);
};

auto Parser::heading2HTML(const QString &in) -> std::string
{
    const QByteArray array = in.toUtf8(); // Use UTF-8 for better support

    std::stringstream out;

    static MD_TOC_OPTIONS toc;
    toc.depth = 0;

    md_html(array.constData(),
            array.size(),
            &captureHeading,
            &out,
            MD_FLAG_HEADINGAUTOID,
            MD_HTML_FLAG_SKIP_UTF8_BOM,
            &toc);

    return out.str();
}

auto Parser::toMarkdown(const QString &in) -> QString
{
    auto html = in.toStdString();

    return QString::fromStdString(html2md::Convert(html));
}
