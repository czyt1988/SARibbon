#include "SAColorGridWidget.h"
#include "SAColorToolButton.h"
#include <QGridLayout>
#include <cmath>
#include <QButtonGroup>

class SAColorGridWidget::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorGridWidget)
public:
    PrivateData(SAColorGridWidget* p);
    // 获取ColorToolButton
    SAColorToolButton* getColorToolButtonAt(int index);
    SAColorToolButton* getColorToolButtonAt(int r, int c);
    SAColorToolButton* getCheckedButton() const;
    void updateGridColor(bool isRemoveSpacer = false);
    void updateGridColorSize();
    void updateGridColorCheckable();
    void iterationColorBtns(SAColorGridWidget::FunColorBtn fn);
    void removeAt(int r, int c);
    void setColorAt(const QColor& clr, int r, int c);
    bool isSpacer(int r, int c) const;

public:
    QList< QColor > mColors;
    QGridLayout* mGridLayout { nullptr };
    QButtonGroup* mButtonGroup { nullptr };
    QSize mIconSize { 16, 16 };
    int mColumnCount { 8 };  ///< 列数，行数量会根据列数量来匹配,如果设置-1或者0，说明不限定列数量，这样会只有一行
    bool mColorCheckable;    ///< 设置颜色是否是checkable
    bool mHorizontalSpacerToRight { false };  ///< 最右边是否有弹簧
};

SAColorGridWidget::PrivateData::PrivateData(SAColorGridWidget* p) : q_ptr(p)
{
    mGridLayout = new QGridLayout(p);
    p->setLayout(mGridLayout);
    mGridLayout->setSpacing(0);
    mGridLayout->setContentsMargins(1, 1, 1, 1);
    mButtonGroup = new QButtonGroup(p);
    mButtonGroup->setExclusive(true);
    p->setMinimumHeight(mIconSize.height());
    p->setMinimumWidth(mIconSize.width());
}

SAColorToolButton* SAColorGridWidget::PrivateData::getColorToolButtonAt(int index)
{
    QLayoutItem* item = mGridLayout->itemAt(index);
    if (nullptr == item) {
        return nullptr;
    }
    return qobject_cast< SAColorToolButton* >(item->widget());
}

SAColorToolButton* SAColorGridWidget::PrivateData::getColorToolButtonAt(int r, int c)
{
    QLayoutItem* item = mGridLayout->itemAtPosition(r, c);
    if (nullptr == item) {
        return nullptr;
    }
    return qobject_cast< SAColorToolButton* >(item->widget());
}

SAColorToolButton* SAColorGridWidget::PrivateData::getCheckedButton() const
{
    return qobject_cast< SAColorToolButton* >(mButtonGroup->checkedButton());
}

/**
 * @brief 根据mColors更新布局
 */
void SAColorGridWidget::PrivateData::updateGridColor(bool isRemoveSpacer)
{
    int row = 1;
    int col = mColumnCount;
    if (col <= 0) {
        col = mColors.size();
    } else {
        row = std::ceil(mColors.size() / (float)col);
    }
    int index = 0;
    for (int r = 0; r < row; ++r) {
        for (int c = 0; c < col; ++c) {
            if (index < mColors.size()) {
                setColorAt(mColors[ index ], r, c);
                ++index;
            } else {
                removeAt(r, c);
            }
        }
    }
    // 清除多余单元格
    int nowGridRow = mGridLayout->rowCount();
    int nowGridCol = mGridLayout->columnCount();
    if (nowGridRow > row) {
        // 多余的清除
        for (int r = row; r < nowGridRow; ++r) {
            for (int c = 0; c < nowGridCol; ++c) {
                removeAt(r, c);
            }
        }
    }
    if (nowGridCol > col) {
        // 多余的列清除
        for (int r = 0; r < row; ++r) {
            for (int c = col; c < nowGridCol; ++c) {
                if (isRemoveSpacer) {
                    removeAt(r, c);
                } else {
                    if (!isSpacer(r, c)) {
                        removeAt(r, c);
                    }
                }
            }
        }
    }
}

/**
 * @brief 更新colorsize
 */
void SAColorGridWidget::PrivateData::updateGridColorSize()
{
    QSize s = mIconSize;
    iterationColorBtns([ s ](SAColorToolButton* btn) {
        if (btn) {
            btn->setIconSize(s);
        }
    });
}

void SAColorGridWidget::PrivateData::updateGridColorCheckable()
{
    bool v = mColorCheckable;
    iterationColorBtns([ v ](SAColorToolButton* btn) {
        if (btn) {
            btn->setCheckable(v);
        }
    });
}

/**
 * @brief 遍历所有的button
 * @param fn
 */
void SAColorGridWidget::PrivateData::iterationColorBtns(FunColorBtn fn)
{
    int cnt = mGridLayout->count();
    for (int i = 0; i < cnt; ++i) {
        SAColorToolButton* tl = getColorToolButtonAt(i);
        fn(tl);
    }
}

/**
 * @brief 删除网格窗口
 * @param r
 * @param c
 */
void SAColorGridWidget::PrivateData::removeAt(int r, int c)
{
    QLayoutItem* item = mGridLayout->itemAtPosition(r, c);
    if (item) {
        QWidget* w = item->widget();
        mGridLayout->removeItem(item);
        delete item;
        if (w) {
            w->deleteLater();
        }
    }
}

void SAColorGridWidget::PrivateData::setColorAt(const QColor& clr, int r, int c)
{
    QLayoutItem* item = mGridLayout->itemAtPosition(r, c);
    if (item) {
        SAColorToolButton* tl = qobject_cast< SAColorToolButton* >(item->widget());
        if (tl) {
            tl->setColor(clr);
        }
    } else {
        SAColorToolButton* tl = new SAColorToolButton(SAColorToolButton::NoColorMenu, q_ptr);
        tl->setToolButtonStyle(Qt::ToolButtonIconOnly);
        tl->setIconSize(mIconSize);
        tl->setMargins(QMargins(4, 4, 4, 4));
        tl->setColor(clr);
        tl->setCheckable(mColorCheckable);
        tl->setAutoRaise(true);
        mButtonGroup->addButton(tl, r + c);
        mGridLayout->addWidget(tl, r, c);
    }
}

bool SAColorGridWidget::PrivateData::isSpacer(int r, int c) const
{
    QLayoutItem* item = mGridLayout->itemAtPosition(r, c);
    if (item) {
        if (QSpacerItem* si = dynamic_cast< QSpacerItem* >(item)) {
            return true;
        }
    }
    return false;
}

//==============================================================
// SAColorGridWidget
//==============================================================

SAColorGridWidget::SAColorGridWidget(QWidget* par) : QWidget(par), d_ptr(new SAColorGridWidget::PrivateData(this))
{
    connect(d_ptr->mButtonGroup,
            QOverload< QAbstractButton* >::of(&QButtonGroup::buttonClicked),
            this,
            &SAColorGridWidget::onButtonClicked);
    connect(d_ptr->mButtonGroup,
            QOverload< QAbstractButton* >::of(&QButtonGroup::buttonPressed),
            this,
            &SAColorGridWidget::onButtonPressed);
    connect(d_ptr->mButtonGroup,
            QOverload< QAbstractButton* >::of(&QButtonGroup::buttonReleased),
            this,
            &SAColorGridWidget::onButtonReleased);
    connect(d_ptr->mButtonGroup,
            QOverload< QAbstractButton*, bool >::of(&QButtonGroup::buttonToggled),
            this,
            &SAColorGridWidget::onButtonToggled);
}

SAColorGridWidget::~SAColorGridWidget()
{
}

/**
 * @brief 设置列数，行数量会根据列数量来匹配,如果设置-1或者0，说明不限定列数量，这样会只有一行
 * @param c
 */
void SAColorGridWidget::setColumnCount(int c)
{
    d_ptr->mColumnCount = c;
    d_ptr->updateGridColor(true);
    if (d_ptr->mHorizontalSpacerToRight) {
        setHorizontalSpacerToRight();
    }
    updateGeometry();
}

/**
 * @brief 设置颜色列表
 * @param c
 */
void SAColorGridWidget::setColorList(const QList< QColor >& cls)
{
    d_ptr->mColors = cls;
    d_ptr->updateGridColor();
    updateGeometry();
}

/**
 * @brief 获取颜色列表
 * @return
 */
QList< QColor > SAColorGridWidget::getColorList() const
{
    return d_ptr->mColors;
}

/**
 * @brief 获取间隔
 * @return
 */
int SAColorGridWidget::spacing() const
{
    return d_ptr->mGridLayout->spacing();
}

/**
 * @brief 设置间隔
 * @param v
 */
void SAColorGridWidget::setSpacing(int v)
{
    d_ptr->mGridLayout->setSpacing(v);
}

/**
 * @brief 获取颜色的数量
 * @return
 */
int SAColorGridWidget::colorCount() const
{
    return d_ptr->mColors.size();
}

/**
 * @brief 设置图标 size
 * @return
 */
void SAColorGridWidget::setColorIconSize(const QSize& s)
{
    d_ptr->mIconSize = s;
    setMinimumHeight(s.height());
    setMinimumWidth(s.width());
    d_ptr->updateGridColorSize();
}

/**
 * @brief 获取图标 size
 * @return
 */
QSize SAColorGridWidget::colorIconSize() const
{
    return d_ptr->mIconSize;
}

/**
 * @brief 设置颜色是否是checkable
 *
 * checkable的颜色按钮是可checked的
 * @param on
 */
void SAColorGridWidget::setColorCheckable(bool on)
{
    d_ptr->mColorCheckable = on;
    d_ptr->updateGridColorCheckable();
}

/**
 * @brief 颜色是否是checkable
 * @return
 */
bool SAColorGridWidget::isColorCheckable() const
{
    return d_ptr->mColorCheckable;
}

/**
 * @brief 获取当前选中的颜色
 * @return
 */
QColor SAColorGridWidget::currentCheckedColor() const
{
    QAbstractButton* btn = d_ptr->mButtonGroup->checkedButton();
    if (nullptr == btn) {
        return QColor();
    }
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (nullptr == t) {
        return QColor();
    }
    return t->color();
}

/**
 * @brief 获取index对应的colorbutton
 * @param index
 * @return 如果没有返回nullptr
 */
SAColorToolButton* SAColorGridWidget::colorButton(int index) const
{
    return d_ptr->getColorToolButtonAt(index);
}

/**
 * @brief 等同GridLayout的VerticalSpacing属性
 * @param v
 */
void SAColorGridWidget::setVerticalSpacing(int v)
{
    d_ptr->mGridLayout->setVerticalSpacing(v);
}
/**
 * @brief 等同GridLayout的VerticalSpacing属性
 * @return
 */
int SAColorGridWidget::verticalSpacing() const
{
    return d_ptr->mGridLayout->verticalSpacing();
}
/**
 * @brief 等同GridLayout的HorizontalSpacing属性
 * @param v
 */
void SAColorGridWidget::setHorizontalSpacing(int v)
{
    d_ptr->mGridLayout->setHorizontalSpacing(v);
}
/**
 * @brief 等同GridLayout的HorizontalSpacing属性
 * @return
 */
int SAColorGridWidget::horizontalSpacing() const
{
    return d_ptr->mGridLayout->horizontalSpacing();
}

/**
 * @brief 清除选中状态，这时没有颜色是选中的
 */
void SAColorGridWidget::clearCheckedState()
{
    if (d_ptr->mButtonGroup->exclusive()) {
        SAColorToolButton* btn = d_ptr->getCheckedButton();
        if (btn) {
            d_ptr->mButtonGroup->setExclusive(false);
            btn->setChecked(false);
            d_ptr->mButtonGroup->setExclusive(true);
        }
    } else {
        d_ptr->iterationColorBtns([](SAColorToolButton* btn) {
            if (btn->isChecked()) {
                btn->setChecked(false);
            }
        });
    }
}

void SAColorGridWidget::iterationColorBtns(SAColorGridWidget::FunColorBtn fn)
{
    d_ptr->iterationColorBtns(fn);
}

void SAColorGridWidget::setRowMinimumHeight(int row, int minSize)
{
    d_ptr->mGridLayout->setRowMinimumHeight(row, minSize);
}

void SAColorGridWidget::setHorizontalSpacerToRight(bool on)
{
    if (on) {
        QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        d_ptr->mGridLayout->addItem(horizontalSpacer, 0, d_ptr->mColumnCount, 1, 1);
    } else {
        d_ptr->removeAt(0, d_ptr->mColumnCount);
    }
    d_ptr->mHorizontalSpacerToRight = on;
}

void SAColorGridWidget::onButtonClicked(QAbstractButton* btn)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        emit colorClicked(t->color());
    }
}

void SAColorGridWidget::onButtonPressed(QAbstractButton* btn)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        emit colorPressed(t->color());
    }
}

void SAColorGridWidget::onButtonToggled(QAbstractButton* btn, bool on)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        emit colorToggled(t->color(), on);
    }
}

QSize SAColorGridWidget::sizeHint() const
{
    return d_ptr->mGridLayout->sizeHint();
    //    int w = d_ptr->mIconSize.width() + d_ptr->mGridLayout->verticalSpacing();
    //    int h = d_ptr->mIconSize.height();
    //    if (d_ptr->mColumnCount > 0) {
    //        h *= d_ptr->mColumnCount;
    //        if (!d_ptr->mColors.empty()) {
    //            int r = std::ceil(d_ptr->mColors.size() / (float)(d_ptr->mColumnCount));
    //            if (r > 0) {
    //                w *= r;
    //            }
    //        }
    //    }
    //    return QSize(w, h);
}

void SAColorGridWidget::onButtonReleased(QAbstractButton* btn)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        emit colorReleased(t->color());
    }
}

namespace SA
{

QList< QColor > getStandardColorList()
{
    static QList< QColor > s_standardColorList({ QColor(192, 0, 0),
                                                 QColor(255, 0, 0),
                                                 QColor(255, 192, 0),
                                                 QColor(255, 255, 0),
                                                 QColor(146, 208, 80),
                                                 QColor(0, 176, 80),
                                                 QColor(0, 176, 240),
                                                 QColor(0, 112, 192),
                                                 QColor(0, 32, 96),
                                                 QColor(112, 48, 160) });
    return s_standardColorList;
}
}
