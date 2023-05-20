#include "SARibbonContextCategory.h"
#include <QList>
#include <QVariant>
#include "SARibbonElementManager.h"
#include <QDebug>

/**
 * @brief The SARibbonCategoryData class
 */
class SAPrivateRibbonCategoryData
{
public:
    SARibbonCategory* categoryPage;
};

/**
 * @brief The SARibbonContextCategoryPrivate class
 */
class SARibbonContextCategory::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonContextCategory)
public:
    PrivateData(SARibbonContextCategory* p);

public:
    QList< SAPrivateRibbonCategoryData > categoryDataList;
    QVariant contextID;
    QColor contextColor;
    QString contextTitle;
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

SARibbonCategory* SARibbonContextCategory::addCategoryPage(const QString& title)
{
    SAPrivateRibbonCategoryData catData;
    SARibbonCategory* category = RibbonSubElementDelegate->createRibbonCategory(parentWidget());

    category->markIsContextCategory(true);
    category->setWindowTitle(title);
    catData.categoryPage = category;
    d_ptr->categoryDataList.append(catData);
    category->installEventFilter(this);
    emit categoryPageAdded(category);

    return (category);
}

int SARibbonContextCategory::categoryCount() const
{
    return (d_ptr->categoryDataList.count());
}

void SARibbonContextCategory::setId(const QVariant& id)
{
    d_ptr->contextID = id;
}

QVariant SARibbonContextCategory::id() const
{
    return (d_ptr->contextID);
}

void SARibbonContextCategory::setContextColor(const QColor color)
{
    d_ptr->contextColor = color;
}

QColor SARibbonContextCategory::contextColor() const
{
    return (d_ptr->contextColor);
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
 * @brief 获取上下文标签的标题
 * @return
 */
QString SARibbonContextCategory::contextTitle() const
{
    return (d_ptr->contextTitle);
}

/**
 * @brief 设置上下文标签的标题，标题仅在office模式下显示 @ref SARibbonBar::RibbonStyle
 * @param contextTitle
 */
void SARibbonContextCategory::setContextTitle(const QString& contextTitle)
{
    d_ptr->contextTitle = contextTitle;
}

/**
 * @brief 获取对应的tab页
 * @param index
 * @return
 */
SARibbonCategory* SARibbonContextCategory::categoryPage(int index)
{
    return (d_ptr->categoryDataList[ index ].categoryPage);
}

/**
 * @brief 获取所有的SARibbonCategory*
 * @return
 */
QList< SARibbonCategory* > SARibbonContextCategory::categoryList() const
{
    QList< SARibbonCategory* > res;

    for (const SAPrivateRibbonCategoryData& c : qAsConst(d_ptr->categoryDataList)) {
        res.append(c.categoryPage);
    }
    return (res);
}

/**
 * @brief 移除这个category，这时SARibbonContextCategory不再管理这个category
 * @param category
 * @return 成功移除返回true
 */
bool SARibbonContextCategory::takeCategory(SARibbonCategory* category)
{
    for (int i = 0; i < d_ptr->categoryDataList.size(); ++i) {
        if (d_ptr->categoryDataList[ i ].categoryPage == category) {
            d_ptr->categoryDataList.takeAt(i);
            return (true);
        }
    }
    return (false);
}
