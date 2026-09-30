#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

class QssSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit QssSyntaxHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QVector<HighlightRule> m_rules;
    QTextCharFormat m_multiLineCommentFormat;
    QRegularExpression m_commentStart;
    QRegularExpression m_commentEnd;
};

class JsonSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit JsonSyntaxHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QVector<HighlightRule> m_rules;
};

#endif