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
