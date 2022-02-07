#ifndef SARIBBONCONTEXTCATEGORY_H
#define SARIBBONCONTEXTCATEGORY_H
#include "SARibbonGlobal.h"
#include "SARibbonCategory.h"
#include <QWidget>
class SARibbonContextCategoryPrivate;

class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
	Q_OBJECT
public:
	SARibbonContextCategory(QWidget *parent = 0);
	~SARibbonContextCategory();
	SARibbonCategory *addCategoryPage(const QString& title);

	int categoryCount() const;

	void setId(const QVariant& id);
	QVariant id() const;

	void setContextColor(const QColor color);
	QColor contextColor() const;

	QString contextTitle() const;
	void setContextTitle(const QString& contextTitle);

	SARibbonCategory *categoryPage(int index);

	QList<SARibbonCategory *> categoryList() const;

	bool takeCategory(SARibbonCategory *category);

signals:
	void categoryPageAdded(SARibbonCategory *category);
	void categoryPageRemoved(SARibbonCategory *category);

protected:
	QWidget *parentWidget() const;
	virtual bool eventFilter(QObject *watched, QEvent *e) override;

private:
	SARibbonContextCategoryPrivate *m_d;
};

#endif // SARIBBONCONTEXTCATEGORY_H
