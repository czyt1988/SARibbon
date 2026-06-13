#include "syntaxhighlighter.h"

QssSyntaxHighlighter::QssSyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    HighlightRule rule;

    // Comments: /* ... */
    m_multiLineCommentFormat.setForeground(QColor("#6A9955"));
    m_multiLineCommentFormat.setFontItalic(true);
    m_commentStart = QRegularExpression("/\\*");
    m_commentEnd = QRegularExpression("\\*/");

    // Selectors (class names like QPushButton, QWidget)
    rule.format.setForeground(QColor("#267F99"));
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    m_rules.append(rule);

    // Pseudo-states (:hover, :pressed, :checked, etc.)
    rule.format.setForeground(QColor("#D73A49"));
    rule.format.setFontWeight(QFont::Normal);
    rule.pattern = QRegularExpression(":[a-z-]+");
    m_rules.append(rule);

    // Properties (background-color, border, font-size, etc.)
    rule.format.setForeground(QColor("#005CC5"));
    rule.pattern = QRegularExpression("\\b[a-z-]+(?=\\s*:)");
    m_rules.append(rule);

    // Colors (#RRGGBB, #AARRGGBB)
    rule.format.setForeground(QColor("#CF222E"));
    rule.pattern = QRegularExpression("#[0-9A-Fa-f]{6,8}");
    m_rules.append(rule);

    // Numbers with units (px, pt, em, %)
    rule.format.setForeground(QColor("#098658"));
    rule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?(px|pt|em|%)?\\b");
    m_rules.append(rule);

    // Strings
    rule.format.setForeground(QColor("#A31515"));
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    m_rules.append(rule);

    // Braces and brackets
    rule.format.setForeground(QColor("#000000"));
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("[{}\\[\\]]");
    m_rules.append(rule);

    // Sub-controls (::drop-down, ::indicator)
    rule.format.setForeground(QColor("#6F42C1"));
    rule.format.setFontWeight(QFont::Normal);
    rule.pattern = QRegularExpression("::[a-z-]+");
    m_rules.append(rule);

    // Special values (none, transparent, inherit)
    rule.format.setForeground(QColor("#008080"));
    rule.format.setFontItalic(true);
    rule.pattern = QRegularExpression("\\b(none|transparent|inherit)\\b");
    m_rules.append(rule);

    // Token placeholders {{token}}
    rule.format.setForeground(QColor("#E36209"));
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\{\\{[^}]+\\}\\}");
    m_rules.append(rule);
}

void QssSyntaxHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightRule& rule : m_rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(m_commentStart);

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        int endIndex = text.indexOf(m_commentEnd, startIndex, &endMatch);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startIndex = text.indexOf(m_commentStart, startIndex + commentLength);
    }
}

JsonSyntaxHighlighter::JsonSyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    HighlightRule rule;

    // Keys (property names in quotes followed by colon)
    rule.format.setForeground(QColor("#0451A5"));
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\"[^\"]*\"(?=\\s*:)");
    m_rules.append(rule);

    // String values (quoted strings not followed by colon)
    rule.format.setForeground(QColor("#A31515"));
    rule.format.setFontWeight(QFont::Normal);
    rule.pattern = QRegularExpression("\"[^\"]*\"(?!\\s*:)");
    m_rules.append(rule);

    // Numbers
    rule.format.setForeground(QColor("#098658"));
    rule.pattern = QRegularExpression("\\b-?\\d+(\\.\\d+)?([eE][+-]?\\d+)?\\b");
    m_rules.append(rule);

    // Boolean and null
    rule.format.setForeground(QColor("#0000FF"));
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\b(true|false|null)\\b");
    m_rules.append(rule);

    // Braces and brackets
    rule.format.setForeground(QColor("#000000"));
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("[{}\\[\\]]");
    m_rules.append(rule);
}

void JsonSyntaxHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightRule& rule : m_rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}