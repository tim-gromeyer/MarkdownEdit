#ifndef PARSER_H
#define PARSER_H

#include <QString>

class Parser
{
public:
    Parser();

    enum Mode {
        Commonmark = 0,
        GitHub = 1
    };

    static QString Parse(QString markdown, Mode mode = GitHub);


};

#endif // PARSER_H
