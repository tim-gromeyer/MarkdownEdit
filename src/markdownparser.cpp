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
                                                    "<title>HTML generated by md4c</title>\n"
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
            parser_flags, MD_HTML_FLAG_SKIP_UTF8_BOM);

    out.append("</body>\n"
               "</html>\n");
    out.squeeze();

    return QString::fromUtf8(out);
}

auto Parser::toMarkdown(QString in) -> QString
{
    // replace Windows line breaks
    in.replace(QChar(QChar::LineSeparator), u'\n');

    // remove some blocks
    in.remove(
        QRegularExpression(QStringLiteral("<head.*?>(.+?)<\\/head>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption));

    in.remove(
        QRegularExpression(QStringLiteral("<script.*?>(.+?)<\\/script>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption));

    in.remove(
        QRegularExpression(QStringLiteral("<style.*?>(.+?)<\\/style>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption));

    // replace some html tags with markdown
    in.replace(
        QRegularExpression(QStringLiteral("<strong.*?>(.+?)<\\/strong>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("**\\1**"));
    in.replace(
        QRegularExpression(QStringLiteral("<b.*?>(.+?)<\\/b>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("**\\1**"));
    in.replace(
        QRegularExpression(QStringLiteral("<em.*?>(.+?)<\\/em>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("*\\1*"));
    in.replace(
        QRegularExpression(QStringLiteral("<i.*?>(.+?)<\\/i>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("*\\1*"));
    in.replace(
        QRegularExpression(QStringLiteral("<pre.*?>(.+?)<\\/pre>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n```\n\\1\n```\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<code.*?>(.+?)<\\/code>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n```\n\\1\n```\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<h1.*?>(.+?)<\\/h1>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n# \\1\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<h2.*?>(.+?)<\\/h2>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n## \\1\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<h3.*?>(.+?)<\\/h3>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n### \\1\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<h4.*?>(.+?)<\\/h4>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n#### \\1\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<h5.*?>(.+?)<\\/h5>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n##### \\1\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<h6.*?>(.+?)<\\/h6>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n###### \\1\n"));
    in.replace(
        QRegularExpression(QStringLiteral("<li.*?>(.+?)<\\/li>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("- \\1\n"));
    in.replace(QRegularExpression(QStringLiteral("<br.*?>"),
                                    QRegularExpression::CaseInsensitiveOption),
                 QStringLiteral("\n"));
    in.replace(QRegularExpression(
                     QStringLiteral("<a[^>]+href=\"(.+?)\".*?>(.+?)<\\/a>"),
                     QRegularExpression::CaseInsensitiveOption |
                         QRegularExpression::DotMatchesEverythingOption),
                 QStringLiteral("[\\2](\\1)"));
    in.replace(
        QRegularExpression(QStringLiteral("<p.*?>(.+?)</p>"),
                           QRegularExpression::CaseInsensitiveOption |
                               QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\n\n\\1\n\n"));

    // replace multiple line breaks
    in.replace(QRegularExpression(QStringLiteral("\n\n+")),
                 QStringLiteral("\n\n"));

    return in;
}
