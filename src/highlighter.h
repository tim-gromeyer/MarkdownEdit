#ifndef HIGHLITER_H
#define HIGHLITER_H

#include <QSyntaxHighlighter>


class Highliter : public QSyntaxHighlighter
{
public:
    explicit Highliter(QTextDocument *doc);

    enum Token {
        CodeBlock,
        CodeKeyWord,
        CodeString,
        CodeBuiltIn,
    };
    Q_ENUM(Token)

protected:
    void highlightBlock(const QString &text) override;

private:
    void xmlHighlighter(const QString &text);

    QHash<Token, QTextCharFormat> _formats;
};

#endif // HIGHLITER_H
