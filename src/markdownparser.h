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

    Q_REQUIRED_RESULT static QString Parse(const QString &in, const int &dia = GitHub);
};

#endif // MARKDOWNPARSER_H
