#include "SARibbonApplicationButton.h"

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonApplicationButton instance
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonApplicationButton 实例
 * @param parent 父窗口部件
 * \endif
 */
SARibbonApplicationButton::SARibbonApplicationButton(QWidget* parent) : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setMinimumWidth(40);
}

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonApplicationButton instance with text
 * @param text Button text
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个带有文本的 SARibbonApplicationButton 实例
 * @param text 按钮文本
 * @param parent 父窗口部件
 * \endif
 */
SARibbonApplicationButton::SARibbonApplicationButton(const QString& text, QWidget* parent) : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setText(text);
    setMinimumWidth(40);
}

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonApplicationButton instance with icon and text
 * @param icon Button icon
 * @param text Button text
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个带有图标和文本的 SARibbonApplicationButton 实例
 * @param icon 按钮图标
 * @param text 按钮文本
 * @param parent 父窗口部件
 * \endif
 */
SARibbonApplicationButton::SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setIcon(icon);
    setText(text);
    setMinimumWidth(40);
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
SARibbonApplicationButton::~SARibbonApplicationButton()
{
}
