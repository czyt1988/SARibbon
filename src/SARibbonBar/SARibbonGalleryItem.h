#ifndef SARIBBONGALLERYITEM_H
#define SARIBBONGALLERYITEM_H
#include "SARibbonGlobal.h"
#include <QIcon>
#include <QVariant>
#include <QMap>
#include <QAction>
class SARibbonGalleryGroup;


class SA_RIBBON_EXPORT SARibbonGalleryItem
{
public:
	SARibbonGalleryItem();
	SARibbonGalleryItem(const QString& text, const QIcon& icon);
	SARibbonGalleryItem(QAction *act);
	virtual ~SARibbonGalleryItem();
	void setData(int role, const QVariant& data);
	virtual QVariant data(int role) const;

	void setText(const QString& text);
	QString text() const;

	void setToolTip(const QString& text);
	QString toolTip() const;

	void setIcon(const QIcon& ico);
	QIcon icon() const;

	bool isSelectable() const;
	void setSelectable(bool isSelectable);

	bool isEnable() const;
	void setEnable(bool isEnable);

	void setFlags(Qt::ItemFlags flag);
	virtual Qt::ItemFlags flags() const;

	void setAction(QAction *act);
	QAction *action();

private:
	friend class SARibbonGalleryGroupModel;
	QMap<int, QVariant> m_datas;
	Qt::ItemFlags m_flags;
	QAction *m_action;
};

#endif // SARIBBONGALLERYITEM_H
