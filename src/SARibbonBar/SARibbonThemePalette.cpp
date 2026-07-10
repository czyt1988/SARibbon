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

        m_deriveRules.insert(it.key(), { fn, baseName, amount });
        if (m_keyColors.contains(baseName)) {
            QColor base = m_keyColors.value(baseName);
            QColor derivedC = ::deriveColor(base, fn, amount, m_isDark);
            m_derivedColors.insert(it.key(), derivedC);
        }
    }

    // Parse fixed colors — store non-color values (e.g. qlineargradient) as raw strings
    QJsonObject fixed = obj.value("fixed").toObject();
    for (auto it = fixed.begin(); it != fixed.end(); ++it) {
        QString value = it.value().toString();
        QColor c(value);
        if (c.isValid()) {
            m_fixedColors.insert(it.key(), c);
        } else if (!value.isEmpty()) {
            m_rawStrings.insert(it.key(), value);
        }
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
 * @brief Get a raw string value for a token name, supporting non-color CSS values
 * @param[in] tokenName The token name to look up
 * @return The raw string value (e.g. "qlineargradient(...)"), or the hex color name
 *         if the token is a color, or an empty QString if the token is not found
 * @details Lookup order: raw strings first, then derived colors, then key colors,
 * then fixed colors. Raw strings take highest priority since they represent
 * complex CSS values that cannot be expressed as a single color.
 * \endif
 *
 * \if CHINESE
 * @brief 获取标记名的原始字符串值，支持非颜色CSS值
 * @param[in] tokenName 要查找的标记名
 * @return 原始字符串值（例如"qlineargradient(...)"），如果标记是颜色则返回十六进制颜色名，
 *         如果标记未找到则返回空QString
 * @details 查找顺序：先查原始字符串，再查派生色，再查键色，最后查固定色。
 * 原始字符串优先级最高，因为它们代表无法用单一颜色表达的复杂CSS值。
 * \endif
 */
QString SARibbonThemePalette::rawValue(const QString& tokenName) const
{
    if (m_rawStrings.contains(tokenName)) {
        return m_rawStrings.value(tokenName);
    }
    if (m_derivedColors.contains(tokenName)) {
        return m_derivedColors.value(tokenName).name();
    }
    if (m_keyColors.contains(tokenName)) {
        return m_keyColors.value(tokenName).name();
    }
    if (m_fixedColors.contains(tokenName)) {
        return m_fixedColors.value(tokenName).name();
    }
    return QString();
}

/**
 * \if ENGLISH
 * @brief Get all color variables as name-to-hex-string pairs
 * @return A hash mapping token names to their hex color strings (e.g. "#ff0000")
 * @details Merges all three layers (key, derived, fixed) into a single hash.
 * Insertion order ensures derived colors take priority over key colors, and
 * key colors over fixed colors, matching the color() lookup order.
 * \endif
 *
 * \if CHINESE
 * @brief 获取所有颜色变量（名称到十六进制字符串的映射）
 * @return 将标记名映射到其十六进制颜色字符串（例如"#ff0000"）的哈希表
 * @details 将三个层（键色、派生色、固定色）合并到单个哈希表中。
 * 插入顺序确保派生色优先于键色，键色优先于固定色，与color()查找顺序一致。
 * \endif
 */
QHash<QString, QString> SARibbonThemePalette::variables() const
{
    QHash<QString, QString> vars;
    // Insert order matters for conflicts: last writer wins
    // Priority: rawStrings > derived > key > fixed (matches rawValue() lookup order)
    for (auto it = m_fixedColors.cbegin(); it != m_fixedColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    for (auto it = m_keyColors.cbegin(); it != m_keyColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    for (auto it = m_derivedColors.cbegin(); it != m_derivedColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    for (auto it = m_rawStrings.cbegin(); it != m_rawStrings.cend(); ++it) {
        vars.insert(it.key(), it.value());
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
 * @brief Recalculate all derived colors from stored rules and current key colors
 * @details Iterates over all derivation rules, resolves each derived color from the current
 * key colors and fixed colors, and refreshes the internal derived color cache.
 * \endif
 *
 * \if CHINESE
 * @brief 根据存储的规则和当前关键色重新计算所有派生颜色
 * @details 遍历所有派生规则，依据当前关键色和固定色解析每个派生颜色，并刷新内部派生颜色缓存。
 * \endif
 */
void SARibbonThemePalette::recalculateDerived()
{
    m_derivedColors.clear();
    for (auto it = m_deriveRules.cbegin(); it != m_deriveRules.cend(); ++it) {
        const DeriveRule& rule = it.value();
        if (m_keyColors.contains(rule.base)) {
            QColor base = m_keyColors.value(rule.base);
            QColor derivedC = ::deriveColor(base, rule.fn, rule.amount, m_isDark);
            m_derivedColors.insert(it.key(), derivedC);
        }
    }
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
    recalculateDerived();
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
    recalculateDerived();
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
    recalculateDerived();
}

} // namespace SA
