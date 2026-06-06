#include "SARibbonThemePalette.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

namespace {
// Apply a lighten/darken derive operation to a base color; reverses direction for dark themes
QColor deriveColor(const QColor& base, const QString& fn, int amount, bool isDark)
{
    if (isDark) {
        // Reverse direction for dark themes
        if (fn == "darken") {
            return base.lighter(100 + amount);
        } else if (fn == "lighten") {
            return base.darker(100 + amount);
        }
    } else {
        if (fn == "darken") {
            return base.darker(100 + amount);
        } else if (fn == "lighten") {
            return base.lighter(100 + amount);
        }
    }
    return base;
}
} // anonymous namespace

namespace SA {

/**
 * \if ENGLISH
 * @brief Default constructor
 * @details Creates an empty palette. Call loadFromJson() or loadFromFile() to populate it.
 * \endif
 *
 * \if CHINESE
 * @brief 默认构造函数
 * @details 创建一个空调色板。调用loadFromJson()或loadFromFile()来填充颜色数据。
 * \endif
 */
SARibbonThemePalette::SARibbonThemePalette() = default;

/**
 * \if ENGLISH
 * @brief Load palette from a JSON file
 * @param[in] jsonPath Filesystem path or Qt resource path (e.g. ":/themes/light.json")
 * @return true if the file was read and parsed successfully, false otherwise
 * @details Opens the file in read-only text mode, reads all content, then delegates to loadFromJson().
 * \endif
 *
 * \if CHINESE
 * @brief 从JSON文件加载调色板
 * @param[in] jsonPath 文件系统路径或Qt资源路径（例如":/themes/light.json"）
 * @return 文件读取并解析成功返回true，否则返回false
 * @details 以只读文本模式打开文件，读取全部内容后委托给loadFromJson()处理。
 * \endif
 */
bool SARibbonThemePalette::loadFromFile(const QString& jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QByteArray data = file.readAll();
    file.close();
    return loadFromJson(data);
}

/**
 * \if ENGLISH
 * @brief Load palette from a JSON byte array
 * @param[in] json Raw JSON data containing the palette definition
 * @return true if the JSON was parsed successfully, false on parse or format error
 * @details The JSON object must contain the following top-level keys:
 * - "isDark": boolean indicating dark theme mode
 * - "keyColors": object mapping token names to hex color strings
 * - "derived": object mapping token names to derive rules (fn, base, amount)
 * - "fixed": object mapping token names to hex color strings
 * \endif
 *
 * \if CHINESE
 * @brief 从JSON字节数组加载调色板
 * @param[in] json 包含调色板定义的原始JSON数据
 * @return JSON解析成功返回true，解析或格式错误返回false
 * @details JSON对象必须包含以下顶层键：
 * - "isDark"：布尔值，表示是否为深色主题
 * - "keyColors"：将标记名映射到十六进制颜色字符串的对象
 * - "derived"：将标记名映射到派生规则（fn、base、amount）的对象
 * - "fixed"：将标记名映射到十六进制颜色字符串的对象
 * \endif
 */
bool SARibbonThemePalette::loadFromJson(const QByteArray& json)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject obj = doc.object();
    m_isDark = obj.value("isDark").toBool(false);

    QJsonObject keyColors = obj.value("keyColors").toObject();
    for (auto it = keyColors.begin(); it != keyColors.end(); ++it) {
        m_keyColors.insert(it.key(), QColor(it.value().toString()));
    }

    // Parse derived colors
    QJsonObject derived = obj.value("derived").toObject();
    for (auto it = derived.begin(); it != derived.end(); ++it) {
        QJsonObject rule = it.value().toObject();
        QString fn = rule.value("fn").toString();
        QString baseName = rule.value("base").toString();
        int amount = rule.value("amount").toInt(10);

        if (m_keyColors.contains(baseName)) {
            QColor base = m_keyColors.value(baseName);
            QColor derivedC = ::deriveColor(base, fn, amount, m_isDark);
            m_derivedColors.insert(it.key(), derivedC);
        }
    }

    // Parse fixed colors
    QJsonObject fixed = obj.value("fixed").toObject();
    for (auto it = fixed.begin(); it != fixed.end(); ++it) {
        m_fixedColors.insert(it.key(), QColor(it.value().toString()));
    }

    return true;
}

/**
 * \if ENGLISH
 * @brief Get a color by token name
 * @param[in] tokenName The color token name to look up
 * @return The resolved QColor, or an invalid QColor if the token is not found
 * @details Lookup order: derived colors first, then key colors, then fixed colors.
 * Derived colors take precedence so that overrides via derive rules always win.
 * \endif
 *
 * \if CHINESE
 * @brief 按标记名获取颜色
 * @param[in] tokenName 要查找的颜色标记名
 * @return 解析后的QColor，如果标记未找到则返回无效的QColor
 * @details 查找顺序：先查派生色，再查键色，最后查固定色。
 * 派生色优先，以确保通过派生规则设置的覆盖始终生效。
 * \endif
 */
QColor SARibbonThemePalette::color(const QString& tokenName) const
{
    if (m_derivedColors.contains(tokenName)) {
        return m_derivedColors.value(tokenName);
    }
    if (m_keyColors.contains(tokenName)) {
        return m_keyColors.value(tokenName);
    }
    if (m_fixedColors.contains(tokenName)) {
        return m_fixedColors.value(tokenName);
    }
    return QColor();
}

/**
 * \if ENGLISH
 * @brief Get all color variables as name-to-hex-string pairs
 * @return A hash mapping token names to their hex color strings (e.g. "#ff0000")
 * @details Merges all three layers (key, derived, fixed) into a single hash.
 * If the same name exists in multiple layers, the last written layer wins.
 * \endif
 *
 * \if CHINESE
 * @brief 获取所有颜色变量（名称到十六进制字符串的映射）
 * @return 将标记名映射到其十六进制颜色字符串（例如"#ff0000"）的哈希表
 * @details 将三个层（键色、派生色、固定色）合并到单个哈希表中。
 * 如果多个层中存在相同的名称，最后写入的层优先。
 * \endif
 */
QHash<QString, QString> SARibbonThemePalette::variables() const
{
    QHash<QString, QString> vars;
    for (auto it = m_keyColors.cbegin(); it != m_keyColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    for (auto it = m_derivedColors.cbegin(); it != m_derivedColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    for (auto it = m_fixedColors.cbegin(); it != m_fixedColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    return vars;
}

/**
 * \if ENGLISH
 * @brief Check if this palette represents a dark theme
 * @return true if the palette was loaded with "isDark": true, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查此调色板是否表示深色主题
 * @return 如果加载的调色板中"isDark"为true，则返回true，否则返回false
 * \endif
 */
bool SARibbonThemePalette::isDark() const
{
    return m_isDark;
}

/**
 * \if ENGLISH
 * @brief Set the accent key color
 * @param[in] color The new accent color
 * @details Overwrites the "accent" entry in the key colors layer.
 * \endif
 *
 * \if CHINESE
 * @brief 设置强调色（键色层）
 * @param[in] color 新的强调色
 * @details 覆盖键色层中的"accent"条目。
 * \endif
 */
void SARibbonThemePalette::setAccentColor(const QColor& color)
{
    m_keyColors.insert("accent", color);
}

/**
 * \if ENGLISH
 * @brief Set the content background key color
 * @param[in] color The new content background color
 * @details Overwrites the "content-bg" entry in the key colors layer.
 * \endif
 *
 * \if CHINESE
 * @brief 设置内容背景色（键色层）
 * @param[in] color 新的内容背景色
 * @details 覆盖键色层中的"content-bg"条目。
 * \endif
 */
void SARibbonThemePalette::setContentBgColor(const QColor& color)
{
    m_keyColors.insert("content-bg", color);
}

/**
 * \if ENGLISH
 * @brief Set the text key color
 * @param[in] color The new text color
 * @details Overwrites the "text-color" entry in the key colors layer.
 * \endif
 *
 * \if CHINESE
 * @brief 设置文字颜色（键色层）
 * @param[in] color 新的文字颜色
 * @details 覆盖键色层中的"text-color"条目。
 * \endif
 */
void SARibbonThemePalette::setTextColor(const QColor& color)
{
    m_keyColors.insert("text-color", color);
}

} // namespace SA
