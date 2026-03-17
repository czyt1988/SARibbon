#include "SARibbonLineWidgetContainer.h"
#include <QHBoxLayout>

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonLineWidgetContainer
 * @param par Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonLineWidgetContainer 构造函数
 * @param par 父窗口
 * \endif
 */
SARibbonLineWidgetContainer::SARibbonLineWidgetContainer(QWidget *par) : QWidget(par)
    , m_innerWidget(nullptr)
{
    m_labelPrefix = new QLabel(this);
    m_labelSuffix = new QLabel(this);
    QHBoxLayout *lay = new QHBoxLayout();

    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_labelPrefix);
    lay->addWidget(m_labelSuffix);
    setLayout(lay);
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonLineWidgetContainer
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonLineWidgetContainer 析构函数
 * \endif
 */
SARibbonLineWidgetContainer::~SARibbonLineWidgetContainer()
{
}

/**
 * \if ENGLISH
 * @brief Set the inner widget
 * @param innerWidget Inner widget to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置内部部件
 * @param innerWidget 要设置的内部部件
 * \endif
 */
void SARibbonLineWidgetContainer::setWidget(QWidget *innerWidget)
{
    QHBoxLayout *lay = static_cast<QHBoxLayout *>(layout());

    if (m_innerWidget) {
        lay->replaceWidget(m_innerWidget, innerWidget);
    }else{
        lay->insertWidget(1, innerWidget);
    }
    m_innerWidget = innerWidget;
}

/**
 * \if ENGLISH
 * @brief Set prefix text
 * @param str Prefix text
 * \endif
 *
 * \if CHINESE
 * @brief 设置前缀文本
 * @param str 前缀文本
 * \endif
 */
void SARibbonLineWidgetContainer::setPrefix(const QString& str)
{
    m_labelPrefix->setText(str);
}

/**
 * \if ENGLISH
 * @brief Set suffix text
 * @param str Suffix text
 * \endif
 *
 * \if CHINESE
 * @brief 设置后缀文本
 * @param str 后缀文本
 * \endif
 */
void SARibbonLineWidgetContainer::setSuffix(const QString& str)
{
    m_labelSuffix->setText(str);
}

/**
 * \if ENGLISH
 * @brief Get prefix label
 * @return Prefix QLabel pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取前缀标签
 * @return 前缀QLabel指针
 * \endif
 */
QLabel *SARibbonLineWidgetContainer::labelPrefix() const
{
    return (m_labelPrefix);
}

/**
 * \if ENGLISH
 * @brief Get suffix label
 * @return Suffix QLabel pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取后缀标签
 * @return 后缀QLabel指针
 * \endif
 */
QLabel *SARibbonLineWidgetContainer::labelSuffix() const
{
    return (m_labelSuffix);
}
