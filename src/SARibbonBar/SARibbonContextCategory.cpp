#include "SARibbonContextCategory.h"
#include <QList>
#include <QVariant>
#include "SARibbonElementManager.h"
#include <QDebug>

class SARibbonCategoryData
{
public:
	SARibbonCategory *categoryPage;
};

class SARibbonContextCategoryPrivate
{
public:
	QList<SARibbonCategoryData> categoryDataList;
	QVariant contextID;
	QColor contextColor;
	QString contextTitle;
};

SARibbonContextCategory::SARibbonContextCategory(QWidget *parent) : QObject(parent)
, m_d(new SARibbonContextCategoryPrivate)
{
}


SARibbonContextCategory::~SARibbonContextCategory()
{
	delete m_d;
}


SARibbonCategory *SARibbonContextCategory::addCategoryPage(const QString& title)
{
	SARibbonCategoryData catData;
	SARibbonCategory *category = RibbonSubElementDelegate->createRibbonCategory(parentWidget());

	category->markIsContextCategory(true);
	category->setWindowTitle(title);
	catData.categoryPage = category;
	m_d->categoryDataList.append(catData);
	category->installEventFilter(this);
	emit categoryPageAdded(category);

	return (category);
}


int SARibbonContextCategory::categoryCount() const
{
	return (m_d->categoryDataList.count());
}


void SARibbonContextCategory::setId(const QVariant& id)
{
	m_d->contextID = id;
}


QVariant SARibbonContextCategory::id() const
{
	return (m_d->contextID);
}


void SARibbonContextCategory::setContextColor(const QColor color)
{
	m_d->contextColor = color;
}


QColor SARibbonContextCategory::contextColor() const
{
	return (m_d->contextColor);
}


QWidget *SARibbonContextCategory::parentWidget() const
{
	return (qobject_cast<QWidget *>(parent()));
}


bool SARibbonContextCategory::eventFilter(QObject *watched, QEvent *e)
{
	if (nullptr == watched) {
		return (false);
	}
	switch (e->type())
	{
	case QEvent::Close:
	{
		SARibbonCategory *c = qobject_cast<SARibbonCategory *>(watched);
		if (c) {
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			qDebug() << " -----------> close event";
#endif
			takeCategory(c);
		}
	}
	break;

	default:
		break;
	}
	return (false);
}


QString SARibbonContextCategory::contextTitle() const
{
	return (m_d->contextTitle);
}


void SARibbonContextCategory::setContextTitle(const QString& contextTitle)
{
	m_d->contextTitle = contextTitle;
}


SARibbonCategory *SARibbonContextCategory::categoryPage(int index)
{
	return (m_d->categoryDataList[index].categoryPage);
}


QList<SARibbonCategory *> SARibbonContextCategory::categoryList() const
{
	QList<SARibbonCategory *> res;

	for (SARibbonCategoryData& c : m_d->categoryDataList)
	{
		res.append(c.categoryPage);
	}
	return (res);
}


bool SARibbonContextCategory::takeCategory(SARibbonCategory *category)
{
	for (int i = 0; i < m_d->categoryDataList.size(); ++i)
	{
		if (m_d->categoryDataList[i].categoryPage == category) {
			m_d->categoryDataList.takeAt(i);
			return (true);
		}
	}
	return (false);
}
