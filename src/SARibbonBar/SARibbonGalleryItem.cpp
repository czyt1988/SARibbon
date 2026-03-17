#include "SARibbonGalleryItem.h"
#include "SARibbonGalleryGroup.h"

/**
 * \if ENGLISH
 * @brief Default constructor for SARibbonGalleryItem
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryItem 默认构造函数
 * \endif
 */
SARibbonGalleryItem::SARibbonGalleryItem() : mFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), mAction(nullptr)
{
}

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonGalleryItem with text and icon
 * @param text Item text
 * @param icon Item icon
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryItem 构造函数（带文本和图标）
 * @param text 项目文本
 * @param icon 项目图标
 * \endif
 */
SARibbonGalleryItem::SARibbonGalleryItem(const QString& text, const QIcon& icon)
    : mFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), mAction(nullptr)
{
    setText(text);
    setIcon(icon);
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonGalleryItem with QAction
 * @param act QAction pointer
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryItem 构造函数（带QAction）
 * @param act QAction指针
 * \endif
 */
SARibbonGalleryItem::SARibbonGalleryItem(QAction* act) : mFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
{
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setAction(act);
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonGalleryItem
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryItem 析构函数
 * \endif
 */
SARibbonGalleryItem::~SARibbonGalleryItem()
{
}

/**
 * \if ENGLISH
 * @brief Set data for role
 * @param role Data role
 * @param data Data value
 * \endif
 *
 * \if CHINESE
 * @brief 为角色设置数据
 * @param role 数据角色
 * @param data 数据值
 * \endif
 */
void SARibbonGalleryItem::setData(int role, const QVariant& data)
{
    mDatas[ role ] = data;
}

/**
 * \if ENGLISH
 * @brief Get data for role
 * @param role Data role
 * @return Data value
 * \endif
 *
 * \if CHINESE
 * @brief 获取角色的数据
 * @param role 数据角色
 * @return 数据值
 * \endif
 */
QVariant SARibbonGalleryItem::data(int role) const
{
    if (mAction) {
        switch (role) {
        case Qt::DisplayRole:
            return (mAction->text());

        case Qt::ToolTipRole:
            return (mAction->toolTip());

        case Qt::DecorationRole:
            return (mAction->icon());
        default:
            break;
        }
    }
    return (mDatas.value(role));
}

/**
 * \if ENGLISH
 * @brief Set text
 * @param text Text
 * \endif
 *
 * \if CHINESE
 * @brief 设置文本
 * @param text 文本
 * \endif
 */
void SARibbonGalleryItem::setText(const QString& text)
{
    setData(Qt::DisplayRole, text);
}

/**
 * \if ENGLISH
 * @brief Get text
 * @return Text
 * \endif
 *
 * \if CHINESE
 * @brief 获取文本
 * @return 文本
 * \endif
 */
QString SARibbonGalleryItem::text() const
{
    if (mAction) {
        return (mAction->text());
    }
    return (data(Qt::DisplayRole).toString());
}

/**
 * \if ENGLISH
 * @brief Set tool tip
 * @param text Tool tip text
 * \endif
 *
 * \if CHINESE
 * @brief 设置工具提示
 * @param text 工具提示文本
 * \endif
 */
void SARibbonGalleryItem::setToolTip(const QString& text)
{
    setData(Qt::ToolTipRole, text);
}

/**
 * \if ENGLISH
 * @brief Get tool tip
 * @return Tool tip text
 * \endif
 *
 * \if CHINESE
 * @brief 获取工具提示
 * @return 工具提示文本
 * \endif
 */
QString SARibbonGalleryItem::toolTip() const
{
    if (mAction) {
        return (mAction->toolTip());
    }
    return (data(Qt::ToolTipRole).toString());
}

/**
 * \if ENGLISH
 * @brief Set icon
 * @param ico Icon
 * \endif
 *
 * \if CHINESE
 * @brief 设置图标
 * @param ico 图标
 * \endif
 */
void SARibbonGalleryItem::setIcon(const QIcon& ico)
{
    setData(Qt::DecorationRole, ico);
}

/**
 * \if ENGLISH
 * @brief Get icon
 * @return Icon
 * \endif
 *
 * \if CHINESE
 * @brief 获取图标
 * @return 图标
 * \endif
 */
QIcon SARibbonGalleryItem::icon() const
{
    if (mAction) {
        return (mAction->icon());
    }
    return (qvariant_cast< QIcon >(data(Qt::DecorationRole)));
}

/**
 * \if ENGLISH
 * @brief Check if item is selectable
 * @return True if selectable
 * \endif
 *
 * \if CHINESE
 * @brief 检查项目是否可选择
 * @return 如果可选择返回true
 * \endif
 */
bool SARibbonGalleryItem::isSelectable() const
{
    return (mFlags & Qt::ItemIsSelectable);
}

/**
 * \if ENGLISH
 * @brief Set selectable
 * @param isSelectable Whether selectable
 * \endif
 *
 * \if CHINESE
 * @brief 设置可选择
 * @param isSelectable 是否可选择
 * \endif
 */
void SARibbonGalleryItem::setSelectable(bool isSelectable)
{
    if (isSelectable) {
        mFlags |= Qt::ItemIsSelectable;
    } else {
        mFlags = (mFlags & (~Qt::ItemIsSelectable));
    }
}

/**
 * \if ENGLISH
 * @brief Check if item is enabled
 * @return True if enabled
 * \endif
 *
 * \if CHINESE
 * @brief 检查项目是否启用
 * @return 如果启用返回true
 * \endif
 */
bool SARibbonGalleryItem::isEnable() const
{
    if (mAction) {
        return (mAction->isEnabled());
    }
    return (mFlags & Qt::ItemIsEnabled);
}

/**
 * \if ENGLISH
 * @brief Set enable
 * @param isEnable Whether enable
 * \endif
 *
 * \if CHINESE
 * @brief 设置启用
 * @param isEnable 是否启用
 * \endif
 */
void SARibbonGalleryItem::setEnable(bool isEnable)
{
    if (mAction) {
        mAction->setEnabled(isEnable);
    }

    if (isEnable) {
        mFlags |= Qt::ItemIsEnabled;
    } else {
        mFlags = (mFlags & (~Qt::ItemIsEnabled));
    }
}

/**
 * \if ENGLISH
 * @brief Set flags
 * @param flag Item flags
 * \endif
 *
 * \if CHINESE
 * @brief 设置标志
 * @param flag 项目标志
 * \endif
 */
void SARibbonGalleryItem::setFlags(Qt::ItemFlags flag)
{
    mFlags = flag;
    if (mAction) {
        mAction->setEnabled(flag & Qt::ItemIsEnabled);
    }
}

/**
 * \if ENGLISH
 * @brief Get flags
 * @return Item flags
 * \endif
 *
 * \if CHINESE
 * @brief 获取标志
 * @return 项目标志
 * \endif
 */
Qt::ItemFlags SARibbonGalleryItem::flags() const
{
    return (mFlags);
}

/**
 * \if ENGLISH
 * @brief Set action
 * @param act QAction pointer
 * \endif
 *
 * \if CHINESE
 * @brief 设置动作
 * @param act QAction指针
 * \endif
 */
void SARibbonGalleryItem::setAction(QAction* act)
{
    mAction = act;
    if (nullptr == mAction) {
        return;
    }
    if (act->isEnabled()) {
        mFlags |= Qt::ItemIsEnabled;
    } else {
        mFlags = (mFlags & (~Qt::ItemIsEnabled));
    }
}

/**
 * \if ENGLISH
 * @brief Get action
 * @return QAction pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取动作
 * @return QAction指针
 * \endif
 */
QAction* SARibbonGalleryItem::action()
{
    return (mAction);
}

/**
 * \if ENGLISH
 * @brief Set text alignment
 * @param a Text alignment
 * \endif
 *
 * \if CHINESE
 * @brief 设置文本对齐方式
 * @param a 文本对齐方式
 * \endif
 */
void SARibbonGalleryItem::setTextAlignment(Qt::Alignment a)
{
    setData(Qt::TextAlignmentRole, (int)a);
}

/**
 * \if ENGLISH
 * @brief Get text alignment
 * @return Text alignment
 * \endif
 *
 * \if CHINESE
 * @brief 获取文本对齐方式
 * @return 文本对齐方式
 * \endif
 */
Qt::Alignment SARibbonGalleryItem::textAlignment() const
{
    return qvariant_cast< Qt::Alignment >(data(Qt::TextAlignmentRole));
}
