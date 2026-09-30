#include "SARibbonPanelItem.h"

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonPanelItem instance
 * @param widget The widget to wrap
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonPanelItem 实例
 * @param widget 要包装的窗口部件
 * \endif
 */
SARibbonPanelItem::SARibbonPanelItem(QWidget* widget)
    : QWidgetItem(widget), rowIndex(-1), columnIndex(-1), action(nullptr), customWidget(false), rowProportion(Large)
{
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
SARibbonPanelItem::~SARibbonPanelItem()
{
}

/**
 * \if ENGLISH
 * @brief Checks if the item is empty
 * @return true if the item is empty, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查项是否为空
 * @return 如果项为空则返回true，否则返回false
 * \endif
 */
bool SARibbonPanelItem::isEmpty() const
{
    return (action == nullptr || !action->isVisible());
}

/**
 * \if ENGLISH
 * @brief Contract isHidden: action not visible (2.x isEmpty semantics, plan-02 S5.1-1)
 * \endif
 *
 * \if CHINESE
 * @brief 契约 isHidden：action 不可见（2.x isEmpty 语义，计划 02 S5.1-1）
 * \endif
 */
bool SARibbonPanelItem::isHidden() const
{
    return isEmpty();
}

/**
 * \if ENGLISH
 * @brief Contract applyGeometry: delegate to QWidgetItem::setGeometry (plan-02 S5.1-1)
 * \endif
 *
 * \if CHINESE
 * @brief 契约 applyGeometry：转 QWidgetItem::setGeometry（计划 02 S5.1-1）
 * \endif
 */
void SARibbonPanelItem::applyGeometry(const QRect& rect)
{
    setGeometry(rect);
}

/**
 * \if ENGLISH
 * @brief Disambiguation override: forward to QWidgetItem (plan-02 S5.1-1)
 * \endif
 *
 * \if CHINESE
 * @brief 消歧覆写：转调 QWidgetItem 实现（计划 02 S5.1-1）
 * \endif
 */
QSize SARibbonPanelItem::sizeHint() const
{
    return QWidgetItem::sizeHint();
}

Qt::Orientations SARibbonPanelItem::expandingDirections() const
{
    return QWidgetItem::expandingDirections();
}
