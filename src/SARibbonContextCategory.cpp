#include "SARibbonContextCategory.h"
#include <QList>
#include <QVariant>
#include "SARibbonElementManager.h"
class SARibbonCategoryData
{
public:
    SARibbonCategory* categoryPage;
};

class SARibbonContextCategoryPrivate
{
public:
    QList<SARibbonCategoryData> categoryDataList;
    QVariant contextID;
    QColor contextColor;
    QString contextTitle;
};

SARibbonContextCategory::SARibbonContextCategory(QWidget *parent):QObject(parent)
  ,m_d(new SARibbonContextCategoryPrivate)
{
}

SARibbonContextCategory::~SARibbonContextCategory()
{
    delete m_d;
}

SARibbonCategory *SARibbonContextCategory::addCategoryPage(const QString &title)
{
    SARibbonCategoryData catData;
    SARibbonCategory* category = RibbonSubElementDelegate->createRibbonCategory(parentWidget());
    category->setWindowTitle(title);
    catData.categoryPage = category;
    m_d->categoryDataList.append(catData);
    emit categoryPageAdded(category);
    return category;
}

int SARibbonContextCategory::categoryCount() const
{
    return m_d->categoryDataList.count();
}

void SARibbonContextCategory::setId(const QVariant &id)
{
    m_d->contextID = id;
}

QVariant SARibbonContextCategory::id() const
{
    return m_d->contextID;
}

void SARibbonContextCategory::setContextColor(const QColor color)
{
    m_d->contextColor = color;
}

QColor SARibbonContextCategory::contextColor() const
{
    return m_d->contextColor;
}

QWidget *SARibbonContextCategory::parentWidget() const
{
    return qobject_cast<QWidget*>(parent());
}

QString SARibbonContextCategory::contextTitle() const
{
    return m_d->contextTitle;
}

void SARibbonContextCategory::setContextTitle(const QString &contextTitle)
{
    m_d->contextTitle = contextTitle;
}
///
/// \brief 获取对应的tab页
/// \param index
/// \return
///
SARibbonCategory *SARibbonContextCategory::categoryPage(int index)
{
    return m_d->categoryDataList[index].categoryPage;
}
