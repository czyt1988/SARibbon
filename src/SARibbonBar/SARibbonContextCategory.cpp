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
 * @brief 添加标签
 * @param title 标签名字
 */
SARibbonCategory* SARibbonContextCategory::addCategoryPage(const QString& title)
{
	SARibbonCategory* category = RibbonSubElementFactory->createRibbonCategory(parentWidget());
	category->setCategoryName(title);
	addCategoryPage(category);
	return (category);
}

/**
 * @brief 添加标签
 * @param page
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
 * @brief 获取上下文标签的标题
 * @return
 */
QString SARibbonContextCategory::contextTitle() const
{
    return (d_ptr->mContextTitle);
}

/**
 * @brief 设置上下文标签的标题，标题仅在office模式下显示 @ref SARibbonBar::RibbonStyle
 * @param contextTitle
 * @note 此函数会触发信号@sa contextTitleChanged
 */
void SARibbonContextCategory::setContextTitle(const QString& contextTitle)
{
	d_ptr->mContextTitle = contextTitle;
	Q_EMIT contextTitleChanged(contextTitle);
}

/**
 * @brief 获取对应的tab页
 * @param index
 * @return
 */
SARibbonCategory* SARibbonContextCategory::categoryPage(int index)
{
    return (d_ptr->mCategoryDataList[ index ].categoryPage);
}

/**
 * @brief 获取所有的SARibbonCategory*
 * @return
 */
QList< SARibbonCategory* > SARibbonContextCategory::categoryList() const
{
	QList< SARibbonCategory* > res;

	for (const SAPrivateRibbonCategoryData& c : qAsConst(d_ptr->mCategoryDataList)) {
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
	for (int i = 0; i < d_ptr->mCategoryDataList.size(); ++i) {
		if (d_ptr->mCategoryDataList[ i ].categoryPage == category) {
			d_ptr->mCategoryDataList.takeAt(i);
			return (true);
		}
	}
	return (false);
}

/**
 * @brief 判断上下文是否维护了此SARibbonCategory
 * @param category
 * @return
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
 * @brief hide contextCategory/隐藏上下文标签
 *
 * 此方法等同于：
 * @code
 * ribbon->hideContextCategory(this);
 * @endcode
 *
 */
void SARibbonContextCategory::hide()
{
	if (SARibbonBar* ribbon = qobject_cast< SARibbonBar* >(parent())) {
		ribbon->hideContextCategory(this);
	}
}

/**
 * @brief show contextCategory/显示上下文标签
 *
 * 此方法等同于：
 * @code
 * ribbon->showContextCategory(this);
 * @endcode
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
