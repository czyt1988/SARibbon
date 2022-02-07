#include "SARibbonPannelLayout.h"
#include "SARibbonCategory.h"
#include "SARibbonPannelOptionButton.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonElementManager.h"
#include "SARibbonMenu.h"
#include <QWidgetAction>
#include <QQueue>
#include <set>
#include "SARibbonPannel.h"
#include "SARibbonPannelItem.h"

const int c_higherModehight = 98;
const int c_lowerModehight = 72;
const int c_iconHighForHigerLarge = 32;
const QSize c_iconSizeForHigerLarge = QSize(c_iconHighForHigerLarge, c_iconHighForHigerLarge);
const int c_iconHighForHigerSmall = 16;
const QSize c_iconSizeForHigerSmall = QSize(c_iconHighForHigerSmall, c_iconHighForHigerSmall);



#define HELP_DRAW_RECT(p, rect)		 \
    do{				 \
        p.save();		 \
        QPen _pen(Qt::DashLine); \
        _pen.setColor(Qt::blue); \
        p.setPen(_pen);		 \
        p.setBrush(QBrush());	 \
        p.drawRect(rect);	 \
        p.restore();		 \
    }while(0)

SARibbonPannelLayout::SARibbonPannelLayout(QWidget *p) : QLayout(p)
, m_columnCount(0)
, m_expandFlag(false)
, m_dirty(true)
{
	setSpacing(1);
	SARibbonPannel *tb = qobject_cast<SARibbonPannel *>(p);

	if (!tb) {
		return;
	}
}


SARibbonPannelLayout::~SARibbonPannelLayout()
{
	while (!m_items.isEmpty())
	{
		SARibbonPannelItem *item = m_items.takeFirst();
		if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item->action)) {
			if (item->customWidget) {
				widgetAction->releaseWidget(item->widget());
			}
		}
		delete item;
	}
}


int SARibbonPannelLayout::indexOf(QAction *action) const
{
	for (int i = 0; i < m_items.count(); ++i)
	{
		if (m_items.at(i)->action == action) {
			return (i);
		}
	}
	return (-1);
}


void SARibbonPannelLayout::addItem(QLayoutItem *item)
{
	Q_UNUSED(item);
	qWarning("SARibbonPannelLayout::addItem(): please use addAction() instead");
	return;
}


void SARibbonPannelLayout::insertAction(int index, QAction *act, SARibbonPannelItem::RowProportion rp)
{
	index = qMax(0, index);
	index = qMin(m_items.count(), index);
	SARibbonPannelItem *item = createItem(act, rp);

	if (item) {
		m_items.insert(index, item);
		invalidate();
	}
}


QLayoutItem *SARibbonPannelLayout::itemAt(int index) const
{
	if ((index < 0) || (index >= m_items.count())) {
		return (nullptr);
	}
	return (m_items.at(index));
}


QLayoutItem *SARibbonPannelLayout::takeAt(int index)
{
	if ((index < 0) || (index >= m_items.count())) {
		return (nullptr);
	}
	SARibbonPannelItem *item = m_items.takeAt(index);

	QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item->action);

	if ((widgetAction != 0) && item->customWidget) {
		widgetAction->releaseWidget(item->widget());
	}
	else {
		// destroy the QToolButton/QToolBarSeparator
		item->widget()->hide();
		item->widget()->deleteLater();
	}

	invalidate();
	return (item);
}


int SARibbonPannelLayout::count() const
{
	return (m_items.count());
}


bool SARibbonPannelLayout::isEmpty() const
{
	return (m_items.isEmpty());
}


void SARibbonPannelLayout::invalidate()
{
	m_dirty = true;
	QLayout::invalidate();
}


Qt::Orientations SARibbonPannelLayout::expandingDirections() const
{
	return (Qt::Horizontal);
}


QSize SARibbonPannelLayout::minimumSize() const
{
	return (m_sizeHint);
}


QSize SARibbonPannelLayout::sizeHint() const
{
	return (m_sizeHint);
}


SARibbonPannelItem *SARibbonPannelLayout::pannelItem(QAction *action) const
{
	int index = indexOf(action);

	if (index >= 0) {
		return (m_items[index]);
	}
	return (nullptr);
}


SARibbonPannelItem *SARibbonPannelLayout::lastItem() const
{
	if (m_items.isEmpty()) {
		return (nullptr);
	}
	return (m_items.last());
}


QWidget *SARibbonPannelLayout::lastWidget() const
{
	SARibbonPannelItem *item = lastItem();

	if (item) {
		return (item->widget());
	}
	return (nullptr);
}


int SARibbonPannelLayout::defaultPannelHeight() const
{
	SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(parentWidget());
	int high = c_higherModehight;

	if (pannel) {
		switch (pannel->pannelLayoutMode())
		{
		case SARibbonPannel::ThreeRowMode:
			high = c_higherModehight;
			break;

		case SARibbonPannel::TwoRowMode:
			high = c_lowerModehight;
			break;

		default:
			high = c_higherModehight;
			break;
		}
	}
	return (high);
}

void SARibbonPannelLayout::move(int from, int to)
{
	if (from == to) {
		return;
	}
	if (to < 0) {
		to = 0;
	}
	if (to >= count()) {
		to = count() - 1;
	}
	m_items.move(from, to);
	invalidate();
}

bool SARibbonPannelLayout::isDirty() const
{
	return (m_dirty);
}


void SARibbonPannelLayout::layoutActions()
{
	if (m_dirty) {
		updateGeomArray(geometry());
	}
	QList<QWidget *> showWidgets, hideWidgets;

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	qDebug() << "\r\n\r\n =============================================="
		"\r\n SARibbonPannelLayout::layoutActions"
		<< " \r\n name:" << parentWidget()->windowTitle()
		<< " sizehint:" << this->sizeHint()
		;
#endif
	for (SARibbonPannelItem *item : m_items)
	{
		if (item->isEmpty()) {
			hideWidgets << item->widget();
		}
		else {
			item->setGeometry(item->itemWillSetGeometry);
			//            item->widget()->setFixedSize(item->itemWillSetGeometry.size());
			//            item->widget()->move(item->itemWillSetGeometry.topLeft());
			showWidgets << item->widget();
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			qDebug() << "[" << item->rowIndex << "," << item->columnIndex << "]"
				<< " -> " << item->itemWillSetGeometry
				<< ":" << item->widget()->metaObject()->className()
				;
#endif
		}
	}

	for (QWidget *w : showWidgets)
	{
		w->show();
	}
	for (QWidget *w : hideWidgets)
	{
		w->hide();
	}
}


SARibbonPannelItem *SARibbonPannelLayout::createItem(QAction *action, SARibbonPannelItem::RowProportion rp)
{
	bool customWidget = false;
	QWidget *widget = nullptr;
	SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(parentWidget());

	if (!pannel) {
		return (nullptr);
	}
	if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action)) {
		widget = widgetAction->requestWidget(pannel);
		if (widget != nullptr) {
			widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
			customWidget = true;
		}
	}
	else if (action->isSeparator()) {
		SARibbonSeparatorWidget *sep = RibbonSubElementDelegate->createRibbonSeparatorWidget(pannel);
		widget = sep;
	}

	if (!widget) {
		SARibbonToolButton::RibbonButtonType buttonType =
			((rp == SARibbonPannelItem::Large) ? SARibbonToolButton::LargeButton
				: SARibbonToolButton::SmallButton);

		SARibbonToolButton *button = RibbonSubElementDelegate->createRibbonToolButton(pannel);
		button->setAutoRaise(true);
		button->setFocusPolicy(Qt::NoFocus);
		button->setButtonType(buttonType);
		if (SARibbonToolButton::LargeButton == buttonType) {
			button->setLargeButtonType((pannel->isTwoRow())
				? SARibbonToolButton::Lite
				: SARibbonToolButton::Normal);
		}
		button->setDefaultAction(action);

		QObject::connect(button, &SARibbonToolButton::triggered
			, pannel, &SARibbonPannel::actionTriggered);
		widget = button;
	}
	widget->hide();
	SARibbonPannelItem *result = new SARibbonPannelItem(widget);

	result->rowProportion = rp;
	result->customWidget = customWidget;
	result->action = action;
	return (result);
}



void SARibbonPannelLayout::updateGeomArray(const QRect& setrect)
{
	SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(parentWidget());

	if (!pannel) {
		return;
	}
	//    QWidget *pannelPar = pannel->parentWidget();
	//    if(pannelPar)
	//    defaultPannelHeight();
	int height = setrect.height();
	const QMargins mag = this->contentsMargins();
	const int spacing = this->spacing();
	int x = mag.left();
	const short rowCount = (pannel->pannelLayoutMode() == SARibbonPannel::ThreeRowMode) ? 3 : 2;
	const int largeHeight = height - mag.top() - mag.bottom() - pannel->titleHeight();

	m_largeHeight = largeHeight;
	const int smallHeight = (largeHeight - (rowCount - 1)*spacing) / rowCount;
	const int yMediumRow0 = (2 == rowCount) ? mag.top()
		: (mag.top() + ((largeHeight - 2 * smallHeight) / 3));
	const int yMediumRow1 = (2 == rowCount) ? (mag.top() + smallHeight + spacing)
		: (mag.top() + ((largeHeight - 2 * smallHeight) / 3) * 2 + smallHeight);
	const int ySmallRow0 = mag.top();
	const int ySmallRow1 = mag.top() + smallHeight + spacing;
	const int ySmallRow2 = mag.top() + 2 * (smallHeight + spacing);
	short row = 0;
	int column = 0;
	int columMaxWidth = 0;
	int totalWidth = 0;

	int itemCount = m_items.count();

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	qDebug() << "\r\n\r\n============================================="
		<< "\r\nSARibbonPannelLayout::updateGeomArray()"
		<< " setrect:" << setrect
		<< "\r\npannel name:" << pannel->windowTitle()
		<< "\r\n largeHeight:" << largeHeight
		<< "\r\n smallHeight:" << smallHeight
		<< "\r\n rowCount:" << rowCount
		;
#endif

	SARibbonPannelItem::RowProportion thisColumnRP0 = SARibbonPannelItem::None;
	SARibbonPannelItem::RowProportion thisColumnRP1 = SARibbonPannelItem::None;
	for (int i = 0; i < itemCount; ++i)
	{
		SARibbonPannelItem *item = m_items.at(i);
		if (item->isEmpty()) {
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			qDebug() << item->widget()->metaObject()->className() << "is hide"
				<< " row:" << row << " col:" << column;
#endif
			item->rowIndex = -1;
			item->columnIndex = -1;
			continue;
		}

		QSize hint = item->sizeHint();
		Qt::Orientations exp = item->expandingDirections();
		if (item->widget()) {
			if ((item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag)) {
				m_expandFlag = true;
			}
		}
		SARibbonPannelItem::RowProportion rp = item->rowProportion;
		if (SARibbonPannelItem::None == rp) {
			if (exp & Qt::Vertical) {
				rp = SARibbonPannelItem::Large;
			}
			else {
				rp = SARibbonPannelItem::Small;
			}
		}
		switch (rp)
		{
		case SARibbonPannelItem::Large:
		{
			if (row != 0) {
				x += (columMaxWidth + spacing);
				++column;
				row = 0;
				columMaxWidth = 0;
			}
			//
			item->rowIndex = 0;
			item->columnIndex = column;
			item->itemWillSetGeometry = QRect(x, mag.top(), hint.width(), largeHeight);
			columMaxWidth = hint.width();
			x += (columMaxWidth + spacing);
			row = 0;
			columMaxWidth = 0;
			++column;
		}
		break;

		case SARibbonPannelItem::Medium:
		{
			if (2 == rowCount) {
				if (0 == row) {
					item->rowIndex = 0;
					item->columnIndex = column;
					item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
					thisColumnRP0 = SARibbonPannelItem::Medium;
					thisColumnRP1 = SARibbonPannelItem::None;
					columMaxWidth = hint.width();
					row = 1;
				}
				else {
					item->rowIndex = 1;
					item->columnIndex = column;
					item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
					thisColumnRP1 = SARibbonPannelItem::Medium;
					columMaxWidth = qMax(columMaxWidth, hint.width());
					x += (columMaxWidth + spacing);
					row = 0;
					columMaxWidth = 0;
					++column;
				}
			}
			else {
				if (0 == row) {
					item->rowIndex = 0;
					item->columnIndex = column;
					item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
					thisColumnRP0 = SARibbonPannelItem::Medium;
					thisColumnRP1 = SARibbonPannelItem::None;
					columMaxWidth = hint.width();
					row = 1;
				}
				else if (1 == row) {
					item->rowIndex = 1;
					item->columnIndex = column;
					item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
					thisColumnRP1 = SARibbonPannelItem::Medium;
					columMaxWidth = qMax(columMaxWidth, hint.width());
					x += (columMaxWidth + spacing);
					row = 0;
					columMaxWidth = 0;
					++column;
				}
				else {
					x += (columMaxWidth + spacing);
					++column;
					row = 0;
					columMaxWidth = 0;
					item->rowIndex = 0;
					item->columnIndex = column;
					item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
					thisColumnRP0 = SARibbonPannelItem::Medium;
					thisColumnRP1 = SARibbonPannelItem::None;
					columMaxWidth = hint.width();
					row = 1;
				}
			}
		}
		break;

		case SARibbonPannelItem::Small:
		{
			if (0 == row) {
				item->rowIndex = 0;
				item->columnIndex = column;
				item->itemWillSetGeometry = QRect(x, ySmallRow0, hint.width(), smallHeight);
				thisColumnRP0 = SARibbonPannelItem::Small;
				thisColumnRP1 = SARibbonPannelItem::None;
				columMaxWidth = hint.width();
				row = 1;
			}
			else if (1 == row) {
				item->rowIndex = 1;
				item->columnIndex = column;
				item->itemWillSetGeometry = QRect(x, ySmallRow1, hint.width(), smallHeight);
				if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
					item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
				}
				thisColumnRP1 = SARibbonPannelItem::Small;
				columMaxWidth = qMax(columMaxWidth, hint.width());
				if (2 == rowCount) {
					x += (columMaxWidth + spacing);
					row = 0;
					columMaxWidth = 0;
					++column;
				}
				else {
					row = 2;
				}
				if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
					x += (columMaxWidth + spacing);
					row = 0;
					columMaxWidth = 0;
					++column;
				}
			}
			else {
				item->rowIndex = 2;
				item->columnIndex = column;
				item->itemWillSetGeometry = QRect(x, ySmallRow2, hint.width(), smallHeight);
				columMaxWidth = qMax(columMaxWidth, hint.width());
				x += (columMaxWidth + spacing);
				row = 0;
				columMaxWidth = 0;
				++column;
			}
		}
		break;

		default:
			break;
		}
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
		qDebug() << item->widget()->metaObject()->className()
			<< " rp:" << rp
			<< " row:" << item->rowIndex << " col:" << item->columnIndex
			<< " new row:" << row << " new column:" << column
			<< " itemWillSetGeometry:" << item->itemWillSetGeometry
			<< " sizeHint:" << hint
			<< " x:" << x
			;
#endif
		if (i == (itemCount - 1)) {
			if (item->columnIndex != column) {
				m_columnCount = column;
				totalWidth = x + mag.right();
			}
			else {
				m_columnCount = column + 1;
				totalWidth = x + columMaxWidth + spacing + mag.right();
			}
		}
	}
	if (pannel->isTwoRow()) {
		if (pannel->isHaveOptionAction()) {
			totalWidth += pannel->optionActionButtonSize().width();
		}
	}
	if (totalWidth < setrect.width()) {
		recalcExpandGeomArray(setrect);
	}
	this->m_sizeHint = QSize(totalWidth, height);
}


void SARibbonPannelLayout::recalcExpandGeomArray(const QRect& setrect)
{
	int expandwidth = setrect.width() - this->m_sizeHint.width();

	if (expandwidth <= 0) {
		return;
	}
	struct _columnExpandInfo {
		int				oldColumnWidth = 0;
		int				columnMaximumWidth = -1;
		int				columnExpandedWidth = 0;
		QList<SARibbonPannelItem *>	expandItems;
	};
	QMap<int, _columnExpandInfo> columnExpandInfo;

	for (SARibbonPannelItem *item : this->m_items)
	{
		if ((!item->isEmpty()) && item->expandingDirections() & Qt::Horizontal) {
			QMap<int, _columnExpandInfo>::iterator i = columnExpandInfo.find(item->columnIndex);
			if (i == columnExpandInfo.end()) {
				i = columnExpandInfo.insert(item->columnIndex, _columnExpandInfo());
			}
			i.value().expandItems.append(item);
		}
	}
	if (columnExpandInfo.size() <= 0) {
		return;
	}
	int oneColCanexpandWidth = expandwidth / columnExpandInfo.size();

	for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end();)
	{
		int& oldColumnWidth = i.value().oldColumnWidth;
		int& columnMaximumWidth = i.value().columnMaximumWidth;
		this->columnWidthInfo(i.key(), oldColumnWidth, columnMaximumWidth);
		if ((oldColumnWidth <= 0) || (oldColumnWidth > columnMaximumWidth)) {
			i = columnExpandInfo.erase(i);
			continue;
		}
		int colwidth = oneColCanexpandWidth + oldColumnWidth;
		if (colwidth >= columnMaximumWidth) {
			i.value().columnExpandedWidth = columnMaximumWidth;
		}
		else {
			i.value().columnExpandedWidth = colwidth;
		}
		++i;
	}
	for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i)
	{
		int moveXLen = i.value().columnExpandedWidth - i.value().oldColumnWidth;
		for (SARibbonPannelItem *item : this->m_items)
		{
			if (item->isEmpty() || (item->columnIndex < i.key())) {
				continue;
			}
			if (item->columnIndex == i.key()) {
				if (i.value().expandItems.contains(item)) {
					item->itemWillSetGeometry.setWidth(i.value().columnExpandedWidth);
				}
				else {
					continue;
				}
			}
			else {
				item->itemWillSetGeometry.moveLeft(item->itemWillSetGeometry.x() + moveXLen);
			}
		}
	}
}



//QMap<int, QRect> SARibbonPannelLayout::columnsGeometry() const
//{
//    QMap<int, QRect> res;
//    for (SARibbonPannelItem *item:m_items){
//        if(item->isEmpty()){
//           continue;
//        }
//        QMap<int, QRect>::iterator i = res.find(item->columnIndex);
//        if(i == res.end())
//        {
//            QRect r = item->geometry();
//            r.setY(this->contentsMargins().top());
//            r.setHeight(m_largeHeight);
//            i = res.insert(item->columnIndex,r);
//        }
//        if(item->itemWillSetGeometry.width() > i.value().width()){
//            i.value().setWidth(item->itemWillSetGeometry.width());
//        }
//    }
//    return res;
//}

void SARibbonPannelLayout::columnWidthInfo(int colindex, int& width, int& maximum) const
{
	width = -1;
	maximum = -1;
	for (SARibbonPannelItem *item : m_items)
	{
		if (!item->isEmpty() && (item->columnIndex == colindex)) {
			width = qMax(width, item->itemWillSetGeometry.width());
			maximum = qMax(maximum, item->widget()->maximumWidth());
		}
	}
}


void SARibbonPannelLayout::setGeometry(const QRect& rect)
{
	m_dirty = false;
	updateGeomArray(rect);
	QLayout::setGeometry(rect);
	layoutActions();
}
