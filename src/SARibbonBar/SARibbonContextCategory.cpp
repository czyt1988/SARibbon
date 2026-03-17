#include "SARibbonContextCategory.h"
#include <QList>
#include <QVariant>
#include "SARibbonElementManager.h"
#include <QDebug>

/**
 * \if ENGLISH
 * @brief Data class for storing SARibbonCategory information
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCategoryData 类
 * \endif
 */
class SAPrivateRibbonCategoryData
{
public:
    SARibbonCategory* categoryPage;
};

/**
 * \if ENGLISH
 * @brief Private data class for SARibbonContextCategory
 * @details This class holds private data for SARibbonContextCategory to implement the PIMPL idiom.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonContextCategoryPrivate 类
 * @details 此类持有 SARibbonContextCategory 的私有数据，实现 PIMPL 设计模式。
 * \endif
 */
class SARibbonContextCategory::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonContextCategory)
public:
    PrivateData(SARibbonContextCategory* p);

public:
    QList< SAPrivateRibbonCategoryData > mCategoryDataList;
    QVariant mContextID;
    QColor mContextColor;
    QString mContextTitle;
};
SARibbonContextCategory::PrivateData::PrivateData(SARibbonContextCategory* p) : q_ptr(p)
{
}

//===================================================
// SARibbonContextCategory
//===================================================
SARibbonContextCategory::SARibbonContextCategory(QWidget* parent)
    : QObject(parent), d_ptr(new SARibbonContextCategory::PrivateData(this))
{
}

SARibbonContextCategory::~SARibbonContextCategory()
{
}

/**
 * \if ENGLISH
 * @brief Add a category page
 * @param title Category name
 * @return Created SARibbonCategory object
 * \endif
 *
 * \if CHINESE
 * @brief 添加标签
 * @param title 标签名字
 * @return 创建的 SARibbonCategory 对象
 * \endif
 */
SARibbonCategory* SARibbonContextCategory::addCategoryPage(const QString& title)
{
    SARibbonCategory* category = RibbonSubElementFactory->createRibbonCategory(parentWidget());
    category->setCategoryName(title);
    addCategoryPage(category);
    return (category);
}

/**
 * \if ENGLISH
 * @brief Add a category page
 * @param category SARibbonCategory object to add
 * \endif
 *
 * \if CHINESE
 * @brief 添加标签
 * @param category 要添加的 SARibbonCategory 对象
 * \endif
 */
void SARibbonContextCategory::addCategoryPage(SARibbonCategory* category)
{
    if (isHaveCategory(category)) {
        // cn:SARibbonContextCategory已经持有标签：%1，将跳过
        qWarning() << tr("SARibbonContextCategory have category %1,will skip").arg(category->categoryName());
        return;
    }
    category->markIsContextCategory(true);
    connect(category, &SARibbonCategory::categoryNameChanged, this, &SARibbonContextCategory::onCategoryTitleChanged);
    SAPrivateRibbonCategoryData catData;
    catData.categoryPage = category;
    d_ptr->mCategoryDataList.append(catData);
    category->installEventFilter(this);
    Q_EMIT categoryPageAdded(category);
}

int SARibbonContextCategory::categoryCount() const
{
    return (d_ptr->mCategoryDataList.count());
}

void SARibbonContextCategory::setId(const QVariant& id)
{
    d_ptr->mContextID = id;
}

QVariant SARibbonContextCategory::id() const
{
    return (d_ptr->mContextID);
}

void SARibbonContextCategory::setContextColor(const QColor color)
{
    d_ptr->mContextColor = color;
}

QColor SARibbonContextCategory::contextColor() const
{
    return (d_ptr->mContextColor);
}

QWidget* SARibbonContextCategory::parentWidget() const
{
    return (qobject_cast< QWidget* >(parent()));
}

bool SARibbonContextCategory::eventFilter(QObject* watched, QEvent* e)
{
    if (nullptr == watched) {
        return (false);
    }
    switch (e->type()) {
    case QEvent::Close: {
        SARibbonCategory* c = qobject_cast< SARibbonCategory* >(watched);
        if (c) {
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug() << " -----------> close event";
#endif
            takeCategory(c);
        }
    } break;

    default:
        break;
    }
    return (false);
}

/**
 * \if ENGLISH
 * @brief Get the context category title
 * @return Context title
 * \endif
 *
 * \if CHINESE
 * @brief 获取上下文标签的标题
 * @return 上下文标题
 * \endif
 */
QString SARibbonContextCategory::contextTitle() const
{
    return (d_ptr->mContextTitle);
}

/**
 * \if ENGLISH
 * @brief Set the context category title
 * @param contextTitle Context title to set
 * @note The title is only displayed in Office mode @ref SARibbonBar::RibbonStyle
 * @note This function will emit the contextTitleChanged signal
 * \endif
 *
 * \if CHINESE
 * @brief 设置上下文标签的标题，标题仅在office模式下显示 @ref SARibbonBar::RibbonStyle
 * @param contextTitle 要设置的上下文标题
 * @note 此函数会触发信号@sa contextTitleChanged
 * \endif
 */
void SARibbonContextCategory::setContextTitle(const QString& contextTitle)
{
    d_ptr->mContextTitle = contextTitle;
    Q_EMIT contextTitleChanged(contextTitle);
}

/**
 * \if ENGLISH
 * @brief Get the corresponding tab page
 * @param index Index of the category page
 * @return SARibbonCategory at the specified index
 * \endif
 *
 * \if CHINESE
 * @brief 获取对应的tab页
 * @param index 标签页的索引
 * @return 指定索引处的 SARibbonCategory
 * \endif
 */
SARibbonCategory* SARibbonContextCategory::categoryPage(int index)
{
    return (d_ptr->mCategoryDataList[ index ].categoryPage);
}

/**
 * \if ENGLISH
 * @brief Get all SARibbonCategory objects
 * @return List of SARibbonCategory pointers
 * \endif
 *
 * \if CHINESE
 * @brief 获取所有的SARibbonCategory*
 * @return SARibbonCategory 指针列表
 * \endif
 */
QList< SARibbonCategory* > SARibbonContextCategory::categoryList() const
{
    QList< SARibbonCategory* > res;

    for (const SAPrivateRibbonCategoryData& c : sa_as_const(d_ptr->mCategoryDataList)) {
        res.append(c.categoryPage);
    }
    return (res);
}

/**
 * \if ENGLISH
 * @brief Remove this category, SARibbonContextCategory will no longer manage this category
 * @param category SARibbonCategory to remove
 * @return true if removal was successful
 * \endif
 *
 * \if CHINESE
 * @brief 移除这个category，这时SARibbonContextCategory不再管理这个category
 * @param category 要移除的 SARibbonCategory
 * @return 成功移除返回true
 * \endif
 */
bool SARibbonContextCategory::takeCategory(SARibbonCategory* category)
{
    for (int i = 0; i < d_ptr->mCategoryDataList.size(); ++i) {
        if (d_ptr->mCategoryDataList[ i ].categoryPage == category) {
            d_ptr->mCategoryDataList.takeAt(i);
            return (true);
        }
    }
    return (false);
}

/**
 * \if ENGLISH
 * @brief Check if the context maintains this SARibbonCategory
 * @param category SARibbonCategory to check
 * @return true if the context maintains this category
 * \endif
 *
 * \if CHINESE
 * @brief 判断上下文是否维护了此SARibbonCategory
 * @param category 要检查的 SARibbonCategory
 * @return 如果上下文维护此category则返回true
 * \endif
 */
bool SARibbonContextCategory::isHaveCategory(SARibbonCategory* category) const
{
    for (int i = 0; i < d_ptr->mCategoryDataList.size(); ++i) {
        if (d_ptr->mCategoryDataList[ i ].categoryPage == category) {
            return (true);
        }
    }
    return (false);
}

/**
 * \if ENGLISH
 * @brief Hide the context category
 * @details This method is equivalent to:
 * @code
 * ribbon->hideContextCategory(this);
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief hide contextCategory/隐藏上下文标签
 * @details 此方法等同于：
 * @code
 * ribbon->hideContextCategory(this);
 * @endcode
 * \endif
 */
void SARibbonContextCategory::hide()
{
    if (SARibbonBar* ribbon = qobject_cast< SARibbonBar* >(parent())) {
        ribbon->hideContextCategory(this);
    }
}

/**
 * \if ENGLISH
 * @brief Show the context category
 * @details This method is equivalent to:
 * @code
 * ribbon->showContextCategory(this);
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief show contextCategory/显示上下文标签
 * @details 此方法等同于：
 * @code
 * ribbon->showContextCategory(this);
 * @endcode
 * \endif
 */
void SARibbonContextCategory::show()
{
    if (SARibbonBar* ribbon = qobject_cast< SARibbonBar* >(parent())) {
        ribbon->showContextCategory(this);
    }
}

void SARibbonContextCategory::onCategoryTitleChanged(const QString& title)
{
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(sender());
    if (category) {
        Q_EMIT categoryTitleChanged(category, title);
    }
}
