#ifndef MARKDOWNPARSER_H
#define MARKDOWNPARSER_H

#include <QString>

class Parser
{
public:
    enum Dialect {
        Commonmark = 0,
        GitHub = 1
    };

    Q_REQUIRED_RESULT static QString toHtml(const QString &in, const int &dia = GitHub);

    Q_REQUIRED_RESULT static QString toMarkdown(QString in);
};

#endif // MARKDOWNPARSER_H
