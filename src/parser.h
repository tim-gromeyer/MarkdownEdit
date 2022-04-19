#ifndef PARSER_H
#define PARSER_H

#include <QString>

class Parser
{
public:
    enum Dialect {
        Commonmark = 0,
        GitHub = 1
    };

    Q_REQUIRED_RESULT static QString Parse(QString in, int dia = GitHub);
};

#endif // PARSER_H
