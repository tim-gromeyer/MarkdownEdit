#ifndef PARSER_H
#define PARSER_H

#include <QString>

class Parser
{
public:
    Parser();

    enum Dialect {
        Commonmark = 0,
        GitHub = 1
    };
    enum Mode {
        MD2HTML = 0,
        HTML2MD = 1
    };

    static QString Parse(QString in, Mode mode = MD2HTML, Dialect dia = GitHub);


};

#endif // PARSER_H
