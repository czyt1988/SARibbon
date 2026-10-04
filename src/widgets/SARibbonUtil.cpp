#include "SARibbonUtil.h"
#include "SARibbonThemePalette.h"
#include <SARibbonCore/SARibbonThemeData.h>
#include <QFile>
#include <QWidget>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QRegularExpression>

// 计划 02 S1：core 函数已迁至 src/core/SARibbonCoreUtil.cpp（纯 move）；
// 本文件只保留 widgets 专属实现（widgetDevicePixelRatio 与 QSS 渲染）。
namespace SA
{

/**
 * \if ENGLISH
 * @brief Get the complete QSS stylesheet string for a built-in ribbon theme
 * @details Loads the base QSS, theme template, and the default palette for the given theme,
 * then returns the fully resolved stylesheet with all {{token}} placeholders replaced.
 * Returns an empty string if the theme has no template or palette.
 * \endif
 *
 * \if CHINESE
 * @brief 获取指定内置ribbon主题的完整QSS样式表字符串
 * @details 加载基础QSS、主题模板和指定主题的默认调色板，返回所有{{token}}占位符已替换的完整样式表。
 * 如果主题没有模板或调色板，则返回空字符串。
 * \endif
 */
QString getBuiltInRibbonThemeQss(SARibbonTheme theme)
{
    // Load base QSS (common styles without colors)
    QFile baseFile(":/SARibbonTheme/resource/theme-base.qss");
    QString baseQss;
    if (baseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        baseQss = QString::fromUtf8(baseFile.readAll());
    }

    // Resolve template resource path for the theme
    auto themeToTemplatePath = [](SARibbonTheme t) -> QString {
        switch (t) {
        case SARibbonTheme::RibbonThemeOffice2016Blue:
        case SARibbonTheme::RibbonThemeOffice2016Green:
        case SARibbonTheme::RibbonThemeOffice2016Dark:
            return ":/SARibbonTheme/resource/templates/office2016.qss";
        case SARibbonTheme::RibbonThemeOffice2021Blue:
        case SARibbonTheme::RibbonThemeOffice2021Green:
        case SARibbonTheme::RibbonThemeOffice2021Dark:
            return ":/SARibbonTheme/resource/templates/office2021.qss";
        case SARibbonTheme::RibbonThemeDark:
            return ":/SARibbonTheme/resource/templates/dark.qss";
        case SARibbonTheme::RibbonThemeDark2:
            return ":/SARibbonTheme/resource/templates/dark2.qss";
        case SARibbonTheme::RibbonThemeWindows7:
            return ":/SARibbonTheme/resource/templates/win7.qss";
        case SARibbonTheme::RibbonThemeOffice2013:
            return ":/SARibbonTheme/resource/templates/office2013.qss";
        default:
            return QString();
        }
    };

    QString templatePath = themeToTemplatePath(theme);
    QString palettePath  = SARibbon::Core::SARibbonThemeData::themePalettePath(theme);
    if (templatePath.isEmpty() || palettePath.isEmpty()) {
        return baseQss;
    }

    // Load the default palette
    SARibbonThemePalette palette;
    if (!palette.loadFromFile(palettePath)) {
        qWarning() << "getBuiltInRibbonThemeQss: failed to load palette" << palettePath;
        return baseQss;
    }

    // Load template and replace tokens
    QFile templateFile(templatePath);
    if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "getBuiltInRibbonThemeQss: failed to load template" << templatePath;
        return baseQss;
    }
    QString templateQss = QString::fromUtf8(templateFile.readAll());
    QString resolvedQss = replaceQssTokens(templateQss, palette);
    return baseQss + "\n" + resolvedQss;
}

/**
 * @brief 获取窗口当前所在屏幕的dpr
 * @param w
 * @return
 */
qreal widgetDevicePixelRatio(QWidget* w)
{
    if (!w) {
        return 1.0;
    }
    // 获取窗口所在的屏幕（优先当前窗口的屏幕）
    QScreen* sc = nullptr;

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    if (QWindow* wh = w->windowHandle()) {
        sc = wh->screen();
    }
#else
    // 先获取窗口的顶层窗口（避免子部件直接调用screen()可能返回null的问题）
    QWidget* topWidget = w->window();
    if (topWidget) {
        sc = topWidget->screen();
    }
#endif
    if (!sc) {
        // qApp->primaryScreen() 拿到的是“整个系统里被用户标记成 primary 的那一块屏,是“全局主屏”
        sc = QApplication::primaryScreen();
    }
    if (!sc) {
        return 1.0;
    }
    return sc->devicePixelRatio();
}

/**
 * @brief Replace {{token}} and {{token|opacity(value)}} patterns in QSS templates with actual color values
 * @param templateQss The QSS template string containing tokens
 * @param palette The theme palette providing color values
 * @return The QSS string with all tokens replaced
 */
QString replaceQssTokens(const QString& templateQss, const SARibbonThemePalette& palette)
{
    QString result = templateQss;

    QRegularExpression re("\\{\\{([^}|]+)(?:\\|opacity\\(([^)]+)\\))?\\}\\}");

    // First pass: collect all token matches and their replacements
    struct TokenMatch
    {
        qsizetype start;
        qsizetype length;
        QString replacement;
    };
    QVector<TokenMatch> matches;

    QRegularExpressionMatchIterator it = re.globalMatch(result);
    while (it.hasNext()) {
        QRegularExpressionMatch match  = it.next();
        QString           tokenName    = match.captured(1);
        QString           opacityStr   = match.captured(2);
        QString           raw          = palette.rawValue(tokenName);

        QString replacement;
        if (!raw.isEmpty()) {
            QColor color(raw);
            if (color.isValid()) {
                if (!opacityStr.isEmpty()) {
                    bool  ok;
                    float opacity = opacityStr.toFloat(&ok);
                    if (ok) {
                        int alpha       = qBound(0, qRound(opacity * 255), 255);
                        replacement = QString("#%1%2").arg(alpha, 2, 16, QChar('0')).arg(color.name().mid(1));
                    } else {
                        replacement = color.name();
                    }
                } else {
                    replacement = color.name();
                }
            } else {
                // Non-color raw string (e.g. qlineargradient(...)) — use directly
                replacement = raw;
            }
        }

        if (!replacement.isEmpty()) {
            matches.append({ match.capturedStart(), match.capturedLength(), replacement });
        }
    }

    // Apply replacements in reverse order to preserve offsets
    for (qsizetype i = matches.size() - 1; i >= 0; --i) {
        result.replace(matches[i].start, matches[i].length, matches[i].replacement);
    }

    // Scan for any unreplaced tokens and warn about them
    {
        QRegularExpression                unreplacedRe("\\{\\{([^}|]+)(?:\\|opacity\\([^)]+\\))?\\}\\}");
        QRegularExpressionMatchIterator   warnIt = unreplacedRe.globalMatch(result);
        while (warnIt.hasNext()) {
            QRegularExpressionMatch warnMatch = warnIt.next();
            qWarning() << "replaceQssTokens: unreplaced token:" << warnMatch.captured(1).trimmed();
        }
    }

    return result;
}

}
