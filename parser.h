#ifndef PARSER_H
#define PARSER_H

#include <QString>

class Parser
{
public:
    Parser();

    QString Parse(QString markdown, bool ok);
    QString errorString() { return _errorString; };

private:
    QString _errorString;
};

#endif // PARSER_H
