// 定义此宏，将SA_RIBBON_EXPORT定义为空
#ifndef SA_RIBBON_BAR_NO_EXPORT
#define SA_RIBBON_BAR_NO_EXPORT
#endif
// 定义此宏，将SA_COLOR_WIDGETS_API定义为空
#ifndef SA_COLOR_WIDGETS_NO_DLL
#define SA_COLOR_WIDGETS_NO_DLL
#endif

/*** Start of inlined file: SARibbonAmalgamTemplateHeaderGlue.h ***/
// This file provides an extra level of indirection for the @remap in the template
#include "SARibbon.h"

/*** End of inlined file: SARibbonAmalgamTemplateHeaderGlue.h ***/

// disable warnings about unsafe standard library calls
#ifdef _MSC_VER
#pragma push_macro("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(push)
#pragma warning(disable : 4996)  // deprecated POSIX names
#endif

/*** Start of inlined file: SAColorMenu.cpp ***/
#include <QWidgetAction>
#include <QColorDialog>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

class SAColorMenu::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorMenu)
public:
    PrivateData(SAColorMenu* p);
    QColor getColorByDialog();
    void recordCustomColor(const QColor& c);
    QWidgetAction* addWidget(QWidget* w);
    // 创建一个无颜色的icon
    QIcon createNoneColorIcon(QSize baseSize = QSize(32, 32));

public:
    QLabel* mTitleLabel { nullptr };  ///< 主题颜色标题
    QWidgetAction* mTitleLabelAction { nullptr };
    SAColorPaletteGridWidget* mThemeColorsWidget { nullptr };  ///< 主题颜色
    QWidgetAction* mThemeColorPaletteAction { nullptr };       ///< ThemeColorsPalette对应的action
    QAction* mCustomColorAction { nullptr };                   ///< 自定义颜色action
    SAColorGridWidget* mCustomColorsWidget { nullptr };        ///< 自定义颜色记录
    QWidgetAction* mCustomColorsWidgetAction { nullptr };      ///< 自定义颜色窗口对应的action
    QAction* mNoneColorAction { nullptr };                     ///< 无颜色action
    QList< QColor > mCustomColors;
    int mMaxCustomColorSize { 10 };                        ///< 记录最多的自定义颜色数量
    QScopedPointer< QColorDialog > mColorDlg { nullptr };  ///< 颜色对话框
};

SAColorMenu::PrivateData::PrivateData(SAColorMenu* p) : q_ptr(p)
{
}

QColor SAColorMenu::PrivateData::getColorByDialog()
{
    if (nullptr == mColorDlg) {
        mColorDlg.reset(new QColorDialog());
    }
    if (QDialog::Accepted == mColorDlg->exec()) {
        return mColorDlg->currentColor();
    }
    return QColor();
}

void SAColorMenu::PrivateData::recordCustomColor(const QColor& c)
{
    if (mCustomColors.size() < mMaxCustomColorSize) {
        mCustomColors.push_back(c);
    } else {
        // 超过数量，就左移动
        for (int i = 1; i < mCustomColors.size(); ++i) {
            mCustomColors[ i - 1 ] = mCustomColors[ i ];
        }
        mCustomColors.back() = c;
    }
}

QWidgetAction* SAColorMenu::PrivateData::addWidget(QWidget* w)
{
    QWidgetAction* wa = new QWidgetAction(q_ptr);
    wa->setDefaultWidget(w);
    q_ptr->addAction(wa);
    return wa;
}

QIcon SAColorMenu::PrivateData::createNoneColorIcon(QSize baseSize)
{
    QPixmap pixmap(baseSize);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    SAColorToolButton::paintNoneColor(&p, QRect(0, 0, pixmap.width(), pixmap.height()).adjusted(1, 1, -1, -1));
    return QIcon(pixmap);
}
//===================================================
// SAColorMenu
//===================================================

SAColorMenu::SAColorMenu(QWidget* parent) : QMenu(parent), d_ptr(new SAColorMenu::PrivateData(this))
{
    init(SA::getStandardColorList());
}

SAColorMenu::SAColorMenu(const QString& title, QWidget* parent)
    : QMenu(title, parent), d_ptr(new SAColorMenu::PrivateData(this))
{
    init(SA::getStandardColorList());
}

SAColorMenu::~SAColorMenu()
{
}

/**
 * @brief 快速绑定ColorToolButton
 * @param btn
 */
void SAColorMenu::bindToColorToolButton(SAColorToolButton* btn)
{
    if (!btn) {
        return;
    }
    if (btn->menu() != this) {
        btn->setMenu(this);
    }
    connect(this, &SAColorMenu::selectedColor, btn, &SAColorToolButton::setColor);
}

/**
 * @brief ThemeColorsPalette对应的action
 * @return
 */
QWidgetAction* SAColorMenu::themeColorsPaletteAction() const
{
    return d_ptr->mThemeColorPaletteAction;
}

/**
 * @brief CustomColorsWidget对应的action
 * @return
 */
QWidgetAction* SAColorMenu::getCustomColorsWidgetAction() const
{
    return d_ptr->mCustomColorsWidgetAction;
}

/**
 * @brief 自定义颜色action
 * @return
 */
QAction* SAColorMenu::customColorAction() const
{
    return d_ptr->mCustomColorAction;
}

/**
 * @brief 获取ThemeColorsPalette
 * @return
 */
SAColorPaletteGridWidget* SAColorMenu::colorPaletteGridWidget() const
{
    return d_ptr->mThemeColorsWidget;
}

/**
 * @brief 获取自定义颜色grid
 * @return
 */
SAColorGridWidget* SAColorMenu::customColorsWidget() const
{
    return d_ptr->mCustomColorsWidget;
}

/**
 * @brief 建立没有颜色的action，可以选择无颜色
 *
 * 无颜色选中会发射selectedColor(QColor())
 * @param on
 */
void SAColorMenu::enableNoneColorAction(bool on)
{
    // 无颜色默认是在自定义颜色的上方
    if (on) {
        if (d_ptr->mNoneColorAction) {
            if (actions().contains(d_ptr->mNoneColorAction)) {
                // 已经包含了NoneColorAction,退出
                return;
            } else {
                insertAction(d_ptr->mCustomColorAction, d_ptr->mNoneColorAction);
            }
        } else {
            QIcon ic                = d_ptr->createNoneColorIcon();
            d_ptr->mNoneColorAction = new QAction(ic, tr("None"), this);
            connect(d_ptr->mNoneColorAction, &QAction::triggered, this, &SAColorMenu::onNoneColorActionTriggered);
            insertAction(d_ptr->mCustomColorAction, d_ptr->mNoneColorAction);
        }
    } else {
        removeAction(d_ptr->mNoneColorAction);
        // remove后暂时不删除action
    }
}

/**
 * @brief 获取None Color Action
 *
 * @note 注意，enableNoneColorAction(true),之后才不是nullptr
 * @return 如果没有建立NoneColorAction，会返回nullptr
 */
QAction* SAColorMenu::noneColorAction() const
{
    return d_ptr->mNoneColorAction;
}

/**
 * @brief 这是一个辅助槽函数，为了让用户自定义的其他action也能关联menu，可以调用此槽函数，实现selectedColor信号以及menu的隐藏
 * @param c
 */
void SAColorMenu::emitSelectedColor(const QColor& c)
{
    emit selectedColor(c);
    hide();
}

void SAColorMenu::init(const QList< QColor >& themeCls)
{
    d_ptr->mTitleLabel = new QLabel(this);
    d_ptr->mTitleLabel->setText(tr("Theme Colors"));
    d_ptr->mTitleLabelAction = d_ptr->addWidget(d_ptr->mTitleLabel);

    d_ptr->mThemeColorsWidget = new SAColorPaletteGridWidget(themeCls, this);
    d_ptr->mThemeColorsWidget->setColorCheckable(false);
    d_ptr->mThemeColorPaletteAction = d_ptr->addWidget(d_ptr->mThemeColorsWidget);

    d_ptr->mCustomColorAction = new QAction(tr("Custom Color"), this);  // cn:自定义颜色
    addAction(d_ptr->mCustomColorAction);

    QSize clrSize              = d_ptr->mThemeColorsWidget->colorIconSize();
    d_ptr->mCustomColorsWidget = new SAColorGridWidget(this);
    d_ptr->mCustomColorsWidget->setRowMinimumHeight(0, clrSize.height());
    d_ptr->mCustomColorsWidget->setHorizontalSpacerToRight();
    d_ptr->mCustomColorsWidget->setColorIconSize(clrSize);
    d_ptr->mCustomColorsWidget->setColumnCount(d_ptr->mMaxCustomColorSize);
    d_ptr->mCustomColorsWidget->setColorCheckable(false);
    d_ptr->mCustomColorsWidgetAction = d_ptr->addWidget(d_ptr->mCustomColorsWidget);

    connect(d_ptr->mCustomColorAction, &QAction::triggered, this, &SAColorMenu::onCustomColorActionTriggered);
    connect(d_ptr->mThemeColorsWidget, &SAColorPaletteGridWidget::colorClicked, this, &SAColorMenu::emitSelectedColor);
    connect(d_ptr->mCustomColorsWidget, &SAColorGridWidget::colorClicked, this, &SAColorMenu::emitSelectedColor);
}

/**
 * @brief 自定义颜色
 * @param on
 */
void SAColorMenu::onCustomColorActionTriggered(bool on)
{
    Q_UNUSED(on);
    QColor c = d_ptr->getColorByDialog();
    if (c.isValid()) {
        d_ptr->recordCustomColor(c);
        d_ptr->mCustomColorsWidget->setColorList(d_ptr->mCustomColors);
        updateGeometry();
        emitSelectedColor(c);
    }
}

/**
 * @brief 无颜色
 * @param on
 */
void SAColorMenu::onNoneColorActionTriggered(bool on)
{
    Q_UNUSED(on);
    emitSelectedColor(QColor());
}

/*** End of inlined file: SAColorMenu.cpp ***/

/*** Start of inlined file: SAColorGridWidget.cpp ***/
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
        SAColorToolButton* tl = new SAColorToolButton(q_ptr);
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
    connect(d_ptr->mButtonGroup, QOverload< QAbstractButton* >::of(&QButtonGroup::buttonClicked), this, &SAColorGridWidget::onButtonClicked);
    connect(d_ptr->mButtonGroup, QOverload< QAbstractButton* >::of(&QButtonGroup::buttonPressed), this, &SAColorGridWidget::onButtonPressed);
    connect(d_ptr->mButtonGroup, QOverload< QAbstractButton* >::of(&QButtonGroup::buttonReleased), this, &SAColorGridWidget::onButtonReleased);
    connect(d_ptr->mButtonGroup, QOverload< QAbstractButton*, bool >::of(&QButtonGroup::buttonToggled), this, &SAColorGridWidget::onButtonToggled);
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

/*** End of inlined file: SAColorGridWidget.cpp ***/

/*** Start of inlined file: SAColorPaletteGridWidget.cpp ***/
// Qt
#include <QMenu>
#include <QWidgetAction>
#include <QVBoxLayout>
#include <QColorDialog>
// SA

class SAColorPaletteGridWidget::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorPaletteGridWidget)
public:
    PrivateData(SAColorPaletteGridWidget* p);
    // 生成color palette
    QList< QColor > makeColorPalette(const QList< QColor >& clrList) const;

public:
    QList< int > mFactor { 180, 160, 140, 75, 50 };  ///< palette的比例因子，将调用QColor的lighter函数执行
    QVBoxLayout* mLayout { nullptr };                ///< 垂直布局
    SAColorGridWidget* mMainColorList { nullptr };   ///< 这个用于显示标准颜色
    SAColorGridWidget* mPaletteColorGrid { nullptr };  ///< 这个用于生成3行亮色，2行暗色的palette
};

SAColorPaletteGridWidget::PrivateData::PrivateData(SAColorPaletteGridWidget* p) : q_ptr(p)
{
    mLayout = new QVBoxLayout(p);
    p->setLayout(mLayout);
    mMainColorList    = new SAColorGridWidget(p);
    mPaletteColorGrid = new SAColorGridWidget(p);
    mLayout->addWidget(mMainColorList);
    mLayout->addWidget(mPaletteColorGrid);
    mLayout->setContentsMargins(1, 1, 1, 1);
    mLayout->setSpacing(8);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    mMainColorList->setSizePolicy(sizePolicy);
    mPaletteColorGrid->setSizePolicy(sizePolicy);
    mMainColorList->setColumnCount(0);
    mPaletteColorGrid->setVerticalSpacing(0);
}

QList< QColor > SAColorPaletteGridWidget::PrivateData::makeColorPalette(const QList< QColor >& clrList) const
{
    QList< QColor > res;
    for (int f : qAsConst(mFactor)) {
        for (const QColor& c : qAsConst(clrList)) {
            res.append(c.lighter(f));
        }
    }
    return res;
}
//==============================================================
// name
//==============================================================

/**
 * @brief 使用默认的标准颜色生成一个color palette
 * @param par
 */
SAColorPaletteGridWidget::SAColorPaletteGridWidget(QWidget* par) : QWidget(par), d_ptr(new PrivateData(this))
{
    init();
    setColorList(SA::getStandardColorList());
}

/**
 * @brief 根据指定的颜色生成一个color palette
 * @param cls
 * @param par
 */
SAColorPaletteGridWidget::SAColorPaletteGridWidget(const QList< QColor >& cls, QWidget* par)
    : QWidget(par), d_ptr(new PrivateData(this))
{
    init();
    setColorList(cls);
}

SAColorPaletteGridWidget::~SAColorPaletteGridWidget()
{
}
void SAColorPaletteGridWidget::init()
{
    connect(d_ptr->mMainColorList, &SAColorGridWidget::colorClicked, this, &SAColorPaletteGridWidget::onMainColorClicked);
    connect(d_ptr->mPaletteColorGrid, &SAColorGridWidget::colorClicked, this, &SAColorPaletteGridWidget::onPaletteColorClicked);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setSizePolicy(sizePolicy);
    setColorIconSize(QSize(10, 10));
}

/**
 * @brief 设置颜色系列，颜色系列的个数决定了主色卡的数量，palette色卡会根据factor进行生成
 * @param cls
 */
void SAColorPaletteGridWidget::setColorList(const QList< QColor >& cls)
{
    d_ptr->mMainColorList->setColumnCount(0);
    d_ptr->mMainColorList->setColorList(cls);
    d_ptr->mPaletteColorGrid->setColumnCount(cls.size());
    d_ptr->mPaletteColorGrid->setColorList(d_ptr->makeColorPalette(cls));
}

/**
 * @brief 获取颜色系列
 * @return
 */
QList< QColor > SAColorPaletteGridWidget::colorList() const
{
    return d_ptr->mMainColorList->getColorList();
}

/**
 * @brief 设置颜色深浅比例factor，factor决定了palette的行数，factor有5个，就有5行
 *
 * 默认为{ 180, 160, 140, 75, 50 },相当于会有5行，每个系数会对标准颜色执行QColor::lighter操作
 * @param factor
 */
void SAColorPaletteGridWidget::setFactor(const QList< int >& factor)
{
    d_ptr->mFactor = factor;
    d_ptr->mPaletteColorGrid->setColorList(d_ptr->makeColorPalette(d_ptr->mMainColorList->getColorList()));
}

/**
 * @brief 获取色卡
 * @return
 */
QList< int > SAColorPaletteGridWidget::factor() const
{
    return d_ptr->mFactor;
}

/**
 * @brief 设置颜色块的尺寸，默认为10*10
 * @param s
 */
void SAColorPaletteGridWidget::setColorIconSize(const QSize& s)
{
    d_ptr->mMainColorList->setColorIconSize(s);
    d_ptr->mPaletteColorGrid->setColorIconSize(s);
}

QSize SAColorPaletteGridWidget::colorIconSize() const
{
    return d_ptr->mMainColorList->colorIconSize();
}

/**
 * @brief 设置颜色块是否能被checked
 * @param on
 */
void SAColorPaletteGridWidget::setColorCheckable(bool on)
{
    d_ptr->mMainColorList->setColorCheckable(on);
    d_ptr->mPaletteColorGrid->setColorCheckable(on);
}

bool SAColorPaletteGridWidget::isColorCheckable() const
{
    return d_ptr->mMainColorList->isColorCheckable();
}

void SAColorPaletteGridWidget::onMainColorClicked(const QColor& c)
{
    d_ptr->mPaletteColorGrid->clearCheckedState();
    emit colorClicked(c);
}

void SAColorPaletteGridWidget::onPaletteColorClicked(const QColor& c)
{
    d_ptr->mMainColorList->clearCheckedState();
    emit colorClicked(c);
}

/*** End of inlined file: SAColorPaletteGridWidget.cpp ***/

/*** Start of inlined file: SAColorToolButton.cpp ***/
#include <QApplication>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOption>
#include <QStyleOptionToolButton>
#include <QResizeEvent>
#include <QDebug>
#define SAColorToolButton_DEBUG_PRINT 0
class SAColorToolButton::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorToolButton)
public:
    PrivateData(SAColorToolButton* p);
    void calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QRect& iconRect);
    QRect getButtonRect(const QStyleOptionToolButton& opt);
    QRect getIndicatorRect(const QStyleOptionToolButton& opt);
    QStyle::State getButtonStyleState(const QStyleOptionToolButton& opt);
    QStyle::State getButtonMenuStyleState(const QStyleOptionToolButton& opt);

public:
    QColor mColor { Qt::white };
    int mSpacing { 2 };                ///< 间隔
    QMargins mMargins { 3, 3, 3, 3 };  ///< box

    static int s_indicatorArrorWidth;  ///< 菜单宽度
};

int SAColorToolButton::PrivateData::s_indicatorArrorWidth = 8;

SAColorToolButton::PrivateData::PrivateData(SAColorToolButton* p) : q_ptr(p)
{
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
    // 确定文本区域
    textRect         = QRect();
    QRect buttonRect = getButtonRect(opt);
    if (opt.icon.isNull()) {
        colorRect = buttonRect;
        iconRect  = QRect();
    } else {
        QSize tmpSize = opt.iconSize;
        if (tmpSize.isNull()) {
            tmpSize = QSize(16, 16);
        }
        tmpSize         = buttonRect.size().boundedTo(tmpSize);
        int colorHeight = tmpSize.height() / 4;
        int totalHeight = colorHeight + tmpSize.height() + mSpacing;
        if (totalHeight > buttonRect.height()) {
            // 过高，这时要通过高度反推tmpSize和colorHeight
            colorHeight = (buttonRect.height() - mSpacing) / 5;
            tmpSize.setHeight(colorHeight * 4);
            if (opt.iconSize.height() > 0) {
                tmpSize.setWidth(tmpSize.height() * opt.iconSize.width() / opt.iconSize.height());  // 等比例
            }
            tmpSize = buttonRect.size().boundedTo(tmpSize);
        }
        // 有icon，颜色位于图标下面
        int y = (buttonRect.height() - colorHeight - mSpacing - tmpSize.height()) / 2;  //(ButtonHeight-TotalHeight)/2
        int x = (buttonRect.width() - tmpSize.width()) / 2;
        iconRect  = QRect(buttonRect.left() + x, buttonRect.top() + y, tmpSize.width(), tmpSize.height());
        colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
    }
}

/**
 * @brief 纯文本情况，左边有5像素显示颜色
 * @param opt
 * @param iconRect
 * @param textRect
 * @param colorRect 左边有5像素显示颜色
 */
void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
    iconRect         = QRect();
    QSize colorSize  = opt.iconSize;
    if (colorSize.isNull()) {
        colorSize = QSize(16, 16);
    }
    colorSize = buttonRect.size().boundedTo(colorSize);
    colorRect = QRect(buttonRect.left(),
                      buttonRect.top() + (buttonRect.height() - colorSize.height()) / 2,
                      colorSize.width(),
                      colorSize.height());
    textRect  = buttonRect.adjusted(colorRect.right() + mSpacing, 0, 0, 0);
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt,
                                                                        QRect& iconRect,
                                                                        QRect& textRect,
                                                                        QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
    if (opt.icon.isNull()) {
        QSize colorSize = opt.iconSize;
        if (colorSize.isNull()) {
            colorSize = QSize(16, 16);
        }
        // 说明没有icon
        // 这时所有都是color
        // iconRect=Null
        colorSize = buttonRect.size().boundedTo(colorSize);
        iconRect  = QRect();
        colorRect = QRect(buttonRect.left(),
                          buttonRect.top() + (buttonRect.height() - colorSize.height()) / 2,
                          colorSize.width(),
                          colorSize.height());
        textRect  = buttonRect.adjusted(colorRect.width() + mSpacing, 0, 0, 0);
    } else {  // 有图标
        QSize tmpSize = opt.iconSize;
        if (tmpSize.isNull()) {
            tmpSize = QSize(16, 16);
        }
        tmpSize         = buttonRect.size().boundedTo(tmpSize);
        int colorHeight = tmpSize.height() / 4;
        int totalHeight = colorHeight + tmpSize.height() + mSpacing;
        if (totalHeight > buttonRect.height()) {
            // 过高，这时要通过高度反推tmpSize和colorHeight
            colorHeight = (buttonRect.height() - mSpacing) / 5;
            tmpSize.setHeight(colorHeight * 4);
            tmpSize.setWidth(tmpSize.height());  // 对于异形也设置为正方行
        }
        // 有icon，颜色位于图标下面
        int y = (buttonRect.height() - colorHeight - mSpacing - tmpSize.height()) / 2;  //(ButtonHeight-TotalHeight)/2
        iconRect  = QRect(buttonRect.left(), buttonRect.top() + y, tmpSize.width(), tmpSize.height());
        colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
        textRect  = buttonRect.adjusted(iconRect.right() + mSpacing, 0, 0, 0);
    }
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt,
                                                                       QRect& iconRect,
                                                                       QRect& textRect,
                                                                       QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
    QSize tmpSize    = opt.iconSize;
    if (tmpSize.isNull()) {
        tmpSize = QSize(16, 16);
    }
    // 获取字体高度
    int textHeight = opt.fontMetrics.height();
    tmpSize        = buttonRect.size().boundedTo(tmpSize);
    if (opt.icon.isNull()) {
        int totalHeight = textHeight + opt.iconSize.height() + mSpacing;
        if (totalHeight < buttonRect.height()) {
            // 足够高
            colorRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                              buttonRect.top() + (buttonRect.height() - totalHeight) / 2,
                              tmpSize.width(),
                              opt.iconSize.height());
        } else {
            // 空间不足
            colorRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                              buttonRect.top() + mSpacing,
                              tmpSize.width(),
                              opt.iconSize.height());
        }
        iconRect = QRect();
        textRect = QRect(buttonRect.left(), colorRect.bottom() + mSpacing, buttonRect.width(), textHeight);

    } else {
        // 有图标
        int colorHeight = tmpSize.height() / 4;
        int totalHeight = textHeight + opt.iconSize.height() + colorHeight + 2 * mSpacing;
        if (totalHeight < buttonRect.height()) {
            // 高度空间足够
            // 先布置icon
            iconRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                             buttonRect.top() + (buttonRect.height() - totalHeight) / 2,
                             tmpSize.width(),
                             opt.iconSize.height());

        } else {
            // 空间不足
            iconRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                             buttonRect.top() + mSpacing,
                             tmpSize.width(),
                             opt.iconSize.height());
        }
        colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
        textRect  = QRect(buttonRect.left(), colorRect.bottom() + mSpacing, buttonRect.width(), textHeight);
    }
}

QPixmap SAColorToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QRect& iconRect)
{
    if (opt.icon.isNull()) {
        return (QPixmap());
    }
    // 有图标
    QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
    QIcon::Mode mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    // return (opt.icon.pixmap(this->window()->windowHandle(), opt.rect.size().boundedTo(realConSize), mode, state));
    return (opt.icon.pixmap(iconRect.size(), mode, state));
}

/**
 * @brief 获取按钮的矩形区域
 * @param opt
 * @return
 */
QRect SAColorToolButton::PrivateData::getButtonRect(const QStyleOptionToolButton& opt)
{
    QRect btnRect = q_ptr->style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton, q_ptr);
    return btnRect.marginsRemoved(mMargins);
}

/**
 * @brief 获取ToolButtonMenu的区域
 * @note 注意，如果不是QStyleOptionToolButton::MenuButtonPopup|QStyleOptionToolButton::HasMenu下调用此函数，返回的是QRect()
 * @param opt
 * @return
 */
QRect SAColorToolButton::PrivateData::getIndicatorRect(const QStyleOptionToolButton& opt)
{
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        return q_ptr->style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, q_ptr);
    }
    return QRect();
}

/**
 * @brief 基于Qt qcommonStyle源码，对QStyleOptionToolButton的style修正
 * @param opt
 * @return
 */
QStyle::State SAColorToolButton::PrivateData::getButtonStyleState(const QStyleOptionToolButton& opt)
{
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
    if (bflags & QStyle::State_AutoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        }
    }
    return bflags;
}

/**
 * @brief 基于Qt qcommonStyle源码，对QStyleOptionToolButton的style修正
 * @param opt
 * @return
 */
QStyle::State SAColorToolButton::PrivateData::getButtonMenuStyleState(const QStyleOptionToolButton& opt)
{
    QStyle::State mflags = opt.state & ~QStyle::State_Sunken;
    if (mflags & QStyle::State_AutoRaise) {
        if (!(mflags & QStyle::State_MouseOver) || !(mflags & QStyle::State_Enabled)) {
            mflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        mflags |= QStyle::State_Sunken;
    }
    return mflags;
}

//==============================================================
// SAColorToolButton
//==============================================================
SAColorToolButton::SAColorToolButton(QWidget* parent) : QToolButton(parent), d_ptr(new PrivateData(this))
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    connect(this, &QToolButton::clicked, this, &SAColorToolButton::onButtonClicked);
}

SAColorToolButton::~SAColorToolButton()
{
}

/**
 * @brief 获取颜色
 * @return
 */
QColor SAColorToolButton::color() const
{
    return d_ptr->mColor;
}

/**
 * @brief 设置Margins
 * @param mg
 */
void SAColorToolButton::setMargins(const QMargins& mg)
{
    d_ptr->mMargins = mg;
    repaint();
}

QMargins SAColorToolButton::margins() const
{
    return d_ptr->mMargins;
}

/**
 * @brief 绘制无颜色表示
 * @param p
 * @param colorRect 绘制的区域
 */
void SAColorToolButton::paintNoneColor(QPainter* p, const QRect& colorRect)
{
    p->save();
    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
    p->setPen(pen);
    p->setRenderHint(QPainter::SmoothPixmapTransform, true);
    p->setRenderHint(QPainter::Antialiasing, true);
    int ss = colorRect.width() / 3;
    p->drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()), QPoint(colorRect.right() - ss, colorRect.top()));
    pen.setColor(Qt::black);
    p->setPen(pen);
    p->drawRect(colorRect);
    p->restore();
}

/**
 * @brief 设置颜色
 * @note 会发射@sa colorChanged 信号
 * @param c
 */
void SAColorToolButton::setColor(const QColor& c)
{
    if (d_ptr->mColor != c) {
        d_ptr->mColor = c;
        repaint();
        emit colorChanged(c);
    }
}

/**
 * @brief 计算各个基本组件的位置
 * @param opt
 * @param iconRect 图标区域
 * @param textRect 文本区域
 * @param colorRect 颜色区域
 */
void SAColorToolButton::calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonTextOnly:
        d_ptr->calcSizeOfToolButtonTextOnly(opt, iconRect, textRect, colorRect);
        break;
    case Qt::ToolButtonTextBesideIcon:
        d_ptr->calcSizeOfToolButtonTextBesideIcon(opt, iconRect, textRect, colorRect);
        break;
    case Qt::ToolButtonTextUnderIcon:
        d_ptr->calcSizeOfToolButtonTextUnderIcon(opt, iconRect, textRect, colorRect);
        break;
    case Qt::ToolButtonFollowStyle:
    case Qt::ToolButtonIconOnly:
        d_ptr->calcSizeOfToolButtonIconOnly(opt, iconRect, textRect, colorRect);
        break;
    default:
        d_ptr->calcSizeOfToolButtonIconOnly(opt, iconRect, textRect, colorRect);
        break;
    }
}

void SAColorToolButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QStylePainter p(this);

    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    QRect iconRect, colorRect, textRect;
    calcRect(opt, iconRect, textRect, colorRect);
    paintButton(&p, opt);
    // 绘制图标
    paintIcon(&p, iconRect, opt);
    // 绘制文字
    paintText(&p, textRect, opt);
    // 绘制颜色
    paintColor(&p, colorRect, d_ptr->mColor, opt);
}

void SAColorToolButton::resizeEvent(QResizeEvent* e)
{
    // 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
}

/**
 * @brief sizeHint重载
 * @return
 */
QSize SAColorToolButton::sizeHint() const
{
    ensurePolished();
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    int w = 0, h = 0;
    if (Qt::ToolButtonIconOnly == opt.toolButtonStyle || Qt::ToolButtonFollowStyle == opt.toolButtonStyle) {
        // 和文本无关
        w = opt.iconSize.width() + d_ptr->mMargins.left() + d_ptr->mMargins.right();
        h = opt.iconSize.height() + d_ptr->mMargins.top() + d_ptr->mMargins.bottom();
    } else if (Qt::ToolButtonTextOnly == opt.toolButtonStyle || Qt::ToolButtonTextBesideIcon == opt.toolButtonStyle) {
        QSize textSize = opt.fontMetrics.size(Qt::TextSingleLine | Qt::TextShowMnemonic, opt.text);
        textSize.setHeight(textSize.height() + 4);
        QSize iconSize = opt.iconSize;
        if (!opt.icon.isNull()) {
            // 有图标，要有iconsize高度的1/4给颜色
            iconSize.setHeight(iconSize.height() + iconSize.height() / 4 + d_ptr->mSpacing);
        }
        w = textSize.width() + d_ptr->mSpacing + iconSize.width() + d_ptr->mMargins.left() + d_ptr->mMargins.right();
        h = qMax(textSize.height(), iconSize.height()) + d_ptr->mMargins.top() + d_ptr->mMargins.bottom();
    } else if (Qt::ToolButtonTextUnderIcon == opt.toolButtonStyle) {
        QSize textSize = opt.fontMetrics.size(Qt::TextSingleLine | Qt::TextShowMnemonic, opt.text);
        textSize.setHeight(textSize.height() + 4);
        QSize iconSize = opt.iconSize;
        if (!opt.icon.isNull()) {
            // 有图标，要有iconsize高度的1/4给颜色
            iconSize.setHeight(iconSize.height() + iconSize.height() / 4 + d_ptr->mSpacing);
        }
        w = qMax(textSize.width(), iconSize.width()) + d_ptr->mMargins.left() + d_ptr->mMargins.right();
        h = textSize.height() + iconSize.height() + d_ptr->mSpacing + d_ptr->mMargins.top() + d_ptr->mMargins.bottom();
    }
    opt.rect.setSize(QSize(w, h));  // PM_MenuButtonIndicator depends on the height
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
    }
    //! Qt6.4 取消了QApplication::globalStrut
    return style()->sizeFromContents(QStyle::CT_ToolButton, &opt, QSize(w, h), this).expandedTo(QSize(2, 2));
}

void SAColorToolButton::onButtonClicked(bool checked)
{
    emit colorClicked(color(), checked);
}

/**
 * @brief 绘制按钮
 * @param p
 * @param opt
 */
void SAColorToolButton::paintButton(QStylePainter* p, const QStyleOptionToolButton& opt)
{
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    // 绘制按钮
    if (autoRaise) {
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, p, this);
    } else {
        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, p, this);
    }
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
        QStyleOption tool = opt;
        tool.state        = d_ptr->getButtonMenuStyleState(opt);
        tool.rect         = d_ptr->getIndicatorRect(opt);
        //        if (tool.state & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised)) {
        //            style()->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, &tool, &p, this);
        //        }
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, p, this);
    }

    // 绘制focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(3, 3, -3, -3);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this), 0);
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, p, this);
    }
}

/**
 * @brief 绘制icon
 * @param p
 * @param iconRect
 * @param opt icon信息从QStyleOptionToolButton获取
 */
void SAColorToolButton::paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt)
{
    if (!iconRect.isNull()) {
        QPixmap pm = d_ptr->createIconPixmap(opt, iconRect);
        style()->drawItemPixmap(p, iconRect, Qt::AlignCenter, pm);
    }
}

/**
 * @brief 绘制文本
 * @param p
 * @param textRect
 * @param opt
 */
void SAColorToolButton::paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt)
{
    if (opt.text.isEmpty()) {
        return;
    }
    if (Qt::ToolButtonIconOnly == opt.toolButtonStyle) {
        return;
    }
    p->save();
    p->setFont(opt.font);
    int alignment = Qt::TextShowMnemonic;
    // 快捷键的下划线
    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
        alignment |= Qt::TextHideMnemonic;
    }
    alignment |= Qt::AlignHCenter | Qt::AlignVCenter;
    style()->drawItemText(p,
                          QStyle::visualRect(opt.direction, opt.rect, textRect),
                          alignment,
                          opt.palette,
                          opt.state & QStyle::State_Enabled,
                          opt.text,
                          QPalette::ButtonText);
    p->restore();
}

/**
 * @brief 绘制color
 * @param p
 * @param iconRect
 * @param opt
 */
void SAColorToolButton::paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt)
{
    Q_UNUSED(opt);
    if (colorRect.isNull()) {
        return;
    }
    // 绘制颜色
    if (color.isValid()) {
        p->fillRect(colorRect, color);
    } else {
        paintNoneColor(p, colorRect);
    }
}

/*** End of inlined file: SAColorToolButton.cpp ***/

// sa ribbon

/*** Start of inlined file: SAFramelessHelper.cpp ***/
#include <QRect>
#include <QRubberBand>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QApplication>
#include <QDebug>

class SAPrivateFramelessWidgetData;

/*****
 * FramelessHelperPrivate
 * 存储界面对应的数据集合，以及是否可移动、可缩放属性
 *****/
class SAFramelessHelper::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SAFramelessHelper)
    friend class SAPrivateFramelessWidgetData;

public:
    PrivateData(SAFramelessHelper* p);
    QHash< QWidget*, SAPrivateFramelessWidgetData* > m_widgetDataHash;
    bool m_bWidgetMovable { true };
    bool m_bWidgetResizable { true };
    bool m_bRubberBandOnResize { true };
    bool m_bRubberBandOnMove { true };
};

SAFramelessHelper::PrivateData::PrivateData(SAFramelessHelper* p) : q_ptr(p)
{
}

/*****
 * CursorPosCalculator
 * 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
 *****/
class SAPrivateFramelessCursorPosCalculator
{
public:
    explicit SAPrivateFramelessCursorPosCalculator();
    void reset();
    void recalculate(const QPoint& globalMousePos, const QRect& frameRect);

public:
    bool m_bOnEdges { true };
    bool m_bOnLeftEdge { true };
    bool m_bOnRightEdge { true };
    bool m_bOnTopEdge { true };
    bool m_bOnBottomEdge { true };
    bool m_bOnTopLeftEdge { true };
    bool m_bOnBottomLeftEdge { true };
    bool m_bOnTopRightEdge { true };
    bool m_bOnBottomRightEdge { true };

    static int m_nBorderWidth;
    static int m_nTitleHeight;
};

int SAPrivateFramelessCursorPosCalculator::m_nBorderWidth = 5;
int SAPrivateFramelessCursorPosCalculator::m_nTitleHeight = 30;

/***** CursorPosCalculator *****/
SAPrivateFramelessCursorPosCalculator::SAPrivateFramelessCursorPosCalculator()
{
    reset();
}

void SAPrivateFramelessCursorPosCalculator::reset()
{
    m_bOnEdges           = false;
    m_bOnLeftEdge        = false;
    m_bOnRightEdge       = false;
    m_bOnTopEdge         = false;
    m_bOnBottomEdge      = false;
    m_bOnTopLeftEdge     = false;
    m_bOnBottomLeftEdge  = false;
    m_bOnTopRightEdge    = false;
    m_bOnBottomRightEdge = false;
}

void SAPrivateFramelessCursorPosCalculator::recalculate(const QPoint& gMousePos, const QRect& frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();

    int frameWidth  = frameRect.width();
    int frameHeight = frameRect.height();

    m_bOnLeftEdge = (globalMouseX >= frameX && globalMouseX <= frameX + m_nBorderWidth);

    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth && globalMouseX <= frameX + frameWidth);

    m_bOnTopEdge = (globalMouseY >= frameY && globalMouseY <= frameY + m_nBorderWidth);

    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth && globalMouseY <= frameY + frameHeight);

    m_bOnTopLeftEdge     = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge  = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge    = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}

/*****
 * WidgetData
 * 更新鼠标样式、移动窗体、缩放窗体
 *****/
class SAPrivateFramelessWidgetData
{
public:
    explicit SAPrivateFramelessWidgetData(SAFramelessHelper::PrivateData* pd, QWidget* pTopLevelWidget);
    ~SAPrivateFramelessWidgetData();
    QWidget* widget();

    // 处理鼠标事件-划过、按下、释放、移动
    bool handleWidgetEvent(QEvent* event);

    // 更新橡皮筋状态
    void updateRubberBandStatus();

private:
    // 更新鼠标样式
    void updateCursorShape(const QPoint& gMousePos);

    // 重置窗体大小
    void resizeWidget(const QPoint& gMousePos);

    // 移动窗体
    void moveWidget(const QPoint& gMousePos);

    // 处理鼠标按下
    bool handleMousePressEvent(QMouseEvent* event);

    // 处理鼠标释放
    bool handleMouseReleaseEvent(QMouseEvent* event);

    // 处理鼠标移动
    bool handleMouseMoveEvent(QMouseEvent* event);

    // 处理鼠标离开
    bool handleLeaveEvent(QEvent* event);

    // 处理鼠标进入
    bool handleHoverMoveEvent(QHoverEvent* event);

    // 处理鼠标双击事件
    bool handleDoubleClickedMouseEvent(QMouseEvent* event);

private:
    SAFramelessHelper::PrivateData* d;
    QRubberBand* m_pRubberBand;
    QWidget* m_pWidget;
    QPoint m_ptDragPos;
    SAPrivateFramelessCursorPosCalculator m_pressedMousePos;
    SAPrivateFramelessCursorPosCalculator m_moveMousePos;
    bool m_bLeftButtonPressed;
    bool m_bCursorShapeChanged;
    bool m_bLeftButtonTitlePressed;
    Qt::WindowFlags m_windowFlags;
};

/***** WidgetData *****/
SAPrivateFramelessWidgetData::SAPrivateFramelessWidgetData(SAFramelessHelper::PrivateData* pd, QWidget* pTopLevelWidget)
{
    d                         = pd;
    m_pWidget                 = pTopLevelWidget;
    m_bLeftButtonPressed      = false;
    m_bCursorShapeChanged     = false;
    m_bLeftButtonTitlePressed = false;
    m_pRubberBand             = NULL;

    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover, true);

    updateRubberBandStatus();
}

SAPrivateFramelessWidgetData::~SAPrivateFramelessWidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    m_pWidget->setAttribute(Qt::WA_Hover, false);

    delete m_pRubberBand;
    m_pRubberBand = NULL;
}

QWidget* SAPrivateFramelessWidgetData::widget()
{
    return (m_pWidget);
}

bool SAPrivateFramelessWidgetData::handleWidgetEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        return (handleMousePressEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::MouseButtonRelease:
        return (handleMouseReleaseEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::MouseMove:
        return (handleMouseMoveEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::Leave:
        return (handleLeaveEvent(static_cast< QMouseEvent* >(event)));

    case QEvent::HoverMove:
        return (handleHoverMoveEvent(static_cast< QHoverEvent* >(event)));

    case QEvent::MouseButtonDblClick:
        return (handleDoubleClickedMouseEvent(static_cast< QMouseEvent* >(event)));

    default:
        break;
    }
    return (false);
}

void SAPrivateFramelessWidgetData::updateRubberBandStatus()
{
    if (d->m_bRubberBandOnMove || d->m_bRubberBandOnResize) {
        if (NULL == m_pRubberBand) {
            m_pRubberBand = new QRubberBand(QRubberBand::Rectangle);
        }
    } else {
        delete m_pRubberBand;
        m_pRubberBand = NULL;
    }
}

void SAPrivateFramelessWidgetData::updateCursorShape(const QPoint& gMousePos)
{
    if (m_pWidget->isFullScreen() || m_pWidget->isMaximized()) {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor();
        }
        return;
    }

    m_moveMousePos.recalculate(gMousePos, m_pWidget->frameGeometry());

    if (m_moveMousePos.m_bOnTopLeftEdge || m_moveMousePos.m_bOnBottomRightEdge) {
        m_pWidget->setCursor(Qt::SizeFDiagCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos.m_bOnTopRightEdge || m_moveMousePos.m_bOnBottomLeftEdge) {
        m_pWidget->setCursor(Qt::SizeBDiagCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos.m_bOnLeftEdge || m_moveMousePos.m_bOnRightEdge) {
        m_pWidget->setCursor(Qt::SizeHorCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos.m_bOnTopEdge || m_moveMousePos.m_bOnBottomEdge) {
        m_pWidget->setCursor(Qt::SizeVerCursor);
        m_bCursorShapeChanged = true;
    } else {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void SAPrivateFramelessWidgetData::resizeWidget(const QPoint& gMousePos)
{
    QRect origRect;

    if (d->m_bRubberBandOnResize) {
        origRect = m_pRubberBand->frameGeometry();
    } else {
        origRect = m_pWidget->frameGeometry();
    }

    int left   = origRect.left();
    int top    = origRect.top();
    int right  = origRect.right();
    int bottom = origRect.bottom();

    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth  = m_pWidget->minimumWidth();
    int minHeight = m_pWidget->minimumHeight();

    if (m_pressedMousePos.m_bOnTopLeftEdge) {
        left = gMousePos.x();
        top  = gMousePos.y();
    } else if (m_pressedMousePos.m_bOnBottomLeftEdge) {
        left   = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressedMousePos.m_bOnTopRightEdge) {
        right = gMousePos.x();
        top   = gMousePos.y();
    } else if (m_pressedMousePos.m_bOnBottomRightEdge) {
        right  = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressedMousePos.m_bOnLeftEdge) {
        left = gMousePos.x();
    } else if (m_pressedMousePos.m_bOnRightEdge) {
        right = gMousePos.x();
    } else if (m_pressedMousePos.m_bOnTopEdge) {
        top = gMousePos.y();
    } else if (m_pressedMousePos.m_bOnBottomEdge) {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid()) {
        if (minWidth > newRect.width()) {
            if (left != origRect.left()) {
                newRect.setLeft(origRect.left());
            } else {
                newRect.setRight(origRect.right());
            }
        }
        if (minHeight > newRect.height()) {
            if (top != origRect.top()) {
                newRect.setTop(origRect.top());
            } else {
                newRect.setBottom(origRect.bottom());
            }
        }

        if (d->m_bRubberBandOnResize) {
            m_pRubberBand->setGeometry(newRect);
        } else {
            m_pWidget->setGeometry(newRect);
        }
    }
}

void SAPrivateFramelessWidgetData::moveWidget(const QPoint& gMousePos)
{
    if (d->m_bRubberBandOnMove) {
        m_pRubberBand->move(gMousePos - m_ptDragPos);
    } else {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}

bool SAPrivateFramelessWidgetData::handleMousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed      = true;
        m_bLeftButtonTitlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;

        QRect frameRect = m_pWidget->frameGeometry();
        auto gp         = SA_MOUSEEVENT_GLOBALPOS_POINT(event);
        m_pressedMousePos.recalculate(gp, frameRect);

        m_ptDragPos = gp - frameRect.topLeft();

        if (m_pressedMousePos.m_bOnEdges) {
            if (m_pWidget->isMaximized()) {
                // 窗口在最大化状态时，点击边界不做任何处理
                return (false);
            }
            if (d->m_bRubberBandOnResize) {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
                return (true);
            }
        } else if (d->m_bRubberBandOnMove && m_bLeftButtonTitlePressed) {
            if (m_pWidget->isMaximized()) {
                // 窗口在最大化状态时，点击标题栏不做任何处理
                return (false);
            }
            m_pRubberBand->setGeometry(frameRect);
            m_pRubberBand->show();
            return (true);
        }
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed      = false;
        m_bLeftButtonTitlePressed = false;
        m_pressedMousePos.reset();
        if (m_pRubberBand && m_pRubberBand->isVisible()) {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->geometry());
            return (true);
        }
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleMouseMoveEvent(QMouseEvent* event)
{
    QPoint p = SA_MOUSEEVENT_GLOBALPOS_POINT(event);
    if (m_bLeftButtonPressed) {
        if (d->m_bWidgetResizable && m_pressedMousePos.m_bOnEdges) {
            if (m_pWidget->isMaximized()) {
                // 窗口在最大化状态时，点击边界不做任何处理
                return (false);
            }
            resizeWidget(p);
            return (true);
        } else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed) {
            if (m_pWidget->isMaximized()) {
                // 先求出窗口到鼠标的相对位置
                QRect normalGeometry = m_pWidget->normalGeometry();
                m_pWidget->showNormal();
                p.ry() -= 10;
                p.rx() -= (normalGeometry.width() / 2);
                m_pWidget->move(p);
                // 这时要重置m_ptDragPos
                m_ptDragPos = QPoint(normalGeometry.width() / 2, 10);
                return (true);
            }
            moveWidget(p);
            return (true);
        }
        return (false);
    } else if (d->m_bWidgetResizable) {
        updateCursorShape(p);
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleLeaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed) {
        m_pWidget->unsetCursor();
        return (true);
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleHoverMoveEvent(QHoverEvent* event)
{
    if (d->m_bWidgetResizable) {
        updateCursorShape(m_pWidget->mapToGlobal(SA_HOVEREVENT_POS_POINT(event)));
    }
    return (false);
}

bool SAPrivateFramelessWidgetData::handleDoubleClickedMouseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_pWidget) {
            SARibbonMainWindow* mainwindow = qobject_cast< SARibbonMainWindow* >(m_pWidget);
            if (mainwindow) {
                if (mainwindow->windowFlags() & Qt::WindowMaximizeButtonHint) {
                    // 在最大化按钮显示时才进行shownormal处理
                    bool titlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;
                    if (titlePressed) {
                        if (m_pWidget->isMaximized()) {
                            m_pWidget->showNormal();
                        } else {
                            m_pWidget->showMaximized();
                        }
                        return (true);
                    }
                }
            }
        }
    }
    return (false);
}

//===================================================
// SAFramelessHelper
//===================================================
SAFramelessHelper::SAFramelessHelper(QObject* parent) : QObject(parent), d_ptr(new SAFramelessHelper::PrivateData(this))
{
    d_ptr->m_bWidgetMovable      = true;
    d_ptr->m_bWidgetResizable    = true;
    d_ptr->m_bRubberBandOnResize = false;
    d_ptr->m_bRubberBandOnMove   = false;
    if (parent) {
        QWidget* w = qobject_cast< QWidget* >(parent);
        if (w) {
            w->setWindowFlags(w->windowFlags() | Qt::FramelessWindowHint);
            setWidgetMovable(true);       // 设置窗体可移动
            setWidgetResizable(true);     // 设置窗体可缩放
            setRubberBandOnMove(false);   // 设置橡皮筋效果-可移动
            setRubberBandOnResize(true);  // 设置橡皮筋效果-可缩放
            activateOn(w);                // 激活当前窗体
        }
    }
}

SAFramelessHelper::~SAFramelessHelper()
{
    QList< QWidget* > keys = d_ptr->m_widgetDataHash.keys();
    int size               = keys.size();

    for (int i = 0; i < size; ++i) {
        delete d_ptr->m_widgetDataHash.take(keys[ i ]);
    }
}

bool SAFramelessHelper::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Leave: {
        SAPrivateFramelessWidgetData* data = d_ptr->m_widgetDataHash.value(static_cast< QWidget* >(obj));
        if (data) {
            return (data->handleWidgetEvent(event));
        }
        break;
    }

    default:
        break;
    }
    return (QObject::eventFilter(obj, event));
}

void SAFramelessHelper::activateOn(QWidget* topLevelWidget)
{
    if (!d_ptr->m_widgetDataHash.contains(topLevelWidget)) {
        SAPrivateFramelessWidgetData* data = new SAPrivateFramelessWidgetData(d_ptr.get(), topLevelWidget);
        d_ptr->m_widgetDataHash.insert(topLevelWidget, data);

        topLevelWidget->installEventFilter(this);
    }
}

void SAFramelessHelper::removeFrom(QWidget* topLevelWidget)
{
    SAPrivateFramelessWidgetData* data = d_ptr->m_widgetDataHash.take(topLevelWidget);

    if (data) {
        topLevelWidget->removeEventFilter(this);
        delete data;
    }
}

void SAFramelessHelper::setRubberBandOnMove(bool movable)
{
    d_ptr->m_bRubberBandOnMove                  = movable;
    QList< SAPrivateFramelessWidgetData* > list = d_ptr->m_widgetDataHash.values();

    foreach (SAPrivateFramelessWidgetData* data, list) {
        data->updateRubberBandStatus();
    }
}

void SAFramelessHelper::setWidgetMovable(bool movable)
{
    d_ptr->m_bWidgetMovable = movable;
}

void SAFramelessHelper::setWidgetResizable(bool resizable)
{
    d_ptr->m_bWidgetResizable = resizable;
}

void SAFramelessHelper::setRubberBandOnResize(bool resizable)
{
    d_ptr->m_bRubberBandOnResize                = resizable;
    QList< SAPrivateFramelessWidgetData* > list = d_ptr->m_widgetDataHash.values();

    foreach (SAPrivateFramelessWidgetData* data, list) {
        data->updateRubberBandStatus();
    }
}

void SAFramelessHelper::setBorderWidth(int width)
{
    if (width > 0) {
        SAPrivateFramelessCursorPosCalculator::m_nBorderWidth = width;
    }
}

void SAFramelessHelper::setTitleHeight(int height)
{
    if (height > 0) {
        SAPrivateFramelessCursorPosCalculator::m_nTitleHeight = height;
    }
}

bool SAFramelessHelper::widgetMovable()
{
    return (d_ptr->m_bWidgetMovable);
}

bool SAFramelessHelper::widgetResizable()
{
    return (d_ptr->m_bWidgetResizable);
}

bool SAFramelessHelper::rubberBandOnMove()
{
    return (d_ptr->m_bRubberBandOnMove);
}

bool SAFramelessHelper::rubberBandOnResisze()
{
    return (d_ptr->m_bRubberBandOnResize);
}

uint SAFramelessHelper::borderWidth()
{
    return (SAPrivateFramelessCursorPosCalculator::m_nBorderWidth);
}

uint SAFramelessHelper::titleHeight()
{
    return (SAPrivateFramelessCursorPosCalculator::m_nTitleHeight);
}

/*** End of inlined file: SAFramelessHelper.cpp ***/

/*** Start of inlined file: SARibbonApplicationButton.cpp ***/
SARibbonApplicationButton::SARibbonApplicationButton(QWidget* parent) : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

SARibbonApplicationButton::SARibbonApplicationButton(const QString& text, QWidget* parent) : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setText(text);
}

SARibbonApplicationButton::SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QToolButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setIcon(icon);
    setText(text);
}

/*** End of inlined file: SARibbonApplicationButton.cpp ***/

/*** Start of inlined file: SARibbonSystemButtonBar.cpp ***/
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>
#include <QScopedPointer>

// 为了避免使用此框架的app设置了全局的qpushbutton 的 qss样式影响此按钮，定义了一个类

/**
 * @brief The SARibbonSystemButtonBar class
 */
class SARibbonSystemButtonBar::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonSystemButtonBar)
public:
    SARibbonSystemToolButton* buttonClose { nullptr };
    SARibbonSystemToolButton* buttonMinimize { nullptr };
    SARibbonSystemToolButton* buttonMaximize { nullptr };
    int mCloseStretch { 4 };
    int mMaxStretch { 3 };
    int mMinStretch { 3 };
    int mWindowButtonWidth { 35 };
    int mTitleBarHeight { 28 };
    Qt::WindowFlags mFlags { Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint };
    SARibbonButtonGroupWidget* mButtonGroup;

public:
    PrivateData(SARibbonSystemButtonBar* p) : q_ptr(p)
    {
        mButtonGroup = new SARibbonButtonGroupWidget(p);
        mButtonGroup->setObjectName("SASystemButtonGroup");
    }

    void setupMinimizeButton(bool on)
    {
        SARibbonSystemButtonBar* par = q_ptr;

        if (on) {
            if (buttonMinimize) {
                buttonMinimize->deleteLater();
                buttonMinimize = nullptr;
            }
            buttonMinimize = new SARibbonSystemToolButton(par);
            buttonMinimize->setObjectName(QStringLiteral("SAMinimizeWindowButton"));
            buttonMinimize->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            buttonMinimize->show();
            par->connect(buttonMinimize, &QAbstractButton::clicked, par, &SARibbonSystemButtonBar::minimizeWindow);
        } else {
            if (buttonMinimize) {
                delete buttonMinimize;
                buttonMinimize = nullptr;
            }
        }
        updateSize();
    }

    void setupMaximizeButton(bool on)
    {
        SARibbonSystemButtonBar* par = q_ptr;

        if (on) {
            if (buttonMaximize) {
                buttonMaximize->deleteLater();
                buttonMaximize = nullptr;
            }
            buttonMaximize = new SARibbonSystemToolButton(par);
            buttonMaximize->setObjectName(QStringLiteral("SAMaximizeWindowButton"));
            buttonMaximize->setCheckable(true);
            buttonMaximize->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            //            buttonMaximize->setIconSize(buttonMaximize->size() * mIconscale);
            buttonMaximize->show();
            par->connect(buttonMaximize, &QAbstractButton::clicked, par, &SARibbonSystemButtonBar::maximizeWindow);
        } else {
            if (buttonMaximize) {
                delete buttonMaximize;
                buttonMaximize = nullptr;
            }
        }
        updateSize();
    }

    void setupCloseButton(bool on)
    {
        SARibbonSystemButtonBar* par = q_ptr;

        if (on) {
            if (buttonClose) {
                buttonClose->deleteLater();
                buttonClose = nullptr;
            }
            buttonClose = new SARibbonSystemToolButton(par);
            buttonClose->setObjectName(QStringLiteral("SACloseWindowButton"));
            buttonClose->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            // buttonClose->setFlat(true);
            par->connect(buttonClose, &QAbstractButton::clicked, par, &SARibbonSystemButtonBar::closeWindow);
            //            buttonClose->setIconSize(buttonClose->size() * mIconscale);
            buttonClose->show();
        } else {
            if (buttonClose) {
                delete buttonClose;
                buttonClose = nullptr;
            }
        }
        updateSize();
    }

    void updateSize()
    {
        resizeElement(q_ptr->size());
    }

    void resizeElement(QSize size)
    {
        int x = size.width();
        if (buttonClose) {
            int w = closeButtonWidthHint();
            x -= w;
            buttonClose->setGeometry(x, 0, w, size.height());
        }
        if (buttonMaximize) {
            int w = maxButtonWidthHint();
            x -= w;
            buttonMaximize->setGeometry(x, 0, w, size.height());
        }
        if (buttonMinimize) {
            int w = minButtonWidthHint();
            x -= w;
            buttonMinimize->setGeometry(x, 0, w, size.height());
        }
        if (mButtonGroup) {
            mButtonGroup->setGeometry(0, 0, x, size.height());
        }
    }

    int closeButtonWidthHint() const
    {
        qreal t = mCloseStretch + mMaxStretch + mMinStretch;
        return (mCloseStretch * (3 * mWindowButtonWidth)) / t;
    }

    int maxButtonWidthHint() const
    {
        qreal t = mCloseStretch + mMaxStretch + mMinStretch;
        return (mMaxStretch * (3 * mWindowButtonWidth)) / t;
    }

    int minButtonWidthHint() const
    {
        qreal t = mCloseStretch + mMaxStretch + mMinStretch;
        return (mMinStretch * (3 * mWindowButtonWidth)) / t;
    }

    QSize sizeHint() const
    {
        int height = mTitleBarHeight;
        if (height < 20) {
            height = 20;
        }
        QSize res(0, 0);
        if (mButtonGroup) {
            res = mButtonGroup->sizeHint();
        }
        res.setHeight(height);
        if (buttonClose) {
            res.setWidth(res.width() + closeButtonWidthHint());
        }
        if (buttonMaximize) {
            res.setWidth(res.width() + maxButtonWidthHint());
        }
        if (buttonMinimize) {
            res.setWidth(res.width() + minButtonWidthHint());
        }
        return res;
    }
};

//===================================================
// SARibbonSystemToolButton
//===================================================
SARibbonSystemToolButton::SARibbonSystemToolButton(QWidget* p) : QToolButton(p)
{
    setAutoRaise(true);
}
//===================================================
// SARibbonSystemButtonBar
//===================================================
SARibbonSystemButtonBar::SARibbonSystemButtonBar(QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonSystemButtonBar::PrivateData(this))
{
    updateWindowFlag();
}

/**
 * @brief 构造函数，强制使用flags，而不是用parent的flags进行构造
 * @param parent
 * @param flags
 */
SARibbonSystemButtonBar::SARibbonSystemButtonBar(QWidget* parent, Qt::WindowFlags flags)
    : QFrame(parent), d_ptr(new SARibbonSystemButtonBar::PrivateData(this))
{
    d_ptr->mFlags = flags;
    updateWindowFlag();
}

SARibbonSystemButtonBar::~SARibbonSystemButtonBar()
{
}

void SARibbonSystemButtonBar::setupMinimizeButton(bool on)
{
    d_ptr->setupMinimizeButton(on);
}

void SARibbonSystemButtonBar::setupMaximizeButton(bool on)
{
    d_ptr->setupMaximizeButton(on);
}

void SARibbonSystemButtonBar::setupCloseButton(bool on)
{
    d_ptr->setupCloseButton(on);
}

void SARibbonSystemButtonBar::updateWindowFlag()
{
    QWidget* topedWidget = this;
    // 找到最顶层窗口
    while (topedWidget->parentWidget()) {
        topedWidget = topedWidget->parentWidget();
    }
    Qt::WindowFlags flags = topedWidget->windowFlags();
    updateWindowFlag(flags);
}

/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 * @param flags
 */
void SARibbonSystemButtonBar::updateWindowFlag(Qt::WindowFlags flags)
{
    d_ptr->mFlags = flags;
    setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);
    setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);
    setupCloseButton(flags & Qt::WindowCloseButtonHint);
}

/**
 * @brief 设置按钮的宽度比例,最终按钮宽度将按照此比例进行设置
 * @param close 关闭按钮比例
 * @param max 最大化按钮比例
 * @param min 最小化按钮比例
 */
void SARibbonSystemButtonBar::setButtonWidthStretch(int close, int max, int min)
{
    d_ptr->mMaxStretch   = max;
    d_ptr->mMinStretch   = min;
    d_ptr->mCloseStretch = close;
}

/**
 * @brief 标题栏高度
 *
 * 标题栏高度会影响sizeHint
 * @param h
 */
void SARibbonSystemButtonBar::setWindowTitleHeight(int h)
{
    d_ptr->mTitleBarHeight = h;
}

/**
 * @brief 标题栏高度
 * @return
 */
int SARibbonSystemButtonBar::windowTitleHeight() const
{
    return d_ptr->mTitleBarHeight;
}

/**
 * @brief 系统按钮的宽度
 * @param w
 */
void SARibbonSystemButtonBar::setWindowButtonWidth(int w)
{
    d_ptr->mWindowButtonWidth = w;
}

/**
 * @brief 系统按钮的宽度
 * @param w
 */
int SARibbonSystemButtonBar::windowButtonWidth() const
{
    return d_ptr->mWindowButtonWidth;
}

/**
 * @brief 设置窗口状态（最大最小化按钮状态）
 * @param s
 */
void SARibbonSystemButtonBar::setWindowStates(Qt::WindowStates s)
{
    if (d_ptr->buttonMaximize) {
        bool on = s.testFlag(Qt::WindowMaximized);
        d_ptr->buttonMaximize->setChecked(on);
        d_ptr->buttonMaximize->setToolTip(on ? tr("Restore") : tr("Maximize"));
    }
}

/**
 * @brief 此函数返回的flags仅包括 Qt::WindowCloseButtonHint，Qt::WindowMaximizeButtonHint，Qt::WindowMinimizeButtonHint
 * 三个
 *
 * @return
 */
Qt::WindowFlags SARibbonSystemButtonBar::windowButtonFlags() const
{
    Qt::WindowFlags f = Qt::Widget;  // widget是000

    if (d_ptr->mFlags & Qt::WindowCloseButtonHint) {
        f |= Qt::WindowCloseButtonHint;
    }
    if (d_ptr->mFlags & Qt::WindowMaximizeButtonHint) {
        f |= Qt::WindowMaximizeButtonHint;
    }
    if (d_ptr->mFlags & Qt::WindowMinimizeButtonHint) {
        f |= Qt::WindowMinimizeButtonHint;
    }

    return (f);
}

QSize SARibbonSystemButtonBar::sizeHint() const
{
    return (d_ptr->sizeHint());
}

QAbstractButton* SARibbonSystemButtonBar::minimizeButton() const
{
    return d_ptr->buttonMinimize;
}

QAbstractButton* SARibbonSystemButtonBar::maximizeButton() const
{
    return d_ptr->buttonMaximize;
}

QAbstractButton* SARibbonSystemButtonBar::closeButton() const
{
    return d_ptr->buttonClose;
}

void SARibbonSystemButtonBar::setIconSize(const QSize& ic)
{
    d_ptr->mButtonGroup->setIconSize(ic);
}

QSize SARibbonSystemButtonBar::iconSize() const
{
    return d_ptr->mButtonGroup->iconSize();
}

QAction* SARibbonSystemButtonBar::addAction(QAction* a, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    return d_ptr->mButtonGroup->addAction(a, buttonStyle, popMode);
}

QAction* SARibbonSystemButtonBar::addAction(const QString& text, const QIcon& icon, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    return d_ptr->mButtonGroup->addAction(text, icon, buttonStyle, popMode);
}

QAction* SARibbonSystemButtonBar::addMenu(QMenu* menu, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    return d_ptr->mButtonGroup->addMenu(menu, buttonStyle, popMode);
}

QAction* SARibbonSystemButtonBar::addSeparator()
{
    return d_ptr->mButtonGroup->addSeparator();
}

QAction* SARibbonSystemButtonBar::addWidget(QWidget* w)
{
    return d_ptr->mButtonGroup->addWidget(w);
}

void SARibbonSystemButtonBar::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    d_ptr->resizeElement(size());
}

void SARibbonSystemButtonBar::closeWindow()
{
    if (parentWidget()) {
        parentWidget()->close();
    }
}

void SARibbonSystemButtonBar::minimizeWindow()
{
    if (parentWidget()) {
        parentWidget()->showMinimized();
    }
}

void SARibbonSystemButtonBar::maximizeWindow()
{
    QWidget* par = parentWidget();

    if (par) {
        if (par->isMaximized()) {
            par->showNormal();
        } else {
            par->showMaximized();
        }
    }
}

/*** End of inlined file: SARibbonSystemButtonBar.cpp ***/

/*** Start of inlined file: SARibbonToolButton.cpp ***/
#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QStyleOptionFocusRect>
#include <QStyleOptionToolButton>
#include <QStylePainter>
#include <QTextOption>
#include <QApplication>
#include <QScreen>
#include <QProxyStyle>

/**
 * @def 定义文字换行时2行文本的矩形高度系数，此系数决定文字区域的高度
 *
 * fontMetrics.lineSpacing*系数 = 文本区域高度
 */
#define SARIBBONTOOLBUTTON_WORDWRAP_TEXT_FACTOR 2.05

/**
 * @def 定义文字不换行时单行文本的矩形高度系数，此系数决定文字区域的高度
 *
 * fontMetrics.lineSpacing*系数 = 文本区域高度
 */
#define SARIBBONTOOLBUTTON_NOWORDWRAP_TEXT_FACTOR 1.2

/**
 * @def 定义小按钮的矩形高度系数，此系数决定文字区域的高度
 *
 * fontMetrics.lineSpacing*系数 = 文本区域高度
 */
#define SARIBBONTOOLBUTTON_SMALLBUTTON_TEXT_FACTOR 1.4

/**
 * @def 文本宽度估算时的宽度比高度系数
 *
 * 超过此系数的宽度时，开始尝试换行，例如按钮高度为h，如果单行文本的宽度大于h*系数，则触发换行估算
 */
#define SARIBBONTOOLBUTTON_WORDWRAP_WIDTH_PER_HEIGHT_RATIO 1.4

/**
 * @def 开启此宏会打印一些常见信息
 */
#define SA_RIBBON_TOOLBUTTON_DEBUG_PRINT 0

#define SARIBBONTOOLBUTTON_DEBUG_DRAW 0

#if SARIBBONTOOLBUTTON_DEBUG_DRAW
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)                                                                    \
    do {                                                                                                               \
        p.save();                                                                                                      \
        p.setPen(Qt::red);                                                                                             \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

#else
#define SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, rect)
#endif
namespace SA
{

QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt)
{
    debug << "=============="
          << "\nQStyleOption(" << (QStyleOption)opt << ")"
          << "\n  QStyleOptionComplex:"
             "\n     subControls("
          << opt.subControls
          << " ) "
             "\n     activeSubControls("
          << opt.activeSubControls
          << "\n  QStyleOptionToolButton"
             "\n     features("
          << opt.features
          << ")"
             "\n     toolButtonStyle("
          << opt.toolButtonStyle << ")";

    return (debug);
}
}

//===================================================
// SARibbonToolButtonProxyStyle
//===================================================

class SARibbonToolButtonProxyStyle : public QProxyStyle
{
public:
    void drawPrimitive(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* widget = nullptr) const override
    {
        if (pe == PE_IndicatorArrowUp || pe == PE_IndicatorArrowDown || pe == PE_IndicatorArrowRight || pe == PE_IndicatorArrowLeft) {
            if (opt->rect.width() <= 1 || opt->rect.height() <= 1)
                return;

            QRect r  = opt->rect;
            int size = qMin(r.height(), r.width());
            QPixmap pixmap;
            qreal pixelRatio = p->device()->devicePixelRatio();
            int border       = qRound(pixelRatio * (size / 4));
            int sqsize       = qRound(pixelRatio * (2 * (size / 2)));
            QImage image(sqsize, sqsize, QImage::Format_ARGB32_Premultiplied);
            image.fill(Qt::transparent);
            QPainter imagePainter(&image);

            QPolygon a;
            switch (pe) {
            case PE_IndicatorArrowUp:
                a.setPoints(3, border, sqsize / 2, sqsize / 2, border, sqsize - border, sqsize / 2);
                break;
            case PE_IndicatorArrowDown:
                a.setPoints(3, border, sqsize / 2, sqsize / 2, sqsize - border, sqsize - border, sqsize / 2);
                break;
            case PE_IndicatorArrowRight:
                a.setPoints(3, sqsize - border, sqsize / 2, sqsize / 2, border, sqsize / 2, sqsize - border);
                break;
            case PE_IndicatorArrowLeft:
                a.setPoints(3, border, sqsize / 2, sqsize / 2, border, sqsize / 2, sqsize - border);
                break;
            default:
                break;
            }

            int bsx = 0;
            int bsy = 0;

            if (opt->state & State_Sunken) {
                bsx = proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, widget);
                bsy = proxy()->pixelMetric(PM_ButtonShiftVertical, opt, widget);
            }

            QRect bounds = a.boundingRect();
            int sx       = sqsize / 2 - bounds.center().x() - 1;
            int sy       = sqsize / 2 - bounds.center().y() - 1;
            imagePainter.translate(sx + bsx, sy + bsy);
            imagePainter.setPen(QPen(opt->palette.buttonText().color(), 1.4));
            imagePainter.setBrush(Qt::NoBrush);

            if (!(opt->state & State_Enabled)) {
                imagePainter.translate(1, 1);
                imagePainter.setPen(QPen(opt->palette.light().color(), 1.4));
                imagePainter.drawPolyline(a);
                imagePainter.translate(-1, -1);
                imagePainter.setPen(QPen(opt->palette.mid().color(), 1.4));
            }

            imagePainter.drawPolyline(a);
            imagePainter.end();
            pixmap = QPixmap::fromImage(image);
            pixmap.setDevicePixelRatio(pixelRatio);

            int xOffset = r.x() + (r.width() - size) / 2;
            int yOffset = r.y() + (r.height() - size) / 2;
            p->drawPixmap(xOffset, yOffset, pixmap);
        } else {
            QProxyStyle::drawPrimitive(pe, opt, p, widget);
        }
    }
};

//===================================================
// SARibbonToolButton::PrivateData
//===================================================

class SARibbonToolButton::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonToolButton)
public:
    PrivateData(SARibbonToolButton* p);
    // 根据鼠标位置更新按钮的信息
    void updateStatusByMousePosition(const QPoint& pos);
    // 更新绘图相关的尺寸
    void updateDrawRect(const QStyleOptionToolButton& opt);
    // 更新SizeHint
    void updateSizeHint(const QStyleOptionToolButton& opt);
    // 计算涉及到的rect尺寸
    void calcDrawRects(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& indicatorArrowRect, int spacing, int indicatorLen) const;
    // 计算小按钮模式下的尺寸
    void calcSmallButtonDrawRects(const QStyleOptionToolButton& opt,
                                  QRect& iconRect,
                                  QRect& textRect,
                                  QRect& indicatorArrowRect,
                                  int spacing,
                                  int indicatorLen) const;
    // 计算大按钮模式下的尺寸
    void calcLargeButtonDrawRects(const QStyleOptionToolButton& opt,
                                  QRect& iconRect,
                                  QRect& textRect,
                                  QRect& indicatorArrowRect,
                                  int spacing,
                                  int indicatorLen) const;
    // 根据按钮的尺寸调节iconsize(注意这里的buttonRect是已经减去mSpacing的情况)
    QSize adjustIconSize(const QRect& buttonRect, const QSize& originIconSize) const;
    // 判断是否有Indicator
    bool hasIndicator(const QStyleOptionToolButton& opt) const;
    // 计算sizehint
    QSize calcSizeHint(const QStyleOptionToolButton& opt);
    QSize calcSmallButtonSizeHint(const QStyleOptionToolButton& opt);
    QSize calcLargeButtonSizeHint(const QStyleOptionToolButton& opt);

    // 计算文本绘制矩形的高度
    int calcTextDrawRectHeight(const QStyleOptionToolButton& opt) const;
    // 估算一个最优的文本宽度
    int estimateLargeButtonTextWidth(int buttonHeight,
                                     int textDrawRectHeight,
                                     const QString& text,
                                     const QFontMetrics& fm,
                                     float widthHeightRatio = SARIBBONTOOLBUTTON_WORDWRAP_WIDTH_PER_HEIGHT_RATIO,
                                     int maxTrycount        = 3);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;
    // 获取文字的对其方式
    int getTextAlignment() const;
    // 确认文字是否确切要换行显示
    bool isTextNeedWrap() const;
    // 仅仅对\n进行剔除，和QString::simplified不一样
    static QString simplified(const QString& str);

public:
    bool mMouseOnSubControl { false };  ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool mMenuButtonPressed { false };  ///< 由于Indicator改变，因此hitButton不能用QToolButton的hitButton
    bool mWordWrap { false };           ///< 标记是否文字换行 @default false
    SARibbonToolButton::RibbonButtonType mButtonType { SARibbonToolButton::LargeButton };
    int mSpacing { 1 };              ///< 按钮和边框的距离
    int mIndicatorLen { 8 };         ///< Indicator的长度
    QRect mDrawIconRect;             ///< 记录icon的绘制位置
    QRect mDrawTextRect;             ///< 记录text的绘制位置
    QRect mDrawIndicatorArrowRect;   ///< 记录IndicatorArrow的绘制位置
    QSize mSizeHint;                 ///< 保存计算好的sizehint
    bool mIsTextNeedWrap { false };  ///< 标记文字是否需要换行显示
public:
    static bool s_enableWordWrap;  ///< 在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
};

// 静态参数初始化
bool SARibbonToolButton::PrivateData::s_enableWordWrap = false;

SARibbonToolButton::PrivateData::PrivateData(SARibbonToolButton* p) : q_ptr(p)
{
    auto proxy = new SARibbonToolButtonProxyStyle();
    proxy->setParent(p);  // take ownership to avoid memleak
    p->setStyle(proxy);
}

/**
 * @brief 根据鼠标的位置更新状态，主要用于判断鼠标是否位于subcontrol
 *
 * 此函数主要应用在action menu模式下
 * @param pos
 */
void SARibbonToolButton::PrivateData::updateStatusByMousePosition(const QPoint& pos)
{
    bool isMouseOnSubControl(false);
    if (SARibbonToolButton::LargeButton == mButtonType) {
        isMouseOnSubControl = mDrawTextRect.united(mDrawIndicatorArrowRect).contains(pos);
    } else {
        // 小按钮模式就和普通toolbutton一样
        isMouseOnSubControl = mDrawIndicatorArrowRect.contains(pos);
    }

    if (mMouseOnSubControl != isMouseOnSubControl) {
        mMouseOnSubControl = isMouseOnSubControl;
        // 从icon变到text过程中刷新一次
        q_ptr->update();
    }
}

/**
 * @brief 更新绘图的几个关键尺寸
 *
 * 包括：
 *
 * - DrawIconRect 绘制图标的矩形区域
 *
 * - DrawTextRect 绘制文本的矩形区域
 *
 * - DrawIndicatorArrowRect 绘制菜单下箭头的矩形区域
 *
 * @param opt
 */
void SARibbonToolButton::PrivateData::updateDrawRect(const QStyleOptionToolButton& opt)
{
    if (!mSizeHint.isValid()) {
        updateSizeHint(opt);
    }
    // 先更新IndicatorLen
    mIndicatorLen = q_ptr->style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, q_ptr);
    if (mIndicatorLen < 3) {
        if (SARibbonToolButton::LargeButton == mButtonType) {
            mIndicatorLen = 8;
        } else {
            mIndicatorLen = 12;  // 小按钮模式下设置为10
        }
    }
    calcDrawRects(opt, mDrawIconRect, mDrawTextRect, mDrawIndicatorArrowRect, mSpacing, mIndicatorLen);
}

/**
 * @brief 更新sizehint
 * @param opt
 */
void SARibbonToolButton::PrivateData::updateSizeHint(const QStyleOptionToolButton& opt)
{
    mSizeHint = calcSizeHint(opt);
}

/**
 * @brief 计算绘图的几个关键区域
 * @param opt
 * @param iconRect  绘制图标的矩形区域
 * @param textRect 绘制文本的矩形区域
 * @param indicatorArrowRect 绘制菜单下箭头的矩形区域
 * @param spacing
 * @param indicatorLen
 */
void SARibbonToolButton::PrivateData::calcDrawRects(const QStyleOptionToolButton& opt,
                                                    QRect& iconRect,
                                                    QRect& textRect,
                                                    QRect& indicatorArrowRect,
                                                    int spacing,
                                                    int indicatorLen) const
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        calcLargeButtonDrawRects(opt, iconRect, textRect, indicatorArrowRect, spacing, indicatorLen);

    } else {
        calcSmallButtonDrawRects(opt, iconRect, textRect, indicatorArrowRect, spacing, indicatorLen);
    }
}

/**
 * @brief 计算小按钮模式下的绘制尺寸
 * @param opt
 * @param iconRect
 * @param textRect
 * @param indicatorArrowRect
 * @param spacing
 * @param indicatorLen
 */
void SARibbonToolButton::PrivateData::calcSmallButtonDrawRects(const QStyleOptionToolButton& opt,
                                                               QRect& iconRect,
                                                               QRect& textRect,
                                                               QRect& indicatorArrowRect,
                                                               int spacing,
                                                               int indicatorLen) const
{
    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        if (hasIndicator(opt)) {
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            iconRect = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing, iconRect.y(), indicatorLen, iconRect.height());
        } else {
            iconRect           = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        // 文本区域为空
        textRect = QRect();
    } break;
    case Qt::ToolButtonTextOnly: {
        if (hasIndicator(opt)) {
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            textRect = opt.rect.adjusted(spacing, spacing, -indicatorLen - spacing, -spacing);
            indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing, spacing, indicatorLen, textRect.height());
        } else {
            textRect           = opt.rect.adjusted(spacing, spacing, -spacing, -spacing);
            indicatorArrowRect = QRect();
        }
        // 绘图区域为空
        iconRect = QRect();
    } break;
    default: {
        bool hasInd = hasIndicator(opt);
        // icon Beside和under都是一样的
        QRect buttonRect = q_ptr->rect();
        buttonRect.adjust(spacing, spacing, -spacing, -spacing);
        // 先设置IconRect
        if (opt.icon.isNull()) {
            // 没有图标
            iconRect = QRect();
        } else {
            QSize iconSize = adjustIconSize(buttonRect, opt.iconSize);
            iconRect = QRect(buttonRect.x(), buttonRect.y(), iconSize.width(), qMax(iconSize.height(), buttonRect.height()));
        }
        // 后设置TextRect
        if (opt.text.isEmpty()) {
            textRect = QRect();
        } else {
            // 分有菜单和没菜单两种情况
            int adjx = iconRect.isValid() ? (iconRect.width() + spacing) : 0;  // 在buttonRect上变换，因此如果没有图标是不用偏移spacing
            if (hasInd) {
                textRect = buttonRect.adjusted(adjx, 0, -indicatorLen, 0);
            } else {
                textRect = buttonRect.adjusted(adjx, 0, 0, 0);  // 在buttonRect上变换，因此如果没有图标是不用偏移spacing
            }
        }
        // 最后设置Indicator
        if (hasInd) {
            if (textRect.isValid()) {
                indicatorArrowRect = QRect(buttonRect.right() - indicatorLen + 1, textRect.y(), indicatorLen, textRect.height());
            } else if (iconRect.isValid()) {
                indicatorArrowRect = QRect(buttonRect.right() - indicatorLen + 1, iconRect.y(), indicatorLen, iconRect.height());
            } else {
                indicatorArrowRect = buttonRect;
            }
        } else {
            indicatorArrowRect = QRect();
        }
    }
    }
}

/**
 * @brief 计算大按钮模式下的绘制尺寸（普通）
 * @param opt
 * @param iconRect
 * @param textRect
 * @param indicatorArrowRect
 * @param spacing
 * @param indicatorLen
 */
void SARibbonToolButton::PrivateData::calcLargeButtonDrawRects(const QStyleOptionToolButton& opt,
                                                               QRect& iconRect,
                                                               QRect& textRect,
                                                               QRect& indicatorArrowRect,
                                                               int spacing,
                                                               int indicatorLen) const
{
    //! 3行模式的图标比较大，文字换行情况下，indicator会动态调整
    // 先获取文字矩形的高度
    int textHeight  = calcTextDrawRectHeight(opt);
    bool hIndicator = hasIndicator(opt);
    if (!hIndicator) {
        // 没有菜单，把len设置为0
        indicatorLen       = 0;
        indicatorArrowRect = QRect();
    }
    // 这里要判断文字是否要换行显示，换行显示的文字的indicatorArrowRect所处的位置不一样
    // 先布置textRect
    if (isEnableWordWrap()) {
        // 在换行模式下
        if (isTextNeedWrap()) {
            // 如果文字的确换行，indicator放在最右边
            textRect = QRect(spacing, opt.rect.bottom() - spacing - textHeight, opt.rect.width() - 2 * spacing - indicatorLen, textHeight);
            if (hIndicator) {
                indicatorArrowRect = QRect(textRect.right(), textRect.y() + textRect.height() / 2, indicatorLen, textHeight / 2);
            }
        } else {
            // 如果文字不需要换行，indicator在下板行
            textRect = QRect(spacing, opt.rect.bottom() - spacing - textHeight, opt.rect.width() - 2 * spacing, textHeight);
            if (hIndicator) {
                int dy = textRect.height() / 2;
                dy += (dy - indicatorLen) / 2;
                indicatorArrowRect = QRect(textRect.x(), textRect.y() + dy, textRect.width(), indicatorLen);
            }
        }
    } else {
        // 文字不换行，indicator放在最右边
        int y = opt.rect.bottom() - spacing - textHeight;
        if (hIndicator) {
            // 先布置indicator
            indicatorArrowRect = QRect(opt.rect.right() - indicatorLen - spacing, y, indicatorLen, textHeight);
            textRect           = QRect(spacing, y, indicatorArrowRect.x() - spacing, textHeight);
        } else {
            textRect = QRect(spacing, y, opt.rect.width() - 2 * spacing, textHeight);
        }
    }
    // 剩下就是icon区域
    iconRect = QRect(spacing, spacing, opt.rect.width() - 2 * spacing, textRect.top() - 2 * spacing);
}

/**
 * @brief 适应iconsize
 * @param buttonRect
 * @param originIconSize
 * @return
 */
QSize SARibbonToolButton::PrivateData::adjustIconSize(const QRect& buttonRect, const QSize& originIconSize) const
{
    QSize iconSize = originIconSize;
    if (iconSize.height() > buttonRect.height()) {
        // 说明图标的icon过大，要匹配到buttonRect
        iconSize.setHeight(buttonRect.height());
        // 等比例设置宽度
        iconSize.setWidth(originIconSize.width() * iconSize.height() / originIconSize.height());
    }
    return iconSize;
}

/**
 * @brief 判断是否有Indicator
 * @param opt
 * @return
 */
bool SARibbonToolButton::PrivateData::hasIndicator(const QStyleOptionToolButton& opt) const
{
    return ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu));
}

/**
 * @brief 计算sizehint
 *
 * 此函数非常关键，因为所有尺寸计算都是基于原始的rect来的
 * @param opt
 * @return
 */
QSize SARibbonToolButton::PrivateData::calcSizeHint(const QStyleOptionToolButton& opt)
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        return calcLargeButtonSizeHint(opt);
    }
    return calcSmallButtonSizeHint(opt);
}

QSize SARibbonToolButton::PrivateData::calcSmallButtonSizeHint(const QStyleOptionToolButton& opt)
{
    int w = 0, h = 0;

    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonIconOnly: {
        w = opt.iconSize.width() + 2 * mSpacing;
        h = opt.iconSize.height() + 2 * mSpacing;
    } break;
    case Qt::ToolButtonTextOnly: {
        QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplified(opt.text));
        textSize.setWidth(textSize.width() + SA_FONTMETRICS_WIDTH(opt.fontMetrics, (QLatin1Char(' '))) * 2);
        textSize.setHeight(calcTextDrawRectHeight(opt));
        w = textSize.width() + 2 * mSpacing;
        h = textSize.height() + 2 * mSpacing;
    } break;
    default: {
        // 先加入icon的尺寸
        w = opt.iconSize.width() + 2 * mSpacing;
        h = opt.iconSize.height() + 2 * mSpacing;
        // 再加入文本的长度
        if (!opt.text.isEmpty()) {
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, simplified(opt.text));
            textSize.setWidth(textSize.width() + SA_FONTMETRICS_WIDTH(opt.fontMetrics, (QLatin1Char(' '))) * 2);
            textSize.setHeight(calcTextDrawRectHeight(opt));
            w += mSpacing;
            w += textSize.width();
            h = qMax(h, (textSize.height() + (2 * mSpacing)));
        } else {
            // 没有文本的时候也要设置一下高度
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, " ");
            h              = qMax(h, (textSize.height() + (2 * mSpacing)));
        }
    }
    }
    if (hasIndicator(opt)) {
        // 存在indicator的按钮，宽度尺寸要扩展
        w += mIndicatorLen;
    }
    if (w < 16) {
        w = 16;
    }
    //! Qt6.4 取消了QApplication::globalStrut
    return QSize(w, h).expandedTo(QSize(2, 2));
}

QSize SARibbonToolButton::PrivateData::calcLargeButtonSizeHint(const QStyleOptionToolButton& opt)
{
    int w    = 0;
    int h    = opt.fontMetrics.lineSpacing() * 4.8;  // 3*1.6
    int minW = h * 0.75;  // 最小宽度，在pannel里面的按钮，最小宽度要和icon适应

    if (SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(q_ptr->parent())) {
        // 对于建立在SARibbonPannel的基础上的大按钮，把高度设置为SARibbonPannel计算的大按钮高度
        h = pannel->largeButtonHeight();
    }
    int textHeight = calcTextDrawRectHeight(opt);
    // 估算字体的宽度作为宽度
    w = estimateLargeButtonTextWidth(h, textHeight, opt.text, opt.fontMetrics);
    w += (2 * mSpacing);
    // 判断是否需要加上indicator
    if (isEnableWordWrap() && isTextNeedWrap()) {
        w += mIndicatorLen;
    }

#if SA_RIBBON_TOOLBUTTON_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| | |-SARibbonToolButton::PrivateData::calcLargeButtonSizeHint,text=" << opt.text
             << "\n| | | |-lineSpacing*4.5=" << opt.fontMetrics.lineSpacing() * 4.5  //
             << "\n| | | |-textHeight=" << textHeight                                //
             << "\n| | | |-mDrawIconRect=" << mDrawIconRect                          //
             << "\n| | | |-minW=" << minW                                            //
             << "\n| | | |-w=" << w                                                  //
            ;
#endif
    //! Qt6.4 取消了QApplication::globalStrut
    return QSize(w, h).expandedTo(QSize(minW, textHeight));
}

/**
 * @brief 计算文本高度
 * @param opt
 * @return
 */
int SARibbonToolButton::PrivateData::calcTextDrawRectHeight(const QStyleOptionToolButton& opt) const
{
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (isEnableWordWrap()) {
            return opt.fontMetrics.lineSpacing() * SARIBBONTOOLBUTTON_WORDWRAP_TEXT_FACTOR + opt.fontMetrics.leading();
        } else {
            return opt.fontMetrics.lineSpacing() * SARIBBONTOOLBUTTON_NOWORDWRAP_TEXT_FACTOR;
        }
    }
    // 小按钮
    return opt.fontMetrics.lineSpacing() * SARIBBONTOOLBUTTON_SMALLBUTTON_TEXT_FACTOR;
}

/**
 * @brief 估算一个最优的文字尺寸，在可以换行的情况下会进行换行，且只会换一行
 * @param buttonHeight 按钮的高度
 * @param textDrawRectHeight 文本绘制的高度
 * @param fm QFontMetrics
 * @param widthHeightRatio 宽高比，宽度/高度的比值，如果大于这个比值，则会进行尝试换行以获取更低的宽度
 * @param maxTrycount 尝试次数
 * @return
 */
int SARibbonToolButton::PrivateData::estimateLargeButtonTextWidth(int buttonHeight,
                                                                  int textDrawRectHeight,
                                                                  const QString& text,
                                                                  const QFontMetrics& fm,
                                                                  float widthHeightRatio,
                                                                  int maxTrycount)
{
    QSize textSize;
    int space        = SA_FONTMETRICS_WIDTH(fm, (QLatin1Char(' '))) * 2;
    int hintMaxWidth = buttonHeight * widthHeightRatio;  ///< 建议的宽度
    if (isEnableWordWrap()) {
        textSize = fm.size(Qt::TextShowMnemonic, text);
        textSize.setWidth(textSize.width() + space);

        if (textSize.height() > fm.lineSpacing() * 1.1) {
            //! 说明文字带有换行符，是用户手动换行，这种情况就直接返回字体尺寸，不进行估算
            mIsTextNeedWrap = true;  // 文字需要换行显示，标记起来
            return textSize.width();
        }

        // 这时候需要估算文本的长度
        if (textSize.width() <= hintMaxWidth) {
            // 范围合理，直接返回
            mIsTextNeedWrap = false;  // 文字不需要换行显示，标记起来
            return textSize.width();
        }

        //! 大于宽高比尝试进行文字换行
        //! 这里先对文本长度逐渐加长估算，一直到和原来长度一致为止
        int trycount  = 0;
        int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
        // 对于英文字体，直接宽度减半是无法满足完全显示两行的，需要进行预估
        QRect textRect(0, 0, textSize.width(), textDrawRectHeight);
        do {
            //! 先计算两行文本的紧凑矩形
            //! 从一半开始逐渐递增
            //! 第1次为 w/2 + w/2 * (0/3)
            //! 第2次为 w/2 + w/2 * (1/3)
            //! 第3次为 w/2 + w/2 * (2/3)
            textRect.setWidth(textSize.width() / 2 + (textSize.width() / 2) * (float(trycount) / maxTrycount));
            textRect = fm.boundingRect(textRect, alignment, text);
            if (textRect.height() <= (fm.lineSpacing() * 2)) {
                // 保证在两行
                mIsTextNeedWrap = true;  // 文字需要换行显示，标记起来
                return textRect.width();
            }
            ++trycount;
#if SARIBBONTOOLBUTTON_DEBUG_DRAW
            if (trycount > 1) {
                qDebug() << "estimateLargeButtonTextWidth,origin textSize=" << textSize << ",trycount=" << trycount
                         << ",textRect=" << textRect;
            }
#endif
        } while (trycount < 3);
        // 到这里说明前面的尝试失败，最终使用原始的长度
        return textSize.width();
    }

    //! 说明是不换行

    mIsTextNeedWrap = false;  // 文字不需要换行显示，标记起来
    // 文字不换行情况下，做simplified处理
    textSize = fm.size(Qt::TextShowMnemonic, simplified(text));
    textSize.setWidth(textSize.width() + space);
    if (textSize.width() < hintMaxWidth) {
        // 范围合理，直接返回
        return textSize.width();
    }
    if (textSize.width() > q_ptr->maximumWidth()) {
        // 超出了极限，就返回极限
        return q_ptr->maximumWidth();
    }
    return hintMaxWidth;
}

QPixmap SARibbonToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const
{
    if (opt.icon.isNull()) {  // 没有有图标
        return (QPixmap());
    }
    QIcon::State state = (opt.state & QStyle::State_On) ? QIcon::On : QIcon::Off;
    QIcon::Mode mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    // 添加高分屏支持
    QSize pxiampSize = iconsize - QSize(2, 2);
    return opt.icon.pixmap(pxiampSize, mode, state);
}

int SARibbonToolButton::PrivateData::getTextAlignment() const
{
    int alignment = Qt::TextShowMnemonic;
    if (SARibbonToolButton::LargeButton == mButtonType) {
        if (isEnableWordWrap()) {
            alignment |= Qt::TextWordWrap | Qt::AlignTop | Qt::AlignHCenter;  // 换行的情况下，顶部对齐
        } else {
            alignment |= Qt::AlignCenter;
        }
    } else {
        alignment |= Qt::AlignCenter;
    }
    return alignment;
}

/**
 * @brief 确认文字是否确切要换行显示
 * @return
 */
bool SARibbonToolButton::PrivateData::isTextNeedWrap() const
{
    return mIsTextNeedWrap;
}

/**
 * @brief 仅仅对\n进行剔除
 * @param str
 * @return
 */
QString SARibbonToolButton::PrivateData::simplified(const QString& str)
{
    QString res = str;
    res.remove('\n');
    return res;
}
//===================================================
// SARibbonToolButton
//===================================================

SARibbonToolButton::SARibbonToolButton(QWidget* parent)
    : QToolButton(parent), d_ptr(new SARibbonToolButton::PrivateData(this))
{
    setAutoRaise(true);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

SARibbonToolButton::SARibbonToolButton(QAction* defaultAction, QWidget* parent)
    : QToolButton(parent), d_ptr(new SARibbonToolButton::PrivateData(this))
{
    setAutoRaise(true);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setDefaultAction(defaultAction);
    setButtonType(SmallButton);
    setMouseTracking(true);
}

SARibbonToolButton::~SARibbonToolButton()
{
}

/**
 * @brief 鼠标移动事件
 *
 * 由于Ribbon的Indicator和正常的Toolbutton不一样，因此无法用QStyleOptionToolButton的activeSubControls的状态
 *
 * 因此需要重新捕获鼠标的位置来更新按钮当前的一些状态
 * @param e
 */
void SARibbonToolButton::mouseMoveEvent(QMouseEvent* e)
{
    d_ptr->updateStatusByMousePosition(e->pos());
    QToolButton::mouseMoveEvent(e);
}

/**
 * @brief SARibbonToolButton::mousePressEvent
 * @param e
 */
void SARibbonToolButton::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::LeftButton) && (popupMode() == MenuButtonPopup)) {
        d_ptr->updateStatusByMousePosition(e->pos());
        if (d_ptr->mMouseOnSubControl) {
            d_ptr->mMenuButtonPressed = true;
            showMenu();
            // showmenu结束后，在判断当前的鼠标位置是否是在subcontrol
            d_ptr->updateStatusByMousePosition(mapFromGlobal(QCursor::pos()));
            return;
        }
    }
    d_ptr->mMenuButtonPressed = false;
    //! 注意这里要用QAbstractButton的mousePressEvent，而不是QToolButton的mousePressEvent
    //! QToolButton的mousePressEvent主要是为了弹出菜单，这里弹出菜单的方式是不一样的，因此不能执行QToolButton的mousePressEvent
    QAbstractButton::mousePressEvent(e);
}

void SARibbonToolButton::mouseReleaseEvent(QMouseEvent* e)
{
    d_ptr->mMenuButtonPressed = false;
    QToolButton::mouseReleaseEvent(e);
}

void SARibbonToolButton::focusOutEvent(QFocusEvent* e)
{
    d_ptr->mMouseOnSubControl = false;
    QToolButton::focusOutEvent(e);
}

void SARibbonToolButton::leaveEvent(QEvent* e)
{
    d_ptr->mMouseOnSubControl = false;
    QToolButton::leaveEvent(e);
}

bool SARibbonToolButton::hitButton(const QPoint& pos) const
{
    if (QToolButton::hitButton(pos)) {
        return (!d_ptr->mMenuButtonPressed);
    }
    return (false);
}

/**
 * @brief 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
 * @param e
 */
void SARibbonToolButton::resizeEvent(QResizeEvent* e)
{
    // 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    updateRect();
}

/**
 * @brief toolbutton的尺寸确定是先定下字体的尺寸，再定下icon的尺寸，自底向上，保证字体能显示两行
 * @note m_sizeHint的刷新需要注意
 * @return
 */
QSize SARibbonToolButton::sizeHint() const
{
    //    if (!d_ptr->mSizeHint.isValid()) {  // 22是给与sizehint的最小值，如果小于这个值，重新计算一下
    //        QStyleOptionToolButton opt;
    //        initStyleOption(&opt);
    //        d_ptr->updateSizeHint(opt);
    //    }
#if SA_RIBBON_TOOLBUTTON_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| | |-SARibbonToolButton::sizeHint";
#endif
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateSizeHint(opt);
    return d_ptr->mSizeHint;
}

void SARibbonToolButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        // 在菜单弹出消失后，需要通过此方法取消掉鼠标停留
        if (!rect().contains(mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }
    paintButton(p, opt);
    paintIcon(p, opt, d_ptr->mDrawIconRect);
    paintText(p, opt, d_ptr->mDrawTextRect);
    paintIndicator(p, opt, d_ptr->mDrawIndicatorArrowRect);
}

/**
 * @brief 绘制按钮
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintButton(QPainter& p, const QStyleOptionToolButton& opt)
{
    // QStyle::State_Sunken 代表按钮按下去了
    // QStyle::State_On 代表按钮按checked
    // QStyle::State_MouseOver 代表当前鼠标位于按钮上面
    QStyleOption tool = opt;
    bool autoRaise    = opt.state & QStyle::State_AutoRaise;
    // 绘制按钮
    if (autoRaise) {
        // 这个是为了实现按钮点击下去后(QStyle::State_Sunken),能出现选中的状态
        // 先绘制一个鼠标不在按钮上的状态
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
    } else {
        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
    }
    // 针对MenuButtonPopup的ribbon样式的特殊绘制
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        if (opt.state & QStyle::State_MouseOver) {                       // 鼠标在按钮上才进行绘制
            if (!(opt.activeSubControls & QStyle::SC_ToolButtonMenu)) {  // 按钮的菜单弹出时不做处理
                if (LargeButton == d_ptr->mButtonType) {                 // 大按钮模式
                    if (d_ptr->mMouseOnSubControl) {                     // 此时鼠标在indecater那
                        // 鼠标在文字区，把图标显示为正常（就是鼠标不放上去的状态）
                        tool.rect = d_ptr->mDrawIconRect;
                        tool.state |= (QStyle::State_Raised);  // 把图标区域显示为正常
                        tool.state &= ~QStyle::State_MouseOver;
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                        }
                    } else {
                        // 鼠标在图标区，把文字显示为正常
                        if (!tool.state.testFlag(QStyle::State_Sunken)) {
                            // State_Sunken说明此按钮正在按下，这时候，文本区域不需要绘制，只有在非按下状态才需要绘制
                            tool.state |= (QStyle::State_Raised);  // 把图标区域显示为正常
                            tool.state &= ~QStyle::State_MouseOver;
                            // 文字和Indicator都显示正常
                            tool.rect = d_ptr->mDrawTextRect.united(d_ptr->mDrawIndicatorArrowRect);
                            if (autoRaise) {
                                style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                            } else {
                                style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                            }
                        }
                    }
                } else {                              // 小按钮模式
                    if (d_ptr->mMouseOnSubControl) {  // 此时鼠标在indecater那
                        // 鼠标在文字区，把图标和文字显示为正常
                        tool.rect  = d_ptr->mDrawIconRect.united(d_ptr->mDrawTextRect);
                        tool.state = (QStyle::State_Raised);  // 把图标区域显示为正常
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                        }
                    } else {
                        // 鼠标在图标区，把文字显示为正常
                        tool.state = (QStyle::State_Raised);  // 把图标区域显示为正常
                        // 文字和Indicator都显示正常
                        tool.rect = d_ptr->mDrawIndicatorArrowRect;
                        if (autoRaise) {
                            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                        } else {
                            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                        }
                    }
                }
            }
        }
    }
    // 绘制Focus
    //     if (opt.state & QStyle::State_HasFocus) {
    //         QStyleOptionFocusRect fr;
    //         fr.QStyleOption::operator=(opt);
    //         fr.rect.adjust(d_ptr->mSpacing, d_ptr->mSpacing, -d_ptr->mSpacing, -d_ptr->mSpacing);
    //         style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
    //     }
}

/**
 * @brief 绘制图标
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect)
{
    if (!iconDrawRect.isValid()) {
        return;
    }

    QPixmap pm = d_ptr->createIconPixmap(opt, iconDrawRect.size());
    style()->drawItemPixmap(&p, iconDrawRect, Qt::AlignCenter, pm);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, iconDrawRect);
}

/**
 * @brief 绘制文本
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect)
{
    int alignment = d_ptr->getTextAlignment();

    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
        alignment |= Qt::TextHideMnemonic;
    }
    QString text;
    if (isSmallRibbonButton()) {
        text = opt.fontMetrics.elidedText(PrivateData::simplified(opt.text), Qt::ElideRight, textDrawRect.width(), alignment);
    } else {
        if (!isEnableWordWrap()) {
            text = opt.fontMetrics.elidedText(PrivateData::simplified(opt.text), Qt::ElideRight, textDrawRect.width(), alignment);
        } else {
            text = opt.text;
        }
    }
    //! 以下内容参考QCommonStyle.cpp
    //! void QCommonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,QPainter *p, const QWidget *widget) const
    //! case CC_ToolButton:
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
    if (bflags & QStyle::State_AutoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        }
    }
    QStyleOptionToolButton label = opt;
    label.state                  = bflags;
    style()->drawItemText(&p, textDrawRect, alignment, label.palette, label.state & QStyle::State_Enabled, text, QPalette::ButtonText);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, textDrawRect);
}

/**
 * @brief 绘制Indicator
 * @param p
 * @param opt
 */
void SARibbonToolButton::paintIndicator(QPainter& p, const QStyleOptionToolButton& opt, const QRect& indicatorDrawRect)
{
    if (!indicatorDrawRect.isValid() || !d_ptr->hasIndicator(opt)) {
        return;
    }

    QStyleOption tool = opt;
    tool.rect         = indicatorDrawRect;
    style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
    SARIBBONTOOLBUTTON_DEBUG_DRAW_RECT(p, indicatorDrawRect);
}

void SARibbonToolButton::drawArrow(const QStyle* style,
                                   const QStyleOptionToolButton* toolbutton,
                                   const QRect& rect,
                                   QPainter* painter,
                                   const QWidget* widget)
{
    QStyle::PrimitiveElement pe;

    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;

    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;

    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;

    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;

    default:
        return;
    }
    QStyleOption arrowOpt = *toolbutton;

    arrowOpt.rect = rect;
    style->drawPrimitive(pe, &arrowOpt, painter, widget);
}

void SARibbonToolButton::actionEvent(QActionEvent* e)
{
    QToolButton::actionEvent(e);
    updateRect();
}

/**
 * @brief 按钮样式
 * @sa setButtonType
 * @return
 */
SARibbonToolButton::RibbonButtonType SARibbonToolButton::buttonType() const
{
    return (d_ptr->mButtonType);
}

/**
 * @brief 设置按钮样式
 * @note 设置按钮样式过程会调用setToolButtonStyle，如果要改变toolButtonStyle,如设置为Qt::ToolButtonIconOnly,需要在此函数之后设置
 * @param buttonType
 */
void SARibbonToolButton::setButtonType(const RibbonButtonType& buttonType)
{
    d_ptr->mButtonType = buttonType;
    // 计算iconrect
    // 根据字体计算文字的高度

    if (LargeButton == buttonType) {
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    } else {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }

    updateRect();
}

/**
 * @brief 是否是小按钮
 * @return
 */
bool SARibbonToolButton::isSmallRibbonButton() const
{
    return (d_ptr->mButtonType == SmallButton);
}

/**
 * @brief 是否是大按钮
 * @return
 */
bool SARibbonToolButton::isLargeRibbonButton() const
{
    return (d_ptr->mButtonType == LargeButton);
}

QSize SARibbonToolButton::minimumSizeHint() const
{
    return (sizeHint());
}

/**
 * @brief 间距是几个重要矩形的间隔
 * @return
 */
int SARibbonToolButton::spacing() const
{
    return d_ptr->mSpacing;
}

void SARibbonToolButton::updateRect()
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->updateDrawRect(opt);
}

/**
 * @brief 设置在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
 * @param on
 */
void SARibbonToolButton::setEnableWordWrap(bool on)
{
    SARibbonToolButton::PrivateData::s_enableWordWrap = on;
}

/**
 * @brief 在lite模式下是否允许文字换行
 * @return
 */
bool SARibbonToolButton::isEnableWordWrap()
{
    return SARibbonToolButton::PrivateData::s_enableWordWrap;
}

bool SARibbonToolButton::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::WindowDeactivate:
        d_ptr->mMouseOnSubControl = false;
        break;
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ActionAdded: {
        d_ptr->mMouseOnSubControl = false;
        updateRect();
    } break;
    default:
        break;
    }

    return (QToolButton::event(e));
}

void SARibbonToolButton::changeEvent(QEvent* e)
{
    if (e) {
        if (e->type() == QEvent::FontChange) {
            // 说明字体改变，需要重新计算和字体相关的信息
            updateRect();
        }
    }
    QToolButton::changeEvent(e);
}

/*** End of inlined file: SARibbonToolButton.cpp ***/

/*** Start of inlined file: SARibbonColorToolButton.cpp ***/
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QDebug>

//===================================================
// SARibbonColorToolButton::PrivateData
//===================================================
const int c_ribbonbutton_color_height = 5;  ///< 颜色块的高度

class SARibbonColorToolButton::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonColorToolButton)
public:
    PrivateData(SARibbonColorToolButton* p);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;
    QIcon createColorIcon(const QColor& c, const QSize& size) const;

public:
    QColor mColor;                                                                                ///< 记录颜色
    SARibbonColorToolButton::ColorStyle mColorStyle { SARibbonColorToolButton::ColorUnderIcon };  ///< 颜色显示样式
    QIcon mOldIcon;                                                                               ///< 记录旧的icon
};

SARibbonColorToolButton::PrivateData::PrivateData(SARibbonColorToolButton* p) : q_ptr(p)
{
}

QPixmap SARibbonColorToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const
{
    if (opt.icon.isNull()) {  // 没有有图标
        return QPixmap();
    }
    // 有icon，在icon下方加入颜色
    QIcon::State state = (opt.state & QStyle::State_On) ? QIcon::On : QIcon::Off;
    QIcon::Mode mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    QSize realIconSize = iconsize - QSize(0, c_ribbonbutton_color_height + 1);
    QPixmap pixmap     = opt.icon.pixmap(q_ptr->window()->windowHandle(), realIconSize, mode, state);
    QPixmap res(pixmap.size() + QSize(4, c_ribbonbutton_color_height + 4));  // 宽度上，颜色块多出2px
    res.fill(Qt::transparent);
    QPainter painter(&res);
    int xpixmap = (res.width() - pixmap.width()) / 2;
    int ypixmap = (res.height() - c_ribbonbutton_color_height - 2 - pixmap.height()) / 2;  // 这里要减去2而不是1，这样奇数偶数都不会影响
    int w         = pixmap.width();
    int h         = pixmap.height();
    QRect rpixmap = QRect(xpixmap, ypixmap, w, h);
    painter.drawPixmap(rpixmap, pixmap);
    QRect colorRect = rpixmap.adjusted(0, h + 1, 0, c_ribbonbutton_color_height + 1);
    if (mColor.isValid()) {
        painter.fillRect(colorRect, mColor);
    } else {
        QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        int ss = colorRect.width() / 3;
        painter.drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()), QPoint(colorRect.right() - ss, colorRect.top()));
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawRect(colorRect);
    }
    return res;
}

QIcon SARibbonColorToolButton::PrivateData::createColorIcon(const QColor& c, const QSize& size) const
{
    QPixmap res(size);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    if (c.isValid()) {
        painter.fillRect(QRect(1, 1, res.height() - 2, res.width() - 2), c);
    } else {
        QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawRect(QRect(1, 1, res.height() - 2, res.width() - 2));
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(QPoint(1, size.height()), QPoint(size.width() - 1, 1));
    }
    return QIcon(res);
}

//===================================================
// SARibbonColorToolButton
//===================================================

SARibbonColorToolButton::SARibbonColorToolButton(QWidget* parent)
    : SARibbonToolButton(parent), d_ptr(new SARibbonColorToolButton::PrivateData(this))
{
    connect(this, &QAbstractButton::clicked, this, &SARibbonColorToolButton::onButtonClicked);
}

SARibbonColorToolButton::SARibbonColorToolButton(QAction* defaultAction, QWidget* parent)
    : SARibbonToolButton(defaultAction, parent), d_ptr(new SARibbonColorToolButton::PrivateData(this))
{
    connect(this, &QAbstractButton::clicked, this, &SARibbonColorToolButton::onButtonClicked);
}

SARibbonColorToolButton::~SARibbonColorToolButton()
{
}

/**
 * @brief 获取按钮维护的颜色
 * @return
 */
QColor SARibbonColorToolButton::color() const
{
    return d_ptr->mColor;
}

/**
 * @brief 设置颜色显示的样式
 * @param s
 */
void SARibbonColorToolButton::setColorStyle(SARibbonColorToolButton::ColorStyle s)
{
    if (d_ptr->mColorStyle == s) {
        return;
    }
    d_ptr->mColorStyle = s;
    if (ColorUnderIcon == s) {
        setIcon(d_ptr->mOldIcon);
    } else {
        d_ptr->mOldIcon = icon();
        setIcon(d_ptr->createColorIcon(d_ptr->mColor, QSize(32, 32)));
    }
    repaint();
}

/**
 * @brief 颜色显示的样式
 * @return
 */
SARibbonColorToolButton::ColorStyle SARibbonColorToolButton::colorStyle() const
{
    return d_ptr->mColorStyle;
}

/**
 * @brief 建立标准的颜色菜单
 * @return
 */
SAColorMenu* SARibbonColorToolButton::setupStandardColorMenu()
{
    setPopupMode(QToolButton::MenuButtonPopup);
    SAColorMenu* m = new SAColorMenu(this);
    m->enableNoneColorAction(true);
    QAction* customColor = m->customColorAction();
    if (customColor) {
        customColor->setIcon(QIcon(":/image/resource/define-color.svg"));
    }
    connect(m, &SAColorMenu::selectedColor, this, &SARibbonColorToolButton::setColor);
    setMenu(m);

    updateRect();
    return m;
}

/**
 * @brief 设置按钮的颜色
 *
 * 此时会生成一个新的icon
 * @note 会发射@sa colorChanged 信号
 * @param c
 */
void SARibbonColorToolButton::setColor(const QColor& c)
{
    if (d_ptr->mColor != c) {
        d_ptr->mColor = c;
        if (ColorFillToIcon == colorStyle()) {
            setIcon(d_ptr->createColorIcon(c, QSize(32, 32)));
        }
        repaint();
        emit colorChanged(c);
    }
}

void SARibbonColorToolButton::onButtonClicked(bool checked)
{
    emit colorClicked(d_ptr->mColor, checked);
}

/**
 * @brief 重写paintIcon函数，把颜色加到icon下面
 * @param p
 * @param opt
 * @param iconDrawRect
 */
void SARibbonColorToolButton::paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect)
{
    if (ColorUnderIcon == colorStyle()) {
        // 有icon
        QPixmap pm = d_ptr->createIconPixmap(opt, iconDrawRect.size());
        style()->drawItemPixmap(&p, iconDrawRect, Qt::AlignCenter, pm);
    } else {
        SARibbonToolButton::paintIcon(p, opt, iconDrawRect);
    }
}

/*** End of inlined file: SARibbonColorToolButton.cpp ***/

/*** Start of inlined file: SARibbonLineWidgetContainer.cpp ***/
#include <QHBoxLayout>

SARibbonLineWidgetContainer::SARibbonLineWidgetContainer(QWidget* par) : QWidget(par), m_innerWidget(nullptr)
{
    m_labelPrefix    = new QLabel(this);
    m_labelSuffix    = new QLabel(this);
    QHBoxLayout* lay = new QHBoxLayout();

    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_labelPrefix);
    lay->addWidget(m_labelSuffix);
    setLayout(lay);
}

void SARibbonLineWidgetContainer::setWidget(QWidget* innerWidget)
{
    QHBoxLayout* lay = static_cast< QHBoxLayout* >(layout());

    if (m_innerWidget) {
        lay->replaceWidget(m_innerWidget, innerWidget);
    } else {
        lay->insertWidget(1, innerWidget);
    }
    m_innerWidget = innerWidget;
}

void SARibbonLineWidgetContainer::setPrefix(const QString& str)
{
    m_labelPrefix->setText(str);
}

void SARibbonLineWidgetContainer::setSuffix(const QString& str)
{
    m_labelSuffix->setText(str);
}

QLabel* SARibbonLineWidgetContainer::labelPrefix() const
{
    return (m_labelPrefix);
}

QLabel* SARibbonLineWidgetContainer::labelSuffix() const
{
    return (m_labelSuffix);
}

/*** End of inlined file: SARibbonLineWidgetContainer.cpp ***/

/*** Start of inlined file: SARibbonActionsManager.cpp ***/
#include <QMap>
#include <QHash>
#include <QDebug>

class SARibbonActionsManager::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonActionsManager)
public:
    PrivateData(SARibbonActionsManager* p);
    void clear();

    QMap< int, QList< QAction* > > mTagToActions;   ///< tag : QList<QAction*>
    QMap< int, QString > mTagToName;                ///< tag对应的名字
    QHash< QString, QAction* > mKeyToAction;        ///< key对应action
    QMap< QAction*, QString > mActionToKey;         ///< action对应key
    QMap< int, SARibbonCategory* > mTagToCategory;  ///< 仅仅在autoRegisteActions函数会有用
    int mSale;  ///< 盐用于生成固定的id，在用户不主动设置key时，id基于msale生成，只要SARibbonActionsManager的调用registeAction顺序不变，生成的id都不变，因为它是基于自增实现的
};

SARibbonActionsManager::PrivateData::PrivateData(SARibbonActionsManager* p) : q_ptr(p), mSale(0)
{
}

void SARibbonActionsManager::PrivateData::clear()
{
    mTagToActions.clear();
    mTagToName.clear();
    mKeyToAction.clear();
    mActionToKey.clear();
    mTagToCategory.clear();
    mSale = 0;
}

SARibbonActionsManager::SARibbonActionsManager(SARibbonBar* bar)
    : QObject(bar), d_ptr(new SARibbonActionsManager::PrivateData(this))
{
    autoRegisteActions(bar);
}

SARibbonActionsManager::~SARibbonActionsManager()
{
}

/**
 * @brief 设置tag对应的名字，通过这个可以得到tag和文本的映射
 * @param tag
 * @param name
 * @note 在支持多语言的环境下，在语言切换时需要重新设置，以更新名字
 */
void SARibbonActionsManager::setTagName(int tag, const QString& name)
{
    d_ptr->mTagToName[ tag ] = name;
}

/**
 * @brief 获取tag对应的中文名字
 * @param tag
 * @return
 */
QString SARibbonActionsManager::tagName(int tag) const
{
    return (d_ptr->mTagToName.value(tag, ""));
}

/**
 * @brief 移除tag
 * @note 注意，这个函数非常耗时
 * @param tag
 */
void SARibbonActionsManager::removeTag(int tag)
{
    QList< QAction* > oldacts = actions(tag);

    // 开始移除
    d_ptr->mTagToActions.remove(tag);
    d_ptr->mTagToName.remove(tag);
    // 开始查找需要移出总表的action
    QList< QAction* > needRemoveAct;
    QList< QAction* > total;

    for (auto i = d_ptr->mTagToActions.begin(); i != d_ptr->mTagToActions.end(); ++i) {
        total += i.value();
    }
    for (QAction* a : qAsConst(oldacts)) {
        if (!total.contains(a)) {
            needRemoveAct.append(a);
        }
    }
    // 从总表移除action
    for (QAction* a : qAsConst(needRemoveAct)) {
        auto i = d_ptr->mActionToKey.find(a);
        if (i != d_ptr->mActionToKey.end()) {
            d_ptr->mKeyToAction.remove(i.value());
            d_ptr->mActionToKey.erase(i);
        }
    }
}

/**
 * @brief 把action注册到管理器中，实现action的管理
 * @param act
 * @param tag tag是可以按照位进行叠加，见 @ref ActionTag 如果
 * 要定义自己的标签，建议定义大于@ref ActionTag::UserDefineActionTag 的值，
 * registeAction的tag是直接记录进去的，如果要多个标签并存，在registe之前先或好tag
 * @param key key是action对应的key，一个key只对应一个action，是查找action的关键
 * ,默认情况为一个QString(),这时key是QAction的objectName
 * @note 同一个action多次注册不同的tag可以通过tag索引到action，但通过action只能索引到最后一个注册的tag
 * @note tag的新增会触发actionTagChanged信号
 */
bool SARibbonActionsManager::registeAction(QAction* act, int tag, const QString& key, bool enableEmit)
{
    if (nullptr == act) {
        return (false);
    }
    QString k = key;

    if (k.isEmpty()) {
        k = QString("id_%1_%2").arg(d_ptr->mSale++).arg(act->objectName());
    }
    if (d_ptr->mKeyToAction.contains(k)) {
        qWarning() << "key: " << k << " have been exist,you can set key in an unique value when use SARibbonActionsManager::registeAction";
        return (false);
    }
    d_ptr->mKeyToAction[ k ]   = act;
    d_ptr->mActionToKey[ act ] = k;
    // 记录tag 对 action
    bool isneedemit = !(d_ptr->mTagToActions.contains(tag));  // 记录是否需要发射信号

    d_ptr->mTagToActions[ tag ].append(act);
    // 绑定槽
    connect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    if (isneedemit && enableEmit) {
        // 说明新增tag
        emit actionTagChanged(tag, false);
    }
    return (true);
}

/**
 * @brief 取消action的注册
 *
 * 如果tag对应的最后一个action被撤销，tag也将一块删除
 * @param act
 * @note tag的删除会触发actionTagChanged信号
 * @note 如果action关联了多个tag，这些tag里的action都会被删除，对应的key也同理
 */
void SARibbonActionsManager::unregisteAction(QAction* act, bool enableEmit)
{
    if (nullptr == act) {
        return;
    }
    // 绑定槽
    disconnect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    removeAction(act, enableEmit);
}

/**
 * @brief 移除action
 *
 * 仅移除内存内容
 * @param act
 * @param enableEmit
 */
void SARibbonActionsManager::removeAction(QAction* act, bool enableEmit)
{
    QList< int > deletedTags;                     // 记录删除的tag，用于触发actionTagChanged
    QMap< int, QList< QAction* > > tagToActions;  ///< tag : QList<QAction*>

    for (auto i = d_ptr->mTagToActions.begin(); i != d_ptr->mTagToActions.end(); ++i) {
        // 把不是act的内容转移到tagToActions和tagToActionKeys中，之后再和m_d里的替换
        auto tmpi = tagToActions.insert(i.key(), QList< QAction* >());
        int count = 0;
        for (int j = 0; j < i.value().size(); ++j) {
            if (i.value()[ j ] != act) {
                tmpi.value().append(act);
                ++count;
            }
        }
        if (0 == count) {
            // 说明这个tag没有内容
            tagToActions.erase(tmpi);
            deletedTags.append(i.key());
        }
    }
    // 删除mKeyToAction
    QString key = d_ptr->mActionToKey.value(act);

    d_ptr->mActionToKey.remove(act);
    d_ptr->mKeyToAction.remove(key);

    // 置换
    d_ptr->mTagToActions.swap(tagToActions);
    // 发射信号
    if (enableEmit) {
        for (int tagdelete : qAsConst(deletedTags)) {
            emit actionTagChanged(tagdelete, true);
        }
    }
}

/**
 * @brief 等同actions
 * @param tag
 * @return
 */
QList< QAction* >& SARibbonActionsManager::filter(int tag)
{
    return (actions(tag));
}

/**
 * @brief 根据tag得到actions
 * @param tag
 * @return
 */
QList< QAction* >& SARibbonActionsManager::actions(int tag)
{
    return (d_ptr->mTagToActions[ tag ]);
}

const QList< QAction* > SARibbonActionsManager::actions(int tag) const
{
    return (d_ptr->mTagToActions[ tag ]);
}

/**
 * @brief 获取所有的标签
 * @return
 */
QList< int > SARibbonActionsManager::actionTags() const
{
    return (d_ptr->mTagToActions.keys());
}

/**
 * @brief 通过key获取action
 * @param key
 * @return 如果没有key，返回nullptr
 */
QAction* SARibbonActionsManager::action(const QString& key) const
{
    return (d_ptr->mKeyToAction.value(key, nullptr));
}

/**
 * @brief 通过action找到key
 * @param act
 * @return 如果找不到，返回QString()
 */
QString SARibbonActionsManager::key(QAction* act) const
{
    return (d_ptr->mActionToKey.value(act, QString()));
}

/**
 * @brief 返回所有管理的action数
 * @return
 */
int SARibbonActionsManager::count() const
{
    return (d_ptr->mKeyToAction.size());
}

/**
 * @brief 返回所有管理的actions
 * @return
 */
QList< QAction* > SARibbonActionsManager::allActions() const
{
    return (d_ptr->mKeyToAction.values());
}

/**
 * @brief 自动加载SARibbonBar的action
 * 此函数会遍历@ref SARibbonBar的父窗口(一般是SARibbonMainWindow)下的所有子object，找到action注册，
 * 并会遍历所有@ref SARibbonCategory,把SARibbonCategory下的action按SARibbonCategory的title name进行分类
 *
 * 此函数会把所有category下的action生成tag并注册，返回的QMap<int, SARibbonCategory *>是记录了category对应的tag
 *
 * 此函数还会把SARibbonBar的父窗口(一般是SARibbonMainWindow)下面的action，但不在任何一个category下的作为NotInRibbonCategoryTag标签注册，默认名字会赋予not
 * in ribbon， 可以通过@ref setTagName 改变
 *
 * @param w
 * @return
 * @note 此函数的调用最好在category设置了标题后调用，因为会以category的标题作为标签的命名
 */
QMap< int, SARibbonCategory* > SARibbonActionsManager::autoRegisteActions(SARibbonBar* bar)
{
    QMap< int, SARibbonCategory* > res;
    // 先遍历SARibbonBar的父窗口(一般是SARibbonMainWindow)下的所有子对象，把所有action找到
    QWidget* parWidget = bar->parentWidget();
    QSet< QAction* > mainwindowActions;
    if (parWidget) {
        for (QObject* o : qAsConst(parWidget->children())) {
            if (QAction* a = qobject_cast< QAction* >(o)) {
                // 说明是action
                if (!a->objectName().isEmpty()) {
                    mainwindowActions.insert(a);
                }
            }
        }
    }
    // 开始遍历每个category，加入action

    if (nullptr == bar) {
        // 非ribbon模式，直接退出
        return (res);
    }
    QSet< QAction* > categoryActions;
    QList< SARibbonCategory* > categorys = bar->categoryPages();
    int tag                              = AutoCategoryDistinguishBeginTag;

    for (SARibbonCategory* c : qAsConst(categorys)) {
        QList< SARibbonPannel* > pannels = c->pannelList();
        for (SARibbonPannel* p : qAsConst(pannels)) {
            categoryActions += autoRegisteWidgetActions(p, tag, false);
        }
        setTagName(tag, c->categoryName());
        res[ tag ] = c;
        ++tag;
    }
    // 找到不在功能区的actions
    QSet< QAction* > notincategory = mainwindowActions - categoryActions;

    for (QAction* a : qAsConst(notincategory)) {
        if (!a->objectName().isEmpty()) {
            registeAction(a, NotInRibbonCategoryTag, a->objectName(), false);
        }
    }
    if (notincategory.size() > 0) {
        setTagName(NotInRibbonCategoryTag, tr("not in ribbon"));
    }
    for (auto i = res.begin(); i != res.end(); ++i) {
        connect(i.value(), &SARibbonCategory::categoryNameChanged, this, &SARibbonActionsManager::onCategoryTitleChanged);
    }
    d_ptr->mTagToCategory = res;
    return (res);
}

/**
 * @brief 自动加载action
 * @param w
 * @param tag
 * @param enableEmit
 * @return 返回成功加入SARibbonActionsManager管理的action
 */
QSet< QAction* > SARibbonActionsManager::autoRegisteWidgetActions(QWidget* w, int tag, bool enableEmit)
{
    QSet< QAction* > res;
    QList< QAction* > was = w->actions();

    for (QAction* a : qAsConst(was)) {
        if (res.contains(a) || a->objectName().isEmpty()) {
            // 重复内容不重复加入
            // 没有object name不加入
            continue;
        }
        if (registeAction(a, tag, a->objectName(), enableEmit)) {
            res.insert(a);
        }
    }
    return (res);
}

/**
 * @brief 根据标题查找action
 * @param text
 * @return
 */
QList< QAction* > SARibbonActionsManager::search(const QString& text)
{
    QList< QAction* > res;

    if (text.isEmpty()) {
        return (res);
    }
    QStringList kws = text.split(" ");

    if (kws.isEmpty()) {
        kws.append(text);
    }
    QList< QAction* > acts = d_ptr->mActionToKey.keys();

    for (const QString& k : qAsConst(kws)) {
        for (auto i = d_ptr->mActionToKey.begin(); i != d_ptr->mActionToKey.end(); ++i) {
            if (i.key()->text().contains(k, Qt::CaseInsensitive)) {
                res.append(i.key());
            }
        }
    }
    return (res);
}

void SARibbonActionsManager::clear()
{
    d_ptr->clear();
}

/**
 * @brief action 被delete时，将触发此槽把管理的action删除
 * @param o
 * @note 这个函数不会触发actionTagChanged信号
 */
void SARibbonActionsManager::onActionDestroyed(QObject* o)
{
    QAction* act = static_cast< QAction* >(o);

    removeAction(act, false);
}

/**
 * @brief autoRegisteActions函数会关联此槽，在标签内容改变时改变tag 对应 文本
 * @param title
 */
void SARibbonActionsManager::onCategoryTitleChanged(const QString& title)
{
    SARibbonCategory* c = qobject_cast< SARibbonCategory* >(sender());

    if (nullptr == c) {
        return;
    }
    int tag = d_ptr->mTagToCategory.key(c, -1);

    if (tag == -1) {
        return;
    }
    setTagName(tag, title);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonActionsModel
////////////////////////////////////////////////////////////////////////////////////////////////////////

class SARibbonActionsManagerModel::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonActionsManagerModel)
public:
    PrivateData(SARibbonActionsManagerModel* p);
    void updateRef();
    int count() const;
    QAction* at(int index);
    bool isNull() const;

public:
    SARibbonActionsManager* mMgr { nullptr };
    int mTag { SARibbonActionsManager::CommonlyUsedActionTag };
    QString mSeatchText;
    QList< QAction* > mActions;
};

SARibbonActionsManagerModel::PrivateData::PrivateData(SARibbonActionsManagerModel* p) : q_ptr(p)
{
}

void SARibbonActionsManagerModel::PrivateData::updateRef()
{
    if (isNull()) {
        return;
    }
    if (!mSeatchText.isEmpty()) {
        mActions = mMgr->search(mSeatchText);
    } else {
        mActions = mMgr->actions(mTag);
    }
}

int SARibbonActionsManagerModel::PrivateData::count() const
{
    if (isNull()) {
        return (0);
    }
    return (mActions.size());
}

QAction* SARibbonActionsManagerModel::PrivateData::at(int index)
{
    if (isNull()) {
        return (nullptr);
    }
    if (index >= count()) {
        return (nullptr);
    }
    return (mActions.at(index));
}

bool SARibbonActionsManagerModel::PrivateData::isNull() const
{
    return (mMgr == nullptr);
}

//===================================================
// SARibbonActionsManagerModel
//===================================================

SARibbonActionsManagerModel::SARibbonActionsManagerModel(QObject* p)
    : QAbstractListModel(p), d_ptr(new SARibbonActionsManagerModel::PrivateData(this))
{
}

SARibbonActionsManagerModel::SARibbonActionsManagerModel(SARibbonActionsManager* m, QObject* p)
    : QAbstractListModel(p), d_ptr(new SARibbonActionsManagerModel::PrivateData(this))
{
    setupActionsManager(m);
}

SARibbonActionsManagerModel::~SARibbonActionsManagerModel()
{
}

int SARibbonActionsManagerModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {  // 非顶层
        return (0);
    }
    // 顶层
    return (d_ptr->count());
}

QVariant SARibbonActionsManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    if (role != Qt::DisplayRole) {
        return (QVariant());
    }
    if (Qt::Horizontal == orientation) {
        return (tr("action name"));
    }
    return (QVariant());
}

Qt::ItemFlags SARibbonActionsManagerModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return (Qt::NoItemFlags);
    }
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

QVariant SARibbonActionsManagerModel::data(const QModelIndex& index, int role) const
{
    QAction* act = indexToAction(index);

    if (nullptr == act) {
        return (QVariant());
    }

    switch (role) {
    case Qt::DisplayRole:
        return (act->text());

    case Qt::DecorationRole:
        return (act->icon());

    default:
        break;
    }
    return (QVariant());
}

void SARibbonActionsManagerModel::setFilter(int tag)
{
    d_ptr->mTag = tag;
    update();
}

void SARibbonActionsManagerModel::update()
{
    beginResetModel();
    d_ptr->updateRef();
    endResetModel();
}

void SARibbonActionsManagerModel::setupActionsManager(SARibbonActionsManager* m)
{
    d_ptr->mMgr     = m;
    d_ptr->mTag     = SARibbonActionsManager::CommonlyUsedActionTag;
    d_ptr->mActions = m->filter(d_ptr->mTag);
    connect(m, &SARibbonActionsManager::actionTagChanged, this, &SARibbonActionsManagerModel::onActionTagChanged);
    update();
}

void SARibbonActionsManagerModel::uninstallActionsManager()
{
    if (!d_ptr->isNull()) {
        disconnect(d_ptr->mMgr, &SARibbonActionsManager::actionTagChanged, this, &SARibbonActionsManagerModel::onActionTagChanged);
        d_ptr->mMgr = nullptr;
        d_ptr->mTag = SARibbonActionsManager::CommonlyUsedActionTag;
    }
    update();
}

QAction* SARibbonActionsManagerModel::indexToAction(QModelIndex index) const
{
    if (!index.isValid()) {
        return (nullptr);
    }
    if (index.row() >= d_ptr->count()) {
        return (nullptr);
    }
    return (d_ptr->at(index.row()));
}

/**
 * @brief 查找
 * @param text
 */
void SARibbonActionsManagerModel::search(const QString& text)
{
    d_ptr->mSeatchText = text;
    update();
}

void SARibbonActionsManagerModel::onActionTagChanged(int tag, bool isdelete)
{
    if (isdelete && (tag == d_ptr->mTag)) {
        d_ptr->mTag = SARibbonActionsManager::UnknowActionTag;
        update();
    } else {
        if (tag == d_ptr->mTag) {
            update();
        }
    }
}

/*** End of inlined file: SARibbonActionsManager.cpp ***/

/*** Start of inlined file: SARibbonLineEdit.cpp ***/
#include <QStyleOption>

SARibbonLineEdit::SARibbonLineEdit(QWidget* parent) : QLineEdit(parent)
{
}

SARibbonLineEdit::SARibbonLineEdit(const QString& text, QWidget* parent) : QLineEdit(text, parent)
{
}

/*** End of inlined file: SARibbonLineEdit.cpp ***/

/*** Start of inlined file: SARibbonCheckBox.cpp ***/
#include <QStyleOption>

SARibbonCheckBox::SARibbonCheckBox(QWidget* parent) : QCheckBox(parent)
{
}

SARibbonCheckBox::SARibbonCheckBox(const QString& text, QWidget* parent) : QCheckBox(text, parent)
{
}

/*** End of inlined file: SARibbonCheckBox.cpp ***/

/*** Start of inlined file: SARibbonComboBox.cpp ***/
SARibbonComboBox::SARibbonComboBox(QWidget* parent) : QComboBox(parent)
{
}

/*** End of inlined file: SARibbonComboBox.cpp ***/

/*** Start of inlined file: SARibbonButtonGroupWidget.cpp ***/
#include <QHBoxLayout>
#include <QDebug>
#include <QMargins>
#include <QChildEvent>
#include <QActionEvent>
#include <QWidgetAction>
#include <QApplication>

//===================================================
// SARibbonButtonGroupWidget::PrivateData
//===================================================
class SARibbonButtonGroupWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonButtonGroupWidget)
public:
    PrivateData(SARibbonButtonGroupWidget* p);
    void init();
    void removeAction(QAction* a);

public:
    QSize mIconSize { 20, 20 };
};

SARibbonButtonGroupWidget::PrivateData::PrivateData(SARibbonButtonGroupWidget* p) : q_ptr(p)
{
}

void SARibbonButtonGroupWidget::PrivateData::init()
{
    QHBoxLayout* layout = new QHBoxLayout(q_ptr);
    // 上下保留一点间隙
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);
    q_ptr->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void SARibbonButtonGroupWidget::PrivateData::removeAction(QAction* a)
{
    QLayout* lay = q_ptr->layout();
    int c        = lay->count();
    QList< QLayoutItem* > willRemoveItems;
    for (int i = 0; i < c; ++i) {
        QLayoutItem* item          = lay->itemAt(i);
        SARibbonControlButton* btn = qobject_cast< SARibbonControlButton* >(item->widget());
        if (nullptr == btn) {
            continue;
        }
        if (a == btn->defaultAction()) {
            willRemoveItems.push_back(item);
        }
    }
    // 从尾部删除
    for (auto i = willRemoveItems.rbegin(); i != willRemoveItems.rend(); ++i) {
        lay->removeItem(*i);
    }
}

//===================================================
// SARibbonButtonGroupWidget
//===================================================

SARibbonButtonGroupWidget::SARibbonButtonGroupWidget(QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonButtonGroupWidget::PrivateData(this))
{
    d_ptr->init();
}

SARibbonButtonGroupWidget::~SARibbonButtonGroupWidget()
{
}

/**
 * @brief 设置图标尺寸
 * @param iconSize
 */
void SARibbonButtonGroupWidget::setIconSize(const QSize& ic)
{
    d_ptr->mIconSize = ic;
    iterate([ ic ](SARibbonControlButton* btn) -> bool {
        btn->setIconSize(ic);
        return true;
    });
}

/**
 * @brief 图标尺寸
 * @return
 */
QSize SARibbonButtonGroupWidget::iconSize() const
{
    return d_ptr->mIconSize;
}

QAction* SARibbonButtonGroupWidget::addAction(QAction* a, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    SARibbonPannel::setActionToolButtonStyleProperty(a, buttonStyle);
    SARibbonPannel::setActionToolButtonPopupModeProperty(a, popMode);
    QWidget::addAction(a);
    return (a);
}

/**
 * @brief 生成action
 * @note action的所有权归SARibbonButtonGroupWidget
 * @param text
 * @param icon
 * @param popMode
 * @return
 */
QAction* SARibbonButtonGroupWidget::addAction(const QString& text, const QIcon& icon, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    QAction* a = new QAction(icon, text, this);
    addAction(a, buttonStyle, popMode);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addMenu(QMenu* menu, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    QAction* a = menu->menuAction();
    addAction(a, buttonStyle, popMode);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addSeparator()
{
    QAction* a = new QAction(this);

    a->setSeparator(true);
    addAction(a);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addWidget(QWidget* w)
{
    QWidgetAction* a = new QWidgetAction(this);

    a->setDefaultWidget(w);
    w->setAttribute(Qt::WA_Hover);
    addAction(a);
    return (a);
}

SARibbonControlButton* SARibbonButtonGroupWidget::actionToRibbonControlToolButton(QAction* action)
{
    SARibbonControlButton* res = nullptr;
    iterate([ &res, action ](SARibbonControlButton* btn) -> bool {
        if (btn->defaultAction() == action) {
            res = btn;
            return false;  // 返回false退出迭代
        }
        return true;
    });
    return (res);
}

QSize SARibbonButtonGroupWidget::sizeHint() const
{
    return (layout()->sizeHint());
}

QSize SARibbonButtonGroupWidget::minimumSizeHint() const
{
    return (layout()->minimumSize());
}

/**
 * @brief 此函数会遍历SARibbonButtonGroupWidget下的所有SARibbonControlButton，执行函数指针(bool(SARibbonControlButton*))，函数指针返回false则停止迭代
 * @param fp
 * @return 中途迭代退出返回false
 */
bool SARibbonButtonGroupWidget::iterate(SARibbonButtonGroupWidget::FpButtonIterate fp)
{
    QLayout* lay = layout();
    int c        = lay->count();
    for (int i = 0; i < c; ++i) {
        auto item = lay->itemAt(i);
        if (!item) {
            continue;
        }
        SARibbonControlButton* btn = qobject_cast< SARibbonControlButton* >(item->widget());
        if (!btn) {
            continue;
        }
        if (!fp(btn)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 处理action的事件
 *
 * 这里处理了ActionAdded，ActionChanged，ActionRemoved三个事件
 * ActionAdded时会生成窗口
 * @param e
 */
void SARibbonButtonGroupWidget::actionEvent(QActionEvent* e)
{
    QAction* a = e->action();
    if (!a) {
        return;
    }

    switch (e->type()) {
    case QEvent::ActionAdded: {
        QWidget* w = nullptr;
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(a)) {
            widgetAction->setParent(this);
            w = widgetAction->requestWidget(this);
            if (w != nullptr) {
                w->setAttribute(Qt::WA_LayoutUsesWidgetRect);
                w->show();
            }
        } else if (a->isSeparator()) {
            SARibbonSeparatorWidget* sp = RibbonSubElementFactory->createRibbonSeparatorWidget(this);
            w                           = sp;
        }
        // 不是widget，自动生成ButtonTyle
        if (!w) {
            SARibbonControlButton* button = RibbonSubElementFactory->createRibbonControlButton(this);
            button->setAutoRaise(true);
            button->setIconSize(d_ptr->mIconSize);
            button->setFocusPolicy(Qt::NoFocus);
            button->setDefaultAction(a);
            // 属性设置
            QToolButton::ToolButtonPopupMode popMode = SARibbonPannel::getActionToolButtonPopupModeProperty(a);
            button->setPopupMode(popMode);
            Qt::ToolButtonStyle buttonStyle = SARibbonPannel::getActionToolButtonStyleProperty(a);
            button->setToolButtonStyle(buttonStyle);
            // 根据QAction的属性设置按钮的大小

            connect(button, &SARibbonToolButton::triggered, this, &SARibbonButtonGroupWidget::actionTriggered);
            w = button;
        }
        layout()->addWidget(w);
        updateGeometry();
    } break;

    case QEvent::ActionChanged: {
        // 让布局重新绘制
        layout()->invalidate();
        updateGeometry();
    } break;

    case QEvent::ActionRemoved: {
        d_ptr->removeAction(e->action());
        updateGeometry();
    } break;

    default:
        break;
    }
    QFrame::actionEvent(e);
}

/*** End of inlined file: SARibbonButtonGroupWidget.cpp ***/

/*** Start of inlined file: SARibbonStackedWidget.cpp ***/
#include <QEventLoop>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>

/**
 * @brief The SARibbonStackedWidgetPrivate class
 */
class SARibbonStackedWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonStackedWidget)
public:
    QEventLoop* eventLoop { nullptr };

public:
    PrivateData(SARibbonStackedWidget* p) : q_ptr(p)
    {
    }

    void init()
    {
        // Parent->setFocusPolicy(Qt::StrongFocus);
    }
};

SARibbonStackedWidget::SARibbonStackedWidget(QWidget* parent)
    : QStackedWidget(parent), d_ptr(new SARibbonStackedWidget::PrivateData(this))
{
    d_ptr->init();
    setNormalMode();
}

SARibbonStackedWidget::~SARibbonStackedWidget()
{
    if (d_ptr->eventLoop) {
        d_ptr->eventLoop->exit();
    }
}

void SARibbonStackedWidget::setPopupMode()
{
    setMouseTracking(true);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFrameShape(QFrame::Panel);
}

bool SARibbonStackedWidget::isPopupMode() const
{
    return (windowFlags() & Qt::Popup);
}

void SARibbonStackedWidget::setNormalMode()
{
    if (d_ptr->eventLoop) {
        d_ptr->eventLoop->exit();
        d_ptr->eventLoop = nullptr;
    }
    setMouseTracking(false);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setFrameShape(QFrame::NoFrame);
}

bool SARibbonStackedWidget::isNormalMode() const
{
    return (!isPopupMode());
}

void SARibbonStackedWidget::exec()
{
    show();
    if (!isPopupMode()) {
        d_ptr->eventLoop = nullptr;
        return;
    }
    QEventLoop event;

    d_ptr->eventLoop = &event;
    event.exec();
    d_ptr->eventLoop = nullptr;
}

/**
 * @brief 类似tabbar的moveTab函数，交换两个窗口的index
 * @param from
 * @param to
 * @note 此操作会触发widgetRemoved(int index)信号
 */
void SARibbonStackedWidget::moveWidget(int from, int to)
{
    QWidget* w = widget(from);

    removeWidget(w);
    insertWidget(to, w);
}

void SARibbonStackedWidget::hideEvent(QHideEvent* e)
{
    if (isPopupMode()) {
        if (d_ptr->eventLoop) {
            d_ptr->eventLoop->exit();
        }
    }
    setFocus();
    emit hidWindow();

    QStackedWidget::hideEvent(e);
}

void SARibbonStackedWidget::resizeEvent(QResizeEvent* e)
{
    QStackedWidget::resizeEvent(e);
    for (int i = 0; i < count(); ++i) {
        if (i == currentIndex()) {
            continue;
        }
        QEvent* layE = new QEvent(QEvent::LayoutRequest);
        QApplication::postEvent(widget(i), layE);
    }
}

/*** End of inlined file: SARibbonStackedWidget.cpp ***/

/*** Start of inlined file: SARibbonSeparatorWidget.cpp ***/
#include <QStylePainter>
#include <QPainter>
#include <QDebug>

SARibbonSeparatorWidget::SARibbonSeparatorWidget(QWidget* parent) : QFrame(parent)
{
    setFrameShape(QFrame::VLine);
    setFrameShadow(QFrame::Plain);
    setLineWidth(1);
    setMidLineWidth(1);
}

QSize SARibbonSeparatorWidget::sizeHint() const
{
    QSize sh = QFrame::sizeHint();
    sh.setWidth(1);
    return sh;
}

/*** End of inlined file: SARibbonSeparatorWidget.cpp ***/

/*** Start of inlined file: SARibbonCtrlContainer.cpp ***/
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <QScreen>
/**
 * @brief The SARibbonCtrlContainerPrivate class
 */
class SARibbonCtrlContainer::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCtrlContainer)
public:
    QWidget* containerWidget { nullptr };
    QHBoxLayout* horizontalLayout { nullptr };
    QLabel* labelPixmap { nullptr };
    QLabel* labelText { nullptr };
    QSize iconSize { 24, 24 };
    QIcon icon;
    PrivateData(SARibbonCtrlContainer* p) : q_ptr(p)
    {
        horizontalLayout = new QHBoxLayout(p);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

        labelPixmap = new QLabel(p);
        labelPixmap->setObjectName(QString::fromUtf8("labelPixmap"));
        labelPixmap->setAlignment(Qt::AlignCenter);
        horizontalLayout->addWidget(labelPixmap);

        labelText = new QLabel(p);
        labelText->setObjectName(QString::fromUtf8("labelText"));
        horizontalLayout->addWidget(labelText);
    }

    void setContainerWidget(QWidget* w)
    {
        if (containerWidget) {
            // 原来有widget
            QWidget* oldwidget = containerWidget;
            takeContainerWidget(oldwidget);
            oldwidget->deleteLater();
        }
        containerWidget = w;
        if (!w) {
            return;
        }
        if (w->parent() != q_ptr) {
            w->setParent(q_ptr);
        }
        // ContainerWidget永远在最右边
        horizontalLayout->insertWidget(horizontalLayout->count(), w);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        w->setSizePolicy(sizePolicy);
    }

    void takeContainerWidget(QWidget* w)
    {
        int i = horizontalLayout->indexOf(w);
        if (i >= 0) {
            QLayoutItem* item = horizontalLayout->takeAt(i);
            delete item;
        }
    }
};

//===================================================
// SARibbonCtrlContainer
//===================================================
SARibbonCtrlContainer::SARibbonCtrlContainer(QWidget* parent)
    : QWidget(parent), d_ptr(new SARibbonCtrlContainer::PrivateData(this))
{
}

SARibbonCtrlContainer::~SARibbonCtrlContainer()
{
}

QWidget* SARibbonCtrlContainer::containerWidget()
{
    return (d_ptr->containerWidget);
}

const QWidget* SARibbonCtrlContainer::containerWidget() const
{
    return (d_ptr->containerWidget);
}

void SARibbonCtrlContainer::setEnableShowIcon(bool b)
{
    d_ptr->labelPixmap->setVisible(b);
}

void SARibbonCtrlContainer::setEnableShowTitle(bool b)
{
    d_ptr->labelText->setVisible(b);
}

/**
 * @brief 判断是否存在容器窗口
 * @return
 */
bool SARibbonCtrlContainer::hasContainerWidget() const
{
    return (d_ptr->containerWidget != nullptr);
}

/**
 * @brief 设置图标
 * @param i
 */
void SARibbonCtrlContainer::setIcon(const QIcon& i)
{
    d_ptr->icon    = i;
    QPixmap pixmap = i.pixmap(d_ptr->iconSize);
    d_ptr->labelPixmap->setPixmap(pixmap);
}

void SARibbonCtrlContainer::setIcon(const QPixmap& pixmap)
{
    d_ptr->labelPixmap->setPixmap(pixmap);
}

/**
 * @brief 获取图标
 * @return
 */
QIcon SARibbonCtrlContainer::icon() const
{
    return d_ptr->icon;
}

/**
 * @brief 设置文字
 * @param t
 */
void SARibbonCtrlContainer::setText(const QString& t)
{
    d_ptr->labelText->setText(t);
}

/**
 * @brief 获取文字
 * @return
 */
QString SARibbonCtrlContainer::text() const
{
    return d_ptr->labelText->text();
}

void SARibbonCtrlContainer::setContainerWidget(QWidget* w)
{
    d_ptr->setContainerWidget(w);
}

/**
 * @brief 获取显示icon的窗口
 * @return
 */
QWidget* SARibbonCtrlContainer::iconWidget() const
{
    return d_ptr->labelPixmap;
}

/*** End of inlined file: SARibbonCtrlContainer.cpp ***/

/*** Start of inlined file: SARibbonQuickAccessBar.cpp ***/
#include <QGuiApplication>

/**
 * @brief The SARibbonQuickAccessBarPrivate class
 */
class SARibbonQuickAccessBar::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonQuickAccessBar)
public:
    PrivateData(SARibbonQuickAccessBar* p);

public:
    SARibbonButtonGroupWidget* groupWidget { nullptr };
};

SARibbonQuickAccessBar::PrivateData::PrivateData(SARibbonQuickAccessBar* p) : q_ptr(p)
{
}

//===================================================
// SARibbonQuickAccessBar
//===================================================
SARibbonQuickAccessBar::SARibbonQuickAccessBar(QWidget* parent)
    : SARibbonCtrlContainer(parent), d_ptr(new SARibbonQuickAccessBar::PrivateData(this))
{
    d_ptr->groupWidget = new SARibbonButtonGroupWidget(this);
    d_ptr->groupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    setContainerWidget(d_ptr->groupWidget);
}

SARibbonQuickAccessBar::~SARibbonQuickAccessBar()
{
}

void SARibbonQuickAccessBar::addSeparator()
{
    d_ptr->groupWidget->addSeparator();
}

void SARibbonQuickAccessBar::addAction(QAction* act, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    d_ptr->groupWidget->addAction(act, buttonStyle, popMode);
}

void SARibbonQuickAccessBar::addWidget(QWidget* w)
{
    d_ptr->groupWidget->addWidget(w);
}

void SARibbonQuickAccessBar::addMenu(QMenu* m, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    d_ptr->groupWidget->addMenu(m, buttonStyle, popMode);
}

/**
 * @brief 获取内部管理的ButtonGroupWidget
 * @return
 */
SARibbonButtonGroupWidget* SARibbonQuickAccessBar::buttonGroupWidget()
{
    return d_ptr->groupWidget;
}

/**
 * @brief 获取内部管理的ButtonGroupWidget
 * @return
 */
const SARibbonButtonGroupWidget* SARibbonQuickAccessBar::buttonGroupWidget() const
{
    return d_ptr->groupWidget;
}

/**
 * @brief 图标尺寸
 * @param s
 */
void SARibbonQuickAccessBar::setIconSize(const QSize& s)
{
    d_ptr->groupWidget->setIconSize(s);
}

/**
 * @brief SARibbonQuickAccessBar::iconSize
 * @return
 */
QSize SARibbonQuickAccessBar::iconSize() const
{
    return d_ptr->groupWidget->iconSize();
}

/*** End of inlined file: SARibbonQuickAccessBar.cpp ***/

/*** Start of inlined file: SARibbonTabBar.cpp ***/
SARibbonTabBar::SARibbonTabBar(QWidget* parent) : QTabBar(parent), m_tabMargin(6, 0, 0, 0)
{
    setExpanding(false);
}

const QMargins& SARibbonTabBar::tabMargin() const
{
    return (m_tabMargin);
}

void SARibbonTabBar::setTabMargin(const QMargins& tabMargin)
{
    m_tabMargin = tabMargin;
}

/*** End of inlined file: SARibbonTabBar.cpp ***/

/*** Start of inlined file: SARibbonControlButton.cpp ***/
SARibbonControlButton::SARibbonControlButton(QWidget* parent) : QToolButton(parent)
{
}

SARibbonControlToolButton::SARibbonControlToolButton(QWidget* parent) : QToolButton(parent)
{
}

/*** End of inlined file: SARibbonControlButton.cpp ***/

/*** Start of inlined file: SARibbonMenu.cpp ***/
#include <QWidgetAction>

SARibbonMenu::SARibbonMenu(QWidget* parent) : QMenu(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

SARibbonMenu::SARibbonMenu(const QString& title, QWidget* parent) : QMenu(title, parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QAction* SARibbonMenu::addRibbonMenu(SARibbonMenu* menu)
{
    return (QMenu::addMenu(menu));
}

SARibbonMenu* SARibbonMenu::addRibbonMenu(const QString& title)
{
    SARibbonMenu* menu = new SARibbonMenu(title, this);

    return (menu);
}

SARibbonMenu* SARibbonMenu::addRibbonMenu(const QIcon& icon, const QString& title)
{
    SARibbonMenu* menu = new SARibbonMenu(title, this);

    menu->setIcon(icon);
    return (menu);
}

QAction* SARibbonMenu::addWidget(QWidget* w)
{
    QWidgetAction* action = new QWidgetAction(this);

    action->setDefaultWidget(w);
    addAction(action);
    return (action);
}

/*** End of inlined file: SARibbonMenu.cpp ***/

/*** Start of inlined file: SARibbonPannelOptionButton.cpp ***/
#include <QAction>

SARibbonPannelOptionButton::SARibbonPannelOptionButton(QWidget* parent) : QToolButton(parent)
{
    setAutoRaise(true);
    setCheckable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize(QSize(10, 10));
    setIcon(QIcon(":/image/resource/ribbonPannelOptionButton.png"));
}

/*** End of inlined file: SARibbonPannelOptionButton.cpp ***/

/*** Start of inlined file: SARibbonPannelItem.cpp ***/
SARibbonPannelItem::SARibbonPannelItem(QWidget* widget)
    : QWidgetItem(widget), rowIndex(-1), columnIndex(-1), action(nullptr), customWidget(false), rowProportion(Large)
{
}

bool SARibbonPannelItem::isEmpty() const
{
    return (action == nullptr || !action->isVisible());
}

/*** End of inlined file: SARibbonPannelItem.cpp ***/

/*** Start of inlined file: SARibbonPannelLayout.cpp ***/
#include <QWidgetAction>
#include <QQueue>

#define SARibbonPannelLayout_DEBUG_PRINT 1
#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

SARibbonPannelLayout::SARibbonPannelLayout(QWidget* p)
    : QLayout(p), m_columnCount(0), m_expandFlag(false), m_dirty(true)
{
    setSpacing(1);
    SARibbonPannel* tb = qobject_cast< SARibbonPannel* >(p);

    if (!tb) {
        return;
    }
}

SARibbonPannelLayout::~SARibbonPannelLayout()
{
    // 参考QToolBarLayout
    while (!m_items.isEmpty()) {
        SARibbonPannelItem* item = m_items.takeFirst();
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item->action)) {
            if (item->customWidget) {
                widgetAction->releaseWidget(item->widget());
            }
        }
        delete item;
    }
}

/**
 * @brief 通过action查找索引，用于actionEvent添加action用
 * @param action
 * @return 没有查到返回-1
 */
int SARibbonPannelLayout::indexByAction(QAction* action) const
{
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)->action == action) {
            return (i);
        }
    }
    return (-1);
}

/**
 * @brief 获取ribbonpannel
 * @return
 */
SARibbonPannel* SARibbonPannelLayout::ribbonPannel() const
{
    return qobject_cast< SARibbonPannel* >(parentWidget());
}

void SARibbonPannelLayout::addItem(QLayoutItem* item)
{
    Q_UNUSED(item);
    qWarning("SARibbonPannelLayout::addItem(): please use addAction() instead");
    return;
}

/**
 * @brief SARibbonPannel主要通过此函数来添加action
 * @param act
 * @param rp 布局策略
 */
void SARibbonPannelLayout::insertAction(int index, QAction* act, SARibbonPannelItem::RowProportion rp)
{
    index                    = qMax(0, index);
    index                    = qMin(m_items.count(), index);
    SARibbonPannelItem* item = createItem(act, rp);

    if (item) {
        m_items.insert(index, item);
        // 标记需要重新计算尺寸
        invalidate();
    }
}

/**
 * @brief 添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
 * @param action
 * @note 要去除OptionAction直接传入nullptr即可
 * @note SARibbonPannel不对QAction的destroy进行关联，如果外部对action进行delete，需要先传入nullptr给addOptionAction
 */
void SARibbonPannelLayout::setOptionAction(QAction* action)
{
    SARibbonPannel* p = ribbonPannel();
    if (!p) {
        return;
    }
    if (action) {
        // 创建option action
        if (nullptr == m_optionActionBtn) {
            m_optionActionBtn = RibbonSubElementFactory->createRibbonPannelOptionButton(p);
            QObject::connect(m_optionActionBtn, &SARibbonToolButton::triggered, p, &SARibbonPannel::actionTriggered);
            // 确保m_optionActionBtn在label之上
            if (m_titleLabel) {
                m_titleLabel->stackUnder(m_optionActionBtn);
            }
        }
        m_optionActionBtn->setDefaultAction(action);
        if (action->icon().isNull()) {
            m_optionActionBtn->setIcon(QIcon(":/image/resource/ribbonPannelOptionButton.png"));
        }
        // 标记需要重新计算尺寸
        invalidate();
    } else {
        // 取消option action
        if (m_optionActionBtn) {
            m_optionActionBtn->hide();
            m_optionActionBtn->deleteLater();
            m_optionActionBtn = nullptr;
            // 标记需要重新计算尺寸
            invalidate();
        }
    }
}

/**
 * @brief 判断是否存在OptionAction
 * @return 存在返回true
 */
bool SARibbonPannelLayout::isHaveOptionAction() const
{
    return (m_optionActionBtn != nullptr);
}

QLayoutItem* SARibbonPannelLayout::itemAt(int index) const
{
    if ((index < 0) || (index >= m_items.count())) {
        return (nullptr);
    }
    return (m_items.at(index));
}

QLayoutItem* SARibbonPannelLayout::takeAt(int index)
{
    if ((index < 0) || (index >= m_items.count())) {
        return (nullptr);
    }
    SARibbonPannelItem* item = m_items.takeAt(index);

    QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item->action);

    if ((widgetAction != 0) && item->customWidget) {
        widgetAction->releaseWidget(item->widget());
    } else {
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
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    if (SARibbonPannel* pannel = ribbonPannel()) {
        qDebug() << "| |-SARibbonPannelLayout sizeHint,sizeHint = " << m_sizeHint;
    }
#endif
    return (m_sizeHint);
}

/**
 * @brief 通过action获取SARibbonPannelItem
 * @param action
 * @return 如果没有返回nullptr
 */
SARibbonPannelItem* SARibbonPannelLayout::pannelItem(QAction* action) const
{
    int index = indexByAction(action);

    if (index >= 0) {
        return (m_items[ index ]);
    }
    return (nullptr);
}

/**
 * @brief 获取最后一个添加的item
 * @return 如果没有返回nullptr
 */
SARibbonPannelItem* SARibbonPannelLayout::lastItem() const
{
    if (m_items.isEmpty()) {
        return (nullptr);
    }
    return (m_items.last());
}

/**
 * @brief 获取最后生成的窗口
 * @return 如果无窗口或者item为空，返回nullptr
 */
QWidget* SARibbonPannelLayout::lastWidget() const
{
    SARibbonPannelItem* item = lastItem();

    if (item) {
        return (item->widget());
    }
    return (nullptr);
}

/**
 * @brief 移动两个item
 * @param from
 * @param to
 * @note 移动完后所有都失效，需要重新布局
 */
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

/**
 * @brief 判断是否需要重新布局
 * @return
 */
bool SARibbonPannelLayout::isDirty() const
{
    return (m_dirty);
}

void SARibbonPannelLayout::updateGeomArray()
{
    updateGeomArray(geometry());
}

/**
 * @brief 布局所有action
 */
void SARibbonPannelLayout::doLayout()
{
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    if (SARibbonPannel* pannel = ribbonPannel()) {
        qDebug() << "| |-SARibbonPannelLayout layoutActions,pannel name = " << pannel->pannelName();
    }
#endif
    if (isDirty()) {
        updateGeomArray();
    }
    QList< QWidget* > showWidgets, hideWidgets;
    SARibbonPannel* pannel = ribbonPannel();
    for (SARibbonPannelItem* item : qAsConst(m_items)) {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
        } else {
            item->setGeometry(item->itemWillSetGeometry);
            showWidgets << item->widget();
        }
    }

    // 不在上面那里进行show和hide因为这会触发SARibbonPannelLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget* w : qAsConst(showWidgets)) {
        if (!w->isVisible())
            w->show();
    }
    for (QWidget* w : qAsConst(hideWidgets)) {
        if (w->isVisible())
            w->hide();
    }
    // 布局label
    if (m_titleLabel) {
        if (isEnableShowPannelTitle()) {
            m_titleLabel->setGeometry(m_titleLabelGeometry);
            if (!m_titleLabel->isVisibleTo(pannel)) {
                m_titleLabel->show();
            }
        } else {
            if (m_titleLabel->isVisibleTo(pannel)) {
                m_titleLabel->hide();
            }
        }
    }
    // 布局m_optionActionBtn
    if (m_optionActionBtn) {
        m_optionActionBtn->setGeometry(m_optionActionBtnGeometry);
        m_optionActionBtn->setIconSize(QSize(m_optionActionBtnGeometry.width(), m_optionActionBtnGeometry.height()));
    }
}

/**
 * @brief 把action转换为item
 *
 * 此函数参考QToolBarItem *QToolBarLayout::createItem(QAction *action)
 *
 * 对于普通QAction，此函数会创建SARibbonToolButton，SARibbonToolButton的类型参考SARibbonPannelItem::RowProportion，
 * @param action
 * @param rp 行高占比情况
 * @return 转换的SARibbonPannelItem
 * @note 每个SARibbonPannelItem最终都会携带一个widget，传入的是QWidgetAction的话，会直接使用QWidgetAction带的widget，
 * 否则会内部生成一个SARibbonToolButton
 *
 */
SARibbonPannelItem* SARibbonPannelLayout::createItem(QAction* action, SARibbonPannelItem::RowProportion rp)
{
    bool customWidget      = false;
    QWidget* widget        = nullptr;
    SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(parentWidget());

    if (!pannel) {
        // 在没有pannel这个函数会返回nullptr
        return (nullptr);
    }
    if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action)) {
        widget = widgetAction->requestWidget(pannel);
        if (widget != nullptr) {
            widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            customWidget = true;  // 标记为true，在移除的时候是不会对这个窗口进行删除，false默认会进行删除如SARibbonSeparatorWidget和SARibbonToolButton
        }
    } else if (action->isSeparator()) {
        SARibbonSeparatorWidget* sep = RibbonSubElementFactory->createRibbonSeparatorWidget(pannel);
        widget                       = sep;
    }
    // 不是widget，自动生成SARibbonToolbutton
    if (!widget) {
        SARibbonToolButton::RibbonButtonType buttonType = ((rp == SARibbonPannelItem::Large) ? SARibbonToolButton::LargeButton
                                                                                             : SARibbonToolButton::SmallButton);

        SARibbonToolButton* button = RibbonSubElementFactory->createRibbonToolButton(pannel);
        button->setFocusPolicy(Qt::NoFocus);
        button->setButtonType(buttonType);
        button->setDefaultAction(action);
        // 属性设置
        QToolButton::ToolButtonPopupMode popMode = SARibbonPannel::getActionToolButtonPopupModeProperty(action);
        button->setPopupMode(popMode);
        // 根据QAction的属性设置按钮的大小

        QObject::connect(button, &SARibbonToolButton::triggered, pannel, &SARibbonPannel::actionTriggered);
        widget = button;
    }
    // 这时总会有widget
    widget->hide();
    SARibbonPannelItem* result = new SARibbonPannelItem(widget);

    result->rowProportion = rp;
    result->customWidget  = customWidget;
    result->action        = action;
    return (result);
}

/**
 * @brief 更新尺寸
 */
void SARibbonPannelLayout::updateGeomArray(const QRect& setrect)
{
    SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(parentWidget());

    if (!pannel) {
        return;
    }

    const int height    = setrect.height();
    const QMargins& mag = contentsMargins();
    const int spacing   = this->spacing();
    int x               = mag.left();
    const int yBegin    = mag.top();
    int titleH          = (m_titleHeight >= 0) ? m_titleHeight : 0;  // 防止负数影响
    int titleSpace      = (m_titleHeight >= 0) ? m_titleSpace : 0;  // 对于没有标题的情况，spacing就不生效
    if (!isEnableShowPannelTitle()) {
        titleH     = 0;
        titleSpace = 0;
    }
    // 获取pannel的布局模式 3行或者2行
    //  rowcount 是ribbon的行，有2行和3行两种
    const short rowCount = (pannel->pannelLayoutMode() == SARibbonPannel::ThreeRowMode) ? 3 : 2;
    // largeHeight是对应large占比的高度
    const int largeHeight = height - mag.bottom() - mag.top() - titleH - titleSpace;
    const int yTitleBegin = height - mag.bottom() - titleH;
    m_largeHeight         = largeHeight;
    // 计算smallHeight的高度
    const int smallHeight = (largeHeight - (rowCount - 1) * spacing) / rowCount;
    // Medium行的y位置
    const int yMediumRow0 = (2 == rowCount) ? yBegin : (yBegin + ((largeHeight - 2 * smallHeight) / 3));
    const int yMediumRow1 = (2 == rowCount) ? (yBegin + smallHeight + spacing)
                                            : (yBegin + ((largeHeight - 2 * smallHeight) / 3) * 2 + smallHeight);
    // Small行的y位置
    const int ySmallRow0 = yBegin;
    const int ySmallRow1 = yBegin + smallHeight + spacing;
    const int ySmallRow2 = yBegin + 2 * (smallHeight + spacing);
    // row用于记录下个item应该属于第几行，item->rowIndex用于记录当前处于第几行，
    // item->rowIndex主要用于SARibbonPannelItem::Medium
    short row  = 0;
    int column = 0;
    // 记录每列最大的宽度
    int columMaxWidth = 0;
    // 记录总宽度
    int totalWidth = 0;

    int itemCount = m_items.count();

#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    QString debug_print__log__;
#endif
    // 本列第一、二行占比
    SARibbonPannelItem::RowProportion thisColumnRP0 = SARibbonPannelItem::None;
    SARibbonPannelItem* lastGeomItem                = nullptr;  // 记录最后一个设置位置的item
    for (int i = 0; i < itemCount; ++i) {
        SARibbonPannelItem* item = m_items.at(i);
        if (item->isEmpty()) {
            // 如果是hide就直接跳过
            item->rowIndex    = -1;
            item->columnIndex = -1;
            continue;
        }

        QSize hint = item->sizeHint();
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
        if (SARibbonToolButton* tb = qobject_cast< SARibbonToolButton* >(item->widget())) {
            auto ss__ = tb->sizeHint();
            debug_print__log__ += QString("| | |-[%1]SARibbonToolButton.sizeHint=(%2,%3),ButtonText=%4\n")
                                          .arg(i)
                                          .arg(ss__.width())
                                          .arg(ss__.height())
                                          .arg(tb->text());
        }
#endif
        Qt::Orientations exp = item->expandingDirections();
        if (item->widget()) {
            // 有窗口是水平扩展，则标记为扩展
            if ((item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag)) {
                m_expandFlag = true;
            }
        }
        SARibbonPannelItem::RowProportion rp = item->rowProportion;
        if (SARibbonPannelItem::None == rp) {
            // 为定义行占比但是垂直扩展，就定义为Large占比，否则就是small占比
            if (exp & Qt::Vertical) {
                rp = SARibbonPannelItem::Large;
            } else {
                rp = SARibbonPannelItem::Small;
            }
        }
        // 开始根据占比和layoutmode来布局
        switch (rp) {
        case SARibbonPannelItem::Large: {
            // ！！在Large，如果不是处于新列的第一行，就需要进行换列处理
            // 把large一直设置在下一列的开始
            if (row != 0) {
                x += (columMaxWidth + spacing);
                ++column;
            }
            //
            item->rowIndex            = 0;
            item->columnIndex         = column;
            item->itemWillSetGeometry = QRect(x, yBegin, hint.width(), largeHeight);
            columMaxWidth             = hint.width();
            // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
            x += (columMaxWidth + spacing);
            row           = 0;
            columMaxWidth = 0;
            ++column;
        } break;

        case SARibbonPannelItem::Medium: {
            // 2行模式下Medium和small等价
            if (2 == rowCount) {
                if (0 == row) {
                    item->rowIndex            = 0;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0             = SARibbonPannelItem::Medium;
                    columMaxWidth             = hint.width();
                    // 下个row为1
                    row = 1;
                    // x不变
                } else {
                    item->rowIndex            = 1;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    // 和上个进行比较得到最长宽度
                    columMaxWidth = qMax(columMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                }
            } else {
                // 3行模式
                if (0 == row) {
                    item->rowIndex            = 0;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0             = SARibbonPannelItem::Medium;
                    columMaxWidth             = hint.width();
                    row                       = 1;
                    // x不变
                } else if (1 == row) {
                    item->rowIndex            = 1;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    columMaxWidth             = qMax(columMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    // 这种模式一般情况会发生在当前列前两行是Small，添加了一个Medium
                    // 这时需要先换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    ++column;
                    // 换列后此时等价于0 == row
                    item->rowIndex            = 0;
                    item->columnIndex         = column;
                    item->itemWillSetGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0             = SARibbonPannelItem::Medium;
                    columMaxWidth             = hint.width();
                    row                       = 1;
                }
            }
        } break;

        case SARibbonPannelItem::Small: {
            if (0 == row) {
                // 第一行
                item->rowIndex            = 0;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow0, hint.width(), smallHeight);
                thisColumnRP0             = SARibbonPannelItem::Small;
                columMaxWidth             = hint.width();
                // 下个row为1
                row = 1;
                // x不变
            } else if (1 == row) {
                // 第二行
                item->rowIndex            = 1;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow1, hint.width(), smallHeight);
                if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium
                    item->itemWillSetGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                }
                // 和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                // 这里要看两行还是三行，确定是否要换列
                if (2 == rowCount) {
                    // 两行模式，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    // 三行模式，继续增加行数
                    row = 2;
                    // x不变
                }
                if ((3 == rowCount) && (SARibbonPannelItem::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                }
            } else {
                // 第三行
                item->rowIndex            = 2;
                item->columnIndex         = column;
                item->itemWillSetGeometry = QRect(x, ySmallRow2, hint.width(), smallHeight);
                // 和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                x += (columMaxWidth + spacing);
                row           = 0;
                columMaxWidth = 0;
                ++column;
            }
        } break;

        default:
            // 不可能出现
            break;
        }
        lastGeomItem = item;
    }
    // 最后一个元素，更新列数
    //  2022-06-20 此句本来在循环里面，如果最后一个元素隐藏，会导致无法到达此判断导致异常
    if (lastGeomItem) {  // 最后一个元素，更新totalWidth
        if (lastGeomItem->columnIndex != column) {
            // 说明最后一个元素处于最后位置，触发了换列，此时真实列数需要减1，直接等于column索引
            m_columnCount = column;
            // 由于最后一个元素触发了换列，x值是新一列的位置，直接作为totalWidth要减去已经加入的spacing
            totalWidth = x - spacing + mag.right();
        } else {
            // 说明最后一个元素处于非最后位置，没有触发下一个换列，此时真实列数等于column索引+1
            m_columnCount = column + 1;
            // 由于最后一个元素未触发换列，需要计算totalWidth
            totalWidth = x + columMaxWidth + mag.right();
        }
    }

    // 在设置完所有窗口后，再设置扩展属性的窗口
    if (totalWidth < setrect.width() && (setrect.width() - totalWidth) > 10) {
        // 说明可以设置扩展属性的窗口
        recalcExpandGeomArray(setrect);
    }
    // 布局label
    bool isTitleWidthThanPannel = false;
    if (isEnableShowPannelTitle()) {
        m_titleLabelGeometry.setRect(mag.left(), yTitleBegin, setrect.width() - mag.left() - mag.right(), titleH);
        // 这里要确认标题宽度是否大于totalWidth，如果大于，则要把标题的宽度作为totalwidth
        QFontMetrics fm = m_titleLabel->fontMetrics();
        int textWidth   = SA_FONTMETRICS_WIDTH(fm, pannel->pannelName());
        textWidth += 4;
        if (totalWidth < textWidth) {
            totalWidth             = textWidth;
            isTitleWidthThanPannel = true;  // 说明标题的长度大于按钮布局的长度
        }
    }
    // 布局optionActionButton

    if (isHaveOptionAction()) {
        QSize optBtnSize = optionActionButtonSize();
        if (isEnableShowPannelTitle()) {
            // 有标题
            m_optionActionBtnGeometry.setRect(m_titleLabelGeometry.right() - m_titleLabelGeometry.height(),
                                              m_titleLabelGeometry.y(),
                                              m_titleLabelGeometry.height(),
                                              m_titleLabelGeometry.height());

            // 特殊情况，如果pannel的标题长度大于totalWidth，那么说明totalWidth比较短
            // 这时候，optionActionBtn的宽度要加上到标题宽度上
            if (isTitleWidthThanPannel) {
                // 由于文字是居中对齐，因此要扩展2个按钮的宽度
                totalWidth += (2 * titleH);
            }
        } else {
            // 无标题
            m_optionActionBtnGeometry.setRect(setrect.right() - optBtnSize.width() - mag.right(),
                                              setrect.bottom() - optBtnSize.height() - mag.bottom(),
                                              optBtnSize.width(),
                                              optBtnSize.height());
            totalWidth += optBtnSize.width();
        }
    }
    // 刷新sizeHint
    int heightHint   = SARibbonPannel::pannelHeightHint(pannel->fontMetrics(), pannel->pannelLayoutMode(), titleH);
    this->m_sizeHint = QSize(totalWidth, heightHint);
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| |-SARibbonPannelLayout updateGeomArray(" << setrect << "),pannel name = " << pannel->pannelName()
             << "\n| | |-size hint =" << this->m_sizeHint  //
             << "\n| | |-totalWidth=" << totalWidth        //
             << "\n| | |-last x=" << x                     //
             << "\n| | |-columMaxWidth=" << columMaxWidth  //
             << "\n| | |-spacing=" << spacing              //
             << "\n| | |-mag=" << mag                      //
             << "\n| | |-largeHeight=" << largeHeight      //
             << "\n| | |-smallHeight=" << smallHeight      //
            ;
    qDebug().noquote() << debug_print__log__;
#endif
}

void SARibbonPannelLayout::recalcExpandGeomArray(const QRect& setrect)
{
    // 计算能扩展的尺寸
    int expandwidth = setrect.width() - this->m_sizeHint.width();

    if (expandwidth <= 0) {
        // 没有必要设置
        return;
    }
    // 列扩展信息
    struct _columnExpandInfo
    {
        int oldColumnWidth      = 0;   ///< 原来的列宽
        int columnMaximumWidth  = -1;  ///< 列的最大宽度
        int columnExpandedWidth = 0;   ///< 扩展后列的宽度
        QList< SARibbonPannelItem* > expandItems;
    };
    // 此变量用于记录可以水平扩展的列和控件，在布局结束后，如果还有空间，就把水平扩展的控件进行扩展
    QMap< int, _columnExpandInfo > columnExpandInfo;

    for (SARibbonPannelItem* item : qAsConst(m_items)) {
        if ((!item->isEmpty()) && item->expandingDirections() & Qt::Horizontal) {
            // 只获取可见的
            QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.find(item->columnIndex);
            if (i == columnExpandInfo.end()) {
                i = columnExpandInfo.insert(item->columnIndex, _columnExpandInfo());
            }
            i.value().expandItems.append(item);
        }
    }
    if (columnExpandInfo.size() <= 0) {
        // 没有需要扩展的就退出
        return;
    }
    // 获取完可扩展的列和控件后，计算对应的列的尺寸
    // 计算能扩展的尺寸
    int oneColCanexpandWidth = expandwidth / columnExpandInfo.size();

    for (QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.begin(); i != columnExpandInfo.end();) {
        int& oldColumnWidth     = i.value().oldColumnWidth;
        int& columnMaximumWidth = i.value().columnMaximumWidth;
        this->columnWidthInfo(i.key(), oldColumnWidth, columnMaximumWidth);
        if ((oldColumnWidth <= 0) || (oldColumnWidth > columnMaximumWidth)) {
            // 如果小于0说明没有这个列，这种属于异常，删除继续
            //  oldColumnWidth > columnMaximumWidth也是异常
            i = columnExpandInfo.erase(i);
            continue;
        }
        // 开始调整
        int colwidth = oneColCanexpandWidth + oldColumnWidth;  // 先扩展了
        if (colwidth >= columnMaximumWidth) {
            // 过最大宽度要求
            i.value().columnExpandedWidth = columnMaximumWidth;
        } else {
            i.value().columnExpandedWidth = colwidth;
        }
        ++i;
    }
    // 从新调整尺寸
    // 由于会涉及其他列的变更，因此需要所有都遍历一下
    for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
        int moveXLen = i.value().columnExpandedWidth - i.value().oldColumnWidth;
        for (SARibbonPannelItem* item : qAsConst(m_items)) {
            if (item->isEmpty() || (item->columnIndex < i.key())) {
                // 之前的列不用管
                continue;
            }
            if (item->columnIndex == i.key()) {
                // 此列的扩展
                if (i.value().expandItems.contains(item)) {
                    // 此列需要扩展的item才扩展尺寸
                    item->itemWillSetGeometry.setWidth(i.value().columnExpandedWidth);
                } else {
                    // 此列不扩展的模块保持原来的尺寸
                    continue;
                }
            } else {
                // 后面的移动
                item->itemWillSetGeometry.moveLeft(item->itemWillSetGeometry.x() + moveXLen);
            }
        }
    }
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| |-SARibbonPannelLayout recalcExpandGeomArray(" << setrect
             << ") pannelName=" << ribbonPannel()->pannelName()  //
             << ",expandwidth=" << expandwidth                   //
            ;
#endif
}

/**
 * @brief 根据列数，计算窗口的宽度，以及最大宽度
 * @param colindex
 * @param width 如果传入没有这个列，返回-1
 * @param maximum 如果传入没有这个列，返回-1
 */
void SARibbonPannelLayout::columnWidthInfo(int colindex, int& width, int& maximum) const
{
    width   = -1;
    maximum = -1;
    for (SARibbonPannelItem* item : m_items) {
        if (!item->isEmpty() && (item->columnIndex == colindex)) {
            width   = qMax(width, item->itemWillSetGeometry.width());
            maximum = qMax(maximum, item->widget()->maximumWidth());
        }
    }
}

SARibbonPannelLabel* SARibbonPannelLayout::pannelTitleLabel() const
{
    return m_titleLabel;
}

/**
 * @brief 获取optionAction 按钮尺寸
 * @return
 */
QSize SARibbonPannelLayout::optionActionButtonSize() const
{
    return (isEnableShowPannelTitle() ? QSize(12, 12) : QSize(m_titleHeight, m_titleHeight));
}

void SARibbonPannelLayout::setPannelTitleLabel(SARibbonPannelLabel* newTitleLabel)
{
    m_titleLabel = newTitleLabel;
    // 确保m_optionActionBtn在label之上
    if (m_optionActionBtn) {
        if (m_titleLabel) {
            m_titleLabel->stackUnder(m_optionActionBtn);
        }
    }
}

/**
 * @brief 标题区域和按钮的间隔
 * @return
 */
int SARibbonPannelLayout::pannelTitleSpace() const
{
    return m_titleSpace;
}

/**
 * @brief 设置标题区域和按钮的间隔
 * @param newTitleSpace
 */
void SARibbonPannelLayout::setPannelTitleSpace(int newTitleSpace)
{
    if (m_titleSpace == newTitleSpace) {
        return;
    }
    m_titleSpace = newTitleSpace;
    invalidate();
}

/**
 * @brief 标题高度
 * @return
 */
int SARibbonPannelLayout::pannelTitleHeight() const
{
    return m_titleHeight;
}

/**
 * @brief 设置标题高度
 * @param newTitleHeight
 */
void SARibbonPannelLayout::setPannelTitleHeight(int newTitleHeight)
{
    if (m_titleHeight == newTitleHeight) {
        return;
    }
    m_titleHeight = newTitleHeight;
    invalidate();
}

/**
 * @brief 判断是否存在标题
 * @return
 */
bool SARibbonPannelLayout::isEnableShowPannelTitle() const
{
    return m_enableShowTitle;
}

/**
 * @brief 设置显示标题
 * @param on
 */
void SARibbonPannelLayout::setEnableShowPannelTitle(bool on)
{
    if (m_enableShowTitle == on) {
        return;
    }
    m_enableShowTitle = on;
    invalidate();
}

/**
 * @brief 大按钮的高度
 * @return
 */
int SARibbonPannelLayout::largeButtonHeight() const
{
    return m_largeHeight;
}

void SARibbonPannelLayout::setGeometry(const QRect& rect)
{
    QRect old = geometry();
    if (old == rect) {
        return;
    }
#if SARibbonPannelLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "| |----->SARibbonPannelLayout.setGeometry(" << rect << "(" << ribbonPannel()->pannelName() << ")=======";
#endif
    QLayout::setGeometry(rect);
    m_dirty = false;
    updateGeomArray(rect);
    doLayout();
}

/*** End of inlined file: SARibbonPannelLayout.cpp ***/

/*** Start of inlined file: SARibbonPannel.cpp ***/
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QGridLayout>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QResizeEvent>
#include <QWidgetAction>

#ifndef SARibbonPannel_DEBUG_PRINT
#define SARibbonPannel_DEBUG_PRINT 0
#endif

#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)
//===============================================================
// SARibbonPannelLabel
//===============================================================

SARibbonPannelLabel::SARibbonPannelLabel(QWidget* parent) : QLabel(parent)
{
}

//===============================================================
// SARibbonPannel::PrivateData
//===============================================================

class SARibbonPannel::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonPannel)
public:
    PrivateData(SARibbonPannel* p);
    // 根据m_pannelLayoutMode返回gridLayout应该增加的行数
    int rowadded();
    void createLayout();
    // 获取layout
    SARibbonPannelLayout* pannelLayout() const;
    // 返回最后一个添加的action对应的button，前提是最后一个是toolbutton，否则返回nullptr
    SARibbonToolButton* lastAddActionButton();
    // 重置labe的字体，这个主要是为了让pannel的标题字体能适应标题高度
    void resetTitleLabelFont();
    // 标题
    QString pannelName() const;
    void setPannelName(const QString& title);

public:
    bool m_isCanCustomize { true };                                                        ///< 记录是否可自定义
    SARibbonPannel::PannelLayoutMode m_pannelLayoutMode { SARibbonPannel::ThreeRowMode };  ///< pannel的布局模式，默认为3行模式ThreeRowMode
    SARibbonPannelOptionButton* m_optionActionButton { nullptr };  ///< 标题栏的y距离
    SARibbonPannelLabel* m_label { nullptr };
};

SARibbonPannel::PrivateData::PrivateData(SARibbonPannel* p) : q_ptr(p)
{
    createLayout();
}

int SARibbonPannel::PrivateData::rowadded()
{
    switch (m_pannelLayoutMode) {
    case SARibbonPannel::ThreeRowMode:
        return (2);

    case SARibbonPannel::TwoRowMode:
        return (3);

    default:
        break;
    }
    return (2);
}

void SARibbonPannel::PrivateData::createLayout()
{
    m_label = new SARibbonPannelLabel(q_ptr);
    m_label->setAlignment(Qt::AlignCenter);
    SARibbonPannelLayout* lay = new SARibbonPannelLayout(q_ptr);
    lay->setPannelTitleLabel(m_label);
    lay->setSpacing(2);
    lay->setContentsMargins(2, 2, 2, 2);
}

SARibbonPannelLayout* SARibbonPannel::PrivateData::pannelLayout() const
{
    return qobject_cast< SARibbonPannelLayout* >(q_ptr->layout());
}

SARibbonToolButton* SARibbonPannel::PrivateData::lastAddActionButton()
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return (qobject_cast< SARibbonToolButton* >(lay->lastWidget()));
    }
    return (nullptr);
}

/**
 * @brief 重置label的字体
 *
 * @note 此函数必须在布局设置后调用
 */
void SARibbonPannel::PrivateData::resetTitleLabelFont()
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        int h   = lay->pannelTitleHeight();
        QFont f = q_ptr->font();
        f.setPixelSize(h * 0.8);
        if (m_label) {
            m_label->setFont(f);
        }
    }
}

QString SARibbonPannel::PrivateData::pannelName() const
{
    if (m_label) {
        return m_label->text();
    }
    return (QString());
}

void SARibbonPannel::PrivateData::setPannelName(const QString& title)
{
    if (m_label) {
        m_label->setText(title);
    }
}

//==================================================
// SARibbonPannel
//==================================================

SARibbonPannel::SARibbonPannel(QWidget* parent) : QFrame(parent), d_ptr(new SARibbonPannel::PrivateData(this))
{
    setPannelLayoutMode(ThreeRowMode);
}

SARibbonPannel::SARibbonPannel(const QString& name, QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonPannel::PrivateData(this))
{
    setPannelLayoutMode(ThreeRowMode);
    setPannelName(name);
}

SARibbonPannel::~SARibbonPannel()
{
}

/**
 * @brief 把action的行属性设置进action中，action自身携带了行属性
 * @param action
 * @param rp
 */
void SARibbonPannel::setActionRowProportionProperty(QAction* action, SARibbonPannelItem::RowProportion rp)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_RowProportion, static_cast< int >(rp));
}

/**
 * @brief 获取action的行属性
 * @param action
 * @return
 */
SARibbonPannelItem::RowProportion SARibbonPannel::getActionRowProportionProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_RowProportion).toInt(&isok);

    if (isok) {
        return (static_cast< SARibbonPannelItem::RowProportion >(r));
    }
    return (SARibbonPannelItem::Large);
}

/**
 * @brief 设置action的ToolButtonPopupMode属性
 * @param action
 * @param popMode
 */
void SARibbonPannel::setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_ToolButtonPopupMode, static_cast< int >(popMode));
}

/**
 * @brief 获取action的ToolButtonPopupMode属性
 * @param action
 * @return
 */
QToolButton::ToolButtonPopupMode SARibbonPannel::getActionToolButtonPopupModeProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_ToolButtonPopupMode).toInt(&isok);

    if (isok) {
        return (static_cast< QToolButton::ToolButtonPopupMode >(r));
    }
    return (QToolButton::InstantPopup);
}

/**
 * @brief 设置action的ToolButtonStyle属性
 * @param action
 * @param buttonStyle
 */
void SARibbonPannel::setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_ToolButtonStyle, static_cast< int >(buttonStyle));
}

/**
 * @brief 获取action的ToolButtonStyle属性
 * @param action
 * @return
 */
Qt::ToolButtonStyle SARibbonPannel::getActionToolButtonStyleProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_ToolButtonStyle).toInt(&isok);

    if (isok) {
        return (static_cast< Qt::ToolButtonStyle >(r));
    }
    return (Qt::ToolButtonIconOnly);
}

/**
 * @brief 添加action
 *
 * action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
 * @param action
 * @param rp 指定action的行占比
 * @param popMode 菜单弹出样式
 */
void SARibbonPannel::addAction(QAction* action, SARibbonPannelItem::RowProportion rp)
{
    Q_CHECK_PTR(action);
    setActionRowProportionProperty(action, rp);
    addAction(action);
}

/**
 * @brief 添加一个action
 * @param act
 * @param popMode 按钮的样式
 * @param rp action在pannel中的占位情况，默认是大图标
 */
void SARibbonPannel::addAction(QAction* act, QToolButton::ToolButtonPopupMode popMode, SARibbonPannelItem::RowProportion rp)
{
    Q_CHECK_PTR(act);
    setActionRowProportionProperty(act, rp);
    setActionToolButtonPopupModeProperty(act, popMode);
    addAction(act);
}

/**
   @brief 添加大图标

   action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
   @param action
 */
void SARibbonPannel::addLargeAction(QAction* action)
{
    addAction(action, SARibbonPannelItem::Large);
}

/**
   @brief 在三栏模式下，强制加为2栏action
   @note 在两行模式下，Medium和Small等价
   主要应用在ThreeRowMode下

   action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
   @param action
 */
void SARibbonPannel::addMediumAction(QAction* action)
{
    addAction(action, SARibbonPannelItem::Medium);
}

/**
   @brief 添加小图标

   action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
   @param action
 */
void SARibbonPannel::addSmallAction(QAction* action)
{
    addAction(action, SARibbonPannelItem::Small);
}

void SARibbonPannel::addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPannelItem::Small);
}

void SARibbonPannel::addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPannelItem::Large);
}

void SARibbonPannel::addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPannelItem::Medium);
}

/**
 * @brief 生成并添加一个action
 *
 * 如果不对此action做操作，SARibbonPannel将管理此action
 *
 * @note action的父对象将设置为SARibbonPannel，SARibbonPannel在删除时将会删除子对象，会把这个action也删除，
 * 如果不想此action也删除，需要对action重新设置父对象
 *
 * @param text action的文字
 * @param icon action的图标
 * @param popMode 按钮的样式
 * @param rp action在pannel中的占位情况，默认是大图标
 * @return 返回添加的action
 */
QAction* SARibbonPannel::addAction(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode, SARibbonPannelItem::RowProportion rp)
{
    QAction* action = new QAction(icon, text, this);
    addAction(action, popMode, rp);
    return (action);
}

/**
 * @brief 添加一个普通菜单
 * @param menu
 * @param rp
 * @param popMode,菜单弹出模式，默认InstantPopup模式
 * @return
 */
void SARibbonPannel::addMenu(QMenu* menu, SARibbonPannelItem::RowProportion rp, QToolButton::ToolButtonPopupMode popMode)
{
    Q_CHECK_PTR(menu);
    QAction* action = menu->menuAction();
    action->setIcon(menu->icon());
    action->setText(menu->title());
    action->setObjectName("action." + menu->objectName());
    addAction(action, popMode, rp);
}

void SARibbonPannel::addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    addMenu(menu, SARibbonPannelItem::Large, popMode);
}

void SARibbonPannel::addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    addMenu(menu, SARibbonPannelItem::Small, popMode);
}

/**
 * @brief 添加窗口
 *
 * @param w
 * @param rp
 * @return 返回action(QWidgetAction)
 * @note SARibbonPannel并不会管理此窗口内存，在delete SARibbonPannel时，此窗口如果父对象不是SARibbonPannel将不会被删除
 */
QAction* SARibbonPannel::addWidget(QWidget* w, SARibbonPannelItem::RowProportion rp)
{
    QWidgetAction* action = new QWidgetAction(this);

    action->setDefaultWidget(w);
    action->setIcon(w->windowIcon());
    action->setText(w->windowTitle());
    action->setObjectName("action." + w->objectName());
    w->setAttribute(Qt::WA_Hover);
    setActionRowProportionProperty(action, rp);
    addAction(action);
    return (action);
}

/**
 * @brief 添加窗口,占用ribbon的一行
 * @param w
 * @return 返回action(QWidgetAction)
 */
QAction* SARibbonPannel::addSmallWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPannelItem::Small));
}

/**
 * @brief 添加窗口,占用ribbon的一行
 * @param w
 * @return
 */
QAction* SARibbonPannel::addMediumWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPannelItem::Medium));
}

/**
 * @brief 添加窗口，占用所有行
 * @param w
 * @return 返回action(QWidgetAction)
 */
QAction* SARibbonPannel::addLargeWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPannelItem::Large));
}

/**
 * @brief SARibbonPannel::addGallery
 * @return
 * @note SARibbonPannel将拥有SARibbonGallery的管理权
 */
SARibbonGallery* SARibbonPannel::addGallery(bool expanding)
{
    SARibbonGallery* gallery = RibbonSubElementFactory->createRibbonGallery(this);

    addWidget(gallery, SARibbonPannelItem::Large);
    if (expanding) {
        setExpanding(expanding);
    }
    return (gallery);
}

/**
 * @brief 添加分割线
 * @param top 上边距 @default 6
 * @param bottom 下边距 @default 6
 */
QAction* SARibbonPannel::addSeparator()
{
    QAction* action = new QAction(this);

    action->setSeparator(true);
    setActionRowProportionProperty(action, SARibbonPannelItem::Large);
    addAction(action);
    return (action);
}

/**
 * @brief 从pannel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
 * @param action
 * @return 如果action没有对应的button，就返回nullptr
 */
SARibbonToolButton* SARibbonPannel::actionToRibbonToolButton(QAction* action)
{
#if 0
	SARibbonPannelLayout* lay = qobject_cast< SARibbonPannelLayout* >(layout());

	if (lay) {
		int index = lay->indexOf(action);
		if (index == -1) {
			return (nullptr);
		}
		QLayoutItem* item       = lay->takeAt(index);
		SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(item ? item->widget() : nullptr);
		return (btn);
	}
	return (nullptr);
#else
    for (auto obj : qAsConst(children())) {
        if (obj->isWidgetType()) {
            if (SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(obj)) {
                if (btn->defaultAction() == action) {
                    return btn;
                }
            }
        }
    }
    return (nullptr);
#endif
}

/**
 * @brief 获取pannel下面的所有toolbutton
 * @return
 */
QList< SARibbonToolButton* > SARibbonPannel::ribbonToolButtons() const
{
    const QObjectList& objs = children();
    QList< SARibbonToolButton* > res;

    for (QObject* o : objs) {
        SARibbonToolButton* b = qobject_cast< SARibbonToolButton* >(o);
        if (b) {
            res.append(b);
        }
    }
    return (res);
}

/**
 * @brief 设置PannelLayoutMode
 * @param mode
 */
void SARibbonPannel::setPannelLayoutMode(SARibbonPannel::PannelLayoutMode mode)
{
    if (d_ptr->m_pannelLayoutMode == mode) {
        return;
    }
    d_ptr->m_pannelLayoutMode = mode;
    updateItemGeometry();
}

SARibbonPannel::PannelLayoutMode SARibbonPannel::pannelLayoutMode() const
{
    return (d_ptr->m_pannelLayoutMode);
}

/**
 * @brief 添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
 * @param action
 * @note 要去除OptionAction直接传入nullptr即可
 * @note SARibbonPannel不对QAction的destroy进行关联，如果外部对action进行delete，需要先传入nullptr给addOptionAction
 */
void SARibbonPannel::setOptionAction(QAction* action)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        lay->setOptionAction(action);
    }
}

/**
 * @brief 判断是否存在OptionAction
 * @return 存在返回true
 */
bool SARibbonPannel::isHaveOptionAction() const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->isHaveOptionAction();
    }
    return false;
}

QSize SARibbonPannel::sizeHint() const
{
    int shWidth  = 500;
    int shHeight = 100;
    if (QLayout* lay = layout()) {
        QSize laySize = layout()->sizeHint();
        shWidth       = laySize.width();
        shHeight      = laySize.height();
    }
    return QSize(shWidth, shHeight);
}

QSize SARibbonPannel::minimumSizeHint() const
{
    return (layout()->minimumSize());
}

/**
 * @brief 把pannel设置为扩展模式，此时会撑大水平区域
 * @param isExpanding
 */
void SARibbonPannel::setExpanding(bool isExpanding)
{
    setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred, QSizePolicy::Fixed);
}

/**
 * @brief 判断此pannel是否为（水平）扩展模式
 * @return 是扩展模式返回true
 */
bool SARibbonPannel::isExpanding() const
{
    QSizePolicy sp = sizePolicy();

    return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}

/**
   @brief 设置标题栏的高度
   @param h
 */
void SARibbonPannel::setTitleHeight(int h)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        lay->setPannelTitleHeight(h);
    }
}

/**
 * @brief 标题栏高度
 * @return
 */
int SARibbonPannel::titleHeight() const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->pannelTitleHeight();
    }
    return -1;
}

/**
 * @brief 判断是否显示标题，只有标题的高度被设置，才会显示标题
 * @return
 */
bool SARibbonPannel::isEnableShowTitle() const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->isEnableShowPannelTitle();
    }
    return false;
}

/**
 * @brief 是否显示标题，显示标题后，标题的高度需要设置，默认高度为15
 * @param on
 */
void SARibbonPannel::setEnableShowTitle(bool on)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->setEnableShowPannelTitle(on);
    }
}

/**
 * @brief action对应的布局index，此操作一般用于移动，其他意义不大
 * @param act
 * @return 没有查到返回-1
 */
int SARibbonPannel::actionIndex(QAction* act) const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return (lay->indexByAction(act));
    }
    return (-1);
}

/**
 * @brief 移动action
 * @param from 要移动action的位置，通过@ref actionIndex 获取
 * @param to 要移动的位置
 */
void SARibbonPannel::moveAction(int from, int to)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return (lay->move(from, to));
    }
    updateGeometry();  // 通知layout进行重新布局
}

/**
 * @brief 判断是否可以自定义
 * @return
 */
bool SARibbonPannel::isCanCustomize() const
{
    return (d_ptr->m_isCanCustomize);
}

/**
 * @brief 设置是否可以自定义
 * @param b
 */
void SARibbonPannel::setCanCustomize(bool b)
{
    d_ptr->m_isCanCustomize = b;
}

/**
 * @brief pannel名称
 * @return
 */
QString SARibbonPannel::pannelName() const
{
    return (d_ptr->pannelName());
}

/**
 * @brief pannel的名称
 * @param title 标题
 * @note 此函数会触发@sa pannelNameChanged 信号
 */
void SARibbonPannel::setPannelName(const QString& title)
{
    QString oldName = pannelName();
    if (oldName != title) {
        d_ptr->setPannelName(title);
        emit pannelNameChanged(title);
    }
}

/**
 * @brief 获取大图标的高度
 * @return
 */
int SARibbonPannel::largeButtonHeight() const
{
    const QMargins& mag = contentsMargins();
    return height() - mag.top() - mag.bottom() - titleHeight();
}

/**
 * @brief 获取pannel layout
 * @return
 */
SARibbonPannelLayout* SARibbonPannel::pannelLayout() const
{
    return d_ptr->pannelLayout();
}

/**
 * @brief 更新布局
 */
void SARibbonPannel::updateItemGeometry()
{
#if SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonPannel updateItemGeometry,pannelName=" << pannelName();
#endif
    // 此函数需要添加，否则SARibbonBar::setEnableWordWrap无法刷新按钮
    resetToolButtonSize();
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        lay->updateGeomArray();
    }
}

/**
   @brief 获取category指针，如果没有parent，或者不在category管理，返回nullptr
   @return
 */
SARibbonCategory* SARibbonPannel::category() const
{
    return qobject_cast< SARibbonCategory* >(parent());
}

/**
   @brief 获取ribbonBar指针，如果没有返回nullptr
   @return
 */
SARibbonBar* SARibbonPannel::ribbonBar() const
{
    if (SARibbonCategory* c = category()) {
        return c->ribbonBar();
    }
    return nullptr;
}

/**
 * @brief pannel高度推荐
 * @param fm
 * @param layMode
 * @param pannelTitleHeight
 * @return
 */
int SARibbonPannel::pannelHeightHint(const QFontMetrics& fm, PannelLayoutMode layMode, int pannelTitleHeight)
{
    int textH = fm.lineSpacing();  // 这里用linespace，因为在换行的情况下，行距是不可忽略的，ribbon的大按钮默认是2行
    switch (layMode) {
    case SARibbonPannel::ThreeRowMode: {
        // 5.5=（3*1.6+1） （三行）,1是给panneltitle预留的
        return textH * 4.8 + pannelTitleHeight;
    } break;
    case SARibbonPannel::TwoRowMode: {
        // 3=2*1.6
        return textH * 3.2 + pannelTitleHeight;
    } break;
    default: {
        qWarning() << "unknow SARibbonPannel::PannelLayoutMode:" << layMode;
    }
    }
    return (textH * 4.8 + pannelTitleHeight);
}

/**
 * @brief 重置按钮的尺寸，在布局改变后（尤其高度变更），按钮的尺寸需要手动变更
 */
void SARibbonPannel::resetToolButtonSize()
{
    QList< SARibbonToolButton* > btns = ribbonToolButtons();

    for (SARibbonToolButton* b : qAsConst(btns)) {
        if ((nullptr == b)) {
            continue;
        }
        b->updateRect();
    }
}

bool SARibbonPannel::event(QEvent* e)
{
#if SA_DEBUG_PRINT_EVENT
    if (e->type() != QEvent::Paint) {
        qDebug() << "SARibbonPannel event(" << e->type() << "),name=" << pannelName();
    }
#endif
    // if (SARibbonPannelLayout* lay = pannelLayout()) {
    //     if (lay->isDirty() && e->type() == QEvent::LayoutRequest) {
    //         if (QWidget* parw = parentWidget()) {
    //             if (QLayout* pl = parw->layout()) {
    //                 pl->invalidate();
    //             }
    //         }
    //         lay->m_dirty = false;
    //     }
    // }
    return QWidget::event(e);
}

/**
 * @brief 处理action的事件
 *
 * 这里处理了ActionAdded，ActionChanged，ActionRemoved三个事件
 *
 * ActionAdded时向布局请求，添加action，布局中同时触发了@ref SARibbonPannelLayout::createItem 函数
 * 此函数用于生成窗口，例如QRibbonToolButton
 *
 * ActionChanged时会让布局重新计算尺寸，并向category请求重新布局，有可能category的所有要重新调整尺寸
 *
 * ActionRemoved会移除布局管理的QLayoutItem，并进行内存清理，这时窗口也会隐藏，同时销毁
 *
 * @param e
 * @note 所有action事件都会向category请求重新布局
 *
 */
void SARibbonPannel::actionEvent(QActionEvent* e)
{
    QAction* action             = e->action();
    QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action);

    switch (e->type()) {
    case QEvent::ActionAdded: {
        SARibbonPannelLayout* lay = pannelLayout();
        if (nullptr != widgetAction) {
            if (widgetAction->parent() != this) {
                widgetAction->setParent(this);
            }
        }
        int index = layout()->count();
        if (e->before()) {
            // 说明是插入
            index = lay->indexByAction(e->before());
            if (-1 == index) {
                index = layout()->count();  // 找不到的时候就插入到最后
            }
        }
        lay->insertAction(index, action, getActionRowProportionProperty(action));
        // 由于pannel的尺寸发生变化，需要让category也调整
        // if (QWidget* parw = parentWidget()) {
        //     if (QLayout* pl = parw->layout()) {
        //         pl->invalidate();
        //     }
        // }
    } break;

    case QEvent::ActionChanged: {
        // 让布局重新绘制
        layout()->invalidate();

        // updateGeometry();
        // 由于pannel的尺寸发生变化，需要让category也调整
        if (QWidget* parw = parentWidget()) {
            if (QLayout* pl = parw->layout()) {
                pl->invalidate();
            }
            //! 强制发送一个resizeevent，让Category能重绘，如果没有这个函数，发现Category的layout虽然设置了invalidate（标记缓存失效）
            //! 但并没有按顺序在pannel尺寸更新后更新Category的尺寸，导致有些pannel的尺寸识别出现异常
            //! 重打印信息上看，pannel的尺寸有进行更新，category的尺寸也进行了更新，但更新的次数和调用invalidate的次数不一样，需要手动触发ResizeEvent
            //! 尝试过调用QEvent::LayoutRequest没有效果：
            //! @code
            //! QEvent* el = new QEvent(QEvent::LayoutRequest);
            //! QApplication::postEvent(parw, el);
            //! @endcode
            //!
            //! 调用parw->updateGeometry();也没有效果，目前看使用resizeevent是最有效果的
            //!
            QResizeEvent* ersize = new QResizeEvent(parw->size(), QSize());
            QApplication::postEvent(parw, ersize);
        }
    } break;

    case QEvent::ActionRemoved: {
        SARibbonPannelLayout* lay = pannelLayout();
        action->disconnect(this);
        int index = lay->indexByAction(action);
        if (index != -1) {
            QLayoutItem* item = lay->takeAt(index);
            delete item;
        }
        // 由于pannel的尺寸发生变化，需要让category也调整
        // if (QWidget* parw = parentWidget()) {
        //     if (QLayout* pl = parw->layout()) {
        //         pl->invalidate();
        //     }
        // }
    } break;

    default:
        break;
    }
}

/**
 * @brief 追踪字体改变事件，把新字体设置到childwidget中
 * @param e
 */
void SARibbonPannel::changeEvent(QEvent* e)
{
    if (nullptr == e) {
        return;
    }
    if (e->type() == QEvent::FontChange) {
        QFont f                       = font();
        QList< QWidget* > listWidgets = findChildren< QWidget* >();
        for (QWidget* w : listWidgets) {
            w->setFont(f);
        }
        if (QLayout* lay = layout()) {
            lay->invalidate();
        }
        d_ptr->resetTitleLabelFont();
    }
    QWidget::changeEvent(e);
}

/**
 * @brief 获取布局item
 * @return
 */
const QList< SARibbonPannelItem* >& SARibbonPannel::ribbonPannelItem() const
{
    return (pannelLayout()->m_items);
}

/*** End of inlined file: SARibbonPannel.cpp ***/

/*** Start of inlined file: SARibbonCategory.cpp ***/
#include <QList>
#include <QResizeEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <QApplication>
#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QResizeEvent>

///
/// \brief ribbon页的代理类
/// 如果需要修改重绘SARibbonCategory，可以通过设置SARibbonCategory::setProxy
///
class SARibbonCategory::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCategory)
public:
    PrivateData(SARibbonCategory* p);

    SARibbonPannel* addPannel(const QString& title);
    SARibbonPannel* insertPannel(const QString& title, int index);
    void addPannel(SARibbonPannel* pannel);
    void insertPannel(int index, SARibbonPannel* pannel);

    // 把pannel从Category中移除，不会销毁，此时pannel的所有权归还操作者
    bool takePannel(SARibbonPannel* pannel);

    // 移除Pannel，Category会直接回收SARibbonPannel内存
    bool removePannel(SARibbonPannel* pannel);
    SARibbonCategory* ribbonCategory();
    const SARibbonCategory* ribbonCategory() const;

    // 返回所有的Pannel
    QList< SARibbonPannel* > pannelList();

    // 更新item的布局,此函数会调用doItemLayout
    void updateItemGeometry();

    void doWheelEvent(QWheelEvent* event);
    // 初始化
    void init(SARibbonCategory* c);

public:
    bool mEnableShowPannelTitle { true };  ///< 是否运行pannel的标题栏显示
    int mPannelTitleHeight { 15 };         ///< pannel的标题栏默认高度
    bool mIsContextCategory { false };     ///< 标记是否是上下文标签
    bool mIsCanCustomize { true };         ///< 标记是否可以自定义
    SARibbonPannel::PannelLayoutMode mDefaultPannelLayoutMode { SARibbonPannel::ThreeRowMode };
};
SARibbonCategory::PrivateData::PrivateData(SARibbonCategory* p) : q_ptr(p)
{
}

SARibbonPannel* SARibbonCategory::PrivateData::addPannel(const QString& title)
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        SARibbonPannel* p = insertPannel(title, lay->pannelCount());
        return p;
    }
    return nullptr;
}

SARibbonPannel* SARibbonCategory::PrivateData::insertPannel(const QString& title, int index)
{
    SARibbonPannel* pannel = RibbonSubElementFactory->createRibbonPannel(ribbonCategory());

    pannel->setPannelName(title);
    pannel->setObjectName(title);
    insertPannel(index, pannel);
    return (pannel);
}

void SARibbonCategory::PrivateData::addPannel(SARibbonPannel* pannel)
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        insertPannel(lay->pannelCount(), pannel);
    }
}

/**
 * @brief 插入pannel到layout
 *
 * 所有的添加操作最终会调用此函数
 * @param index
 * @param pannel
 */
void SARibbonCategory::PrivateData::insertPannel(int index, SARibbonPannel* pannel)
{
    if (nullptr == pannel) {
        return;
    }
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (nullptr == lay) {
        return;
    }
    if (pannel->parentWidget() != q_ptr) {
        pannel->setParent(q_ptr);
    }
    // 同步一些状态
    pannel->setEnableShowTitle(mEnableShowPannelTitle);
    pannel->setTitleHeight(mPannelTitleHeight);
    pannel->setPannelLayoutMode(mDefaultPannelLayoutMode);
    index = qMax(0, index);
    index = qMin(lay->pannelCount(), index);
    lay->insertPannel(index, pannel);
    pannel->setVisible(true);
}

bool SARibbonCategory::PrivateData::takePannel(SARibbonPannel* pannel)
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (nullptr == lay) {
        return false;
    }
    return lay->takePannel(pannel);
}

bool SARibbonCategory::PrivateData::removePannel(SARibbonPannel* pannel)
{
    if (takePannel(pannel)) {
        pannel->hide();
        pannel->deleteLater();
        return (true);
    }
    return (false);
}

QList< SARibbonPannel* > SARibbonCategory::PrivateData::pannelList()
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        return lay->pannelList();
    }
    return QList< SARibbonPannel* >();
}

SARibbonCategory* SARibbonCategory::PrivateData::ribbonCategory()
{
    return (q_ptr);
}

const SARibbonCategory* SARibbonCategory::PrivateData::ribbonCategory() const
{
    return (q_ptr);
}

void SARibbonCategory::PrivateData::updateItemGeometry()
{
#if SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonCategory::PrivateData::updateItemGeometry,categoryName=" << q_ptr->categoryName();
#endif
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (!lay) {
        return;
    }
    const QList< SARibbonPannel* > pannels = lay->pannelList();
    for (auto pannel : pannels) {
        pannel->updateItemGeometry();
    }
    lay->updateGeometryArr();
    return;
}

void SARibbonCategory::PrivateData::doWheelEvent(QWheelEvent* event)
{
    SARibbonCategoryLayout* lay = q_ptr->categoryLayout();
    if (nullptr == lay) {
        return;
    }
    QSize contentSize = lay->categoryContentSize();
    // 求总宽
    int totalWidth = lay->categoryTotalWidth();

    if (totalWidth > contentSize.width()) {
        // 这个时候滚动有效
        int scrollpix = 40;
        // Qt6 取消了QWheelEvent::delta函数
        // 是要下面方法可兼容qt5/6
        QPoint numPixels  = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;
        if (!numPixels.isNull()) {
            if (numDegrees.y() < 0) {
                scrollpix = -scrollpix;
            }
        } else if (!numDegrees.isNull()) {
            if (numDegrees.y() < 0) {
                scrollpix = -scrollpix;
            }
        }
        lay->scroll(scrollpix);
    } else {
        // 这时候无需处理事件，把滚动事件上发让父级也能接收
        event->ignore();
        // 如滚动过就还原
        if (lay->isScrolled()) {
            lay->scroll(0);
        }
    }
}

void SARibbonCategory::PrivateData::init(SARibbonCategory* c)
{
    c->setLayout(new SARibbonCategoryLayout(c));
    c->connect(c, &SARibbonCategory::windowTitleChanged, c, &SARibbonCategory::categoryNameChanged);
}

//----------------------------------------------------
// SARibbonCategory
//----------------------------------------------------

SARibbonCategory::SARibbonCategory(QWidget* p) : QFrame(p), d_ptr(new SARibbonCategory::PrivateData(this))
{
    d_ptr->init(this);
}

SARibbonCategory::SARibbonCategory(const QString& name, QWidget* p)
    : QFrame(p), d_ptr(new SARibbonCategory::PrivateData(this))
{
    setCategoryName(name);
    d_ptr->init(this);
}

SARibbonCategory::~SARibbonCategory()
{
}

/**
 * @brief category的名字,等同windowTitle函数
 * @return
 */
QString SARibbonCategory::categoryName() const
{
    return (windowTitle());
}

/**
 * @brief 设置category名字，等同setWindowTitle
 * @param title
 */
void SARibbonCategory::setCategoryName(const QString& title)
{
    setWindowTitle(title);
}

bool SARibbonCategory::event(QEvent* e)
{
#if SA_DEBUG_PRINT_EVENT
    if (e->type() != QEvent::Paint) {
        qDebug() << "SARibbonCategory event(" << e->type() << "),name=" << categoryName();
    }
#endif
    return QWidget::event(e);
}

/**
 * @brief pannel的模式
 * @return
 */
SARibbonPannel::PannelLayoutMode SARibbonCategory::pannelLayoutMode() const
{
    return (d_ptr->mDefaultPannelLayoutMode);
}

/**
 * @brief 设置pannel的模式
 *
 * 在@ref SARibbonBar 调用@ref SARibbonBar::setRibbonStyle 函数时，会对所有的SARibbonCategory调用此函数
 * 把新的SARibbonPannel::PannelLayoutMode设置进去
 * @param m
 */
void SARibbonCategory::setPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
    d_ptr->mDefaultPannelLayoutMode = m;
    iterate([ m ](SARibbonPannel* p) -> bool {
        p->setPannelLayoutMode(m);
        return true;
    });
    updateItemGeometry();
}

/**
 * @brief 添加pannel
 *
 * @note pannel的所有权由SARibbonCategory来管理，请不要在外部对其进行销毁
 * @param title pannel的标题，在office/wps的三行模式下会显示在pannel的下方
 * @return 返回生成的@ref SARibbonPannel 指针
 * @see 对SARibbonPannel的其他操作，参考 @ref SARibbonCategory::takePannel
 */
SARibbonPannel* SARibbonCategory::addPannel(const QString& title)
{
    return (d_ptr->addPannel(title));
}

/**
 * @brief 添加pannel
 * @param pannel pannel的所有权SARibbonCategory来管理
 */
void SARibbonCategory::addPannel(SARibbonPannel* pannel)
{
    d_ptr->addPannel(pannel);
}

/**
 * @brief qt designer专用
 * @param pannel
 */
void SARibbonCategory::addPannel(QWidget* pannel)
{
    SARibbonPannel* p = qobject_cast< SARibbonPannel* >(pannel);
    if (p) {
        addPannel(p);
    }
}

/**
 * @brief 新建一个pannel，并插入到index位置
 * @param title pannel的title
 * @param index 插入的位置，如果index超出category里pannel的个数，将插入到最后
 * @return 返回生成的@ref SARibbonPannel 指针
 * @note 如果
 */
SARibbonPannel* SARibbonCategory::insertPannel(const QString& title, int index)
{
    return (d_ptr->insertPannel(title, index));
}

/**
 * @brief 通过名字查找pannel
 * @param title
 * @return 如果有重名，只会返回第一个符合条件的
 */
SARibbonPannel* SARibbonCategory::pannelByName(const QString& title) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelByName(title);
    }
    return nullptr;
}

/**
 * @brief 通过ObjectName查找pannel
 * @param objname
 * @return
 */
SARibbonPannel* SARibbonCategory::pannelByObjectName(const QString& objname) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelByObjectName(objname);
    }
    return nullptr;
}

/**
 * @brief 通过索引找到pannel，如果超过索引范围，会返回nullptr
 * @param index
 * @return 如果超过索引范围，会返回nullptr
 */
SARibbonPannel* SARibbonCategory::pannelByIndex(int index) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelByIndex(index);
    }
    return nullptr;
}

/**
 * @brief 查找pannel对应的索引
 * @param p
 * @return 如果找不到，返回-1
 */
int SARibbonCategory::pannelIndex(SARibbonPannel* p) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelIndex(p);
    }
    return -1;
}

/**
 * @brief 移动一个Pannel从from index到to index
 * @param from 要移动pannel的index
 * @param to 要移动到的位置
 */
void SARibbonCategory::movePannel(int from, int to)
{
    if (from == to) {
        return;
    }
    if (to < 0) {
        to = 0;
    }
    if (to >= pannelCount()) {
        to = pannelCount() - 1;
    }
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        lay->movePannel(from, to);
    }
}

/**
 * @brief 把pannel脱离SARibbonCategory的管理
 * @param pannel 需要提取的pannel
 * @return 成功返回true，否则返回false
 */
bool SARibbonCategory::takePannel(SARibbonPannel* pannel)
{
    return (d_ptr->takePannel(pannel));
}

/**
 * @brief 移除Pannel，Category会直接回收SARibbonPannel内存
 * @param pannel 需要移除的pannel
 * @note 移除后pannel为野指针，一般操作完建议把pannel指针设置为nullptr
 *
 * 此操作等同于：
 *
 * @code
 * SARibbonPannel* pannel;
 * ...
 * category->takePannel(pannel);
 * pannel->hide();
 * pannel->deleteLater();
 * @endcode
 */
bool SARibbonCategory::removePannel(SARibbonPannel* pannel)
{
    return (d_ptr->removePannel(pannel));
}

/**
 * @brief 移除pannel
 * @param index pannel的索引，如果超出会返回false
 * @return 成功返回true
 */
bool SARibbonCategory::removePannel(int index)
{
    SARibbonPannel* p = pannelByIndex(index);

    if (nullptr == p) {
        return (false);
    }
    return (removePannel(p));
}

/**
 * @brief 返回Category下的所有pannel
 * @return
 */
QList< SARibbonPannel* > SARibbonCategory::pannelList() const
{
    return (d_ptr->pannelList());
}

QSize SARibbonCategory::sizeHint() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->sizeHint();
    }
    return QSize(1000, 100);
}

/**
 * @brief 如果是ContextCategory，此函数返回true
 * @return
 */
bool SARibbonCategory::isContextCategory() const
{
    return (d_ptr->mIsContextCategory);
}

/**
 * @brief 返回pannel的个数
 * @return
 */
int SARibbonCategory::pannelCount() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelCount();
    }
    return -1;
}

/**
 * @brief 判断是否可以自定义
 * @return
 */
bool SARibbonCategory::isCanCustomize() const
{
    return (d_ptr->mIsCanCustomize);
}

/**
 * @brief 设置是否可以自定义
 * @param b
 */
void SARibbonCategory::setCanCustomize(bool b)
{
    d_ptr->mIsCanCustomize = b;
}

/**
 * @brief pannel标题栏的高度
 * @return
 */
int SARibbonCategory::pannelTitleHeight() const
{
    return d_ptr->mPannelTitleHeight;
}
/**
 * @brief 设置pannel的高度
 * @param h
 */
void SARibbonCategory::setPannelTitleHeight(int h)
{
    d_ptr->mPannelTitleHeight = h;
    iterate([ h ](SARibbonPannel* p) -> bool {
        p->setTitleHeight(h);
        return true;
    });
}

/**
 * @brief 是否pannel显示标题栏
 * @return
 */
bool SARibbonCategory::isEnableShowPannelTitle() const
{
    return d_ptr->mEnableShowPannelTitle;
}

/**
 * @brief 设置显示pannel标题
 * @param on
 */
void SARibbonCategory::setEnableShowPannelTitle(bool on)
{
    d_ptr->mEnableShowPannelTitle = on;
    iterate([ on ](SARibbonPannel* p) -> bool {
        p->setEnableShowTitle(on);
        return true;
    });
}

/**
 * @brief 获取对应的ribbonbar
 * @return 如果没有加入ribbonbar的管理，此值为null
 */
SARibbonBar* SARibbonCategory::ribbonBar() const
{
    // 第一个par是stackwidget
    if (QWidget* par = parentWidget()) {
        // 理论此时是ribbonbar
        par = par->parentWidget();
        while (par) {
            if (SARibbonBar* ribbon = qobject_cast< SARibbonBar* >(par)) {
                return ribbon;
            }
            par = par->parentWidget();
        }
    }
    return nullptr;
}

/**
 * @brief 刷新category的布局，适用于改变ribbon的模式之后调用
 */
void SARibbonCategory::updateItemGeometry()
{
#if SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonCategory name=" << categoryName() << " updateItemGeometry";
#endif
    d_ptr->updateItemGeometry();
}

/**
 * @brief 此函数会遍历Category下的所有pannel,执行函数指针
 * @param fp 函数指针返回false则停止迭代
 * @return 返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
 */
bool SARibbonCategory::iterate(FpPannelIterate fp)
{
    const QList< SARibbonPannel* > ps = pannelList();
    for (SARibbonPannel* p : ps) {
        if (!fp(p)) {
            return false;
        }
    }
    return true;
}

/**
   @brief 设置Category的对齐方式
   @param al
 */
void SARibbonCategory::setCategoryAlignment(SARibbonAlignment al)
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(layout());
    if (lay) {
        lay->setCategoryAlignment(al);
    }
}

/**
   @brief Category的对齐方式
   @return
 */
SARibbonAlignment SARibbonCategory::categoryAlignment() const
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(layout());
    if (lay) {
        return lay->categoryAlignment();
    }
    return SARibbonAlignment::AlignLeft;
}

/**
 * @brief 在超出边界情况下，滚轮可滚动pannel
 * @param event
 */
void SARibbonCategory::wheelEvent(QWheelEvent* event)
{
    d_ptr->doWheelEvent(event);
}

void SARibbonCategory::changeEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::StyleChange: {
        if (layout()) {
#if SA_DEBUG_PRINT_SIZE_HINT
            qDebug() << "SARibbonCategory changeEvent(StyleChange),categoryName=" << categoryName();
#endif
            layout()->invalidate();
        }
    } break;
    case QEvent::FontChange: {
#if SA_DEBUG_PRINT_SIZE_HINT
        qDebug() << "SARibbonCategory changeEvent(FontChange),categoryName=" << categoryName();
#endif
        QFont f = font();
        iterate([ f ](SARibbonPannel* p) -> bool {
            p->setFont(f);
            return true;
        });
        if (layout()) {
            layout()->invalidate();
        }
    } break;
    default:
        break;
    };
    return QWidget::changeEvent(event);
}

/**
 * @brief 标记这个是上下文标签
 * @param isContextCategory
 */
void SARibbonCategory::markIsContextCategory(bool isContextCategory)
{
    d_ptr->mIsContextCategory = isContextCategory;
}

/**
 * @brief 获取SARibbonCategoryLayoutlayout
 * @return
 */
SARibbonCategoryLayout* SARibbonCategory::categoryLayout() const
{
    return qobject_cast< SARibbonCategoryLayout* >(layout());
}

//===================================================
// SARibbonCategoryScrollButton
//===================================================
SARibbonCategoryScrollButton::SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p) : QToolButton(p)
{
    setArrowType(arr);
}

/*** End of inlined file: SARibbonCategory.cpp ***/

/*** Start of inlined file: SARibbonCategoryLayout.cpp ***/
#include <QLayoutItem>

#include <QApplication>
#include <QDebug>

#ifndef SARibbonCategoryLayout_DEBUG_PRINT
#define SARibbonCategoryLayout_DEBUG_PRINT 1
#endif
/**
 * @brief The SARibbonCategoryLayoutPrivate class
 */
class SARibbonCategoryLayout::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCategoryLayout)
public:
    PrivateData(SARibbonCategoryLayout* p);
    // 计算所有元素的sizehint总宽度
    int totalSizeHintWidth() const;

public:
    bool mDirty { true };
    bool mIsRightScrollBtnShow { false };                       ///< 标记右滚动按钮是否需要显示
    bool mIsLeftScrollBtnShow { false };                        ///< 标记左滚动按钮是否需要显示
    SARibbonCategoryScrollButton* mLeftScrollBtn { nullptr };   ///< 在区域无法显示时显示的按钮
    SARibbonCategoryScrollButton* mRightScrollBtn { nullptr };  ///< 在区域无法显示时显示的按钮
    int mTotalWidth { 0 };
    int mXBase { 0 };
    QSize mSizeHint;
    QSize mMinSizeHint;
    QList< SARibbonCategoryLayoutItem* > mItemList;
    SARibbonAlignment mCategoryAlignment { SARibbonAlignment::AlignLeft };  ///< 对齐方式
};

//=============================================================
// SARibbonCategoryLayoutPrivate
//=============================================================

SARibbonCategoryLayout::PrivateData::PrivateData(SARibbonCategoryLayout* p) : q_ptr(p)
{
}

/**
 * @brief 计算所有元素的SizeHint宽度总和
 * @return
 */
int SARibbonCategoryLayout::PrivateData::totalSizeHintWidth() const
{
    int total    = 0;
    QMargins mag = q_ptr->contentsMargins();
#if SA_DEBUG_PRINT_SIZE_HINT
    int debug_i__ = 0;
    QString debug_totalSizeHintWidth__;
#endif
    if (!mag.isNull()) {
        total += (mag.left() + mag.right());
    }
    // 先计算总长
    for (SARibbonCategoryLayoutItem* item : qAsConst(mItemList)) {
        if (item->isEmpty()) {
// 如果是hide就直接跳过
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
            ++debug_i__;
            debug_totalSizeHintWidth__ += QString("   [%1](%2)is empty skip\n")
                                                  .arg(debug_i__)
                                                  .arg(item->toPannelWidget()->pannelName());
#endif
            continue;
        }
        // 这里要使用widget()->sizeHint()，因为pannel的标题会影总体布局，此处需要修改
        //  TODO
        QSize pannelSize = item->widget()->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item->separatorWidget) {
            SeparatorSize = item->separatorWidget->sizeHint();
        }
        total += pannelSize.width();
        total += SeparatorSize.width();
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
        ++debug_i__;
        debug_totalSizeHintWidth__ += QString("|-[%1]pannelSize=(%2,%3),SeparatorSize=(%4,%5),name=(%6) \n")
                                              .arg(debug_i__)
                                              .arg(pannelSize.width())
                                              .arg(pannelSize.height())
                                              .arg(SeparatorSize.width())
                                              .arg(SeparatorSize.height())
                                              .arg(item->toPannelWidget()->pannelName());
#endif
    }
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonCategoryLayout.totalSizeHintWidth=" << total;
    qDebug().noquote() << debug_totalSizeHintWidth__;
#endif
    return (total);
}

//=============================================================
// SARibbonCategoryLayout
//=============================================================

SARibbonCategoryLayout::SARibbonCategoryLayout(SARibbonCategory* parent)
    : QLayout(parent), d_ptr(new SARibbonCategoryLayout::PrivateData(this))
{
    setContentsMargins(1, 1, 1, 1);
    d_ptr->mLeftScrollBtn  = new SARibbonCategoryScrollButton(Qt::LeftArrow, parent);
    d_ptr->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, parent);
    d_ptr->mLeftScrollBtn->setVisible(false);
    d_ptr->mRightScrollBtn->setVisible(false);
    connect(d_ptr->mLeftScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onLeftScrollButtonClicked);
    connect(d_ptr->mRightScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onRightScrollButtonClicked);
}

SARibbonCategoryLayout::~SARibbonCategoryLayout()
{
    while (auto item = takePannelItem(0)) {
        delete item;
    }
}

SARibbonCategory* SARibbonCategoryLayout::ribbonCategory() const
{
    return (qobject_cast< SARibbonCategory* >(parentWidget()));
}

void SARibbonCategoryLayout::addItem(QLayoutItem* item)
{
    Q_UNUSED(item);
    qWarning() << tr("in SARibbonCategoryLayout cannot addItem,use addPannel instead");
    invalidate();
}

/**
 * @brief 返回pannel的layout
 * @param index 索引
 * @return
 * @note 注意，pannel是和分割线一起的，但这个只返回一个pannel对应的layout
 */
QLayoutItem* SARibbonCategoryLayout::itemAt(int index) const
{
    SARibbonCategoryLayoutItem* item = d_ptr->mItemList.value(index, nullptr);

    return (item);
}

/**
 * @brief 提取layout
 * @param index
 * @return
 */
QLayoutItem* SARibbonCategoryLayout::takeAt(int index)
{
    QLayoutItem* r = takePannelItem(index);
    invalidate();
    return r;
}

SARibbonCategoryLayoutItem* SARibbonCategoryLayout::takePannelItem(int index)
{
    if ((index >= 0) && (index < d_ptr->mItemList.size())) {
        SARibbonCategoryLayoutItem* item = d_ptr->mItemList.takeAt(index);
        if (item->widget()) {
            item->widget()->hide();
        }
        if (item->separatorWidget) {
            item->separatorWidget->hide();
        }
        return (item);
    }
    return (nullptr);
}

SARibbonCategoryLayoutItem* SARibbonCategoryLayout::takePannelItem(SARibbonPannel* pannel)
{
    for (int i = 0; i < d_ptr->mItemList.size(); ++i) {
        SARibbonCategoryLayoutItem* item = d_ptr->mItemList[ i ];
        if (item->widget() == pannel) {
            return (takePannelItem(i));
        }
    }
    return (nullptr);
}

/**
 * @brief 移除pannel，对应的分割线也会删除
 * @param pannel
 * @return
 */
bool SARibbonCategoryLayout::takePannel(SARibbonPannel* pannel)
{
    SARibbonCategoryLayoutItem* i = takePannelItem(pannel);
    if (i) {
        SARibbonSeparatorWidget* sp = i->separatorWidget;
        if (sp) {
            sp->deleteLater();
        }
        delete i;
        invalidate();
        return true;
    }
    return false;
}

int SARibbonCategoryLayout::count() const
{
    return (d_ptr->mItemList.size());
}

void SARibbonCategoryLayout::addPannel(SARibbonPannel* pannel)
{
    insertPannel(d_ptr->mItemList.count(), pannel);
}

/**
 * @brief 插入一个pannel
 * @param index 索引
 * @param pannel
 * @return 返回对应的分割线SARibbonSeparatorWidget
 * @note 在SARibbonCategoryLayout的布局中，一个pannel会携带一个分割线
 */
void SARibbonCategoryLayout::insertPannel(int index, SARibbonPannel* pannel)
{
    index                            = qMax(0, index);
    index                            = qMin(d_ptr->mItemList.count(), index);
    SARibbonCategoryLayoutItem* item = new SARibbonCategoryLayoutItem(pannel);

    // 分割线
    item->separatorWidget = RibbonSubElementFactory->createRibbonSeparatorWidget(parentWidget());
    // 插入list中
    d_ptr->mItemList.insert(index, item);
    // 标记需要重新计算尺寸
    invalidate();
}

QSize SARibbonCategoryLayout::sizeHint() const
{
    if (d_ptr->mSizeHint.isNull()) {
        SARibbonCategoryLayout* that = const_cast< SARibbonCategoryLayout* >(this);
        that->updateGeometryArr();
    }
    return (d_ptr->mSizeHint);
}

QSize SARibbonCategoryLayout::minimumSize() const
{
    if (d_ptr->mMinSizeHint.isNull()) {
        SARibbonCategoryLayout* that = const_cast< SARibbonCategoryLayout* >(this);
        that->updateGeometryArr();
    }
    return (d_ptr->mMinSizeHint);
}

/**
 * @brief SARibbonCategory充满整个stacked widget
 * @return
 */
Qt::Orientations SARibbonCategoryLayout::expandingDirections() const
{
    return (Qt::Horizontal | Qt::Vertical);
}

void SARibbonCategoryLayout::invalidate()
{
    d_ptr->mDirty = true;
    QLayout::invalidate();
}
/**
 * @brief category的内容尺寸（把margins减去）
 * @return
 */
QSize SARibbonCategoryLayout::categoryContentSize() const
{
    SARibbonCategory* category = ribbonCategory();
    QSize s                    = category->size();
    QMargins mag               = contentsMargins();
    if (!mag.isNull()) {
        s.rheight() -= (mag.top() + mag.bottom());
        s.rwidth() -= (mag.right() + mag.left());
    }
    return (s);
}

/**
 * @brief 更新尺寸
 */
void SARibbonCategoryLayout::updateGeometryArr()
{
    SARibbonCategory* category = ribbonCategory();
    if (nullptr == category) {
        return;
    }
    int categoryWidth = category->width();
    QMargins mag      = contentsMargins();
    int height        = category->height();
    int y             = 0;

    if (!mag.isNull()) {
        y = mag.top();
        height -= (mag.top() + mag.bottom());
        categoryWidth -= (mag.right() + mag.left());
    }
    // total 是总宽，不是x坐标系，x才是坐标系
    int total = d_ptr->totalSizeHintWidth();

    // 扩展的宽度
    int expandWidth = 0;

// 如果total < categoryWidth,m_d->mXBase可以设置为0
// 判断是否超过总长度
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonCategoryLayout::updateGeometryArr"
             << "\n|-category name=" << category->categoryName()  //
             << "\n|-category height=" << height                  //
             << "\n|-totalSizeHintWidth=" << total                //
             << "\n|-y=" << y                                     //
             << "\n|-expandWidth:" << expandWidth                 //
             << "\n|-mag=" << mag;
#endif
    if (total > categoryWidth) {
        // 超过总长度，需要显示滚动按钮
        if (0 == d_ptr->mXBase) {
            // 已经移动到最左，需要可以向右移动
            d_ptr->mIsRightScrollBtnShow = true;
            d_ptr->mIsLeftScrollBtnShow  = false;
        } else if (d_ptr->mXBase <= (categoryWidth - total)) {
            // 已经移动到最右，需要可以向左移动
            d_ptr->mIsRightScrollBtnShow = false;
            d_ptr->mIsLeftScrollBtnShow  = true;
        } else {
            // 移动到中间两边都可以动
            d_ptr->mIsRightScrollBtnShow = true;
            d_ptr->mIsLeftScrollBtnShow  = true;
        }
    } else {
        // 说明total 小于 categoryWidth
        // 记录可以扩展的数量
        int canExpandingCount        = 0;
        d_ptr->mIsRightScrollBtnShow = false;
        d_ptr->mIsLeftScrollBtnShow  = false;
        // 这个是避免一开始totalWidth > categorySize.width()，通过滚动按钮调整了m_d->mBaseX
        // 随之调整了窗体尺寸，调整后totalWidth < categorySize.width()导致category在原来位置
        // 无法显示，必须这里把mBaseX设置为0

        d_ptr->mXBase = 0;
        //

        for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
            if (SARibbonPannel* p = qobject_cast< SARibbonPannel* >(item->widget())) {
                if (p->isExpanding()) {
                    // pannel可扩展
                    ++canExpandingCount;
                }
            }
        }
        // 计算可扩展的宽度
        if (canExpandingCount > 0) {
            expandWidth = (categoryWidth - total) / canExpandingCount;
        } else {
            expandWidth = 0;
        }
    }
    int x = d_ptr->mXBase;
    if ((categoryAlignment() == SARibbonAlignment::AlignCenter) && (total < categoryWidth) && (0 == expandWidth)) {
        // 如果是居中对齐，同时没有伸缩的pannel，同时总宽度没有超过category的宽度
        x = (categoryWidth - total) / 2;
    }
    total = 0;  // total重新计算
    // 先按照sizeHint设置所有的尺寸
    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        if (item->isEmpty()) {
            // 如果是hide就直接跳过
            if (item->separatorWidget) {
                // pannel hide分割线也要hide
                item->separatorWidget->hide();
            }
            item->mWillSetGeometry          = QRect(0, 0, 0, 0);
            item->mWillSetSeparatorGeometry = QRect(0, 0, 0, 0);
            continue;
        }
        SARibbonPannel* p = item->toPannelWidget();
        if (nullptr == p) {
            qDebug() << "unknow widget in SARibbonCategoryLayout";
            continue;
        }
        // p->layout()->update();
        QSize pannelSize = p->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item->separatorWidget) {
            SeparatorSize = item->separatorWidget->sizeHint();
        }
        if (p->isExpanding()) {
            // 可扩展，就把pannel扩展到最大
            pannelSize.setWidth(pannelSize.width() + expandWidth);
        }
        int w = pannelSize.width();

        item->mWillSetGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
        w                               = SeparatorSize.width();
        item->mWillSetSeparatorGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
    }
    d_ptr->mTotalWidth  = total;
    d_ptr->mSizeHint    = QSize(d_ptr->mTotalWidth, height);
    d_ptr->mMinSizeHint = QSize(categoryWidth, height);
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonCategoryLayout updateGeometryArr,SizeHint=" << d_ptr->mSizeHint
             << ",Category name=" << category->categoryName();
#endif
}

/**
 * @brief 执行layout调整
 */
void SARibbonCategoryLayout::doLayout()
{
    if (d_ptr->mDirty) {
        updateGeometryArr();
    }
    SARibbonCategory* category = ribbonCategory();
    // 两个滚动按钮的位置永远不变
    d_ptr->mLeftScrollBtn->setGeometry(0, 0, 12, category->height());
    d_ptr->mRightScrollBtn->setGeometry(category->width() - 12, 0, 12, category->height());
    QList< QWidget* > showWidgets, hideWidgets;
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    int debug_i__(0);
    qDebug() << "SARibbonCategoryLayout::doLayout(),name=" << category->categoryName();
#endif
    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
            if (item->separatorWidget) {
                hideWidgets << item->separatorWidget;
            }
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
            qDebug() << "|-[" << debug_i__ << "]pannelName(" << item->toPannelWidget()->pannelName() << ",will hide";
            ++debug_i__;
#endif
        } else {
            // item->widget()->setFixedSize(item->mWillSetGeometry.size());
            // item->widget()->move(item->mWillSetGeometry.topLeft());
            item->setGeometry(item->mWillSetGeometry);
            showWidgets << item->widget();
            if (item->separatorWidget) {
                item->separatorWidget->setGeometry(item->mWillSetSeparatorGeometry);
                showWidgets << item->separatorWidget;
            }
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
            qDebug() << "|-[" << debug_i__ << "]pannelName(" << item->toPannelWidget()->pannelName()
                     << "),willSetGeometry:" << item->mWillSetGeometry
                     << ",WillSetSeparatorGeometry:" << item->mWillSetSeparatorGeometry;
            ++debug_i__;
#endif
        }
    }

    d_ptr->mRightScrollBtn->setVisible(d_ptr->mIsRightScrollBtnShow);
    d_ptr->mLeftScrollBtn->setVisible(d_ptr->mIsLeftScrollBtnShow);
    if (d_ptr->mIsRightScrollBtnShow) {
        d_ptr->mRightScrollBtn->raise();
    }
    if (d_ptr->mIsLeftScrollBtnShow) {
        d_ptr->mLeftScrollBtn->raise();
    }
    // 不在上面那里进行show和hide因为这会触发SARibbonPannelLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget* w : qAsConst(showWidgets)) {
        if (!w->isVisible()) {
            w->show();
        }
    }
    for (QWidget* w : qAsConst(hideWidgets)) {
        if (w->isVisible()) {
            w->hide();
        }
    }
}

/**
 * @brief 返回所有pannels
 * @return
 */
QList< SARibbonPannel* > SARibbonCategoryLayout::pannels() const
{
    QList< SARibbonPannel* > res;

    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        SARibbonPannel* p = item->toPannelWidget();
        res.append(p);
    }
    return (res);
}

/**
 * @brief 通过ObjectName查找pannel
 * @param objname
 * @return
 */
SARibbonPannel* SARibbonCategoryLayout::pannelByObjectName(const QString& objname) const
{
    for (SARibbonCategoryLayoutItem* item : d_ptr->mItemList) {
        if (SARibbonPannel* pannel = item->toPannelWidget()) {
            if (pannel->objectName() == objname) {
                return pannel;
            }
        }
    }
    return nullptr;
}

/**
 * @brief 通过名字查找pannel
 * @param title
 * @return 如果有重名，只会返回第一个符合条件的
 */
SARibbonPannel* SARibbonCategoryLayout::pannelByName(const QString& pannelname) const
{
    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        if (SARibbonPannel* pannel = item->toPannelWidget()) {
            if (pannel->pannelName() == pannelname) {
                return (pannel);
            }
        }
    }
    return (nullptr);
}

/**
 * @brief 通过索引找到pannel，如果超过索引范围，会返回nullptr
 * @param i
 * @return
 */
SARibbonPannel* SARibbonCategoryLayout::pannelByIndex(int i) const
{
    if (i >= 0 && i < d_ptr->mItemList.size()) {
        return d_ptr->mItemList[ i ]->toPannelWidget();
    }
    return nullptr;
}

/**
 * @brief 移动pannel
 * @param from
 * @param to
 */
void SARibbonCategoryLayout::movePannel(int from, int to)
{
    d_ptr->mItemList.move(from, to);
    doLayout();
}

/**
 * @brief 返回pannel的个数
 * @return
 */
int SARibbonCategoryLayout::pannelCount() const
{
    return d_ptr->mItemList.size();
}

/**
 * @brief 查找pannel对应的索引
 * @param p
 * @return 如果找不到，返回-1
 */
int SARibbonCategoryLayout::pannelIndex(SARibbonPannel* p) const
{
    int c = pannelCount();

    for (int i = 0; i < c; ++i) {
        if (d_ptr->mItemList[ i ]->toPannelWidget() == p) {
            return (i);
        }
    }
    return (-1);
}

/**
 * @brief 获取所有的pannel
 * @return
 */
QList< SARibbonPannel* > SARibbonCategoryLayout::pannelList() const
{
    QList< SARibbonPannel* > res;

    for (SARibbonCategoryLayoutItem* i : qAsConst(d_ptr->mItemList)) {
        if (SARibbonPannel* p = i->toPannelWidget()) {
            res.append(p);
        }
    }
    return (res);
}

/**
 * @brief 执行滚动
 * @param px
 */
void SARibbonCategoryLayout::scroll(int px)
{
    QSize contentSize = categoryContentSize();
    d_ptr->mXBase += px;
    if (d_ptr->mXBase > 0) {
        d_ptr->mXBase = 0;
    } else if ((d_ptr->mXBase + d_ptr->mTotalWidth) < contentSize.width()) {
        d_ptr->mXBase = contentSize.width() - d_ptr->mTotalWidth;
    }
    invalidate();
}

/**
 * @brief 判断是否滚动过
 * @return
 */
bool SARibbonCategoryLayout::isScrolled() const
{
    return (d_ptr->mXBase != 0);
}

/**
 * @brief 这个宽度是实际内容的宽度，有可能大于size.width，也有可能小于
 * @return
 */
int SARibbonCategoryLayout::categoryTotalWidth() const
{
    return d_ptr->mTotalWidth;
}

/**
   @brief 设置Category的对齐方式

   居中对齐会让pannel以居中进行对齐
   @param al
 */
void SARibbonCategoryLayout::setCategoryAlignment(SARibbonAlignment al)
{
    d_ptr->mCategoryAlignment = al;
}

/**
   @brief Category的对齐方式
   @return
 */
SARibbonAlignment SARibbonCategoryLayout::categoryAlignment() const
{
    return d_ptr->mCategoryAlignment;
}

void SARibbonCategoryLayout::onLeftScrollButtonClicked()
{
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
    int width                  = category->width();
    // 求总宽
    int totalWidth = d_ptr->mTotalWidth;

    if (totalWidth > width) {
        int tmp = d_ptr->mXBase + width;
        if (tmp > 0) {
            tmp = 0;
        }
        d_ptr->mXBase = tmp;
    } else {
        d_ptr->mXBase = 0;
    }
    invalidate();
}

void SARibbonCategoryLayout::onRightScrollButtonClicked()
{
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
    int width                  = category->width();
    // 求总宽
    int totalWidth = d_ptr->mTotalWidth;

    if (totalWidth > width) {
        int tmp = d_ptr->mXBase - width;
        if (tmp < (width - totalWidth)) {
            tmp = width - totalWidth;
        }
        d_ptr->mXBase = tmp;
    } else {
        d_ptr->mXBase = 0;
    }
    invalidate();
}

void SARibbonCategoryLayout::setGeometry(const QRect& rect)
{
    QRect old = geometry();
    if (old == rect) {
        return;
    }
#if SARibbonCategoryLayout_DEBUG_PRINT && SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "===========SARibbonCategoryLayout.setGeometry(" << rect << "(" << ribbonCategory()->categoryName() << ")=======";
#endif
    QLayout::setGeometry(rect);
    d_ptr->mDirty = false;
    updateGeometryArr();
    doLayout();
}
//=============================================================
// SARibbonCategoryLayoutItem
//=============================================================

SARibbonCategoryLayoutItem::SARibbonCategoryLayoutItem(SARibbonPannel* w) : QWidgetItem(w)
{
    separatorWidget = nullptr;
}

SARibbonPannel* SARibbonCategoryLayoutItem::toPannelWidget()
{
    return qobject_cast< SARibbonPannel* >(widget());
}

/*** End of inlined file: SARibbonCategoryLayout.cpp ***/

/*** Start of inlined file: SARibbonContextCategory.cpp ***/
#include <QList>
#include <QVariant>

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
    d_ptr->categoryDataList.append(catData);
    category->installEventFilter(this);
    emit categoryPageAdded(category);
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
 * @note 此函数会触发信号@sa contextTitleChanged
 */
void SARibbonContextCategory::setContextTitle(const QString& contextTitle)
{
    d_ptr->contextTitle = contextTitle;
    emit contextTitleChanged(contextTitle);
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

/**
 * @brief 判断上下文是否维护了此SARibbonCategory
 * @param category
 * @return
 */
bool SARibbonContextCategory::isHaveCategory(SARibbonCategory* category) const
{
    for (int i = 0; i < d_ptr->categoryDataList.size(); ++i) {
        if (d_ptr->categoryDataList[ i ].categoryPage == category) {
            return (true);
        }
    }
    return (false);
}

void SARibbonContextCategory::onCategoryTitleChanged(const QString& title)
{
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(sender());
    if (category) {
        emit categoryTitleChanged(category, title);
    }
}

/*** End of inlined file: SARibbonContextCategory.cpp ***/

/*** Start of inlined file: SARibbonGalleryItem.cpp ***/
SARibbonGalleryItem::SARibbonGalleryItem() : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), m_action(nullptr)
{
}

SARibbonGalleryItem::SARibbonGalleryItem(const QString& text, const QIcon& icon)
    : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable), m_action(nullptr)
{
    setText(text);
    setIcon(icon);
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

SARibbonGalleryItem::SARibbonGalleryItem(QAction* act) : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
{
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setAction(act);
}

SARibbonGalleryItem::~SARibbonGalleryItem()
{
}

void SARibbonGalleryItem::setData(int role, const QVariant& data)
{
    m_datas[ role ] = data;
}

QVariant SARibbonGalleryItem::data(int role) const
{
    if (m_action) {
        switch (role) {
        case Qt::DisplayRole:
            return (m_action->text());

        case Qt::ToolTipRole:
            return (m_action->toolTip());

        case Qt::DecorationRole:
            return (m_action->icon());
        default:
            break;
        }
    }
    return (m_datas.value(role));
}

void SARibbonGalleryItem::setText(const QString& text)
{
    setData(Qt::DisplayRole, text);
}

QString SARibbonGalleryItem::text() const
{
    if (m_action) {
        return (m_action->text());
    }
    return (data(Qt::DisplayRole).toString());
}

void SARibbonGalleryItem::setToolTip(const QString& text)
{
    setData(Qt::ToolTipRole, text);
}

QString SARibbonGalleryItem::toolTip() const
{
    if (m_action) {
        return (m_action->toolTip());
    }
    return (data(Qt::ToolTipRole).toString());
}

void SARibbonGalleryItem::setIcon(const QIcon& ico)
{
    setData(Qt::DecorationRole, ico);
}

QIcon SARibbonGalleryItem::icon() const
{
    if (m_action) {
        return (m_action->icon());
    }
    return (qvariant_cast< QIcon >(data(Qt::DecorationRole)));
}

bool SARibbonGalleryItem::isSelectable() const
{
    return (m_flags & Qt::ItemIsSelectable);
}

void SARibbonGalleryItem::setSelectable(bool isSelectable)
{
    if (isSelectable) {
        m_flags |= Qt::ItemIsSelectable;
    } else {
        m_flags = (m_flags & (~Qt::ItemIsSelectable));
    }
}

bool SARibbonGalleryItem::isEnable() const
{
    if (m_action) {
        return (m_action->isEnabled());
    }
    return (m_flags & Qt::ItemIsEnabled);
}

void SARibbonGalleryItem::setEnable(bool isEnable)
{
    if (m_action) {
        m_action->setEnabled(isEnable);
    }

    if (isEnable) {
        m_flags |= Qt::ItemIsEnabled;
    } else {
        m_flags = (m_flags & (~Qt::ItemIsEnabled));
    }
}

void SARibbonGalleryItem::setFlags(Qt::ItemFlags flag)
{
    m_flags = flag;
    if (m_action) {
        m_action->setEnabled(flag & Qt::ItemIsEnabled);
    }
}

Qt::ItemFlags SARibbonGalleryItem::flags() const
{
    return (m_flags);
}

void SARibbonGalleryItem::setAction(QAction* act)
{
    m_action = act;
    if (nullptr == m_action) {
        return;
    }
    if (act->isEnabled()) {
        m_flags |= Qt::ItemIsEnabled;
    } else {
        m_flags = (m_flags & (~Qt::ItemIsEnabled));
    }
}

QAction* SARibbonGalleryItem::action()
{
    return (m_action);
}

void SARibbonGalleryItem::setTextAlignment(Qt::Alignment a)
{
    setData(Qt::TextAlignmentRole, (int)a);
}

Qt::Alignment SARibbonGalleryItem::textAlignment() const
{
    return qvariant_cast< Qt::Alignment >(data(Qt::TextAlignmentRole));
}

/*** End of inlined file: SARibbonGalleryItem.cpp ***/

/*** Start of inlined file: SARibbonGalleryGroup.cpp ***/
#include <QPainter>
#include <QDebug>
#include <QActionGroup>
#include <QItemSelectionModel>

/**
 * @brief The SARibbonGalleryGroupPrivate class
 */
class SARibbonGalleryGroup::PrivateData
{
public:
    SARibbonGalleryGroup* q_ptr;
    QString mGroupTitle;
    SARibbonGalleryGroup::GalleryGroupStyle mPreStyle { SARibbonGalleryGroup::IconWithText };
    SARibbonGalleryGroup::DisplayRow mDisplayRow { SARibbonGalleryGroup::DisplayOneRow };
    bool mBlockRecalc { false };
    int mGridMinimumWidth { 0 };             ///< grid最小宽度
    int mGridMaximumWidth { 0 };             ///< grid最大宽度
    QActionGroup* mActionGroup { nullptr };  ///< 所有GalleryGroup管理的actions都由这个actiongroup管理
public:
    PrivateData(SARibbonGalleryGroup* p) : q_ptr(p)
    {
        mActionGroup = new QActionGroup(p);
        p->connect(mActionGroup, &QActionGroup::triggered, p, &SARibbonGalleryGroup::triggered);
        p->connect(mActionGroup, &QActionGroup::hovered, p, &SARibbonGalleryGroup::hovered);
    }
};

//===================================================
// SARibbonGalleryGroupItemDelegate
//===================================================

SARibbonGalleryGroupItemDelegate::SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent)
    : QStyledItemDelegate(parent), m_group(group)
{
}

void SARibbonGalleryGroupItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (nullptr == m_group) {
        return;
    }
    switch (m_group->galleryGroupStyle()) {
    case SARibbonGalleryGroup::IconWithText:
        paintIconWithText(painter, option, index);
        break;
    case SARibbonGalleryGroup::IconWithWordWrapText:
        paintIconWithTextWordWrap(painter, option, index);
        break;
    case SARibbonGalleryGroup::IconOnly:
        paintIconOnly(painter, option, index);
        break;
    default:
        paintIconWithText(painter, option, index);
        break;
    }
}

void SARibbonGalleryGroupItemDelegate::paintIconOnly(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyle* style = m_group->style();
    int sp        = m_group->spacing();
    sp += 3;
    painter->save();
    painter->setClipRect(option.rect);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, m_group);
    // draw the icon
    QRect iconRect = option.rect;

    iconRect.adjust(sp, sp, -sp, -sp);
    QIcon ico = qvariant_cast< QIcon >(index.data(Qt::DecorationRole));

    ico.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    painter->restore();
}

void SARibbonGalleryGroupItemDelegate::paintIconWithText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

void SARibbonGalleryGroupItemDelegate::paintIconWithTextWordWrap(QPainter* painter,
                                                                 const QStyleOptionViewItem& option,
                                                                 const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize SARibbonGalleryGroupItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    return m_group->gridSize();
}

//===================================================
// SARibbonGalleryGroupModel
//===================================================

SARibbonGalleryGroupModel::SARibbonGalleryGroupModel(QObject* parent) : QAbstractListModel(parent)
{
}

SARibbonGalleryGroupModel::~SARibbonGalleryGroupModel()
{
    clear();
}

int SARibbonGalleryGroupModel::rowCount(const QModelIndex& parent) const
{
    return (parent.isValid() ? 0 : m_items.size());
}

Qt::ItemFlags SARibbonGalleryGroupModel::flags(const QModelIndex& index) const
{
    if (!index.isValid() || (index.row() >= m_items.size())) {
        return (Qt::NoItemFlags);
    }
    return (m_items.at(index.row())->flags());
}

QVariant SARibbonGalleryGroupModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return (QVariant());
    }
    return (m_items.at(index.row())->data(role));
}

QModelIndex SARibbonGalleryGroupModel::index(int row, int column, const QModelIndex& parent) const
{
    if (hasIndex(row, column, parent)) {
        return (createIndex(row, column, m_items.at(row)));
    }
    return (QModelIndex());
}

bool SARibbonGalleryGroupModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return (false);
    }

    m_items.at(index.row())->setData(role, value);
    return (true);
}

void SARibbonGalleryGroupModel::clear()
{
    beginResetModel();
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)) {
            delete m_items.at(i);
        }
    }
    m_items.clear();
    endResetModel();
}

SARibbonGalleryItem* SARibbonGalleryGroupModel::at(int row) const
{
    return (m_items.value(row));
}

void SARibbonGalleryGroupModel::insert(int row, SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}

SARibbonGalleryItem* SARibbonGalleryGroupModel::take(int row)
{
    if ((row < 0) || (row >= m_items.count())) {
        return (0);
    }

    beginRemoveRows(QModelIndex(), row, row);
    SARibbonGalleryItem* item = m_items.takeAt(row);

    endRemoveRows();
    return (item);
}

void SARibbonGalleryGroupModel::append(SARibbonGalleryItem* item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count() + 1);
    m_items.append(item);
    endInsertRows();
}

//===================================================
// SARibbonGalleryGroup
//===================================================
SARibbonGalleryGroup::SARibbonGalleryGroup(QWidget* w)
    : QListView(w), d_ptr(new SARibbonGalleryGroup::PrivateData(this))
{
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setSelectionRectVisible(true);
    setUniformItemSizes(true);
    setSpacing(1);
    setItemDelegate(new SARibbonGalleryGroupItemDelegate(this, this));
    connect(this, &QAbstractItemView::clicked, this, &SARibbonGalleryGroup::onItemClicked);
    SARibbonGalleryGroupModel* m = new SARibbonGalleryGroupModel(this);
    setModel(m);
}

SARibbonGalleryGroup::~SARibbonGalleryGroup()
{
}

/**
 * @brief 是否禁止计算
 * @param on
 */
void SARibbonGalleryGroup::setRecalcGridSizeBlock(bool on)
{
    d_ptr->mBlockRecalc = on;
}

bool SARibbonGalleryGroup::isRecalcGridSizeBlock() const
{
    return d_ptr->mBlockRecalc;
}

/**
 * @brief 重新计算grid和icon的尺寸
 */
void SARibbonGalleryGroup::recalcGridSize()
{
    recalcGridSize(height());
}

void SARibbonGalleryGroup::recalcGridSize(int galleryHeight)
{
    if (isRecalcGridSizeBlock()) {
        return;
    }
    // 首先通过DisplayRow计算GridSize
    int dr = static_cast< int >(displayRow());
    if (dr < 1) {
        dr = 1;
    } else if (dr > 3) {
        dr = 3;
    }
    int h = galleryHeight / dr;
    if (h <= 1) {
        h = galleryHeight;
    }
    int w = h;
    if (gridMinimumWidth() > 0) {
        if (w < gridMinimumWidth()) {
            w = gridMinimumWidth();
        }
    }
    if (gridMaximumWidth() > 0) {
        if (w > gridMaximumWidth()) {
            w = gridMaximumWidth();
        }
    }
    setGridSize(QSize(w, h));
    // 在通过GalleryGroupStyle确定icon的尺寸
    const int shiftpix = 4;  // 这个是移动像素，qt在鼠标移动到图标上时会移动一下，给用户明确的动态，导致如果布局很满会超出显示范围，因此要在此基础上缩放一点
    switch (galleryGroupStyle()) {
    case IconWithText: {
        int textHeight = fontMetrics().lineSpacing();
        int iconHeight = h - textHeight - 2 * spacing() - shiftpix;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconWithWordWrapText: {
        int textHeight = fontMetrics().lineSpacing() * 2;
        int iconHeight = h - textHeight;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight - 2 * spacing() - shiftpix));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconOnly: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    default: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    }
#if 0
	qDebug() << "SARibbonGalleryGroup::recalcGridSize(" << galleryHeight << "): gridSize=" << gridSize()
			 << " iconSize=" << iconSize();
#endif
}

///
/// \brief 设置默认的预设样式
/// \param style
///
void SARibbonGalleryGroup::setGalleryGroupStyle(SARibbonGalleryGroup::GalleryGroupStyle style)
{
    d_ptr->mPreStyle = style;
    if (style == IconWithWordWrapText) {
        setWordWrap(true);
    }
    recalcGridSize();
}

SARibbonGalleryGroup::GalleryGroupStyle SARibbonGalleryGroup::galleryGroupStyle() const
{
    return d_ptr->mPreStyle;
}

void SARibbonGalleryGroup::addItem(const QString& text, const QIcon& icon)
{
    if (nullptr == groupModel()) {
        return;
    }
    addItem(new SARibbonGalleryItem(text, icon));
}

/**
 * @brief 添加一个条目
 *
 * @param item 条目的内存所有权归属SARibbonGalleryGroup管理
 */
void SARibbonGalleryGroup::addItem(SARibbonGalleryItem* item)
{
    if (nullptr == groupModel()) {
        return;
    }
    groupModel()->append(item);
}

void SARibbonGalleryGroup::addActionItem(QAction* act)
{
    if (nullptr == groupModel()) {
        return;
    }
    d_ptr->mActionGroup->addAction(act);
    groupModel()->append(new SARibbonGalleryItem(act));
}

void SARibbonGalleryGroup::addActionItemList(const QList< QAction* >& acts)
{
    SARibbonGalleryGroupModel* model = groupModel();

    if (nullptr == model) {
        return;
    }
    for (QAction* a : acts) {
        d_ptr->mActionGroup->addAction(a);
    }
    for (int i = 0; i < acts.size(); ++i) {
        model->append(new SARibbonGalleryItem(acts[ i ]));
    }
}

///
/// \brief 构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
///
void SARibbonGalleryGroup::setupGroupModel()
{
    setModel(new SARibbonGalleryGroupModel(this));
}

SARibbonGalleryGroupModel* SARibbonGalleryGroup::groupModel()
{
    return (qobject_cast< SARibbonGalleryGroupModel* >(model()));
}

void SARibbonGalleryGroup::setGroupTitle(const QString& title)
{
    d_ptr->mGroupTitle = title;
    emit groupTitleChanged(d_ptr->mGroupTitle);
}

QString SARibbonGalleryGroup::groupTitle() const
{
    return (d_ptr->mGroupTitle);
}

void SARibbonGalleryGroup::selectByIndex(int i)
{
    SARibbonGalleryGroupModel* model = groupModel();

    if (nullptr == model) {
        return;
    }
    QModelIndex ind               = model->index(i, 0, QModelIndex());
    QItemSelectionModel* selmodel = selectionModel();

    if (selmodel) {
        selmodel->select(ind, QItemSelectionModel::SelectCurrent);
    }
}

/**
 * @brief 设置显示的行数
 * @param r
 */
void SARibbonGalleryGroup::setDisplayRow(DisplayRow r)
{
    d_ptr->mDisplayRow = r;
    recalcGridSize();
}

/**
 * @brief Gallery显示的行数
 * @return
 */
SARibbonGalleryGroup::DisplayRow SARibbonGalleryGroup::displayRow() const
{
    return d_ptr->mDisplayRow;
}

/**
 * @brief 设置grid最小的宽度，默认为0（不限制）
 * @param w
 */
void SARibbonGalleryGroup::setGridMinimumWidth(int w)
{
    d_ptr->mGridMinimumWidth = w;
}

/**
 * @brief grid最小的宽度，默认为0（不限制）
 * @return
 */
int SARibbonGalleryGroup::gridMinimumWidth() const
{
    return d_ptr->mGridMinimumWidth;
}

/**
 * @brief 设置grid最大的宽度，默认为0（不限制）
 * @param w
 */
void SARibbonGalleryGroup::setGridMaximumWidth(int w)
{
    d_ptr->mGridMaximumWidth = w;
}

/**
 * @brief grid最大的的宽度，默认为0（不限制）
 * @param w
 */
int SARibbonGalleryGroup::gridMaximumWidth() const
{
    return d_ptr->mGridMaximumWidth;
}

/**
 * @brief 获取SARibbonGalleryGroup管理的actiongroup
 * @return
 */
QActionGroup* SARibbonGalleryGroup::actionGroup() const
{
    return d_ptr->mActionGroup;
}

void SARibbonGalleryGroup::onItemClicked(const QModelIndex& index)
{
    if (index.isValid()) {
        SARibbonGalleryItem* item = (SARibbonGalleryItem*)index.internalPointer();
        if (item) {
            QAction* act = item->action();
            if (act) {
                act->activate(QAction::Trigger);
            }
        }
    }
}

void SARibbonGalleryGroup::onItemEntered(const QModelIndex& index)
{
    if (index.isValid()) {
        SARibbonGalleryItem* item = (SARibbonGalleryItem*)index.internalPointer();
        if (item) {
            QAction* act = item->action();
            if (act) {
                act->activate(QAction::Hover);
            }
        }
    }
}

/*** End of inlined file: SARibbonGalleryGroup.cpp ***/

/*** Start of inlined file: SARibbonGallery.cpp ***/
#include <QIcon>
#include <QApplication>
#define ICON_ARROW_UP QIcon(":/image/resource/ArrowUp.png")
#define ICON_ARROW_DOWN QIcon(":/image/resource/ArrowDown.png")
#define ICON_ARROW_MORE QIcon(":/image/resource/ArrowMore.png")
#include <QResizeEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>

#include <QActionGroup>

/**
 * @brief The SARibbonGalleryPrivate class
 */
class SARibbonGallery::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonGallery)
public:
    static int sGalleryButtonMaximumWidth;
    SARibbonGalleryButton* buttonUp { nullptr };
    SARibbonGalleryButton* buttonDown { nullptr };
    SARibbonGalleryButton* buttonMore { nullptr };
#if 0
	SARibbonMenu *popupWidget;
#else
    SARibbonGalleryViewport* popupWidget { nullptr };
#endif
    SARibbonGalleryGroup* viewportGroup { nullptr };
    QBoxLayout* btnLayout { nullptr };
    QBoxLayout* layout { nullptr };
    PrivateData(SARibbonGallery* p) : q_ptr(p)
    {
    }

    void init()
    {
        buttonUp   = new SARibbonGalleryButton(q_ptr);
        buttonDown = new SARibbonGalleryButton(q_ptr);
        buttonMore = new SARibbonGalleryButton(q_ptr);
        buttonUp->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonDown->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonMore->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonUp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        buttonDown->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        buttonMore->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        buttonUp->setObjectName(QStringLiteral("SARibbonGalleryButtonUp"));
        buttonDown->setObjectName(QStringLiteral("SARibbonGalleryButtonDown"));
        buttonMore->setObjectName(QStringLiteral("SARibbonGalleryButtonMore"));
        buttonUp->setMaximumWidth(sGalleryButtonMaximumWidth);
        buttonDown->setMaximumWidth(sGalleryButtonMaximumWidth);
        buttonMore->setMaximumWidth(sGalleryButtonMaximumWidth);
        buttonUp->setIcon(ICON_ARROW_UP);
        buttonDown->setIcon(ICON_ARROW_DOWN);
        buttonMore->setIcon(ICON_ARROW_MORE);
        q_ptr->connect(buttonUp, &QAbstractButton::clicked, q_ptr, &SARibbonGallery::pageUp);
        q_ptr->connect(buttonDown, &QAbstractButton::clicked, q_ptr, &SARibbonGallery::pageDown);
        q_ptr->connect(buttonMore, &QAbstractButton::clicked, q_ptr, &SARibbonGallery::showMoreDetail);
        // 信号转发
        q_ptr->connect(q_ptr, &SARibbonGallery::triggered, q_ptr, &SARibbonGallery::onTriggered);
        popupWidget   = nullptr;
        viewportGroup = nullptr;
        btnLayout     = new QBoxLayout(QBoxLayout::TopToBottom);
        btnLayout->setSpacing(0);
        btnLayout->setContentsMargins(0, 0, 0, 0);
        btnLayout->addWidget(buttonUp);
        btnLayout->addWidget(buttonDown);
        btnLayout->addWidget(buttonMore);
        layout = new QBoxLayout(QBoxLayout::RightToLeft);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addLayout(btnLayout);
        layout->addStretch();
        q_ptr->setLayout(layout);
    }

    bool isValid() const
    {
        return (q_ptr != nullptr);
    }

    void createPopupWidget()
    {
        if (nullptr == popupWidget) {
#if 0
			popupWidget = new SARibbonMenu(Parent);
#else
            popupWidget = new SARibbonGalleryViewport(q_ptr);
#endif
        }
    }

    void setViewPort(SARibbonGalleryGroup* v)
    {
        if (nullptr == viewportGroup) {
            viewportGroup = RibbonSubElementFactory->createRibbonGalleryGroup(q_ptr);
            layout->addWidget(viewportGroup, 1);
        }
        viewportGroup->setRecalcGridSizeBlock(true);
        viewportGroup->setGalleryGroupStyle(v->galleryGroupStyle());
        viewportGroup->setDisplayRow(v->displayRow());
        viewportGroup->setSpacing(v->spacing());
        viewportGroup->setGridMaximumWidth(v->gridMaximumWidth());
        viewportGroup->setGridMinimumWidth(v->gridMinimumWidth());
        viewportGroup->setRecalcGridSizeBlock(false);
        viewportGroup->recalcGridSize(viewportGroup->height());
        viewportGroup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        viewportGroup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        viewportGroup->setModel(v->model());
        viewportGroup->show();
    }
};

// 静态变量初始化

/**
 * @brief SARibbonGalleryPrivate::sGalleryButtonMaximumWidth
 */
int SARibbonGallery::PrivateData::sGalleryButtonMaximumWidth = 15;

//===================================================
// SARibbonGalleryButton
//===================================================

SARibbonGalleryButton::SARibbonGalleryButton(QWidget* parent) : QToolButton(parent)
{
}

//===================================================
// SARibbonGalleryViewport
//===================================================

SARibbonGalleryViewport::SARibbonGalleryViewport(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::Popup);
    QPalette pl = palette();
    pl.setBrush(QPalette::Window, pl.brush(QPalette::Base));
    setPalette(pl);
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
}

/**
 * @brief 添加窗口不带标题
 * @param w
 */
void SARibbonGalleryViewport::addWidget(QWidget* w)
{
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addWidget(w);
}

/**
 * @brief 添加窗口，带标题
 * @param w
 * @param title
 */
void SARibbonGalleryViewport::addWidget(QWidget* w, const QString& title)
{
    QLabel* label = new QLabel(this);
    label->setText(title);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addWidget(label);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addWidget(w);
    _widgetToTitleLable[ w ] = label;
}

/**
 * @brief 通过SARibbonGalleryGroup获取对应的标题，用户可以通过此函数设置QLabel的属性
 * @param g
 * @return 如果没有管理group，将返回nullptr
 */
QLabel* SARibbonGalleryViewport::titleLabel(QWidget* w)
{
    return _widgetToTitleLable.value(w, nullptr);
}

/**
 * @brief 移除窗口
 * @param w
 */
void SARibbonGalleryViewport::removeWidget(QWidget* w)
{
    QLabel* label = titleLabel(w);
    if (label) {
        m_layout->removeWidget(label);
    }
    m_layout->removeWidget(w);
}

/**
 * @brief widget的标题改变
 * @param g
 * @param title
 */
void SARibbonGalleryViewport::onTitleChanged(QWidget* w, const QString& title)
{
    QLabel* l = titleLabel(w);
    if (l) {
        l->setText(title);
    }
}

//===================================================
// SARibbonGallery
//===================================================

SARibbonGallery::SARibbonGallery(QWidget* parent) : QFrame(parent), d_ptr(new SARibbonGallery::PrivateData(this))
{
    d_ptr->init();
    setFrameShape(QFrame::Box);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(200);
}

SARibbonGallery::~SARibbonGallery()
{
}

QSize SARibbonGallery::sizeHint() const
{
    return (QSize(100, 62));
}

/**
 * @brief 获取一个空白SARibbonGalleryGroup
 * @return
 */
SARibbonGalleryGroup* SARibbonGallery::addGalleryGroup()
{
    SARibbonGalleryGroup* group = RibbonSubElementFactory->createRibbonGalleryGroup(this);
    addGalleryGroup(group);
    return (group);
}

/**
 * @brief 添加一组SARibbonGalleryGroup
 * @param group
 */
void SARibbonGallery::addGalleryGroup(SARibbonGalleryGroup* group)
{
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    SARibbonGalleryViewport* viewport = ensureGetPopupViewPort();
    viewport->addWidget(group, group->groupTitle());
    connect(group, &QAbstractItemView::clicked, this, &SARibbonGallery::onItemClicked);
    connect(group, &SARibbonGalleryGroup::groupTitleChanged, this, [ group, viewport ](const QString& t) {
        viewport->onTitleChanged(group, t);
    });
    connect(group, &SARibbonGalleryGroup::triggered, this, &SARibbonGallery::triggered);
    connect(group, &SARibbonGalleryGroup::hovered, this, &SARibbonGallery::hovered);
    setCurrentViewGroup(group);
}

/**
 * @brief 添加一组actions
 * @param title actions组的名字
 * @param actions
 * @return 返回SARibbonGalleryGroup，用户可以通过修改SARibbonGalleryGroup属性控制其显示方法
 */
SARibbonGalleryGroup* SARibbonGallery::addCategoryActions(const QString& title, QList< QAction* > actions)
{
    SARibbonGalleryGroup* group = RibbonSubElementFactory->createRibbonGalleryGroup(this);
    if (!title.isEmpty()) {
        group->setGroupTitle(title);
    }
    group->addActionItemList(actions);
    addGalleryGroup(group);
    return (group);
}

void SARibbonGallery::setCurrentViewGroup(SARibbonGalleryGroup* group)
{
    d_ptr->setViewPort(group);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

SARibbonGalleryGroup* SARibbonGallery::currentViewGroup() const
{
    return (d_ptr->viewportGroup);
}

/**
 * @brief 获取弹出窗口
 * @return
 */
SARibbonGalleryViewport* SARibbonGallery::getPopupViewPort() const
{
    return d_ptr->popupWidget;
}

/**
 * @brief 设置最右边三个控制按钮的最大宽度（默认15）
 * @param w
 */
void SARibbonGallery::setGalleryButtonMaximumWidth(int w)
{
    SARibbonGallery::PrivateData::sGalleryButtonMaximumWidth = w;
}

/**
 * @brief 上翻页
 */
void SARibbonGallery::pageDown()
{
    if (d_ptr->viewportGroup) {
        QScrollBar* vscrollBar = d_ptr->viewportGroup->verticalScrollBar();
        int v                  = vscrollBar->value();
        v += vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}

/**
 * @brief 下翻页
 */
void SARibbonGallery::pageUp()
{
    if (d_ptr->viewportGroup) {
        QScrollBar* vscrollBar = d_ptr->viewportGroup->verticalScrollBar();
        int v                  = vscrollBar->value();
        v -= vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}

/**
 * @brief 显示更多触发，默认弹出内部管理的SARibbonGalleryViewport，用户可重载此函数实现自定义的弹出
 */
void SARibbonGallery::showMoreDetail()
{
    if (nullptr == d_ptr->popupWidget) {
        return;
    }
    QSize popupMenuSize = d_ptr->popupWidget->sizeHint();
    QPoint start        = mapToGlobal(QPoint(0, 0));

    int width = d_ptr->viewportGroup->width();  // viewport

    width += qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);  // scrollbar
    d_ptr->popupWidget->setGeometry(start.x(), start.y(), width, popupMenuSize.height());
    d_ptr->popupWidget->show();
}

void SARibbonGallery::onItemClicked(const QModelIndex& index)
{
    QObject* obj                = sender();
    SARibbonGalleryGroup* group = qobject_cast< SARibbonGalleryGroup* >(obj);
    if (group) {
        setCurrentViewGroup(group);
        SARibbonGalleryGroup* curGroup = currentViewGroup();
        curGroup->scrollTo(index);
        curGroup->setCurrentIndex(index);
    }
}

void SARibbonGallery::onTriggered(QAction* action)
{
    Q_UNUSED(action);
    // 点击后关闭弹出窗口
    if (d_ptr->popupWidget) {
        if (d_ptr->popupWidget->isVisible()) {
            d_ptr->popupWidget->hide();
        }
    }
}

SARibbonGalleryViewport* SARibbonGallery::ensureGetPopupViewPort()
{
    if (nullptr == d_ptr->popupWidget) {
        d_ptr->createPopupWidget();
    }
    return (d_ptr->popupWidget);
}

void SARibbonGallery::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    // 对SARibbonGalleryViewport所有SARibbonGalleryGroup重置尺寸
    int h = layout()->contentsRect().height();
    if (d_ptr->viewportGroup) {
        h = d_ptr->viewportGroup->height();
        d_ptr->viewportGroup->recalcGridSize();
    }
    if (d_ptr->popupWidget) {
        QLayout* lay = d_ptr->popupWidget->layout();
        if (!lay) {
            return;
        }
        int c = lay->count();
        for (int i = 0; i < c; ++i) {
            QLayoutItem* item = lay->itemAt(i);
            if (!item) {
                continue;
            }
            QWidget* w = item->widget();
            if (!w) {
                continue;
            }
            SARibbonGalleryGroup* g = qobject_cast< SARibbonGalleryGroup* >(w);
            if (!g) {
                continue;
            }
            g->recalcGridSize(h);
        }
    }
}

void SARibbonGallery::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
}

/*** End of inlined file: SARibbonGallery.cpp ***/

/*** Start of inlined file: SARibbonBar.cpp ***/
#include <QPointer>
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QHoverEvent>
#include <QLinearGradient>
#include <QPainter>
#include <QResizeEvent>
#include <QSet>
#include <QStyleOptionMenuItem>
#include <QTimer>
#include <QVariant>

#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashDotDotLine);                                                                                 \
        _pen.setColor(QColor(219, 26, 59));                                                                            \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

class _SAContextCategoryManagerData
{
public:
    SARibbonContextCategory* contextCategory;
    QList< int > tabPageIndex;
    bool operator==(const SARibbonContextCategory* contextPage)
    {
        return (this->contextCategory == contextPage);
    }
};

/**
 * @todo 此处要修改，此方式容易异常
 */
class _SARibbonTabData
{
public:
    SARibbonCategory* category;
    int index;
    _SARibbonTabData() : category(nullptr), index(-1)
    {
    }
};
Q_DECLARE_METATYPE(_SARibbonTabData)

class SARibbonBar::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonBar)
public:
    QPointer< QAbstractButton > mApplicationButton;
    QPointer< SARibbonTabBar > mRibbonTabBar;
    QPointer< SARibbonStackedWidget > mStackedContainerWidget;
    QPointer< SARibbonButtonGroupWidget > mRightButtonGroup;  ///< 在tab bar右边的按钮群
    QPointer< SARibbonQuickAccessBar > mQuickAccessBar;       ///< 快速响应栏
    QAction* mMinimumCategoryButtonAction { nullptr };        ///< 隐藏面板按钮action
    QList< _SAContextCategoryManagerData > mCurrentShowingContextCategory;
    QList< SARibbonContextCategory* > mContextCategoryList;  ///< 存放所有的上下文标签
    QList< _SARibbonTabData > mHidedCategory;
    int mIconRightBorderPosition { 1 };  ///< 标题栏x值得最小值，在有图标和快捷启动按钮，此值都需要变化
    SARibbonBar::RibbonStyles mRibbonStyle { SARibbonBar::RibbonStyleLooseThreeRow };  ///< ribbon的风格
    SARibbonBar::RibbonMode mCurrentRibbonMode { SARibbonBar::NormalRibbonMode };      ///< 记录当前模式
    QSize mWindowButtonSize;                    ///< 由SARibbonMainWindow告诉的windowbutton的尺寸
    QList< QColor > mContextCategoryColorList;  ///< contextCategory的色系
    int mContextCategoryColorListIndex { -1 };  ///< 记录contextCategory色系索引
    QColor mTitleTextColor;  ///< 标题文字颜色,默认无效，无效的情况下和SARibbonBar的qss:color属性一致
    QColor mTabBarBaseLineColor { QColor(186, 201, 219) };  ///< tabbar 底部会绘制一条线条，定义线条颜色
    Qt::Alignment mTitleAligment { Qt::AlignCenter };       ///< 标题对齐方式
    bool mIsTitleVisible { true };                          ///< 标题是否显示
    SARibbonAlignment mRibbonAlignment { SARibbonAlignment::AlignLeft };                         ///< 对齐方式
    SARibbonPannel::PannelLayoutMode mDefaulePannelLayoutMode { SARibbonPannel::ThreeRowMode };  ///< 默认的PannelLayoutMode
    bool mEnableShowPannelTitle { true };  ///< 是否运行pannel的标题栏显示
    bool mIsTabOnTitle { false };          ///< 是否tab在标题栏上
    int mTitleBarHeight { 30 };            ///< 标题栏高度
    int mTabBarHeight { 28 };              ///< tabbar高度
    int mPannelTitleHeight { 15 };         ///< pannel的标题栏默认高度
    int mCategoryHeight { 60 };            ///< Category的高度
    std::unique_ptr< int > mUserDefTitleBarHeight;  ///< 用户定义的标题栏高度，正常不使用用户设定的高度，而是使用自动计算的高度
    std::unique_ptr< int > mUserDefTabBarHeight;  ///< 用户定义的tabbar高度，正常不使用用户设定的高度，而是使用自动计算的高度
    std::unique_ptr< int > mUserDefCategoryHeight;  ///< 用户定义的Category的高度，正常不使用用户设定的高度，而是使用自动计算的高度
public:
    PrivateData(SARibbonBar* par) : q_ptr(par)
    {
        mContextCategoryColorList = SARibbonBar::defaultContextCategoryColorList();
    }
    void init();
    // 计算tabbar高度
    int calcTabBarHeight();
    // 根据字体信息计算标题栏高度
    int calcTitleBarHeight();
    // 根据字体信息计算category的高度
    int calcCategoryHeight();
    // 计算tabbar高度
    static int calcMainBarHeight(int tabHegith, int titleHeight, int categoryHeight, bool tabOnTitle, SARibbonBar::RibbonMode rMode);
    // 获取当前最小模式下的高度
    int getCurrentMinimumModeMainBarHeight() const;
    // 获取当前正常模式下的高度
    int getCurrentNormalModeMainBarHeight() const;
    // 重置尺寸
    void resetSize();
    // 获取标题栏高度
    int titleBarHeight() const;
    // tabbar 高度
    int tabBarHeigth() const;
    // category高度
    int categoryHeight() const;
    // 更新推荐的尺寸值
    void updateHintSize();

    void setApplicationButton(QAbstractButton* btn);

    bool isContainContextCategoryInList(SARibbonContextCategory* contextCategory);

    void setMinimumMode();

    void setNormalMode();

    QColor getContextCategoryColor();

    void updateTabData();

    /**
     * @brief 通过输入高度计算iconSize
     * @param h
     * @return
     */
    static QSize calcIconSizeByHeight(int h);
};

void SARibbonBar::PrivateData::init()
{
    mApplicationButton = RibbonSubElementFactory->createRibbonApplicationButton(q_ptr);
    q_ptr->connect(mApplicationButton, &QAbstractButton::clicked, q_ptr, &SARibbonBar::applicationButtonClicked);
    mRibbonTabBar = RibbonSubElementFactory->createRibbonTabBar(q_ptr);
    mRibbonTabBar->setObjectName(QStringLiteral("objSARibbonTabBar"));
    mRibbonTabBar->setDrawBase(false);
    q_ptr->connect(mRibbonTabBar, &QTabBar::currentChanged, q_ptr, &SARibbonBar::onCurrentRibbonTabChanged);
    q_ptr->connect(mRibbonTabBar, &QTabBar::tabBarClicked, q_ptr, &SARibbonBar::onCurrentRibbonTabClicked);
    q_ptr->connect(mRibbonTabBar, &QTabBar::tabBarDoubleClicked, q_ptr, &SARibbonBar::onCurrentRibbonTabDoubleClicked);
    q_ptr->connect(mRibbonTabBar, &QTabBar::tabMoved, q_ptr, &SARibbonBar::onTabMoved);
    //
    mStackedContainerWidget = RibbonSubElementFactory->createRibbonStackedWidget(q_ptr);
    mStackedContainerWidget->setObjectName(QStringLiteral("objSARibbonStackedContainerWidget"));
    mStackedContainerWidget->connect(mStackedContainerWidget, &SARibbonStackedWidget::hidWindow, q_ptr, &SARibbonBar::onStackWidgetHided);
    // 捕获事件，在popmode时必须用到
    mStackedContainerWidget->installEventFilter(q_ptr);
    //
    mQuickAccessBar = RibbonSubElementFactory->createQuickAccessBar(q_ptr);
    mQuickAccessBar->setObjectName(QStringLiteral("objSARibbonQuickAccessBar"));
    mQuickAccessBar->setIcon(q_ptr->windowIcon());
    //
    mRightButtonGroup = RibbonSubElementFactory->craeteButtonGroupWidget(q_ptr);
    //
    setNormalMode();
}

/**
 * @brief 估算tabbar的高度
 * @param fm
 * @return
 */
int SARibbonBar::PrivateData::calcTabBarHeight()
{
    int defaultHeight = q_ptr->style()->pixelMetric(QStyle::PM_TabBarBaseHeight);
    int fontHeight = q_ptr->fontMetrics().lineSpacing();  // 不要用height，像宋体这种字体，height=12，lineSpacing=14，有些就无法显示
    int defaultHeight2 = fontHeight * 1.6;
    if (defaultHeight2 < fontHeight + 10) {
        defaultHeight2 = fontHeight + 10;  // 主要为了满足office2021主题，tab下有个4px的横杠
    }
    int r = qMax(defaultHeight, defaultHeight2);
    if (r < 20) {
        r = 20;
    }
    return r;
}

/**
 * @brief 估算标题栏的高度
 * @param fm
 * @return
 */
int SARibbonBar::PrivateData::calcTitleBarHeight()
{
    int defaultHeight  = q_ptr->style()->pixelMetric(QStyle::PM_TitleBarHeight);
    int defaultHeight2 = q_ptr->fontMetrics().height() * 1.8;
    int r              = qMax(defaultHeight, defaultHeight2);
    if (r < 25) {
        r = 25;
    }
    return r;
}

/**
 * @brief 估算category的高度
 * @note 经过对照，1.6行高和office的高度比较接近
 * @param fm
 * @param s
 * @return
 */
int SARibbonBar::PrivateData::calcCategoryHeight()
{
    int textH = q_ptr->fontMetrics().lineSpacing();  // 这里用linespace，因为在换行的情况下，行距是不可忽略的，ribbon的大按钮默认是2行
    if (SARibbonPannel::ThreeRowMode == mDefaulePannelLayoutMode) {
        // 5.5=（3*1.6+1） （三行）,1是给panneltitle预留的
        return textH * 4.8 + mPannelTitleHeight;
    } else {
        // 3=2*1.6
        return textH * 3.2 + mPannelTitleHeight;
    }
    return (textH * 4.8 + mPannelTitleHeight);
}

/**
 * @brief 计算总高
 *
 * @note 此总高是普通模式下的总高
 *
 * @param tabHegith
 * @param titleHeight
 * @param categoryHeight
 * @return
 */
int SARibbonBar::PrivateData::calcMainBarHeight(int tabHegith, int titleHeight, int categoryHeight, bool tabOnTitle, SARibbonBar::RibbonMode rMode)
{
    if (rMode == MinimumRibbonMode) {
        // 最小模式，没有categoryHeight
        if (tabOnTitle) {
            return titleHeight;
        } else {
            return titleHeight + tabHegith;
        }
    } else {
        if (tabOnTitle) {
            return titleHeight + categoryHeight;
        } else {
            return tabHegith + titleHeight + categoryHeight;
        }
    }
    return tabHegith + titleHeight + categoryHeight;
}

int SARibbonBar::PrivateData::getCurrentMinimumModeMainBarHeight() const
{
    return calcMainBarHeight(tabBarHeigth(), titleBarHeight(), categoryHeight(), mIsTabOnTitle, SARibbonBar::MinimumRibbonMode);
}

int SARibbonBar::PrivateData::getCurrentNormalModeMainBarHeight() const
{
    return calcMainBarHeight(tabBarHeigth(), titleBarHeight(), categoryHeight(), mIsTabOnTitle, SARibbonBar::NormalRibbonMode);
}

/**
 * @brief 重新计算尺寸
 */
void SARibbonBar::PrivateData::resetSize()
{
    updateHintSize();
    const int th      = tabBarHeigth();
    int mainBarHeight = calcMainBarHeight(th, titleBarHeight(), categoryHeight(), mIsTabOnTitle, mCurrentRibbonMode);
    if (mRibbonTabBar) {
        mRibbonTabBar->setFixedHeight(th);
    }
    // 处于最小模式下时，bar的高度为tabbar的bottom,这个调整必须在resize event之后
    q_ptr->setFixedHeight(mainBarHeight);
}

/**
 * @brief 标题栏高度
 * @return
 */
int SARibbonBar::PrivateData::titleBarHeight() const
{
    if (mUserDefTitleBarHeight) {
        return *mUserDefTitleBarHeight;
    }
    return mTitleBarHeight;
}

/**
 * @brief tab高度
 * @return
 */
int SARibbonBar::PrivateData::tabBarHeigth() const
{
    if (mUserDefTabBarHeight) {
        return *mUserDefTabBarHeight;
    }
    return mTabBarHeight;
}

/**
 * @brief category高度
 * @return
 */
int SARibbonBar::PrivateData::categoryHeight() const
{
    if (mUserDefCategoryHeight) {
        return *mUserDefCategoryHeight;
    } else {
        return mCategoryHeight;
    }
}

/**
 * @brief 更新推荐的尺寸值
 */
void SARibbonBar::PrivateData::updateHintSize()
{
    auto fm         = q_ptr->fontMetrics();
    mTitleBarHeight = calcTitleBarHeight();
    // mTabBarHeight有大于0的值说明用户设置了，就使用用户设置的值
    mTabBarHeight   = calcTabBarHeight();
    mCategoryHeight = calcCategoryHeight();
}

void SARibbonBar::PrivateData::setApplicationButton(QAbstractButton* btn)
{
    if (mApplicationButton) {
        mApplicationButton->hide();
        mApplicationButton->deleteLater();
    }
    if (btn) {
        if (btn->parent() != q_ptr) {
            btn->setParent(q_ptr);
        }
        btn->move(0, q_ptr->titleBarHeight());
        btn->show();
        q_ptr->connect(btn, &QAbstractButton::clicked, q_ptr, &SARibbonBar::applicationButtonClicked);
    }
    mApplicationButton = btn;
}

bool SARibbonBar::PrivateData::isContainContextCategoryInList(SARibbonContextCategory* contextCategory)
{
    for (int i = 0; i < mCurrentShowingContextCategory.size(); ++i) {
        if (mCurrentShowingContextCategory[ i ] == contextCategory) {
            return (true);
        }
    }
    return (false);
}

void SARibbonBar::PrivateData::setMinimumMode()
{
    mCurrentRibbonMode = SARibbonBar::MinimumRibbonMode;
    this->mStackedContainerWidget->setPopupMode();
    this->mStackedContainerWidget->setFocusPolicy(Qt::NoFocus);
    this->mStackedContainerWidget->clearFocus();
    this->mRibbonTabBar->setFocus();
    this->mStackedContainerWidget->hide();
    resetSize();
}

void SARibbonBar::PrivateData::setNormalMode()
{
    mCurrentRibbonMode = SARibbonBar::NormalRibbonMode;
    this->mStackedContainerWidget->setNormalMode();
    this->mStackedContainerWidget->setFocus();
    this->mStackedContainerWidget->show();
    resetSize();
}

QColor SARibbonBar::PrivateData::getContextCategoryColor()
{
    if (mContextCategoryColorList.isEmpty()) {
        mContextCategoryColorListIndex = -1;
        return (QColor());
    }
    ++mContextCategoryColorListIndex;
    if ((mContextCategoryColorListIndex >= mContextCategoryColorList.size()) || (mContextCategoryColorListIndex < 0)) {
        mContextCategoryColorListIndex = 0;
    }
    return (mContextCategoryColorList.at(mContextCategoryColorListIndex));
}

void SARibbonBar::PrivateData::updateTabData()
{
    int tabcount = mRibbonTabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = mRibbonTabBar->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            p.index            = i;
            mRibbonTabBar->setTabData(i, QVariant::fromValue(p));
        }
    }
    // 刷新完tabdata信息也要接着刷新ContextCategory信息
    for (_SAContextCategoryManagerData& cd : mCurrentShowingContextCategory) {
        cd.tabPageIndex.clear();
        for (int i = 0; i < cd.contextCategory->categoryCount(); ++i) {
            SARibbonCategory* category = cd.contextCategory->categoryPage(i);
            for (int t = 0; t < tabcount; ++t) {
                QVariant v = mRibbonTabBar->tabData(t);
                if (v.isValid()) {
                    _SARibbonTabData d = v.value< _SARibbonTabData >();
                    if (d.category == category) {
                        cd.tabPageIndex.append(t);
                    }
                } else {
                    cd.tabPageIndex.append(-1);
                }
            }
        }
    }
}

QSize SARibbonBar::PrivateData::calcIconSizeByHeight(int h)
{
    if (h - 8 >= 20) {
        return QSize(h - 8, h - 8);
    }
    return QSize(h - 4, h - 4);
}

//===================================================
// SARibbonBar
//===================================================

/**
 * @brief SARibbonBar构造函数
 * @param parent
 */
SARibbonBar::SARibbonBar(QWidget* parent) : QMenuBar(parent), d_ptr(new SARibbonBar::PrivateData(this))
{
    d_ptr->init();
    ensurePolished();
    setNativeMenuBar(false);
    // #ifdef Q_OS_MACOS
    //     setNativeMenuBar(false);
    // #endif
    // #ifdef Q_OS_LINUX
    //     setNativeMenuBar(false);
    // #endif
    if (parent) {
        connect(parent, &QWidget::windowTitleChanged, this, &SARibbonBar::onWindowTitleChanged);
        connect(parent, &QWidget::windowIconChanged, this, &SARibbonBar::onWindowIconChanged);
    }
    setRibbonStyle(RibbonStyleLooseThreeRow);
}

SARibbonBar::~SARibbonBar()
{
}

/**
 * @brief 判断样式是否为2行
 * @param s
 * @return 2行返回true，返回false代表当前是3行
 */
bool SARibbonBar::isTwoRowStyle(SARibbonBar::RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleTwoRow);
}

/**
 * @brief 判断样式是否为3行
 * @param s
 * @return 3行返回true，返回false代表当前是2行
 */
bool SARibbonBar::isThreeRowStyle(RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleThreeRow);
}

/**
 * @brief 判断是否是宽松样式
 * @param s
 * @return 宽松样式（带标题栏）返回true，否则就是紧凑样式
 */
bool SARibbonBar::isLooseStyle(SARibbonBar::RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleLoose);
}

/**
 * @brief 判断是否是紧凑样式
 * @param s
 * @return 紧凑样式（不带标题栏）返回true，否则就是宽松样式
 */
bool SARibbonBar::isCompactStyle(RibbonStyles s)
{
    return s.testFlag(SARibbonBar::RibbonStyleCompact);
}

/**
 * @brief 获取版本信息
 * @return {SA_RIBBON_BAR_VERSION_MAJ}.{SA_RIBBON_BAR_VERSION_MIN}.{SA_RIBBON_BAR_VERSION_PAT}
 */
QString SARibbonBar::versionString()
{
    return QString("%1.%2.%3").arg(SA_RIBBON_BAR_VERSION_MAJ).arg(SA_RIBBON_BAR_VERSION_MIN).arg(SA_RIBBON_BAR_VERSION_PAT);
}

/**
   @brief 获取默认的上下文标签颜色列表
   @return
 */
QList< QColor > SARibbonBar::defaultContextCategoryColorList()
{
    QList< QColor > res;
    res << QColor(201, 89, 156)  // 玫红
        << QColor(242, 203, 29)  // 黄
        << QColor(255, 157, 0)   // 橙
        << QColor(14, 81, 167)   // 蓝
        << QColor(228, 0, 69)    // 红
        << QColor(67, 148, 0)    // 绿
            ;
    return res;
}

/**
 * @brief 提供高分屏的支持静态函数
 *
 * @note 此函数需要在main函数，QApplication生成之前调用
 * @code
 * int main(int argc, char* argv[]){
 *    SARibbonBar::initHighDpi();
 *    QApplication a(argc, argv);
 *    ...
 * }
 * @endcode
 */
void SARibbonBar::initHighDpi()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}

/**
 * @brief 返回applicationButton
 * @return 默认的applicationButton是@ref SARibbonApplicationButton 生成，通过@ref setApplicationButton 可设置为其他button
 */
QAbstractButton* SARibbonBar::applicationButton()
{
    return (d_ptr->mApplicationButton);
}

/**
 * @brief 设置applicationButton,如果想隐藏，可以传入nullptr
 *
 * 默认会有一个SARibbonApplicationButton，如果想取消，可传入nullptr，或者自定义的button也可以传入
 *
 * @note applicationButton的所有权归SARibbonBar所有，不要在外部对applicationButton进行delete操作
 * @param btn applicationButton指针，可以传入@ref SARibbonApplicationButton，
 * SA已经对SARibbonApplicationButton进行了样式设置
 */
void SARibbonBar::setApplicationButton(QAbstractButton* btn)
{
    d_ptr->setApplicationButton(btn);
    if (btn) {
        if (btn->objectName().isEmpty()) {
            btn->setObjectName(QStringLiteral("SARibbonApplicationButton"));
        }
        btn->setVisible(true);
        // btn->setGeometry(applicationButtonGeometry());
    }
    // 无论设置为什么都触发resize
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * @brief 返回tabbar
 * @return SARibbonTabBar指针
 * @sa SARibbonTabBar
 */
SARibbonTabBar* SARibbonBar::ribbonTabBar()
{
    return (d_ptr->mRibbonTabBar);
}

/**
 * @brief 添加一个标签
 * 如果需要删除，直接delete即可，SARibbonBar会对其进行处理
 * @param title 标签名字，默认情况下SARibbonCategory的object name也被设置为title
 * @return 返回一个窗口容器，在Category里可以添加其他控件
 * @sa SARibbonCategory
 */
SARibbonCategory* SARibbonBar::addCategoryPage(const QString& title)
{
    SARibbonCategory* category = RibbonSubElementFactory->createRibbonCategory(this);

    category->setObjectName(title);
    category->setCategoryName(title);
    addCategoryPage(category);
    return (category);
}

/**
 * @brief 添加一个标签
 * @param category
 */
void SARibbonBar::addCategoryPage(SARibbonCategory* category)
{
    if (nullptr == category) {
        return;
    }
    int index = d_ptr->mRibbonTabBar->count();
    insertCategoryPage(category, index);
}

/**
 * @brief qtdesigner专用
 * @param category
 */
void SARibbonBar::addCategoryPage(QWidget* category)
{
    SARibbonCategory* c = qobject_cast< SARibbonCategory* >(category);

    if (c) {
        addCategoryPage(c);
    }
}

/**
 * @brief 添加一个category，category的位置在index，如果当前category数量少于index，将插入到最后
 * @param title category的标题
 * @param index category的位置
 * @return
 */
SARibbonCategory* SARibbonBar::insertCategoryPage(const QString& title, int index)
{
    SARibbonCategory* category = RibbonSubElementFactory->createRibbonCategory(this);

    category->setObjectName(title);
    category->setCategoryName(title);
    insertCategoryPage(category, index);
    return (category);
}

void SARibbonBar::insertCategoryPage(SARibbonCategory* category, int index)
{
    if (nullptr == category) {
        return;
    }
    category->setPannelLayoutMode(d_ptr->mDefaulePannelLayoutMode);
    int i = d_ptr->mRibbonTabBar->insertTab(index, category->categoryName());

    _SARibbonTabData tabdata;

    tabdata.category = category;
    tabdata.index    = i;
    d_ptr->mRibbonTabBar->setTabData(i, QVariant::fromValue(tabdata));
    d_ptr->mStackedContainerWidget->insertWidget(index, category);
    connect(category, &QWidget::windowTitleChanged, this, &SARibbonBar::onCategoryWindowTitleChanged);
    // 更新index信息
    d_ptr->updateTabData();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * @brief 通过名字查找Category
 * @param title Category的名字，既标签的标题
 * @return 如果没有找到，将返回nullptr，如果有重名，将返回第一个查询到的名字，因此，尽量避免重名标签
 * @note 由于翻译等原因，可能title会变化，因此如果想通过固定内容查找category，应该使用 @ref categoryByObjectName
 * @see categoryByObjectName
 */
SARibbonCategory* SARibbonBar::categoryByName(const QString& title) const
{
    int c = d_ptr->mStackedContainerWidget->count();

    for (int i = 0; i < c; ++i) {
        SARibbonCategory* w = qobject_cast< SARibbonCategory* >(d_ptr->mStackedContainerWidget->widget(i));
        if (w) {
            if (w->categoryName() == title) {
                return (w);
            }
        }
    }
    return (nullptr);
}

/**
 * @brief 通过ObjectName查找Category
 * @param objname
 * @return 如果没有找到，将返回nullptr，如果有同样的ObjectName，将返回第一个查询到的名字，因此，尽量避免ObjectName重名
 * @see categoryByName
 */
SARibbonCategory* SARibbonBar::categoryByObjectName(const QString& objname) const
{
    int c = d_ptr->mStackedContainerWidget->count();

    for (int i = 0; i < c; ++i) {
        SARibbonCategory* w = qobject_cast< SARibbonCategory* >(d_ptr->mStackedContainerWidget->widget(i));
        if (w) {
            if (w->objectName() == objname) {
                return (w);
            }
        }
    }
    return (nullptr);
}

/**
 * @brief 通过索引找到category，如果超过索引范围，会返回nullptr
 * @param index 索引
 * @return 如果超过索引范围，会返回nullptr
 * @note 如果此时有上下文标签，上下文的标签也会返回
 * @note 通过索引查找的category必须是visible状态的category，如果通过@ref hideCategory 隐藏的标签，通过索引是找不到的
 * @note 通过@ref categoryByObjectName 可以找到所有加入过的标签，
 * 如果想得到ribbonbar管理的所有标签，可以通过函数@ref categoryPages 得到
 * @see categoryIndex categoryByObjectName categoryByName
 */
SARibbonCategory* SARibbonBar::categoryByIndex(int index) const
{
    QVariant var = d_ptr->mRibbonTabBar->tabData(index);

    if (var.isValid()) {
        _SARibbonTabData p = var.value< _SARibbonTabData >();
        return (p.category);
    }
    return (nullptr);
}

/**
 * @brief 隐藏category,并不会删除或者取走，只是隐藏
 * @param category
 */
void SARibbonBar::hideCategory(SARibbonCategory* category)
{
    int tabcount = d_ptr->mRibbonTabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = d_ptr->mRibbonTabBar->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            if (p.category == category) {
                d_ptr->mHidedCategory.append(p);
                d_ptr->mRibbonTabBar->removeTab(i);  // 仅仅把tab移除
                // 注意Category隐藏后，contex的位置就会发生变化，需要更新
                d_ptr->updateTabData();
                return;
            }
        }
    }
}

/**
 * @brief 显示被隐藏的category
 * @param category
 */
void SARibbonBar::showCategory(SARibbonCategory* category)
{
    for (auto i = d_ptr->mHidedCategory.begin(); i != d_ptr->mHidedCategory.end(); ++i) {
        if (i->category == category) {
            // 说明要显示
            int index = d_ptr->mRibbonTabBar->insertTab(i->index, i->category->categoryName());
            i->index  = index;
            d_ptr->mRibbonTabBar->setTabData(index, QVariant::fromValue(*i));
            d_ptr->mHidedCategory.erase(i);  // 移除
            // 更新index信息
            d_ptr->updateTabData();
            raiseCategory(category);
            return;
        }
    }
    raiseCategory(category);
}

/**
 * @brief 判断这个category是否在显示状态，也就是tabbar有这个category
 * @param category
 * @return
 */
bool SARibbonBar::isCategoryVisible(const SARibbonCategory* c) const
{
    int tabindex = categoryIndex(c);

    return (tabindex >= 0);
}

/**
 * @brief 获取category的索引
 * @param c
 * @return 如果找不到，返回-1
 */
int SARibbonBar::categoryIndex(const SARibbonCategory* c) const
{
    // category的顺序不能以stackedwidget为准，因为存在contextcategory，contextcategory正常是不显示的
    int tabcount = d_ptr->mRibbonTabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = d_ptr->mRibbonTabBar->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            if (p.category == c) {
                return (i);
            }
        }
    }
    return (-1);
}

/**
 * @brief 移动一个Category从from index到to index
 * @param from
 * @param to
 */
void SARibbonBar::moveCategory(int from, int to)
{
    d_ptr->mRibbonTabBar->moveTab(from, to);
    // 这时要刷新所有tabdata的index信息
    d_ptr->updateTabData();
    // 这里会触发tabMoved信号，在tabMoved信号中调整stacked里窗口的位置
}

/**
 * @brief 获取当前显示的所有的SARibbonCategory，包含未显示的SARibbonContextCategory的SARibbonCategory也一并返回
 *
 * @return
 */
QList< SARibbonCategory* > SARibbonBar::categoryPages(bool getAll) const
{
    int c = d_ptr->mStackedContainerWidget->count();
    QList< SARibbonCategory* > res;

    for (int i = 0; i < c; ++i) {
        SARibbonCategory* w = qobject_cast< SARibbonCategory* >(d_ptr->mStackedContainerWidget->widget(i));
        if (w) {
            if (!getAll && w->isContextCategory()) {
                // 不是getall且是上下文时跳过
                continue;
            }
            res.append(w);
        }
    }
    return (res);
}

/**
 * @brief 移除SARibbonCategory
 *
 * SARibbonBar不会delete SARibbonCategory*，但这个SARibbonCategory会脱离SARibbonBar的管理
 * 表现在tabbar会移除，面板会移除，使用此函数后可以对SARibbonCategory进行delete
 * @param category
 */
void SARibbonBar::removeCategory(SARibbonCategory* category)
{
    int index     = tabIndex(category);
    bool isupdate = false;
    if (index >= 0) {
        d_ptr->mRibbonTabBar->removeTab(index);
        isupdate = true;
    }
    d_ptr->mStackedContainerWidget->removeWidget(category);
    // 同时验证这个category是否是contexcategory里的

    for (SARibbonContextCategory* c : qAsConst(d_ptr->mContextCategoryList)) {
        c->takeCategory(category);
    }
    // 这时要刷新所有tabdata的index信息
    if (isupdate) {
        d_ptr->updateTabData();
    }
    // 移除完后需要重绘
    repaint();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * @brief 添加上下文标签
 *
 * 上下文标签是特殊时候触发的标签，需要用户手动触发
 *
 * 调用@ref SARibbonContextCategory::addCategoryPage 可在上下文标签中添加SARibbonCategory，
 * 在上下文标签添加的SARibbonCategory，只有在上下文标签显示的时候才会显示
 * @param title 上下文标签的标题，在Office模式下会显示，在wps模式下不显示。默认情况下SARibbonContextCategory的object name也被设置为title
 * @param color 上下文标签的颜色，如果指定为空QColor(),将会使用SARibbonBar的默认色系
 * @param id 上下文标签的id，以便进行查找
 * @return 返回上下文标签指针
 * @note SARibbonBar拥有SARibbonContextCategory的管理权，用户避免在外部直接delete,如果要删除，调用@ref destroyContextCategory 函数
 */
SARibbonContextCategory* SARibbonBar::addContextCategory(const QString& title, const QColor& color, const QVariant& id)
{
    SARibbonContextCategory* context = RibbonSubElementFactory->createRibbonContextCategory(this);

    context->setObjectName(title);
    context->setContextTitle(title);
    context->setId(id);
    context->setContextColor(color.isValid() ? color : d_ptr->getContextCategoryColor());
    addContextCategory(context);
    return (context);
}

/**
 * @brief 添加上下文标签
 * @param context
 */
void SARibbonBar::addContextCategory(SARibbonContextCategory* context)
{
    if (nullptr == context) {
        return;
    }
    connect(context, &SARibbonContextCategory::categoryPageAdded, this, &SARibbonBar::onContextsCategoryPageAdded);
    connect(context, &SARibbonContextCategory::categoryTitleChanged, this, &SARibbonBar::onContextsCategoryCategoryNameChanged);
    // remove并没有绑定，主要是remove后在stacked里也不会显示，remove且delete的话，stacked里也会删除
    d_ptr->mContextCategoryList.append(context);
}

/**
 * @brief 显示上下文标签
 * @param context 上下文标签指针
 */
void SARibbonBar::showContextCategory(SARibbonContextCategory* context)
{
    if (isContextCategoryVisible(context)) {
        return;
    }
    _SAContextCategoryManagerData contextCategoryData;

    contextCategoryData.contextCategory = context;
    for (int i = 0; i < context->categoryCount(); ++i) {
        SARibbonCategory* category = context->categoryPage(i);
        // 此句如果模式重复设置不会进行多余操作
        category->setPannelLayoutMode(d_ptr->mDefaulePannelLayoutMode);
        // 切换模式后会改变高度，上下文标签显示时要保证显示出来
        int index = d_ptr->mRibbonTabBar->addTab(category->categoryName());
        contextCategoryData.tabPageIndex.append(index);

        _SARibbonTabData tabdata;
        tabdata.category = category;
        tabdata.index    = index;
        d_ptr->mRibbonTabBar->setTabData(index, QVariant::fromValue(tabdata));
    }
    d_ptr->mCurrentShowingContextCategory.append(contextCategoryData);
    // 由于上下文都是在最后追加，不需要调用updateTabData();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * @brief 隐藏上下文标签
 * @param context 上下文标签指针
 */
void SARibbonBar::hideContextCategory(SARibbonContextCategory* context)
{
    bool needResize = false;

    for (int i = 0; i < d_ptr->mCurrentShowingContextCategory.size(); ++i) {
        if (d_ptr->mCurrentShowingContextCategory[ i ].contextCategory == context) {
            const QList< int >& indexs = d_ptr->mCurrentShowingContextCategory[ i ].tabPageIndex;
            for (int j = indexs.size() - 1; j >= 0; --j) {
                d_ptr->mRibbonTabBar->removeTab(indexs[ j ]);
            }
            // 注意，再删除ContextCategory后，tab的序号就会改变，这时，这个tab后面的都要调整它的序号
            needResize = true;
            d_ptr->mCurrentShowingContextCategory.removeAt(i);
            // 移除了ContextCategory后需要break
            break;
        }
    }
    if (needResize) {
        d_ptr->updateTabData();
        QApplication::postEvent(this, new QResizeEvent(size(), size()));
    }
}

/**
 * @brief 判断上下文是否在显示状态
 * @param context
 * @return 在显示状态返回true
 * @sa setContextCategoryVisible
 */
bool SARibbonBar::isContextCategoryVisible(SARibbonContextCategory* context)
{
    return (d_ptr->isContainContextCategoryInList(context));
}

/**
 * @brief 设置上下文标签的显示状态
 *
 * 上下文标签的当前显示状态可通过 @ref isContextCategoryVisible 进行判断
 * @param context 上下文标签
 * @param visible 显示状态，true为显示
 */
void SARibbonBar::setContextCategoryVisible(SARibbonContextCategory* context, bool visible)
{
    if (nullptr == context) {
        return;
    }
    if (visible) {
        showContextCategory(context);
    } else {
        hideContextCategory(context);
    }
}

/**
 * @brief 获取所有的上下文标签
 * @return 返回上下文标签列表
 */
QList< SARibbonContextCategory* > SARibbonBar::contextCategoryList() const
{
    return (d_ptr->mContextCategoryList);
}

/**
 * @brief 销毁上下文标签，上下文标签的SARibbonCategory也会随之销毁
 * @param context 需要销毁的上下文标签指针
 */
void SARibbonBar::destroyContextCategory(SARibbonContextCategory* context)
{
    if (nullptr == context) {
        return;
    }
    //! 1、如果上下文标签显示中，先隐藏
    if (isContextCategoryVisible(context)) {
        hideContextCategory(context);
    }
    //! 2、删除上下文标签的相关内容
    d_ptr->mContextCategoryList.removeAll(context);
    //!
    QList< SARibbonCategory* > res = context->categoryList();

    for (SARibbonCategory* c : qAsConst(res)) {
        c->hide();
        c->deleteLater();
    }
    context->deleteLater();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * @brief 设置为最小/正常模式
 *
 * 隐藏模式下，只会显示tabbar，不会显示内容，默认状态是显示模式
 *
 * 默认下双击tabbar会切换隐藏显示模式，如果想禁用此功能，可以重载 @ref onCurrentRibbonTabDoubleClicked
 * 函数，不对函数进行任何处理即可
 *
 * @param isMinimum 参数为true时，切换为Minimum模式
 * @see 此函数会改变@ref RibbonState 状态，通过@ref currentRibbonState 函数可以查看当前状态
 */
void SARibbonBar::setMinimumMode(bool isMinimum)
{
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "SARibbonBar::setHideMode " << isMinimum;
#endif
    if (isMinimum) {
        d_ptr->setMinimumMode();
    } else {
        d_ptr->setNormalMode();
    }
    QResizeEvent resizeEvent(size(), size());
    QApplication::sendEvent(this, &resizeEvent);
    // 发射信号
    emit ribbonModeChanged(isMinimum ? MinimumRibbonMode : NormalRibbonMode);
}

///
/// \brief 当前ribbon是否是隐藏模式
/// \return
///
bool SARibbonBar::isMinimumMode() const
{
    return (d_ptr->mStackedContainerWidget->isPopupMode());
}

///
/// \brief 设置显示隐藏ribbon按钮
///
void SARibbonBar::showMinimumModeButton(bool isShow)
{
    if (isShow) {
        activeRightButtonGroup();

        d_ptr->mMinimumCategoryButtonAction = new QAction(this);
        d_ptr->mMinimumCategoryButtonAction->setIcon(
                style()->standardIcon(isMinimumMode() ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton, nullptr));
        connect(d_ptr->mMinimumCategoryButtonAction, &QAction::triggered, this, [ this ]() {
            this->setMinimumMode(!isMinimumMode());
            this->d_ptr->mMinimumCategoryButtonAction->setIcon(
                    style()->standardIcon(isMinimumMode() ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton, nullptr));
        });
        d_ptr->mRightButtonGroup->addAction(d_ptr->mMinimumCategoryButtonAction);

    } else {
        if (nullptr != d_ptr->mMinimumCategoryButtonAction) {
            d_ptr->mMinimumCategoryButtonAction->deleteLater();
            d_ptr->mMinimumCategoryButtonAction = nullptr;
        }
    }
    QResizeEvent resizeEvent(size(), size());

    QApplication::sendEvent(this, &resizeEvent);
}

/**
 * @brief 是否显示隐藏ribbon按钮
 * @return
 */
bool SARibbonBar::haveShowMinimumModeButton() const
{
    return (nullptr != d_ptr->mMinimumCategoryButtonAction);
}

/**
   @brief 隐藏ribbon对应的action
   @return
 */
QAction* SARibbonBar::minimumModeAction() const
{
    return d_ptr->mMinimumCategoryButtonAction;
}

/**
 * @brief 当前ribbon的状态（正常|最小化）
 * @return
 */
SARibbonBar::RibbonMode SARibbonBar::currentRibbonState() const
{
    return (d_ptr->mCurrentRibbonMode);
}

/**
   @brief tabBar的高度
   @return
 */
int SARibbonBar::tabBarHeight() const
{
    return d_ptr->mRibbonTabBar->height();
}

/**
 * @brief 设置tabbar的高度
 *
 * 用户调用setTabBarHeight后，将使用用户设定的高度，而不使用自动计算的高度，这时tabbar高度不会跟随字体等信息重新计算
 *
 * @note 注意，在RibbonStyleCompact**模式下，tabbar高度要保证小于等于titlebar高度，否则会显示异常
 * @note 此函数不会自动刷新，如果需要刷新调用此函数后需要调用@ref updateRibbonGeometry
 * @param h
 */
void SARibbonBar::setTabBarHeight(int h, bool resizeByNow)
{
    if (nullptr == d_ptr->mUserDefTabBarHeight) {
        d_ptr->mUserDefTabBarHeight = std::make_unique< int >(h);
    } else {
        *(d_ptr->mUserDefTabBarHeight) = h;
    }
    if (resizeByNow) {
        updateRibbonGeometry();
    }
}

/**
   @brief 返回标题栏高度
   @sa setTitleBarHeight
   @return
 */
int SARibbonBar::titleBarHeight() const
{
    return d_ptr->titleBarHeight();
}

/**
   @brief 设置标题栏的高度
   @sa titleBarHeight
   @note 此操作会发射@ref titleBarHeightChanged 信号
   @param h
 */
void SARibbonBar::setTitleBarHeight(int h, bool resizeByNow)
{
    int oldHeight = d_ptr->mTitleBarHeight;
    if (nullptr == d_ptr->mUserDefTitleBarHeight) {
        d_ptr->mUserDefTitleBarHeight = std::make_unique< int >(h);
    } else {
        *(d_ptr->mUserDefTitleBarHeight) = h;
        //
        oldHeight = *(d_ptr->mUserDefTitleBarHeight);
    }

    if (resizeByNow) {
        updateRibbonGeometry();
    }
    emit titleBarHeightChanged(oldHeight, h);
}

/**
 * @brief category的高度
 * @return
 */
int SARibbonBar::categoryHeight() const
{
    return d_ptr->mStackedContainerWidget->height();
}

/**
 * @brief 设置category的高度
 * @param h
 * @param resizeByNow
 */
void SARibbonBar::setCategoryHeight(int h, bool resizeByNow)
{
    if (nullptr == d_ptr->mUserDefCategoryHeight) {
        d_ptr->mUserDefCategoryHeight = std::make_unique< int >(h);
    } else {
        *(d_ptr->mUserDefCategoryHeight) = h;
    }
    if (resizeByNow) {
        updateRibbonGeometry();
    }
}

void SARibbonBar::onWindowTitleChanged(const QString& title)
{
    Q_UNUSED(title);
    update();
}

void SARibbonBar::onWindowIconChanged(const QIcon& i)
{
    if (quickAccessBar()) {
        quickAccessBar()->setIcon(i);
    }
}

/**
 * @brief category的名字发生改变触发
 * @param title
 */
void SARibbonBar::onCategoryWindowTitleChanged(const QString& title)
{
    // 全部更新一遍
    Q_UNUSED(title);
    updateCategoryTitleToTabName();
}

///
/// \brief ribbon的显示界面隐藏
///
void SARibbonBar::onStackWidgetHided()
{
}

/**
 * @brief 标签切换触发槽函数
 * @param index
 */
void SARibbonBar::onCurrentRibbonTabChanged(int index)
{
    QVariant var               = d_ptr->mRibbonTabBar->tabData(index);
    SARibbonCategory* category = nullptr;

    if (var.isValid()) {
        _SARibbonTabData p = var.value< _SARibbonTabData >();
        category           = p.category;
    }
    if (category) {
        if (d_ptr->mStackedContainerWidget->currentWidget() != category) {
            d_ptr->mStackedContainerWidget->setCurrentWidget(category);
        }
        if (isMinimumMode()) {
            d_ptr->mRibbonTabBar->clearFocus();
            if (!d_ptr->mStackedContainerWidget->isVisible()) {
                if (d_ptr->mStackedContainerWidget->isPopupMode()) {
                    // 在stackedContainerWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                    QHoverEvent ehl(QEvent::HoverLeave,
                                    d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()),
                                    d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()));
                    QApplication::sendEvent(d_ptr->mRibbonTabBar, &ehl);
                    resizeStackedContainerWidget();
                    d_ptr->mStackedContainerWidget->setFocus();
                    d_ptr->mStackedContainerWidget->exec();
                    // 在最小模式下，每次显示完stackedContainerWidget后把tab的
                    // 的index设置为-1，这样每次点击都会触发onCurrentRibbonTabChanged
                }
            } else {
            }
        }
    }
    emit currentRibbonTabChanged(index);
}

/**
 * @brief ribbon tab bar单击
 *
 * 此实现必须在eventfilter中传递stackedwidget的QEvent::MouseButtonDblClick事件到tabbar中，否则会导致双击变两次单击
 *
 * 单击事件仅用于响应点击同一个tab时
 * @param index
 */
void SARibbonBar::onCurrentRibbonTabClicked(int index)
{
    if (index != d_ptr->mRibbonTabBar->currentIndex()) {
        // 点击的标签不一致通过changed槽去处理
        return;
    }
    if (this->isMinimumMode()) {
        if (!this->d_ptr->mStackedContainerWidget->isVisible()) {
            if (this->d_ptr->mStackedContainerWidget->isPopupMode()) {
                // 在stackedContainerWidget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
                QHoverEvent ehl(QEvent::HoverLeave,
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()),
                                d_ptr->mRibbonTabBar->mapToGlobal(QCursor::pos()));
                QApplication::sendEvent(d_ptr->mRibbonTabBar, &ehl);
                // 弹出前都调整一下位置，避免移动后位置异常
                resizeStackedContainerWidget();
                this->d_ptr->mStackedContainerWidget->setFocus();
                this->d_ptr->mStackedContainerWidget->exec();
            }
        }
    }
}

/**
 * @brief ribbon tab bar双击
 *
 * 默认情况下双击会切换最小和正常模式
 * @param index
 */
void SARibbonBar::onCurrentRibbonTabDoubleClicked(int index)
{
    Q_UNUSED(index);
    setMinimumMode(!isMinimumMode());
}

void SARibbonBar::onContextsCategoryPageAdded(SARibbonCategory* category)
{
    Q_ASSERT_X(category != nullptr, "onContextsCategoryPageAdded", "add nullptr page");
    d_ptr->mStackedContainerWidget->addWidget(category);  // 这里stackedWidget用append，其他地方都应该使用insert
}

/**
 * @brief 上下文标签管理的标签的名字发生变换
 * @param category
 * @param title
 */
void SARibbonBar::onContextsCategoryCategoryNameChanged(SARibbonCategory* category, const QString& title)
{
    Q_UNUSED(category);
    Q_UNUSED(title);
    updateCategoryTitleToTabName();
}

/**
 * @brief 标签移动的信号
 * @param from
 * @param to
 */
void SARibbonBar::onTabMoved(int from, int to)
{
    const QSignalBlocker blocker(d_ptr->mStackedContainerWidget);
    // 调整stacked widget的顺序，调整顺序是为了调用categoryPages函数返回的QList<SARibbonCategory *>顺序和tabbar一致
    d_ptr->mStackedContainerWidget->moveWidget(from, to);
}

/**
 * @brief 根据SARibbonCategory*指针查找tabbar的index
 *
 * @param c SARibbonCategory对应的QObject指针
 * @return 如果没有找到，返回-1
 * @note 此函数不会调用SARibbonCategory*的任何方法，因此可以在SARibbonCategory的destroyed槽中调用
 */
int SARibbonBar::tabIndex(SARibbonCategory* obj)
{
    const int size = d_ptr->mRibbonTabBar->count();

    for (int i = 0; i < size; ++i) {
        QVariant v = d_ptr->mRibbonTabBar->tabData(i);
        if (v.isValid()) {
            _SARibbonTabData innervalue = v.value< _SARibbonTabData >();
            if (innervalue.category == obj) {
                return (i);
            }
        }
    }
    // 如果找不到就从stackedwidget中找

    return (-1);
}

void SARibbonBar::resizeAll()
{
    if (isLooseStyle()) {
        resizeInLooseStyle();
    } else {
        resizeInCompactStyle();
    }
    update();
}

/**
 * @brief 把ribbonbar的内容，同步进各个category中
 * @param autoUpdate
 */
void SARibbonBar::synchronousCategoryData(bool autoUpdate)
{
    iterate([ this ](SARibbonCategory* c) -> bool {
        c->setEnableShowPannelTitle(this->isEnableShowPannelTitle());
        c->setPannelTitleHeight(this->pannelTitleHeight());
        c->setCategoryAlignment(this->ribbonAlignment());
        c->setPannelLayoutMode(this->pannelLayoutMode());
        return true;
    });

    //! 直接给一个resizeevent，让所有刷新
    if (autoUpdate) {
        QResizeEvent* e = new QResizeEvent(size(), QSize());
        QApplication::postEvent(this, e);
    }
}

/**
   @brief 激活tabbar右边的按钮群
   @return 返回右边的按钮群指针
 */
SARibbonButtonGroupWidget* SARibbonBar::activeRightButtonGroup()
{
    if (nullptr == d_ptr->mRightButtonGroup) {
        d_ptr->mRightButtonGroup = RibbonSubElementFactory->craeteButtonGroupWidget(this);
    }
    d_ptr->mRightButtonGroup->show();
    return d_ptr->mRightButtonGroup;
}

/**
   @brief 返回右边的按钮群指针
   @return 如果没有创建，返回nullptr
 */
SARibbonButtonGroupWidget* SARibbonBar::rightButtonGroup()
{
    return d_ptr->mRightButtonGroup;
}

/**
   @brief 激活QuickAccessBar
   @return
 */
SARibbonQuickAccessBar* SARibbonBar::activeQuickAccessBar()
{
    if (nullptr == d_ptr->mQuickAccessBar) {
        d_ptr->mQuickAccessBar = RibbonSubElementFactory->createQuickAccessBar(this);
        d_ptr->mQuickAccessBar->setObjectName(QStringLiteral("objSARibbonQuickAccessBar"));
        d_ptr->mQuickAccessBar->setIcon(windowIcon());
    }
    d_ptr->mQuickAccessBar->show();
    return d_ptr->mQuickAccessBar;
}

SARibbonQuickAccessBar* SARibbonBar::quickAccessBar()
{
    return (d_ptr->mQuickAccessBar);
}

/**
 * @brief 设置ribbonbar的风格，此函数会重新设置所有元素，包括button的布局方式，
 * 尤其是从三行变到两行的过程，重设的内容较多
 * @note 此函数会自动触发ResizeEvent，不需要手动调用
 * @note 默认情况下2行模式不换行,如果想在两行模式换行，在调用SARibbonBar::setRibbonStyle后，再SARibbonToolButton::setEnableWordWrap(true)
 * ,同时再调用updateRibbonElementGeometry()刷新布局
 * @param v 样式，见@ref SARibbonBar::RibbonStyle
 */
void SARibbonBar::setRibbonStyle(SARibbonBar::RibbonStyles v)
{
    // 先幅值给变量
    d_ptr->mRibbonStyle = v;
#if SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "setRibbonStyle(" << v << ")"                //
             << "\n  isThreeRowStyle=" << isThreeRowStyle()  //
             << "\n  isTwoRowStyle=" << isTwoRowStyle()      //
             << "\n  isLooseStyle=" << isLooseStyle()        //
             << "\n  isCompactStyle=" << isCompactStyle()    //
            ;
#endif
    // 执行判断
    setEnableWordWrap(isThreeRowStyle(v));
    setTabOnTitle(isCompactStyle());
    d_ptr->mQuickAccessBar->setEnableShowIcon(isLooseStyle());
    setEnableShowPannelTitle(isThreeRowStyle(v));
    setPannelLayoutMode(isThreeRowStyle(v) ? SARibbonPannel::ThreeRowMode : SARibbonPannel::TwoRowMode);

    // 此函数会调用setFixedHeight
    synchronousCategoryData(false);  // 这里不急着刷新，下面会继续刷新
    d_ptr->resetSize();

    emit ribbonStyleChanged(d_ptr->mRibbonStyle);
}

/**
   @brief 返回当前ribbon的风格
   @return 返回当前ribbon的风格
 */
SARibbonBar::RibbonStyles SARibbonBar::currentRibbonStyle() const
{
    return (d_ptr->mRibbonStyle);
}

/**
   @brief 切换到对应标签
   @param index 标签索引
 */
void SARibbonBar::setCurrentIndex(int index)
{
    d_ptr->mRibbonTabBar->setCurrentIndex(index);
    // onCurrentRibbonTabChanged(index);
}

/**
 * @brief 返回当前的tab索引
 * @return 当前的索引
 */
int SARibbonBar::currentIndex()
{
    return (d_ptr->mRibbonTabBar->currentIndex());
}

/**
 * @brief 确保标签显示出来，tab并切换到对应页
 * @param category 标签指针
 */
void SARibbonBar::raiseCategory(SARibbonCategory* category)
{
    int index = d_ptr->mStackedContainerWidget->indexOf(category);

    if (index >= 0) {
        setCurrentIndex(index);
    }
}

/**
 * @brief 判断当前的样式是否为2行
 * @return
 */
bool SARibbonBar::isTwoRowStyle() const
{
    return (d_ptr->mDefaulePannelLayoutMode == SARibbonPannel::TwoRowMode);
}

/**
 * @brief 判断当前的样式是否为3行
 * @return
 */
bool SARibbonBar::isThreeRowStyle() const
{
    return (d_ptr->mDefaulePannelLayoutMode == SARibbonPannel::ThreeRowMode);
}

/**
 * @brief 判断当前的样式是否为宽松样式
 * @return
 */
bool SARibbonBar::isLooseStyle() const
{
    return (SARibbonBar::isLooseStyle(currentRibbonStyle()));
}

/**
 * @brief 判断当前的样式是否为紧凑样式
 * @return
 */
bool SARibbonBar::isCompactStyle() const
{
    return (SARibbonBar::isCompactStyle(currentRibbonStyle()));
}

/**
 * @brief 设置标题的文字颜色
 *
 * 标题时mainwindow的windowTitle，如果要设置标题，直接调用SARibbonMainWindow::setWindowTitle 进行设置
 *
 * 如果不设置标题颜色，默认是SARibbonBar的qss的color属性
 * @param clr
 * @note 此函数不会刷新，刷新请自行调用repaint
 */
void SARibbonBar::setWindowTitleTextColor(const QColor& clr)
{
    d_ptr->mTitleTextColor = clr;
}

/**
 * @brief 获取标题的文字颜色
 * @return 如果返回的是无效颜色，!QColor::isValid()，说明没有手动设置颜色，颜色将跟随SARibbonBar的qss的文字颜色
 */
QColor SARibbonBar::windowTitleTextColor() const
{
    return d_ptr->mTitleTextColor;
}

/**
 * @brief tabbar 底部会绘制一条线条，此接口定义线条颜色
 * @param clr
 */
void SARibbonBar::setTabBarBaseLineColor(const QColor& clr)
{
    d_ptr->mTabBarBaseLineColor = clr;
}

/**
 * @brief 获取tabbar底部基线颜色
 * @return
 */
QColor SARibbonBar::tabBarBaseLineColor() const
{
    return d_ptr->mTabBarBaseLineColor;
}

/**
 * @brief 更新ribbon的布局数据，此函数适用于一些关键性尺寸变化，换起ribbon下面元素的布局
 *
 * @note 此函数调用较慢，避免在高速要求场合调用
 */
void SARibbonBar::updateRibbonGeometry()
{
    d_ptr->resetSize();
    iterate([](SARibbonCategory* c) -> bool {
        c->updateItemGeometry();
        return true;
    });
    //! 直接给一个resizeevent，让所有刷新
    // QResizeEvent* e = new QResizeEvent(size(), QSize());
    // QApplication::postEvent(this, e);
}

/**
 * @brief SARibbonPannel的布局模式
 * @return
 */
SARibbonPannel::PannelLayoutMode SARibbonBar::pannelLayoutMode() const
{
    return d_ptr->mDefaulePannelLayoutMode;
}

/**
 * @brief SARibbonPannel的布局模式设置
 * @param m
 */
void SARibbonBar::setPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
    d_ptr->mDefaulePannelLayoutMode = m;
    iterate([ m ](SARibbonCategory* c) -> bool {
        c->setPannelLayoutMode(m);
        return true;
    });
}

/**
 * @brief 设置tab在title上面，这样可以省略title区域
 * @param on
 */
void SARibbonBar::setTabOnTitle(bool on)
{
    if (d_ptr->mIsTabOnTitle != on) {
        d_ptr->mIsTabOnTitle = on;
        d_ptr->resetSize();
    }
}

/**
 * @brief tab是否在title上面
 * @return
 */
bool SARibbonBar::isTabOnTitle() const
{
    return d_ptr->mIsTabOnTitle;
}

/**
 * @brief 设置标题的对齐方式
 * @param al
 */
void SARibbonBar::setWindowTitleAligment(Qt::Alignment al)
{
    d_ptr->mTitleAligment = al;
}

/**
 * @brief 获取标题的对齐方式
 * @return
 */
Qt::Alignment SARibbonBar::windowTitleAligment() const
{
    return d_ptr->mTitleAligment;
}

/**
 * @brief 设置ribbonbar的按钮文字允许换行
 * @param on
 */
void SARibbonBar::setEnableWordWrap(bool on)
{
    SARibbonToolButton::setEnableWordWrap(on);
    updateRibbonGeometry();
}

/**
 * @brief 判断是否允许换行
 * @return
 */
bool SARibbonBar::isEnableWordWrap() const
{
    return SARibbonToolButton::isEnableWordWrap();
}

/**
 * @brief pannel标题栏的高度
 * @return
 */
int SARibbonBar::pannelTitleHeight() const
{
    return d_ptr->mPannelTitleHeight;
}

/**
 * @brief 设置pannel的高度
 * @param h
 */
void SARibbonBar::setPannelTitleHeight(int h)
{
    d_ptr->mPannelTitleHeight = h;
    iterate([ h ](SARibbonCategory* c) -> bool {
        c->setPannelTitleHeight(h);
        return true;
    });
}

/**
 * @brief 是否pannel显示标题栏
 * @return
 */
bool SARibbonBar::isEnableShowPannelTitle() const
{
    return d_ptr->mEnableShowPannelTitle;
}

/**
 * @brief 设置显示pannel标题
 * @param on
 */
void SARibbonBar::setEnableShowPannelTitle(bool on)
{
    d_ptr->mEnableShowPannelTitle = on;
    iterate([ on ](SARibbonCategory* c) -> bool {
        c->setEnableShowPannelTitle(on);
        return true;
    });
}

/**
 * @brief ribbonbar内部的StackedWidget
 * 所有的category都放置在StackedWidget中
 * @return
 */
SARibbonStackedWidget* SARibbonBar::ribbonStackedWidget()
{
    return d_ptr->mStackedContainerWidget;
}

/**
 * @brief 设置是否显示标题
 * @param on
 */
void SARibbonBar::setTitleVisible(bool on)
{
    d_ptr->mIsTitleVisible = on;
}

/**
 * @brief 判断标题是否显示
 * @return
 */
bool SARibbonBar::isTitleVisible() const
{
    return d_ptr->mIsTitleVisible;
}

/**
   @brief 设置上下文标签的颜色列表

        上下文标签显示的时候，会从颜色列表中取颜色进行标签的渲染
   @param cls
 */
void SARibbonBar::setContextCategoryColorList(const QList< QColor >& cls)
{
    d_ptr->mContextCategoryColorList = cls;
    if (d_ptr->mContextCategoryColorList.isEmpty()) {
        d_ptr->mContextCategoryColorList = defaultContextCategoryColorList();
    }
    d_ptr->mContextCategoryColorListIndex = 0;
    // 这时需要对已经显示的contextCategoryData的颜色进行重新设置
    for (SARibbonContextCategory* c : d_ptr->mContextCategoryList) {
        c->setContextColor(d_ptr->getContextCategoryColor());
    }
}

/**
   @brief 获取上下文标签的颜色列表
   @return
 */
QList< QColor > SARibbonBar::contextCategoryColorList() const
{
    return d_ptr->mContextCategoryColorList;
}

/**
   @brief 设置ribbon的对齐方式
   @param al
 */
void SARibbonBar::setRibbonAlignment(SARibbonAlignment al)
{
    d_ptr->mRibbonAlignment = al;
    synchronousCategoryData();
}

/**
   @brief ribbon的对齐方式
   @return
 */
SARibbonAlignment SARibbonBar::ribbonAlignment() const
{
    return d_ptr->mRibbonAlignment;
}

/**
 * @brief 此函数会遍历SARibbonBar下的所有Category，执行函数指针bool(SARibbonCategory*)
 * @param fp 函数指针返回false则停止迭代
 * @return 返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
 */
bool SARibbonBar::iterate(FpCategoryIterate fp)
{
    const QList< SARibbonCategory* > cs = categoryPages(true);
    for (SARibbonCategory* c : cs) {
        if (!fp(c)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 此函数会遍历SARibbonBar下的所有Category,并迭代所有的pannel，执行函数指针bool(SARibbonPannel*)
 * @param fp 函数指针返回false则停止迭代
 * @return 返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
 */
bool SARibbonBar::iterate(FpPannelIterate fp)
{
    const QList< SARibbonCategory* > cs = categoryPages(true);
    for (SARibbonCategory* c : cs) {
        if (!c->iterate(fp)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 设置边角widget可见性，对于mdi窗口，会出现TopLeftCorner和TopRightCorner两个corner widget
 * @param on
 * @param c
 */
void SARibbonBar::setCornerWidgetVisible(bool on, Qt::Corner c)
{
    if (QWidget* w = cornerWidget(c)) {
        w->setVisible(on);
    }
}

/**
 * @brief SARibbonBar::eventFilter
 * @param obj
 * @param e
 * @return
 */
bool SARibbonBar::eventFilter(QObject* obj, QEvent* e)
{
    if (obj) {
        // 调整多文档时在窗口模式下的按钮更新
        if ((obj == cornerWidget(Qt::TopLeftCorner)) || (obj == cornerWidget(Qt::TopRightCorner))) {
            if ((QEvent::UpdateLater == e->type()) || (QEvent::MouseButtonRelease == e->type())
                || (QEvent::WindowActivate == e->type())) {
                QApplication::postEvent(this, new QResizeEvent(size(), size()));
            }
        } else if (obj == d_ptr->mStackedContainerWidget) {
            // 在stack 是popup模式时，点击的是stackedContainerWidget区域外的时候，如果是在ribbonTabBar上点击
            // 那么忽略掉这次点击，把点击下发到ribbonTabBar,这样可以避免stackedContainerWidget在点击ribbonTabBar后
            // 隐藏又显示，出现闪烁
            if ((QEvent::MouseButtonPress == e->type()) || (QEvent::MouseButtonDblClick == e->type())) {
                if (d_ptr->mStackedContainerWidget->isPopupMode()) {
                    QMouseEvent* mouseEvent = static_cast< QMouseEvent* >(e);
                    if (!d_ptr->mStackedContainerWidget->rect().contains(mouseEvent->pos())) {
                        QWidget* clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());
                        if (clickedWidget == d_ptr->mRibbonTabBar) {
                            const QPoint targetPoint = clickedWidget->mapFromGlobal(mouseEvent->globalPos());
                            QMouseEvent* evPress     = new QMouseEvent(mouseEvent->type(),
                                                                   targetPoint,
                                                                   mouseEvent->globalPos(),
                                                                   mouseEvent->button(),
                                                                   mouseEvent->buttons(),
                                                                   mouseEvent->modifiers());
                            QApplication::postEvent(clickedWidget, evPress);
                            return (true);
                        }
                    }
                }
            }
        }
    }
    return (QMenuBar::eventFilter(obj, e));
}

/**
 * @brief 根据情况重置tabbar的宽度，主要针对wps模式
 */
int SARibbonBar::calcMinTabBarWidth() const
{
    // 20200831
    // tabBarWidth的宽度原来为endX - x;，现需要根据实际进行调整
    // 为了把tabbar没有tab的部分不占用，这里的宽度需要根据tab的size来进行设置，让tabbar的长度刚刚好，这样能让出
    // mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
    // 计算tab所占用的宽度
    const QMargins& mg = d_ptr->mRibbonTabBar->tabMargin();
    return d_ptr->mRibbonTabBar->sizeHint().width() + (mg.left() + mg.right());
}

/**
 * @brief 正常模式下的高度
 *
 * 有可能SARibbonBar::height和mainBarHeight不相等，这种情况发生在RibbonState::MinimumRibbonMode状态下
 * @return 高度
 */
int SARibbonBar::normalModeMainBarHeight() const
{
    return d_ptr->getCurrentNormalModeMainBarHeight();
}

/**
   @brief 最小模式下的高度
   @return
 */
int SARibbonBar::minimumModeMainBarHeight() const
{
    return d_ptr->getCurrentMinimumModeMainBarHeight();
}

/**
 * @brief 更新所有的category title对应的tab名
 *
 * 此函数会对所有的category的名字和tab进行匹配，如果匹配不上会重新设置tab名
 */
void SARibbonBar::updateCategoryTitleToTabName()
{
    SARibbonTabBar* tab = d_ptr->mRibbonTabBar;
    for (int i = 0; i < tab->count(); ++i) {
        // 鉴于tab不会很多，不考虑效率问题
        QVariant var = tab->tabData(i);
        if (var.isValid()) {
            _SARibbonTabData p = var.value< _SARibbonTabData >();
            if (p.category && p.category->categoryName() != tab->tabText(i)) {
                tab->setTabText(i, p.category->categoryName());
            }
        }
    }
    repaint();
}

/**
 * @brief 告知WindowButtonGroup的尺寸
 * @param s
 */
void SARibbonBar::setWindowButtonGroupSize(const QSize& s)
{
    d_ptr->mWindowButtonSize = s;
}

void SARibbonBar::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    if (isLooseStyle()) {
        paintInLooseStyle();
    } else {
        paintInCompactStyle();
    }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    QPainter p(this);
    HELP_DRAW_RECT(p, m_d->quickAccessBar->geometry());
    HELP_DRAW_RECT(p, m_d->ribbonTabBar->geometry());
    HELP_DRAW_RECT(p, m_d->stackedContainerWidget->geometry());
#endif
}

void SARibbonBar::paintInLooseStyle()
{
    QPainter p(this);

    //!绘制tabbar下的基线，这个函数仅仅对office2013主题有用，大部分主题都不绘制基线
    paintTabbarBaseLine(p);

    //! 显示上下文标签
    p.save();
    QList< _SAContextCategoryManagerData > contextCategoryDataList = d_ptr->mCurrentShowingContextCategory;
    // bool isCurrentSelectContextCategoryPage                        = false;

    QPoint contextCategoryRegion(width(), -1);
    QMargins border = contentsMargins();

    for (int i = 0; i < contextCategoryDataList.size(); ++i) {
        QRect contextTitleRect;
        QList< int > indexs = contextCategoryDataList[ i ].tabPageIndex;
        QColor clr          = contextCategoryDataList[ i ].contextCategory->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = d_ptr->mRibbonTabBar->tabRect(indexs.first());
            QRect endRect    = d_ptr->mRibbonTabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            contextTitleRect.translate(d_ptr->mRibbonTabBar->x(), d_ptr->mRibbonTabBar->y());
            contextTitleRect.setHeight(d_ptr->mRibbonTabBar->height() - 1);  // 减1像素，避免tabbar基线覆盖
            contextTitleRect -= d_ptr->mRibbonTabBar->tabMargin();
            // 把区域顶部扩展到窗口顶部
            contextTitleRect.setTop(border.top());
            // 绘制
            paintContextCategoryTab(p, contextCategoryDataList[ i ].contextCategory->contextTitle(), contextTitleRect, clr);
            // 更新上下文标签的范围，用于控制标题栏的显示
            if (contextTitleRect.left() < contextCategoryRegion.x()) {
                contextCategoryRegion.setX(contextTitleRect.left());
            }
            if (contextTitleRect.right() > contextCategoryRegion.y()) {
                contextCategoryRegion.setY(contextTitleRect.right());
            }
        }
        // isCurrentSelectContextCategoryPage = indexs.contains(d_ptr->mRibbonTabBar->currentIndex());
        // if (isCurrentSelectContextCategoryPage) {
        //     QPen pen;
        //     pen.setColor(clr);
        //     pen.setWidth(1);
        //     p.setPen(pen);
        //     p.setBrush(Qt::NoBrush);
        //     p.drawRect(d_ptr->mStackedContainerWidget->geometry());
        //     isCurrentSelectContextCategoryPage = false;
        // }
    }
    p.restore();
    //! 显示标题等
    QWidget* parWindow = parentWidget();

    if (parWindow) {
        QRect titleRegion;
        if (contextCategoryRegion.y() < 0) {
            titleRegion.setRect(d_ptr->mQuickAccessBar->geometry().right() + 1,
                                border.top(),
                                width() - d_ptr->mIconRightBorderPosition - border.right()
                                        - d_ptr->mWindowButtonSize.width() - d_ptr->mQuickAccessBar->geometry().right() - 1,
                                titleBarHeight());
        } else {
            int leftwidth = contextCategoryRegion.x() - d_ptr->mQuickAccessBar->geometry().right() - d_ptr->mIconRightBorderPosition;
            int rightwidth = width() - contextCategoryRegion.y() - d_ptr->mWindowButtonSize.width();
            //            if (width() - contextCategoryRegion.y() > contextCategoryRegion.x()-x) {
            if (rightwidth > leftwidth) {
                // 说明右边的区域大一点，标题显示在右，显示在右边需要减去windowbutton宽度
                titleRegion.setRect(contextCategoryRegion.y(), border.top(), rightwidth, titleBarHeight());
            } else {
                // 说明左边的大一点
                titleRegion.setRect(d_ptr->mIconRightBorderPosition + d_ptr->mQuickAccessBar->geometry().right(),
                                    border.top(),
                                    leftwidth,
                                    titleBarHeight());
            }
        }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
        p.save();
        p.setBrush(QColor(255, 0, 0, 120));
        p.drawRect(titleRegion);
        p.restore();
#endif
        paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
    }
}

void SARibbonBar::paintInCompactStyle()
{
    QPainter p(this);
    //!
    paintTabbarBaseLine(p);
    //! 显示上下文标签
    p.save();
    QList< _SAContextCategoryManagerData > contextCategoryDataList = d_ptr->mCurrentShowingContextCategory;
    QMargins border                                                = contentsMargins();
    for (int i = 0; i < contextCategoryDataList.size(); ++i) {
        QRect contextTitleRect;
        QList< int > indexs = contextCategoryDataList[ i ].tabPageIndex;
        QColor clr          = contextCategoryDataList[ i ].contextCategory->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = d_ptr->mRibbonTabBar->tabRect(indexs.first());
            QRect endRect    = d_ptr->mRibbonTabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            contextTitleRect.translate(d_ptr->mRibbonTabBar->x(), d_ptr->mRibbonTabBar->y());
            contextTitleRect.setHeight(d_ptr->mRibbonTabBar->height() - 1);
            contextTitleRect -= d_ptr->mRibbonTabBar->tabMargin();
            // 把区域顶部扩展到窗口顶部
            contextTitleRect.setTop(border.top());
            // 绘制
            paintContextCategoryTab(p, QString(), contextTitleRect, clr);
        }
    }
    p.restore();
    //! 显示标题等

    QWidget* parWindow = parentWidget();

    if (parWindow) {
        int start = d_ptr->mRibbonTabBar->x() + d_ptr->mRibbonTabBar->width();
        int width = d_ptr->mQuickAccessBar->x() - start;
        if (width > 20) {
            QRect titleRegion(start, border.top(), width, titleBarHeight());
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            p.save();
            p.setBrush(QColor(255, 0, 0, 120));
            p.drawRect(titleRegion);
            p.restore();
#endif
            paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
        }
    }
}

void SARibbonBar::resizeStackedContainerWidget()
{
    QMargins border                   = contentsMargins();
    const QRect& ribbonTabBarGeometry = d_ptr->mRibbonTabBar->geometry();

    int x = border.left();
    int y = ribbonTabBarGeometry.bottom() + 1;
    int w = width() - border.left() - border.right();
    int h = d_ptr->mCategoryHeight;
    if (d_ptr->mStackedContainerWidget->isPopupMode()) {
        // 弹出模式时，位置为全局位置
        QPoint absPosition = mapToGlobal(QPoint(x, y));
        x                  = absPosition.x();
        y                  = absPosition.y();
    }
    d_ptr->mStackedContainerWidget->setFixedSize(QSize(w, h));
    d_ptr->mStackedContainerWidget->setGeometry(x, y, w, h);
}

/**
 * @brief 刷新所有ContextCategoryManagerData，这个在单独一个Category删除时调用
 */
void SARibbonBar::updateContextCategoryManagerData()
{
    const int c = d_ptr->mRibbonTabBar->count();

    for (_SAContextCategoryManagerData& cd : d_ptr->mCurrentShowingContextCategory) {
        cd.tabPageIndex.clear();
        for (int i = 0; i < cd.contextCategory->categoryCount(); ++i) {
            SARibbonCategory* category = cd.contextCategory->categoryPage(i);
            for (int t = 0; t < c; ++t) {
                QVariant v = d_ptr->mRibbonTabBar->tabData(t);
                if (v.isValid()) {
                    _SARibbonTabData d = v.value< _SARibbonTabData >();
                    if (d.category == category) {
                        cd.tabPageIndex.append(t);
                    }
                } else {
                    cd.tabPageIndex.append(-1);
                }
            }
        }
    }
}

/**
 * @brief 绘制上下文标签的背景
 * @param painter 绘图QPainter
 * @param title 上下文标签的title
 * @param contextRect 上下文标签的绘制区域
 * @param color 上下文标签赋予的颜色
 */
void SARibbonBar::paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color)
{
    // 绘制上下文标签
    // 首先有5像素的实体粗线位于顶部
    QMargins border = contentsMargins();
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(QRect(contextRect.x(), border.top(), contextRect.width(), 5));

    // 剩下把颜色变亮90%
    QColor gColor = color.lighter(190);

    // 减去之前的5像素
    contextRect -= QMargins(0, 5, 0, 0);
    painter.fillRect(contextRect, gColor);

    // 只有在office模式下才需要绘制标题
    if (isLooseStyle()) {
        if (!title.isEmpty()) {
            contextRect.setBottom(d_ptr->mRibbonTabBar->geometry().top());
            painter.setPen(color);
            painter.drawText(contextRect, Qt::AlignCenter, title);
        }
    }
    painter.restore();
}

void SARibbonBar::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    if (isLooseStyle()) {
        resizeInLooseStyle();
    } else {
        resizeInCompactStyle();
    }
    update();
}

/**
 * @brief 重写moveevent是为了在移动时调整isPopupMode状态下的stackedContainerWidget位置
 * @param event
 */
void SARibbonBar::moveEvent(QMoveEvent* e)
{
    if (d_ptr->mStackedContainerWidget) {
        if (d_ptr->mStackedContainerWidget->isPopupMode()) {
            // 弹出模式时，窗口发生了移动，同步调整StackedContainerWidget的位置
            resizeStackedContainerWidget();
        }
    }
    QMenuBar::moveEvent(e);
}

/**
 * @brief 跟踪字体改变
 * @param event
 */
void SARibbonBar::changeEvent(QEvent* e)
{
    if (nullptr == e) {
        return;
    }
    switch (e->type()) {
    case QEvent::FontChange: {
        QFont f                       = font();
        QList< QWidget* > listWidgets = findChildren< QWidget* >();
        for (QWidget* w : listWidgets) {
            w->setFont(f);
        }
        updateRibbonGeometry();
    } break;
    case QEvent::StyleChange: {
        updateRibbonGeometry();
    } break;
    default:
        break;
    }
    QMenuBar::changeEvent(e);
}

bool SARibbonBar::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::Show:
        // 第一次显示刷新
        updateRibbonGeometry();
        break;
    default:
        break;
    }
    return QMenuBar::event(e);
}

void SARibbonBar::resizeInLooseStyle()
{
    synchronousCategoryData(false);

    QMargins border = contentsMargins();
    int x           = border.left();
    int y           = border.top();

    // cornerWidget - TopLeftCorner
    const int validTitleBarHeight = d_ptr->titleBarHeight();
    const int tabH                = d_ptr->tabBarHeigth();

    // 布局corner widget
    x += d_ptr->mIconRightBorderPosition + 5;
    if (QWidget* connerL = cornerWidget(Qt::TopLeftCorner)) {
        if (connerL->isVisibleTo(this)) {
            QSize connerSize = connerL->sizeHint();
            if (connerSize.height() < validTitleBarHeight) {
                int detal = (validTitleBarHeight - connerSize.height()) / 2;
                connerL->setGeometry(x, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerL->setGeometry(x, y, connerSize.width(), validTitleBarHeight);
            }
            x = connerL->geometry().right() + 5;
        }
    }
    // quick access bar定位
    if (d_ptr->mQuickAccessBar) {
        if (d_ptr->mQuickAccessBar->isVisibleTo(this)) {
            if (d_ptr->mQuickAccessBar->height() != validTitleBarHeight) {
                d_ptr->mQuickAccessBar->setFixedHeight(validTitleBarHeight);
            }
            QSize quickAccessBarSize = d_ptr->mQuickAccessBar->sizeHint();
            // 上下留1px的边线
            d_ptr->mQuickAccessBar->setGeometry(x, y + 1, quickAccessBarSize.width(), validTitleBarHeight);
        }
    }
    // 第二行，开始布局applicationButton，tabbar，tabBarRightSizeButtonGroupWidget，TopRightCorner
    x = border.left();
    y += validTitleBarHeight;
    // applicationButton 定位
    if (d_ptr->mApplicationButton) {
        if (d_ptr->mApplicationButton->isVisibleTo(this)) {
            d_ptr->mApplicationButton->setGeometry(x, y, d_ptr->mApplicationButton->sizeHint().width(), tabH);
            x = d_ptr->mApplicationButton->geometry().right();
        }
    }
    // top right是一定要配置的，对于多文档窗口，子窗口的缩放等按钮就是通过这个窗口实现，
    // 由于这个窗口一定要在最右，因此先对这个窗口进行布局
    // cornerWidget - TopRightCorner
    // 获取最右边的位置
    int endX = width() - border.right();

    if (QWidget* connerW = cornerWidget(Qt::TopRightCorner)) {
        if (connerW->isVisibleTo(this)) {
            QSize connerSize = connerW->sizeHint();
            endX -= connerSize.width();
            if (connerSize.height() < tabH) {
                int detal = (tabH - connerSize.height()) / 2;
                connerW->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerW->setGeometry(endX, y, connerSize.width(), tabH);
            }
        }
    }
    // applicationButton和TopRightCorner完成定位，才可以定位tab bar
    // tab bar 定位

    // tabBar 右边的附加按钮组，这里一般会附加一些类似登录等按钮组
    // 20231106 把visible的判断去掉 && d_ptr->mRightButtonGroup->isVisible()
    if (d_ptr->mRightButtonGroup) {
        QSize wSize = d_ptr->mRightButtonGroup->sizeHint();
        endX -= wSize.width();
        // 上下留1px的边线
        d_ptr->mRightButtonGroup->setGeometry(endX, y + 1, wSize.width(), tabH - 2);
    }
    // 最后确定tabbar宽度
    int tabBarAllowedWidth = endX - x;
    if (ribbonAlignment() == SARibbonAlignment::AlignLeft) {
        d_ptr->mRibbonTabBar->setGeometry(x, y, tabBarAllowedWidth, tabH);
    } else {
        // 居中对齐的情况下，Tab要居中显示
        // 得到tab的推荐尺寸
        int mintabBarWidth = calcMinTabBarWidth();
        if (mintabBarWidth >= tabBarAllowedWidth) {
            // 这时tabbar没有居中对齐的必要性，空间位置不够了
            d_ptr->mRibbonTabBar->setGeometry(x, y, tabBarAllowedWidth, tabH);
        } else {
            // 说明tabbar的宽度有居中的可能性
            int xoffset = (tabBarAllowedWidth - mintabBarWidth) / 2;
            d_ptr->mRibbonTabBar->setGeometry(x + xoffset, y, mintabBarWidth, tabH);
        }
    }
    resizeStackedContainerWidget();
}

void SARibbonBar::resizeInCompactStyle()
{
    synchronousCategoryData(false);
    QMargins border = contentsMargins();
    int x           = border.left();
    int y           = border.top();

    const int validTitleBarHeight = titleBarHeight();

    // 先布局右边内容
    //  cornerWidget - TopRightCorner
    int endX = width() - border.right() - d_ptr->mWindowButtonSize.width();

    if (QWidget* connerW = cornerWidget(Qt::TopRightCorner)) {
        if (connerW->isVisibleTo(this)) {
            QSize connerSize = connerW->sizeHint();
            endX -= connerSize.width();
            if (connerSize.height() < validTitleBarHeight) {
                int detal = (validTitleBarHeight - connerSize.height()) / 2;
                connerW->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerW->setGeometry(endX, y, connerSize.width(), validTitleBarHeight);
            }
        }
    }

    // tabBar 右边的附加按钮组
    // 20231106 把visible的判断去掉 && d_ptr->mRightButtonGroup->isVisible()
    if (d_ptr->mRightButtonGroup) {
        QSize wSize = d_ptr->mRightButtonGroup->sizeHint();
        endX -= wSize.width();
        // 上下留1px的边线
        d_ptr->mRightButtonGroup->setGeometry(endX, y + 1, wSize.width(), validTitleBarHeight - 2);
    }
    // quick access bar定位
    if (d_ptr->mQuickAccessBar) {
        if (d_ptr->mQuickAccessBar->isVisibleTo(this)) {
            QSize quickAccessBarSize = d_ptr->mQuickAccessBar->sizeHint();
            endX -= quickAccessBarSize.width();
            // 上下留1px的边线
            d_ptr->mQuickAccessBar->setGeometry(endX, y + 1, quickAccessBarSize.width(), validTitleBarHeight - 2);
        }
    }
    // cornerWidget - TopLeftCorner
    if (QWidget* connerL = cornerWidget(Qt::TopLeftCorner)) {
        if (connerL->isVisibleTo(this)) {
            QSize connerSize = connerL->sizeHint();
            endX -= connerSize.width();
            if (connerSize.height() < validTitleBarHeight) {
                int detal = (validTitleBarHeight - connerSize.height()) / 2;
                connerL->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
            } else {
                connerL->setGeometry(endX, y, connerSize.width(), validTitleBarHeight);
            }
        }
    }

    // tab 的y值需要重新计算
    int tabH = tabBarHeight();

    if (tabH > validTitleBarHeight) {
        // 这种直接把tabH设置为validTitleBarHeight
        tabH = validTitleBarHeight;
    }
    y = y + validTitleBarHeight - tabH;  // 如果tabH较小，则下以，让tab底部和title的底部对齐

    // applicationButton 定位，与TabBar同高
    if (d_ptr->mApplicationButton) {
        if (d_ptr->mApplicationButton->isVisibleTo(this)) {
            d_ptr->mApplicationButton->setGeometry(x, y, d_ptr->mApplicationButton->sizeHint().width(), tabH);
            x = d_ptr->mApplicationButton->geometry().right() + 2;
        }
    }
    // tab bar 定位 wps模式下applicationButton的右边就是tab bar
    int tabBarAllowedWidth = endX - x;
    // 20200831
    // tabBarWidth的宽度原来为endX - x;，现需要根据实际进行调整
    // 为了把tabbar没有tab的部分不占用，这里的宽度需要根据tab的size来进行设置，让tabbar的长度刚刚好，这样能让出
    // mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
    // 计算tab所占用的宽度
    int mintabBarWidth = calcMinTabBarWidth();

    if (ribbonAlignment() == SARibbonAlignment::AlignLeft) {
        if (mintabBarWidth < tabBarAllowedWidth) {
            tabBarAllowedWidth = mintabBarWidth;
        }
        d_ptr->mRibbonTabBar->setGeometry(x, y, tabBarAllowedWidth, tabH);
    } else {
        // 居中对齐
        if (mintabBarWidth >= tabBarAllowedWidth) {
            // 这时tabbar没有居中对齐的必要性，空间位置不够了
            d_ptr->mRibbonTabBar->setGeometry(x, y, tabBarAllowedWidth, tabH);
        } else {
            // 说明tabbar的宽度有居中的可能性
            int xoffset = (tabBarAllowedWidth - mintabBarWidth) / 2;
            d_ptr->mRibbonTabBar->setGeometry(x + xoffset, y, mintabBarWidth, tabH);
        }
    }
    // 调整整个stackedContainer
    resizeStackedContainerWidget();
}

/**
 * @brief 绘制tabbar下的基准线，这个方法仅仅在office2013模式下需要
 * @param painter
 */
void SARibbonBar::paintTabbarBaseLine(QPainter& painter)
{
    if (!d_ptr->mTabBarBaseLineColor.isValid()) {
        return;
    }
    painter.save();
    // 在tabbar下绘制一条线
    const int lineY = d_ptr->mRibbonTabBar->geometry().bottom();
    QPen pen(d_ptr->mTabBarBaseLineColor);
    QMargins border = contentsMargins();

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawLine(QPoint(border.left(), lineY), QPoint(width() - border.right() - 1, lineY));
    painter.restore();
}

///
/// \brief 绘制标题栏
/// \param painter
/// \param title 标题
/// \param contextCategoryRegion 当前显示的上下文标签的范围，上下文标签是可以遮挡标题栏的，因此需要知道上下文标签的范围
/// x表示左边界，y表示右边界
///
void SARibbonBar::paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion)
{
    // 如果标题不显示直接跳出
    if (!isTitleVisible()) {
        return;
    }
    painter.save();
    if (d_ptr->mTitleTextColor.isValid()) {
        painter.setPen(d_ptr->mTitleTextColor);
    }
    painter.drawText(titleRegion, d_ptr->mTitleAligment, title);
    painter.restore();
}

#if SA_DEBUG_PRINT_SARIBBONBAR
QDebug operator<<(QDebug debug, const SARibbonBar& ribbon)
{
    QDebugStateSaver saver(debug);
    QFontMetrics fm = ribbon.fontMetrics();
    debug.nospace() << "SARibbonBar(" << ribbon.versionString() << ")"                            //
                    << "\nribbon font metrics info:"                                              //
                    << "\n - lineSpacing:" << fm.lineSpacing()                                    //
                    << "\n - height:" << fm.height()                                              //
                    << "\n - em:" << fm.boundingRect("M").width()                                 //
                    << "\n - ex:" << fm.boundingRect("X").height()                                //
                    << "\nribbon info:"                                                           //
                    << "\n -mTitleBarHeight=" << ribbon.d_ptr->mTitleBarHeight                    //
                    << "\n -mTabBarHeight=" << ribbon.d_ptr->mTabBarHeight                        //
                    << "\n -mPannelTitleHeight=" << ribbon.d_ptr->mPannelTitleHeight              //
                    << "\n -mCategoryHeight=" << ribbon.d_ptr->mCategoryHeight                    //
                    << "\n -mIsTabOnTitle=" << ribbon.d_ptr->mIsTabOnTitle                        //
                    << "\n -mEnableShowPannelTitle=" << ribbon.d_ptr->mEnableShowPannelTitle      //
                    << "\n -mWindowButtonSize=" << ribbon.d_ptr->mWindowButtonSize                //
                    << "\n -mIconRightBorderPosition=" << ribbon.d_ptr->mIconRightBorderPosition  //
            ;

    return debug;
}
#endif

/*** End of inlined file: SARibbonBar.cpp ***/

/*** Start of inlined file: SARibbonElementFactory.cpp ***/
#include <QApplication>
#include <QFontMetrics>

SARibbonElementFactory::SARibbonElementFactory()
{
}

SARibbonElementFactory::~SARibbonElementFactory()
{
}

SARibbonTabBar* SARibbonElementFactory::createRibbonTabBar(QWidget* parent)
{
    return (new SARibbonTabBar(parent));
}

SARibbonApplicationButton* SARibbonElementFactory::createRibbonApplicationButton(QWidget* parent)
{
    return (new SARibbonApplicationButton(parent));
}

SARibbonCategory* SARibbonElementFactory::createRibbonCategory(QWidget* parent)
{
    return (new SARibbonCategory(parent));
}

SARibbonContextCategory* SARibbonElementFactory::createRibbonContextCategory(QWidget* parent)
{
    return (new SARibbonContextCategory(parent));
}

SARibbonPannel* SARibbonElementFactory::createRibbonPannel(QWidget* parent)
{
    return (new SARibbonPannel(parent));
}

SARibbonSeparatorWidget* SARibbonElementFactory::createRibbonSeparatorWidget(QWidget* parent)
{
    return (new SARibbonSeparatorWidget(parent));
}

SARibbonGallery* SARibbonElementFactory::createRibbonGallery(QWidget* parent)
{
    return (new SARibbonGallery(parent));
}

SARibbonGalleryGroup* SARibbonElementFactory::createRibbonGalleryGroup(QWidget* parent)
{
    return (new SARibbonGalleryGroup(parent));
}

SARibbonToolButton* SARibbonElementFactory::createRibbonToolButton(QWidget* parent)
{
    return (new SARibbonToolButton(parent));
}

SARibbonControlButton* SARibbonElementFactory::createRibbonControlButton(QWidget* parent)
{
    return (new SARibbonControlButton(parent));
}

SARibbonControlToolButton* SARibbonElementFactory::createRibbonControlToolButton(QWidget* parent)
{
    return (new SARibbonControlToolButton(parent));
}

SARibbonStackedWidget* SARibbonElementFactory::createRibbonStackedWidget(SARibbonBar* parent)
{
    return (new SARibbonStackedWidget(parent));
}

SARibbonButtonGroupWidget* SARibbonElementFactory::craeteButtonGroupWidget(QWidget* parent)
{
    return (new SARibbonButtonGroupWidget(parent));
}

SARibbonQuickAccessBar* SARibbonElementFactory::createQuickAccessBar(QWidget* parent)
{
    return (new SARibbonQuickAccessBar(parent));
}

SARibbonSystemButtonBar* SARibbonElementFactory::createWindowButtonGroup(QWidget* parent)
{
    return (new SARibbonSystemButtonBar(parent));
}

/**
 * @brief 创建SARibbonPannelOptionButton
 * @param pannel 附属的pannel
 * @return
 * @sa SARibbonPannelOptionButton
 */
SARibbonPannelOptionButton* SARibbonElementFactory::createRibbonPannelOptionButton(SARibbonPannel* pannel)
{
    return (new SARibbonPannelOptionButton(pannel));
}

/*** End of inlined file: SARibbonElementFactory.cpp ***/

/*** Start of inlined file: SARibbonElementManager.cpp ***/
SARibbonElementManager::SARibbonElementManager()
{
    mFactory.reset(new SARibbonElementFactory());
}

SARibbonElementManager::~SARibbonElementManager()
{
}

SARibbonElementManager* SARibbonElementManager::instance()
{
    static SARibbonElementManager s_instance;

    return (&(s_instance));
}

SARibbonElementFactory* SARibbonElementManager::factory()
{
    return (mFactory.data());
}

void SARibbonElementManager::setupFactory(SARibbonElementFactory* delegate)
{
    mFactory.reset(delegate);
}

/*** End of inlined file: SARibbonElementManager.cpp ***/

/*** Start of inlined file: SARibbonCustomizeData.cpp ***/
#include <QDebug>
#include <QObject>
////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonCustomizeData
////////////////////////////////////////////////////////////////////////////////////////////////////////

SARibbonCustomizeData::SARibbonCustomizeData()
    : indexValue(-1), actionRowProportionValue(SARibbonPannelItem::Large), m_type(UnknowActionType), m_actionsManagerPointer(nullptr)
{
}

SARibbonCustomizeData::SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr)
    : indexValue(-1), actionRowProportionValue(SARibbonPannelItem::Large), m_type(type), m_actionsManagerPointer(mgr)
{
}

/**
 * @brief 获取CustomizeData的action type
 * @return
 */
SARibbonCustomizeData::ActionType SARibbonCustomizeData::actionType() const
{
    return (m_type);
}

/**
 * @brief 设置CustomizeData的action type
 * @param a
 */
void SARibbonCustomizeData::setActionType(SARibbonCustomizeData::ActionType a)
{
    m_type = a;
}

/**
 * @brief 判断是否是一个正常的CustomizeData
 *
 * 实际逻辑actionType() != UnknowActionType
 * @return 有用的CustomizeData返回true
 */
bool SARibbonCustomizeData::isValid() const
{
    return (actionType() != UnknowActionType);
}

/**
 * @brief 应用SARibbonCustomizeData到SARibbonBar
 * @param m
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
 */
bool SARibbonCustomizeData::apply(SARibbonBar* bar) const
{
    if (nullptr == bar) {
        return (false);
    }
    switch (actionType()) {
    case UnknowActionType:
        return (false);

    case AddCategoryActionType: {
        // 添加标签
        SARibbonCategory* c = bar->insertCategoryPage(keyValue, indexValue);
        if (nullptr == c) {
            return (false);
        }
        c->setObjectName(categoryObjNameValue);
        SARibbonCustomizeData::setCanCustomize(c);
        return (true);
    }

    case AddPannelActionType: {
        // 添加pannel
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel* p = c->insertPannel(keyValue, indexValue);
        p->setObjectName(pannelObjNameValue);
        SARibbonCustomizeData::setCanCustomize(p);
        return (true);
    }

    case AddActionActionType: {
        if (nullptr == m_actionsManagerPointer) {
            return (false);
        }
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel* pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        QAction* act = m_actionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        SARibbonCustomizeData::setCanCustomize(act);
        pannel->addAction(act, actionRowProportionValue);
        return (true);
    }

    case RemoveCategoryActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        bar->removeCategory(c);
        return (true);
    }

    case RemovePannelActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel* pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        c->removePannel(pannel);
        return (true);
    }

    case RemoveActionActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel* pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        QAction* act = m_actionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        pannel->removeAction(act);
        return (true);
    }

    case ChangeCategoryOrderActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        int currentindex = bar->categoryIndex(c);
        if (-1 == currentindex) {
            return (false);
        }
        int toindex = currentindex + indexValue;
        bar->moveCategory(currentindex, toindex);
        return (true);
    }

    case ChangePannelOrderActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel* pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        int pannelIndex = c->pannelIndex(pannel);
        if (-1 == pannelIndex) {
            return (false);
        }
        c->movePannel(pannelIndex, pannelIndex + indexValue);
        return (true);
    }

    case ChangeActionOrderActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel* pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        QAction* act = m_actionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        int actindex = pannel->actionIndex(act);
        if (actindex <= -1) {
            return (false);
        }
        pannel->moveAction(actindex, actindex + indexValue);
        return (true);
    }

    case RenameCategoryActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        c->setCategoryName(keyValue);
        return (true);
    }

    case RenamePannelActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel* pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        pannel->setPannelName(keyValue);
        return (true);
    }

    case VisibleCategoryActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        if (1 == indexValue) {
            bar->showCategory(c);
        } else {
            bar->hideCategory(c);
        }
        return (true);
    }

    default:
        break;
    }
    return (false);
}

/**
 * @brief 获取actionmanager指针
 * @return
 */
SARibbonActionsManager* SARibbonCustomizeData::actionManager()
{
    return (m_actionsManagerPointer);
}

/**
 * @brief 设置ActionsManager
 * @param mgr
 */
void SARibbonCustomizeData::setActionsManager(SARibbonActionsManager* mgr)
{
    m_actionsManagerPointer = mgr;
}

/**
 * @brief 创建一个AddCategoryActionType的SARibbonCustomizeData
 * @param title category 的标题
 * @param index category要插入的位置
 * @param objName category的object name
 * @return 返回AddCategoryActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName)
{
    SARibbonCustomizeData d(AddCategoryActionType);

    d.indexValue           = index;
    d.keyValue             = title;
    d.categoryObjNameValue = objName;
    return (d);
}

/**
 * @brief 创建一个AddPannelActionType的SARibbonCustomizeData
 * @param title pannel的标题
 * @param index pannel的index
 * @param categoryobjName pannel的category的objectname
 * @param objName pannel的objname
 * @return 返回AddPannelActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddPannelCustomizeData(const QString& title,
                                                                        int index,
                                                                        const QString& categoryobjName,
                                                                        const QString& objName)
{
    SARibbonCustomizeData d(AddPannelActionType);

    d.indexValue           = index;
    d.keyValue             = title;
    d.pannelObjNameValue   = objName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}

/**
 * @brief 添加action
 * @param key action的索引
 * @param mgr action管理器
 * @param rp 定义action的占位情况
 * @param categoryObjName action添加到的category的objname
 * @param pannelObjName action添加到的category下的pannel的objname
 * @param index action添加到的pannel的索引
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddActionCustomizeData(const QString& key,
                                                                        SARibbonActionsManager* mgr,
                                                                        SARibbonPannelItem::RowProportion rp,
                                                                        const QString& categoryObjName,
                                                                        const QString& pannelObjName)
{
    SARibbonCustomizeData d(AddActionActionType, mgr);

    d.keyValue                 = key;
    d.categoryObjNameValue     = categoryObjName;
    d.pannelObjNameValue       = pannelObjName;
    d.actionRowProportionValue = rp;

    return (d);
}

/**
 * @brief 创建一个RenameCategoryActionType的SARibbonCustomizeData
 * @param newname 新名字
 * @param index category的索引
 * @return 返回RenameCategoryActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName)
{
    SARibbonCustomizeData d(RenameCategoryActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize rename category,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.keyValue             = newname;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}

/**
 * @brief 创建一个RenamePannelActionType的SARibbonCustomizeData
 * @param newname pannel的名字
 * @param indexValue pannel的索引
 * @param categoryobjName pannel对应的category的object name
 * @return 返回RenamePannelActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenamePannelCustomizeData(const QString& newname,
                                                                           const QString& categoryobjName,
                                                                           const QString& pannelObjName)
{
    SARibbonCustomizeData d(RenamePannelActionType);

    if (pannelObjName.isEmpty() || categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize rename pannel,"
                                "but get an empty category/pannel object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.keyValue             = newname;
    d.pannelObjNameValue   = pannelObjName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}

/**
 * @brief 对应ChangeCategoryOrderActionType
 * @param categoryobjName 需要移动的categoryobjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(const QString& categoryobjName, int moveindex)
{
    SARibbonCustomizeData d(ChangeCategoryOrderActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change category order,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.indexValue           = moveindex;
    return (d);
}

/**
 * @brief 对应ChangePannelOrderActionType
 * @param categoryobjName 需要移动的pannel对应的categoryobjName
 * @param pannelObjName 需要移动的pannelObjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangePannelOrderCustomizeData(const QString& categoryobjName,
                                                                                const QString& pannelObjName,
                                                                                int moveindex)
{
    SARibbonCustomizeData d(ChangePannelOrderActionType);

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change pannel order,"
                                "but get an empty category/pannel object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue   = pannelObjName;
    d.indexValue           = moveindex;
    return (d);
}

/**
 * @brief 对应ChangeActionOrderActionType
 * @param categoryobjName 需要移动的pannel对应的categoryobjName
 * @param pannelObjName 需要移动的pannelObjName
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangeActionOrderCustomizeData(const QString& categoryobjName,
                                                                                const QString& pannelObjName,
                                                                                const QString& key,
                                                                                SARibbonActionsManager* mgr,
                                                                                int moveindex)
{
    SARibbonCustomizeData d(ChangeActionOrderActionType, mgr);

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change action order,"
                                "but get an empty category/pannel/action object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue   = pannelObjName;
    d.keyValue             = key;
    d.indexValue           = moveindex;
    return (d);
}

/**
 * @brief 对应RemoveCategoryActionType
 * @param categoryobjName 需要移除的objname
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemoveCategoryCustomizeData(const QString& categoryobjName)
{
    SARibbonCustomizeData d(RemoveCategoryActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove category,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    return (d);
}

/**
 * @brief 对应RemovePannelActionType
 * @param categoryobjName pannel对应的category的obj name
 * @param pannelObjName pannel对应的 obj name
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemovePannelCustomizeData(const QString& categoryobjName, const QString& pannelObjName)
{
    SARibbonCustomizeData d(RemovePannelActionType);

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove pannel,"
                                "but get an empty category/pannel object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue   = pannelObjName;
    return (d);
}

/**
 * @brief 对应RemoveActionActionType
 * @param categoryobjName pannel对应的category的obj name
 * @param pannelObjName pannel对应的 obj name
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemoveActionCustomizeData(const QString& categoryobjName,
                                                                           const QString& pannelObjName,
                                                                           const QString& key,
                                                                           SARibbonActionsManager* mgr)
{
    SARibbonCustomizeData d(RemoveActionActionType, mgr);

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove action,"
                                "but get an empty category/pannel/action object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue   = pannelObjName;
    d.keyValue             = key;
    return (d);
}

/**
 * @brief SARibbonCustomizeData::makeVisibleCategoryCustomizeData
 * @param categoryobjName
 * @param isShow
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow)
{
    SARibbonCustomizeData d(VisibleCategoryActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize visible category,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.indexValue           = isShow ? 1 : 0;
    return (d);
}

/**
 * @brief 判断外置属性，是否允许自定义
 * @param obj
 * @return
 */
bool SARibbonCustomizeData::isCanCustomize(QObject* obj)
{
    QVariant v = obj->property(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE);

    if (v.isValid()) {
        return (v.toBool());
    }
    return (false);
}

/**
 * @brief 设置外置属性允许自定义
 * @param obj
 * @param canbe
 */
void SARibbonCustomizeData::setCanCustomize(QObject* obj, bool canbe)
{
    obj->setProperty(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE, canbe);
}

QList< SARibbonCustomizeData > remove_indexs(const QList< SARibbonCustomizeData >& csd, const QList< int >& willremoveIndex);

QList< SARibbonCustomizeData > remove_indexs(const QList< SARibbonCustomizeData >& csd, const QList< int >& willremoveIndex)
{
    QList< SARibbonCustomizeData > res;

    for (int i = 0; i < csd.size(); ++i) {
        if (!willremoveIndex.contains(i)) {
            res << csd[ i ];
        }
    }
    return (res);
}

/**
 * @brief 对QList<SARibbonCustomizeData>进行简化操作
 *
 * 此函数会执行如下操作：
 * 1、针对同一个category/pannel连续出现的添加和删除操作进行移除（前一步添加，后一步删除）
 *
 * 2、针对VisibleCategoryActionType，对于连续出现的操作只保留最后一步
 *
 * 3、针对RenameCategoryActionType和RenamePannelActionType操作，只保留最后一个
 *
 * 4、针对连续的ChangeCategoryOrderActionType，ChangePannelOrderActionType，ChangeActionOrderActionType进行合并为一个动作，
 * 如果合并后原地不动，则删除
 *
 * @param csd
 * @return 返回简化的QList<SARibbonCustomizeData>
 */
QList< SARibbonCustomizeData > SARibbonCustomizeData::simplify(const QList< SARibbonCustomizeData >& csd)
{
    int size = csd.size();

    if (size <= 1) {
        return (csd);
    }
    QList< SARibbonCustomizeData > res;
    QList< int > willremoveIndex;  // 记录要删除的index

    //! 首先针对连续出现的添加和删除操作进行优化
    for (int i = 1; i < size; ++i) {
        if ((csd[ i - 1 ].actionType() == AddCategoryActionType) && (csd[ i ].actionType() == RemoveCategoryActionType)) {
            if (csd[ i - 1 ].categoryObjNameValue == csd[ i ].categoryObjNameValue) {
                willremoveIndex << i - 1 << i;
            }
        } else if ((csd[ i - 1 ].actionType() == AddPannelActionType) && (csd[ i ].actionType() == RemovePannelActionType)) {
            if ((csd[ i - 1 ].pannelObjNameValue == csd[ i ].pannelObjNameValue)
                && (csd[ i - 1 ].categoryObjNameValue == csd[ i ].categoryObjNameValue)) {
                willremoveIndex << i - 1 << i;
            }
        } else if ((csd[ i - 1 ].actionType() == AddActionActionType) && (csd[ i ].actionType() == RemoveActionActionType)) {
            if ((csd[ i - 1 ].keyValue == csd[ i ].keyValue) && (csd[ i - 1 ].pannelObjNameValue == csd[ i ].pannelObjNameValue)
                && (csd[ i - 1 ].categoryObjNameValue == csd[ i ].categoryObjNameValue)) {
                willremoveIndex << i - 1 << i;
            }
        }
    }
    res = remove_indexs(csd, willremoveIndex);
    willremoveIndex.clear();

    //! 筛选VisibleCategoryActionType，对于连续出现的操作只保留最后一步
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[ i - 1 ].actionType() == VisibleCategoryActionType) && (res[ i ].actionType() == VisibleCategoryActionType)) {
            if (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue) {
                // 要保证操作的是同一个内容
                willremoveIndex << i - 1;  // 删除前一个只保留最后一个
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对RenameCategoryActionType和RenamePannelActionType操作，只需保留最后一个
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if (res[ i ].actionType() == RenameCategoryActionType) {
            // 向后查询，如果查询到有同一个Category改名，把这个索引加入删除队列
            for (int j = i + 1; j < size; ++j) {
                if ((res[ j ].actionType() == RenameCategoryActionType)
                    && (res[ i ].categoryObjNameValue == res[ j ].categoryObjNameValue)) {
                    willremoveIndex << i;
                }
            }
        } else if (res[ i ].actionType() == RenamePannelActionType) {
            // 向后查询，如果查询到有同一个pannel改名，把这个索引加入删除队列
            for (int j = i + 1; j < size; ++j) {
                if ((res[ j ].actionType() == RenamePannelActionType) && (res[ i ].pannelObjNameValue == res[ j ].pannelObjNameValue)
                    && (res[ i ].categoryObjNameValue == res[ j ].categoryObjNameValue)) {
                    willremoveIndex << i;
                }
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对连续的ChangeCategoryOrderActionType，ChangePannelOrderActionType，ChangeActionOrderActionType进行合并
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[ i - 1 ].actionType() == ChangeCategoryOrderActionType) && (res[ i ].actionType() == ChangeCategoryOrderActionType)
            && (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[ i ].indexValue += res[ i - 1 ].indexValue;
            willremoveIndex << i - 1;
        } else if ((res[ i - 1 ].actionType() == ChangePannelOrderActionType) && (res[ i ].actionType() == ChangePannelOrderActionType)
                   && (res[ i - 1 ].pannelObjNameValue == res[ i ].pannelObjNameValue)
                   && (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[ i ].indexValue += res[ i - 1 ].indexValue;
            willremoveIndex << i - 1;
        } else if ((res[ i - 1 ].actionType() == ChangeActionOrderActionType)
                   && (res[ i ].actionType() == ChangeActionOrderActionType) && (res[ i - 1 ].keyValue == res[ i ].keyValue)
                   && (res[ i - 1 ].pannelObjNameValue == res[ i ].pannelObjNameValue)
                   && (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[ i ].indexValue += res[ i - 1 ].indexValue;
            willremoveIndex << i - 1;
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 上一步操作可能会产生indexvalue为0的情况，此操作把indexvalue为0的删除
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if ((res[ i ].actionType() == ChangeCategoryOrderActionType) || (res[ i ].actionType() == ChangePannelOrderActionType)
            || (res[ i ].actionType() == ChangeActionOrderActionType)) {
            if (0 == res[ i ].indexValue) {
                willremoveIndex << i;
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();
    return (res);
}

/*** End of inlined file: SARibbonCustomizeData.cpp ***/

/*** Start of inlined file: SARibbonCustomizeWidget.cpp ***/
#include <QDebug>
#include <QtCore/QVariant>
#include <QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QListView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <QStandardItemModel>
#include <QButtonGroup>
#include <QInputDialog>
#include <QLineEdit>
#include <QDateTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QFile>
#include <QMessageBox>

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonCustomizeWidget
////////////////////////////////////////////////////////////////////////////////////////////////////////

bool sa_customize_datas_to_xml(QXmlStreamWriter* xml, const QList< SARibbonCustomizeData >& cds)
{
    if (cds.size() <= 0) {
        return (false);
    }

    xml->writeStartElement("sa-ribbon-customize");
    for (const SARibbonCustomizeData& d : cds) {
        xml->writeStartElement("customize-data");
        xml->writeAttribute("type", QString::number(d.actionType()));
        xml->writeAttribute("index", QString::number(d.indexValue));
        xml->writeAttribute("key", d.keyValue);
        xml->writeAttribute("category", d.categoryObjNameValue);
        xml->writeAttribute("pannel", d.pannelObjNameValue);
        xml->writeAttribute("row-prop", QString::number(d.actionRowProportionValue));

        xml->writeEndElement();
    }
    xml->writeEndElement();
    if (xml->hasError()) {
        qWarning() << "write has error";
    }
    return (true);
}

QList< SARibbonCustomizeData > sa_customize_datas_from_xml(QXmlStreamReader* xml, SARibbonActionsManager* mgr)
{
    // 先找到"sa-ribbon-customize"
    while (!xml->atEnd()) {

        if (xml->isStartElement() && (xml->name().toString() == "sa-ribbon-customize")) {
            break;
        }
        xml->readNext();
    }
    QList< SARibbonCustomizeData > res;

    // 开始遍历"customize-data"
    while (!xml->atEnd()) {
        if (xml->isStartElement() && (xml->name().toString() == "customize-data")) {
            // 首先读取属性type
            SARibbonCustomizeData d;
            QXmlStreamAttributes attrs = xml->attributes();
            if (!attrs.hasAttribute("type")) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            bool isOk = false;
            int v     = xml->attributes().value("type").toInt(&isOk);
            if (!isOk) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            d.setActionType(static_cast< SARibbonCustomizeData::ActionType >(v));
            // 开始读取子对象
            if (attrs.hasAttribute("index")) {
                v = xml->attributes().value("index").toInt(&isOk);
                if (isOk) {
                    d.indexValue = v;
                }
            }
            if (attrs.hasAttribute("key")) {
                d.keyValue = attrs.value("key").toString();
            }
            if (attrs.hasAttribute("category")) {
                d.categoryObjNameValue = attrs.value("category").toString();
            }
            if (attrs.hasAttribute("pannel")) {
                d.pannelObjNameValue = attrs.value("pannel").toString();
            }
            if (attrs.hasAttribute("row-prop")) {
                v = xml->attributes().value("row-prop").toInt(&isOk);
                if (isOk) {
                    d.actionRowProportionValue = static_cast< SARibbonPannelItem::RowProportion >(v);
                }
            }
            d.setActionsManager(mgr);
            res.append(d);
        }
        xml->readNext();
    }
    if (xml->hasError()) {
        qWarning() << xml->errorString();
    }
    return (res);
}

int sa_customize_datas_apply(const QList< SARibbonCustomizeData >& cds, SARibbonBar* bar)
{
    int c = 0;

    for (const SARibbonCustomizeData& d : cds) {
        if (d.apply(bar)) {
            ++c;
        }
    }
    return (c);
}

int sa_customize_datas_reverse(const QList< SARibbonCustomizeData >& cds, SARibbonBar* bar)
{
    int c = 0;
    // todo 支持反向操作
    return (c);
}

bool sa_apply_customize_from_xml_file(const QString& filePath, SARibbonBar* bar, SARibbonActionsManager* mgr)
{
    QFile f(filePath);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return (false);
    }
    f.seek(0);
    QXmlStreamReader xml(&f);

    return (SARibbonCustomizeWidget::fromXml(&xml, bar, mgr));
}

/**
 * @brief 构建SARibbonCustomizeWidget的Ui
 */
class SARibbonCustomizeWidgetUi
{
public:
    QHBoxLayout* horizontalLayoutMain;
    QVBoxLayout* verticalLayoutSelect;
    QLabel* labelSelectAction;
    QHBoxLayout* horizontalLayoutSearch;
    QComboBox* comboBoxActionIndex;
    QLineEdit* lineEditSearchAction;
    QListView* listViewSelect;
    QVBoxLayout* verticalLayoutMidButtons;
    QSpacerItem* verticalSpacerUp;
    QPushButton* pushButtonAdd;
    QPushButton* pushButtonDelete;
    QPushButton* pushButtonReset;
    QSpacerItem* verticalSpacerDown;
    QLabel* labelProportion;
    QComboBox* comboBoxActionProportion;
    QVBoxLayout* verticalLayoutResult;
    QLabel* labelCustomize;
    QHBoxLayout* horizontalLayoutCategorySelect;
    QRadioButton* radioButtonMainCategory;
    QRadioButton* radioButtonAllCategory;
    QButtonGroup* radioButtonGroup;
    QTreeView* treeViewResult;
    QHBoxLayout* horizontalLayoutActionOptBtns;
    QPushButton* pushButtonNewCategory;
    QPushButton* pushButtonNewPannel;
    QPushButton* pushButtonRename;
    QVBoxLayout* verticalLayoutRightButtons;
    QSpacerItem* verticalSpacerUp2;
    QToolButton* toolButtonUp;
    QToolButton* toolButtonDown;
    QSpacerItem* verticalSpacerDown2;

    void setupUi(QWidget* customizeWidget)
    {
        if (customizeWidget->objectName().isEmpty()) {
            customizeWidget->setObjectName(QStringLiteral("SARibbonCustomizeWidget"));
        }
        customizeWidget->resize(800, 600);
        horizontalLayoutMain = new QHBoxLayout(customizeWidget);
        horizontalLayoutMain->setObjectName(QStringLiteral("horizontalLayoutMain"));
        verticalLayoutSelect = new QVBoxLayout();
        verticalLayoutSelect->setObjectName(QStringLiteral("verticalLayoutSelect"));
        labelSelectAction = new QLabel(customizeWidget);
        labelSelectAction->setObjectName(QStringLiteral("labelSelectAction"));

        verticalLayoutSelect->addWidget(labelSelectAction);

        horizontalLayoutSearch = new QHBoxLayout();
        horizontalLayoutSearch->setObjectName(QStringLiteral("horizontalLayoutSearch"));
        comboBoxActionIndex = new QComboBox(customizeWidget);
        comboBoxActionIndex->setObjectName(QStringLiteral("comboBoxActionIndex"));
        comboBoxActionIndex->setEditable(false);

        horizontalLayoutSearch->addWidget(comboBoxActionIndex);

        lineEditSearchAction = new QLineEdit(customizeWidget);
        lineEditSearchAction->setObjectName(QStringLiteral("lineEditSearchAction"));

        horizontalLayoutSearch->addWidget(lineEditSearchAction);

        verticalLayoutSelect->addLayout(horizontalLayoutSearch);

        listViewSelect = new QListView(customizeWidget);
        listViewSelect->setObjectName(QStringLiteral("listViewSelect"));

        verticalLayoutSelect->addWidget(listViewSelect);

        horizontalLayoutMain->addLayout(verticalLayoutSelect);

        verticalLayoutMidButtons = new QVBoxLayout();
        verticalLayoutMidButtons->setObjectName(QStringLiteral("verticalLayoutMidButtons"));
        verticalSpacerUp = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutMidButtons->addItem(verticalSpacerUp);

        pushButtonAdd = new QPushButton(customizeWidget);
        pushButtonAdd->setObjectName(QStringLiteral("pushButtonAdd"));
        pushButtonAdd->setEnabled(false);

        verticalLayoutMidButtons->addWidget(pushButtonAdd);

        pushButtonDelete = new QPushButton(customizeWidget);
        pushButtonDelete->setObjectName(QStringLiteral("pushButtonDelete"));
        pushButtonDelete->setEnabled(false);

        verticalLayoutMidButtons->addWidget(pushButtonDelete);

        verticalSpacerDown = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutMidButtons->addItem(verticalSpacerDown);

        labelProportion = new QLabel(customizeWidget);
        labelProportion->setObjectName(QStringLiteral("labelProportion"));
        verticalLayoutMidButtons->addWidget(labelProportion);

        comboBoxActionProportion = new QComboBox(customizeWidget);
        comboBoxActionProportion->setObjectName(QStringLiteral("comboBoxActionProportion"));
        comboBoxActionProportion->setEditable(false);
        verticalLayoutMidButtons->addWidget(comboBoxActionProportion);
        horizontalLayoutMain->addLayout(verticalLayoutMidButtons);

        verticalLayoutResult = new QVBoxLayout();
        verticalLayoutResult->setObjectName(QStringLiteral("verticalLayoutResult"));
        labelCustomize = new QLabel(customizeWidget);
        labelCustomize->setObjectName(QStringLiteral("labelCustomize"));

        verticalLayoutResult->addWidget(labelCustomize);

        horizontalLayoutCategorySelect = new QHBoxLayout();
        horizontalLayoutCategorySelect->setObjectName(QStringLiteral("horizontalLayoutCategorySelect"));
        radioButtonMainCategory = new QRadioButton(customizeWidget);
        radioButtonMainCategory->setObjectName(QStringLiteral("radioButtonMainCategory"));
        radioButtonMainCategory->setChecked(false);

        horizontalLayoutCategorySelect->addWidget(radioButtonMainCategory);

        radioButtonAllCategory = new QRadioButton(customizeWidget);
        radioButtonAllCategory->setObjectName(QStringLiteral("radioButtonAllCategory"));
        radioButtonAllCategory->setChecked(true);

        horizontalLayoutCategorySelect->addWidget(radioButtonAllCategory);

        radioButtonGroup = new QButtonGroup(customizeWidget);
        radioButtonGroup->addButton(radioButtonMainCategory);
        radioButtonGroup->addButton(radioButtonAllCategory);

        verticalLayoutResult->addLayout(horizontalLayoutCategorySelect);

        treeViewResult = new QTreeView(customizeWidget);
        treeViewResult->setObjectName(QStringLiteral("treeViewResult"));
        treeViewResult->setHeaderHidden(true);
        treeViewResult->setSelectionMode(QAbstractItemView::SingleSelection);
        treeViewResult->setAnimated(true);                                   // 支持动画
        treeViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 不允许直接在item上重命名

        verticalLayoutResult->addWidget(treeViewResult);

        horizontalLayoutActionOptBtns = new QHBoxLayout();
        horizontalLayoutActionOptBtns->setObjectName(QStringLiteral("horizontalLayoutActionOptBtns"));
        pushButtonNewCategory = new QPushButton(customizeWidget);
        pushButtonNewCategory->setObjectName(QStringLiteral("pushButtonNewCategory"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewCategory);

        pushButtonNewPannel = new QPushButton(customizeWidget);
        pushButtonNewPannel->setObjectName(QStringLiteral("pushButtonNewPannel"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewPannel);

        pushButtonRename = new QPushButton(customizeWidget);
        pushButtonRename->setObjectName(QStringLiteral("pushButtonRename"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonRename);

        pushButtonReset = new QPushButton(customizeWidget);
        pushButtonReset->setObjectName(QStringLiteral("pushButtonReset"));
        horizontalLayoutActionOptBtns->addWidget(pushButtonReset);

        verticalLayoutResult->addLayout(horizontalLayoutActionOptBtns);

        horizontalLayoutMain->addLayout(verticalLayoutResult);

        verticalLayoutRightButtons = new QVBoxLayout();
        verticalLayoutRightButtons->setObjectName(QStringLiteral("verticalLayoutRightButtons"));
        verticalSpacerUp2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutRightButtons->addItem(verticalSpacerUp2);

        toolButtonUp = new QToolButton(customizeWidget);
        toolButtonUp->setObjectName(QStringLiteral("pushButtonUp"));
        toolButtonUp->setArrowType(Qt::UpArrow);
        toolButtonUp->setAutoRaise(true);

        verticalLayoutRightButtons->addWidget(toolButtonUp);

        toolButtonDown = new QToolButton(customizeWidget);
        toolButtonDown->setObjectName(QStringLiteral("pushButtonDown"));
        toolButtonDown->setArrowType(Qt::DownArrow);
        toolButtonDown->setAutoRaise(true);

        verticalLayoutRightButtons->addWidget(toolButtonDown);

        verticalSpacerDown2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutRightButtons->addItem(verticalSpacerDown2);

        horizontalLayoutMain->addLayout(verticalLayoutRightButtons);

        retranslateUi(customizeWidget);
    }  // setupUi

    void retranslateUi(QWidget* customizeWidget)
    {
        customizeWidget->setWindowTitle(QApplication::translate("SARibbonCustomizeWidget", "Customize Widget", Q_NULLPTR));
        labelSelectAction->setText(QApplication::translate("SARibbonCustomizeWidget", "Please Select", Q_NULLPTR));  // cn:请选择
        lineEditSearchAction->setInputMask(QString());
        lineEditSearchAction->setText(QString());
        lineEditSearchAction->setPlaceholderText(QApplication::translate("SARibbonCustomizeWidget", "Find Command", Q_NULLPTR));  // cn:查找命令
        pushButtonAdd->setText(QApplication::translate("SARibbonCustomizeWidget", "Add >>", Q_NULLPTR));  // cn:添加 >>
        pushButtonDelete->setText(QApplication::translate("SARibbonCustomizeWidget", "<< Remove", Q_NULLPTR));  // cn:<< 移除
        labelCustomize->setText(QApplication::translate("SARibbonCustomizeWidget", "Customize the Ribbon", Q_NULLPTR));  // cn:自定义功能区
        radioButtonMainCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "Main Category", Q_NULLPTR));  // cn:主选项卡
        radioButtonAllCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "All Category", Q_NULLPTR));  // cn:所有选项卡
        pushButtonNewCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "New Category", Q_NULLPTR));  // cn:新建选项卡
        pushButtonNewPannel->setText(QApplication::translate("SARibbonCustomizeWidget", "New Group", Q_NULLPTR));  // cn:新建组
        pushButtonRename->setText(QApplication::translate("SARibbonCustomizeWidget", "Rename", Q_NULLPTR));  // cn:重命名
        pushButtonReset->setText(QApplication::translate("SARibbonCustomizeWidget", "reset", Q_NULLPTR));  // cn:重置
        labelProportion->setText(QApplication::translate("SARibbonCustomizeWidget", "proportion:", Q_NULLPTR));  // cn:比例
    }  // retranslateUi
};

/**
 * @brief 管理SARibbonCustomizeWidget的业务逻辑
 */
class SARibbonCustomizeWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCustomizeWidget)
public:
    SARibbonCustomizeWidget::RibbonTreeShowType mShowType { SARibbonCustomizeWidget::ShowAllCategory };  ///< 显示类型
    SARibbonBar* mRibbonBar { nullptr };                   ///< 保存SARibbonMainWindow的指针
    SARibbonActionsManager* mActionMgr { nullptr };        ///< action管理器
    SARibbonActionsManagerModel* mAcionModel { nullptr };  ///< action管理器对应的model
    QStandardItemModel* mRibbonModel { nullptr };          ///< 用于很成ribbon的树
    int mCustomizeCategoryCount { 0 };                     ///< 记录自定义Category的个数
    int mCustomizePannelCount { 0 };                       ///< 记录自定义Pannel的个数
public:
    PrivateData(SARibbonCustomizeWidget* p);
    void updateModel();

    QList< SARibbonCustomizeData > mCustomizeDatasCache;    ///< 缓存记录所有的自定义动作
    QList< SARibbonCustomizeData > mCustomizeDatasApplied;  ///< 应用后的所有的自定义动作
    QList< SARibbonCustomizeData > mOldCustomizeDatas;      ///< 记录旧的自定义动作,本地文件缓存
    // 创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch_suf
    static QString makeRandomObjName(const QString& pre);

    int itemLevel(QStandardItem* item) const;

    //
    bool isCustomizeItem(QStandardItem* item) const;

    // 把item转换为category
    SARibbonCategory* itemToCategory(QStandardItem* item) const;

    // 把item转换为SARibbonPannel
    SARibbonPannel* itemToPannel(QStandardItem* item) const;

    // 获取item对应的object name
    QString itemObjectName(QStandardItem* item) const;

    // 判断是否可以自定义
    bool isItemCanCustomize(QStandardItem* item) const;

    // 从item转为action
    QAction* itemToAction(QStandardItem* item) const;
};

SARibbonCustomizeWidget::PrivateData::PrivateData(SARibbonCustomizeWidget* p)
    : q_ptr(p), mAcionModel(new SARibbonActionsManagerModel(p)), mRibbonModel(new QStandardItemModel(p))
{
}

void SARibbonCustomizeWidget::PrivateData::updateModel()
{
    if (mRibbonBar == nullptr) {
        return;
    }
    mRibbonModel->clear();
    SARibbonBar* ribbonbar               = mRibbonBar;
    QList< SARibbonCategory* > categorys = ribbonbar->categoryPages();

    for (const SARibbonCategory* c : qAsConst(categorys)) {
        if ((mShowType == SARibbonCustomizeWidget::ShowMainCategory) && c->isContextCategory()) {
            // 如果是只显示主内容，如果是上下文标签就忽略
            continue;
        }
        QStandardItem* ci = new QStandardItem();
        if (c->isContextCategory()) {
            ci->setText(QString("[%1]").arg(c->categoryName()));
        } else {
            ci->setText(c->categoryName());
        }
        if (c->isCanCustomize() && !c->isContextCategory()) {
            // 上下文标签不做显示隐藏处理
            ci->setCheckable(true);
            ci->setCheckState(ribbonbar->isCategoryVisible(c) ? Qt::Checked : Qt::Unchecked);
            ci->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 标记这个是可以自定义的
        }
        ci->setData(0, SARibbonCustomizeWidget::LevelRole);
        ci->setData(QVariant::fromValue< qintptr >(qintptr(c)), SARibbonCustomizeWidget::PointerRole);
        QList< SARibbonPannel* > pannels = c->pannelList();
        for (const SARibbonPannel* p : qAsConst(pannels)) {
            QStandardItem* pi = new QStandardItem(p->pannelName());
            pi->setData(1, SARibbonCustomizeWidget::LevelRole);
            pi->setData(QVariant::fromValue< qintptr >(qintptr(p)), SARibbonCustomizeWidget::PointerRole);
            if (p->isCanCustomize()) {
                pi->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 标记这个是可以自定义的
            }
            ci->appendRow(pi);
            const QList< SARibbonPannelItem* >& items = p->ribbonPannelItem();
            for (SARibbonPannelItem* i : qAsConst(items)) {
                if (i->action->isSeparator()) {
                    continue;
                }
                QStandardItem* ii = new QStandardItem();
                //                if (i->customWidget) {
                //                    //如果是自定义窗口
                //                    if (i->widget()->windowTitle().isEmpty() && i->widget()->windowIcon().isNull()) {
                //                        delete ii;
                //                        continue;  //如果窗口啥也没有，就跳过
                //                    }
                //                    ii->setText(i->widget()->windowTitle());
                //                    ii->setIcon(i->widget()->windowIcon());
                //                    if (SARibbonCustomizeData::isCanCustomize(i->widget())) {
                //                        ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole); //标记这个是可以自定义的
                //                    }
                //                } else {
                //                    //不是自定义，说明是action
                //                    ii->setText(i->action->text());
                //                    ii->setIcon(i->action->icon());
                //                    if (SARibbonCustomizeData::isCanCustomize(i->action)) {
                //                        ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole); //标记这个是可以自定义的
                //                    }
                //                }
                ii->setText(i->action->text());
                ii->setIcon(i->action->icon());
                if (SARibbonCustomizeData::isCanCustomize(i->action)) {
                    ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 标记这个是可以自定义的
                }
                ii->setData(2, SARibbonCustomizeWidget::LevelRole);
                ii->setData(QVariant::fromValue< qintptr >(qintptr(i)), SARibbonCustomizeWidget::PointerRole);
                pi->appendRow(ii);
            }
        }
        mRibbonModel->appendRow(ci);
    }
}

/**
 * @brief 创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch
 * @param pre 前缀
 * @return
 */
QString SARibbonCustomizeWidget::PrivateData::makeRandomObjName(const QString& pre)
{
    return (QString("%1_%2").arg(pre).arg(QDateTime::currentMSecsSinceEpoch()));
}

/**
 * @brief 获取item的level
 * @param item
 * @return
 */
int SARibbonCustomizeWidget::PrivateData::itemLevel(QStandardItem* item) const
{
    return (item->data(SARibbonCustomizeWidget::LevelRole).toInt());
}

/**
 * @brief 判断itemn为自定义的item，自定义的item都带有CustomizeRole角色
 * @param item
 * @return
 */
bool SARibbonCustomizeWidget::PrivateData::isCustomizeItem(QStandardItem* item) const
{
    if (nullptr == item) {
        return (false);
    }
    return (item->data(SARibbonCustomizeWidget::CustomizeRole).isValid());
}

/**
 * @brief 把item转换为category
 * @param item
 * @return无法转换返回nullptr
 */
SARibbonCategory* SARibbonCustomizeWidget::PrivateData::itemToCategory(QStandardItem* item) const
{
    int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

    if (level != 0) {
        return (nullptr);
    }
    qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >();

    return (reinterpret_cast< SARibbonCategory* >(p));
}

/**
 * @brief 把item转换为pannel
 * @param item
 * @return 无法转换返回nullptr
 */
SARibbonPannel* SARibbonCustomizeWidget::PrivateData::itemToPannel(QStandardItem* item) const
{
    int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

    if (level != 1) {
        return (nullptr);
    }
    qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >();

    return (reinterpret_cast< SARibbonPannel* >(p));
}

/**
 * @brief 获取item对应的objectname
 * @param item
 * @return 如果无法获取，返回一个空的QString
 */
QString SARibbonCustomizeWidget::PrivateData::itemObjectName(QStandardItem* item) const
{
    QString objName;

    if (isCustomizeItem(item)) {
        // 说明是自定义的
        objName = item->data(SARibbonCustomizeWidget::CustomizeObjNameRole).toString();
    } else {
        // 说明这个是非自定义的
        int level = itemLevel(item);
        if (0 == level) {
            SARibbonCategory* category = itemToCategory(item);
            if (category) {
                objName = category->objectName();
            }
        } else if (1 == level) {
            SARibbonPannel* pannel = itemToPannel(item);
            if (pannel) {
                objName = pannel->objectName();
            }
        }
    }
    return (objName);
}

/**
 * @brief 判断item是否可自定义
 * @param item
 * @return
 */
bool SARibbonCustomizeWidget::PrivateData::isItemCanCustomize(QStandardItem* item) const
{
    if (nullptr == item) {
        return (false);
    }
    QVariant v = item->data(SARibbonCustomizeWidget::CanCustomizeRole);

    if (v.isValid()) {
        return (v.toBool());
    }
    return (false);
}

/**
 * @brief 从item 转为action
 * @param item
 * @return
 */
QAction* SARibbonCustomizeWidget::PrivateData::itemToAction(QStandardItem* item) const
{
    if (2 != itemLevel(item)) {
        return (nullptr);
    }
    // 这里要非常注意，SARibbonCustomizeWidget::CustomizeRole为true时，说明这个是自定义的内容，
    // 这时PointerRole里存放的是action指针，不是SARibbonPannelItem
    QAction* act = nullptr;

    if (item->data(SARibbonCustomizeWidget::CustomizeRole).toBool()) {
        act = reinterpret_cast< QAction* >(item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >());
    } else {
        SARibbonPannelItem* pi = reinterpret_cast< SARibbonPannelItem* >(
                item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >());
        act = (pi->action);
    }
    return (act);
}

//===================================================
// SARibbonCustomizeWidget
//===================================================
/**
 * @brief SARibbonCustomizeWidget::SARibbonCustomizeWidget
 * @param ribbonWindow 传入需要管理的SARibbonMainWindow指针
 * @param parent 用于界面生成的parent，可以和SARibbonMainWindow一样
 * @param f 同QWidget::QWidget的第二个参数
 */
SARibbonCustomizeWidget::SARibbonCustomizeWidget(SARibbonMainWindow* ribbonWindow, QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f), d_ptr(new SARibbonCustomizeWidget::PrivateData(this)), ui(new SARibbonCustomizeWidgetUi)
{
    init(ribbonWindow->ribbonBar());
}

SARibbonCustomizeWidget::SARibbonCustomizeWidget(SARibbonBar* ribbonbar, QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f), d_ptr(new SARibbonCustomizeWidget::PrivateData(this)), ui(new SARibbonCustomizeWidgetUi)
{
    init(ribbonbar);
}

void SARibbonCustomizeWidget::init(SARibbonBar* ribbonbar)
{
    d_ptr->mRibbonBar = ribbonbar;
    ui->setupUi(this);
    ui->listViewSelect->setModel(d_ptr->mAcionModel);
    ui->treeViewResult->setModel(d_ptr->mRibbonModel);
    initConnection();
    updateModel();
}

SARibbonCustomizeWidget::~SARibbonCustomizeWidget()
{
    delete ui;
}

void SARibbonCustomizeWidget::initConnection()
{
    //    这个需要qt5.8以上支持
    //    connect(ui->comboBoxActionIndex, QOverload<int>::of(&QComboBox::currentIndexChanged)
    //        , this, &SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    connect(ui->comboBoxActionIndex,
            static_cast< void (QComboBox::*)(int) >(&QComboBox::currentIndexChanged),
            this,
            &SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    //    这个需要qt5.8以上支持
    //    connect(ui->radioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked)
    //        , this, &SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
    connect(ui->radioButtonGroup,
            static_cast< void (QButtonGroup::*)(QAbstractButton*) >(&QButtonGroup::buttonClicked),
            this,
            &SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
    connect(ui->pushButtonNewCategory, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonNewCategoryClicked);
    connect(ui->pushButtonNewPannel, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonNewPannelClicked);
    connect(ui->pushButtonRename, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonRenameClicked);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonAddClicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonDeleteClicked);
    connect(ui->listViewSelect, &QAbstractItemView::clicked, this, &SARibbonCustomizeWidget::onListViewSelectClicked);
    connect(ui->treeViewResult, &QAbstractItemView::clicked, this, &SARibbonCustomizeWidget::onTreeViewResultClicked);
    connect(ui->toolButtonUp, &QToolButton::clicked, this, &SARibbonCustomizeWidget::onToolButtonUpClicked);
    connect(ui->toolButtonDown, &QToolButton::clicked, this, &SARibbonCustomizeWidget::onToolButtonDownClicked);
    connect(d_ptr->mRibbonModel, &QStandardItemModel::itemChanged, this, &SARibbonCustomizeWidget::onItemChanged);
    connect(ui->lineEditSearchAction, &QLineEdit::textEdited, this, &SARibbonCustomizeWidget::onLineEditSearchActionTextEdited);
    connect(ui->pushButtonReset, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonResetClicked);
}

/**
 * @brief 设置action管理器
 * @param mgr
 */
void SARibbonCustomizeWidget::setupActionsManager(SARibbonActionsManager* mgr)
{
    d_ptr->mActionMgr = mgr;
    if (d_ptr->mActionMgr) {
        d_ptr->mAcionModel->uninstallActionsManager();
    }
    d_ptr->mAcionModel->setupActionsManager(mgr);
    // 更新左边复选框
    QList< int > tags = mgr->actionTags();

    ui->comboBoxActionIndex->clear();
    for (int tag : qAsConst(tags)) {
        if (mgr->tagName(tag).isEmpty())
            continue;
        ui->comboBoxActionIndex->addItem(mgr->tagName(tag), tag);
    }
}

/**
 * @brief //判断用户是否有要存储的内容，对应save动作
 * @return
 */
bool SARibbonCustomizeWidget::isApplied() const
{
    return (d_ptr->mCustomizeDatasApplied.size() > 0);
}

/**
 * @brief 判断用户是否有改动内容，对应apply动作
 * @return
 */
bool SARibbonCustomizeWidget::isCached() const
{
    return (d_ptr->mCustomizeDatasCache.size() > 0);
}

/**
 * @brief 获取model
 * @return
 */
const QStandardItemModel* SARibbonCustomizeWidget::model() const
{
    return (d_ptr->mRibbonModel);
}

/**
 * @brief 根据当前的radiobutton选项来更新model
 */
void SARibbonCustomizeWidget::updateModel()
{
    updateModel(ui->radioButtonAllCategory->isChecked() ? ShowAllCategory : ShowMainCategory);
    if (d_ptr->mRibbonBar) {
        SARibbonBar* bar = d_ptr->mRibbonBar;
        if (bar) {
            ui->comboBoxActionProportion->clear();
            if (bar->isTwoRowStyle()) {
                ui->comboBoxActionProportion->addItem(tr("large"), SARibbonPannelItem::Large);
                ui->comboBoxActionProportion->addItem(tr("small"), SARibbonPannelItem::Small);
            } else {
                ui->comboBoxActionProportion->addItem(tr("large"), SARibbonPannelItem::Large);
                ui->comboBoxActionProportion->addItem(tr("medium"), SARibbonPannelItem::Medium);
                ui->comboBoxActionProportion->addItem(tr("small"), SARibbonPannelItem::Small);
            }
        }
    }
}

/**
 * @brief 更新model
 */
void SARibbonCustomizeWidget::updateModel(RibbonTreeShowType type)
{
    d_ptr->mShowType = type;
    d_ptr->updateModel();
}

/**
 * @brief 应用所有设定
 * @return 应用成功返回true
 * @note 所有设定有一个应用成功都会返回true
 */
bool SARibbonCustomizeWidget::applys()
{
    simplify();
    if (sa_customize_datas_apply(d_ptr->mCustomizeDatasCache, d_ptr->mRibbonBar) > 0) {
        // 将临时操作存入已应用操作，并清空临时操作
        makeActionsApplied();
        clearCache();
        return true;
    } else {
        return false;
    }
}

/**
 * @brief 转换为xml
 *
 * 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 * 需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 *
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 * 因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造，如：
 * @code
 * SARibbonCustomizeDialog dlg(this);//this为SARibbonMainWindow的窗口
 * dlg.setupActionsManager(m_actMgr);
 * if (SARibbonCustomizeDialog::Accepted == dlg.exec()) {
 *    dlg.applys();
 *    QByteArray str;
 *    QXmlStreamWriter xml(&str);//QXmlStreamWriter不建议通过QString构造，遇到中文会异常
 *    xml.setAutoFormatting(true);
 *    xml.setAutoFormattingIndent(2);
 *    xml.setCodec("utf-8");//在writeStartDocument之前指定编码
 *    xml.writeStartDocument();
 *    bool isok = dlg.toXml(&xml);
 *    xml.writeEndDocument();
 *    if (isok) {
 *        QFile f("customize.xml");
 *        if (f.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate)) {
 *            QTextStream s(&f);
 *            s.setCodec("utf-8");//指定编码输出
 *            s << str;
 *            s.flush();
 *        }
 *        m_edit->append("write xml:");//m_edit的定义为：QTextEdit *m_edit;
 *        m_edit->append(str);
 *    }
 * }
 * @endcode
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 * @see sa_customize_datas_to_xml
 */
bool SARibbonCustomizeWidget::toXml(QXmlStreamWriter* xml) const
{
    QList< SARibbonCustomizeData > res = d_ptr->mOldCustomizeDatas;

    if (isApplied())
        res << d_ptr->mCustomizeDatasApplied;
    if (isCached())
        res << d_ptr->mCustomizeDatasCache;

    res = SARibbonCustomizeData::simplify(res);
    return (sa_customize_datas_to_xml(xml, res));
}

/**
 * @brief 把配置写入文件中
 * @param xmlpath
 * @return
 */
bool SARibbonCustomizeWidget::toXml(const QString& xmlpath) const
{
    QFile f(xmlpath);

    if (!f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        return (false);
    }
    QXmlStreamWriter xml(&f);

    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");                  // 在writeStartDocument之前指定编码
#endif
    xml.writeStartDocument();
    bool isOK = toXml(&xml);

    xml.writeEndDocument();
    f.close();
    return (isOK);
}

/**
 * @brief 从xml中加载QList<SARibbonCustomizeData>
 *
 * 对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
 * @param xml
 * @return
 * @note 此函数要在@ref setupActionsManager 函数之后调用
 */
void SARibbonCustomizeWidget::fromXml(QXmlStreamReader* xml)
{
    QList< SARibbonCustomizeData > cds = sa_customize_datas_from_xml(xml, d_ptr->mActionMgr);

    d_ptr->mOldCustomizeDatas = cds;
}

/**
 * @brief 从xml中加载QList<SARibbonCustomizeData>
 *
 * 对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
 * @param xmlpath
 * @note 此函数要在@ref setupActionsManager 函数之后调用
 * @note 如果程序启动后加载了自定义配置，再调用此窗口时需要调用此函数，把原来的配置加载进来，
 * 在生成新动作时会把旧动作保存，但在调用applys时不会调用此加载的动作
 */
void SARibbonCustomizeWidget::fromXml(const QString& xmlpath)
{
    QFile f(xmlpath);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    f.seek(0);
    QXmlStreamReader xml(&f);

    fromXml(&xml);
}

/**
 * @brief 应用xml配置
 *
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次，如：
 * @code
 * //只能调用一次
 * static bool has_call = false;
 * if (!has_call) {
 *     QFile f("customize.xml");
 *     if (!f.open(QIODevice::ReadWrite|QIODevice::Text)) {
 *         return;
 *     }
 *     f.seek(0);
 *     QXmlStreamReader xml(&f);
 *     has_call = SARibbonCustomizeWidget::fromXml(&xml, this, m_actMgr);
 * }
 * @endcode
 * @param xml
 * @param bar SARibbonBar
 * @return 所有设定有一个应用成功都会返回true
 * @see sa_customize_datas_from_xml sa_customize_datas_apply sa_apply_customize_from_xml_file
 */
bool SARibbonCustomizeWidget::fromXml(QXmlStreamReader* xml, SARibbonBar* bar, SARibbonActionsManager* mgr)
{
    // 先找到sa-ribbon-customize标签
    QList< SARibbonCustomizeData > cds = sa_customize_datas_from_xml(xml, mgr);

    return (sa_customize_datas_apply(cds, bar) > 0);
}

/**
 * @brief 清除已应用的动作
 */
void SARibbonCustomizeWidget::makeActionsApplied()
{
    d_ptr->mCustomizeDatasApplied << d_ptr->mCustomizeDatasCache;
}

/**
 * @brief 清除applied的动作，cancel操作后需要清空已应用的动作
 */
void SARibbonCustomizeWidget::clearApplied()
{
    d_ptr->mCustomizeDatasApplied.clear();
}

/**
 * @brief 清除缓存动作
 *
 * 在执行applys函数后，如果要继续调用，应该clear，否则会导致异常
 */
void SARibbonCustomizeWidget::clearCache()
{
    d_ptr->mCustomizeDatasCache.clear();
}

/**
 * @brief 清除所有动作，不包含本地读取的数据
 */
void SARibbonCustomizeWidget::clear()
{
    clearApplied();
    clearCache();
}

/**
 * @brief 精简
 */
void SARibbonCustomizeWidget::simplify()
{
    d_ptr->mCustomizeDatasCache = SARibbonCustomizeData::simplify(d_ptr->mCustomizeDatasCache);
}

/**
 * @brief 获取当前界面选中的行属性
 * @return
 */
SARibbonPannelItem::RowProportion SARibbonCustomizeWidget::selectedRowProportion() const
{
    return (static_cast< SARibbonPannelItem::RowProportion >(ui->comboBoxActionProportion->currentData().toInt()));
}

/**
 * @brief 获取listview中选中的action
 * @return 如果没有选中action，返回nullptr
 * @note 如果要获取treeview选中的action，使用@ref itemToAction 函数
 */
QAction* SARibbonCustomizeWidget::selectedAction() const
{
    QItemSelectionModel* m = ui->listViewSelect->selectionModel();

    if ((nullptr == m) || !m->hasSelection()) {
        return (nullptr);
    }
    QModelIndex i = m->currentIndex();

    return (d_ptr->mAcionModel->indexToAction(i));
}

/**
 * @brief 把item转换为action
 * @param item
 * @return 如果没有action可转换，返回nullptr
 */
QAction* SARibbonCustomizeWidget::itemToAction(QStandardItem* item) const
{
    return (d_ptr->itemToAction(item));
}

/**
 * @brief 获取ribbon tree选中的item
 * @return
 */
QStandardItem* SARibbonCustomizeWidget::selectedItem() const
{
    QItemSelectionModel* m = ui->treeViewResult->selectionModel();

    if ((nullptr == m) || !m->hasSelection()) {
        return (nullptr);
    }
    QModelIndex i = m->currentIndex();

    return (d_ptr->mRibbonModel->itemFromIndex(i));
}

/**
 * @brief 获取选中的ribbon tree 的level
 * @return -1为选中异常，0代表选中了category 1代表选中了pannel 2代表选中了action
 */
int SARibbonCustomizeWidget::selectedRibbonLevel() const
{
    QStandardItem* item = selectedItem();

    if (item) {
        return (itemLevel(item));
    }
    return (-1);
}

/**
 * @brief 获取StandardItem 的level
 * @param item
 * @return
 */
int SARibbonCustomizeWidget::itemLevel(QStandardItem* item) const
{
    return (d_ptr->itemLevel(item));
}

/**
 * @brief 设置某个item被选中
 * @param item
 */
void SARibbonCustomizeWidget::setSelectItem(QStandardItem* item, bool ensureVisible)
{
    QItemSelectionModel* m = ui->treeViewResult->selectionModel();

    if (nullptr == m) {
        return;
    }
    if (m) {
        m->clearSelection();
        m->setCurrentIndex(item->index(), QItemSelectionModel::SelectCurrent);
    }
    if (ensureVisible) {
        ui->treeViewResult->scrollTo(item->index());
    }
}

/**
 * @brief 判断itemn能否改动，可以改动返回true
 * @param item
 * @return
 */
bool SARibbonCustomizeWidget::isItemCanCustomize(QStandardItem* item) const
{
    return (d_ptr->isItemCanCustomize(item));
}

bool SARibbonCustomizeWidget::isSelectedItemCanCustomize() const
{
    return (isItemCanCustomize(selectedItem()));
}

/**
 * @brief 判断itemn能否改动，可以改动返回true
 * @param item
 * @return
 */
bool SARibbonCustomizeWidget::isCustomizeItem(QStandardItem* item) const
{
    return (d_ptr->isCustomizeItem(item));
}

bool SARibbonCustomizeWidget::isSelectedItemIsCustomize() const
{
    return (isCustomizeItem(selectedItem()));
}

void SARibbonCustomizeWidget::removeItem(QStandardItem* item)
{
    if (item->parent()) {
        item->parent()->removeRow(item->row());
    } else {
        d_ptr->mRibbonModel->removeRow(item->row());
    }
}

void SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged(int index)
{
    int tag = ui->comboBoxActionIndex->itemData(index).toInt();

    d_ptr->mAcionModel->setFilter(tag);
}

void SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked(QAbstractButton* b)
{
    updateModel(b == ui->radioButtonAllCategory ? ShowAllCategory : ShowMainCategory);
}

void SARibbonCustomizeWidget::onPushButtonNewCategoryClicked()
{
    int row                = d_ptr->mRibbonModel->rowCount();
    QItemSelectionModel* m = ui->treeViewResult->selectionModel();

    if (m && m->hasSelection()) {
        QModelIndex i = m->currentIndex();
        while (i.parent().isValid()) {
            i = i.parent();
        }
        // 获取选中的最顶层item
        row = i.row() + 1;
    }
    QStandardItem* ni = new QStandardItem(tr("new category[customize]%1").arg(++(d_ptr->mCustomizeCategoryCount)));

    ni->setData(0, SARibbonCustomizeWidget::LevelRole);
    d_ptr->mRibbonModel->insertRow(row, ni);
    // 设置新增的为选中
    setSelectItem(ni);
    // 把动作插入动作列表中
    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddCategoryCustomizeData(ni->text(),
                                                                                  ni->row(),
                                                                                  SARibbonCustomizeWidget::PrivateData::makeRandomObjName(
                                                                                          "category"));

    d_ptr->mCustomizeDatasCache.append(d);
    ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 有CustomizeRole，必有CanCustomizeRole
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    ni->setData(d.categoryObjNameValue, SARibbonCustomizeWidget::CustomizeObjNameRole);
}

void SARibbonCustomizeWidget::onPushButtonNewPannelClicked()
{
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    int level = selectedRibbonLevel();

    QStandardItem* ni = new QStandardItem(tr("new pannel[customize]%1").arg(++(d_ptr->mCustomizePannelCount)));

    ni->setData(1, SARibbonCustomizeWidget::LevelRole);

    if (0 == level) {
        // 说明是category,插入到最后
        item->appendRow(ni);
    } else if (1 == level) {
        // 说明选择的是pannel，插入到此pannel之后
        QStandardItem* categoryItem = item->parent();
        if (nullptr == categoryItem) {
            return;
        }
        categoryItem->insertRow(item->row() + 1, ni);
    } else {
        // 不符合就删除退出
        delete ni;
        ni = nullptr;
        return;
    }
    // 查找category的object name
    QStandardItem* categoryItem = ni->parent();
    QString categoryObjName     = "";

    categoryObjName         = d_ptr->itemObjectName(categoryItem);
    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddPannelCustomizeData(ni->text(),
                                                                                ni->row(),
                                                                                categoryObjName,
                                                                                SARibbonCustomizeWidget::PrivateData::makeRandomObjName(
                                                                                        "pannel"));

    d_ptr->mCustomizeDatasCache.append(d);
    ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 有CustomizeRole，必有CanCustomizeRole
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    ni->setData(d.pannelObjNameValue, SARibbonCustomizeWidget::CustomizeObjNameRole);
    setSelectItem(ni);
}

void SARibbonCustomizeWidget::onPushButtonRenameClicked()
{
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    bool ok;
    QString text = "";

    text = QInputDialog::getText(this, tr("rename"), tr("name:"), QLineEdit::Normal, item->text(), &ok);

    if (!ok || text.isEmpty()) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 改Category名
        QString cateObjName     = d_ptr->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRenameCategoryCustomizeData(text, cateObjName);
        d_ptr->mCustomizeDatasCache.append(d);
    } else if (1 == level) {
        QString cateObjName   = d_ptr->itemObjectName(item->parent());
        QString pannelObjName = d_ptr->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRenamePannelCustomizeData(text, cateObjName, pannelObjName);
        d_ptr->mCustomizeDatasCache.append(d);
    } else {
        // action 不允许改名
        return;
    }
    item->setText(text);
}

void SARibbonCustomizeWidget::onPushButtonAddClicked()
{
    QAction* act        = selectedAction();
    QStandardItem* item = selectedItem();

    if ((nullptr == act) || (nullptr == item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 选中category不进行操作
        return;
    } else if (2 == level) {
        // 选中action，添加到这个action之后,把item设置为pannel
        item = item->parent();
    }
    QString pannelObjName   = d_ptr->itemObjectName(item);
    QString categoryObjName = d_ptr->itemObjectName(item->parent());
    QString key             = d_ptr->mActionMgr->key(act);

    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddActionCustomizeData(key,
                                                                                d_ptr->mActionMgr,
                                                                                selectedRowProportion(),
                                                                                categoryObjName,
                                                                                pannelObjName);

    d_ptr->mCustomizeDatasCache.append(d);

    QStandardItem* actItem = new QStandardItem(act->icon(), act->text());

    actItem->setData(2, SARibbonCustomizeWidget::LevelRole);
    actItem->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 有CustomizeRole，必有CanCustomizeRole
    actItem->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    actItem->setData(act->objectName(), SARibbonCustomizeWidget::CustomizeObjNameRole);
    actItem->setData(qintptr(act), SARibbonCustomizeWidget::PointerRole);  // 把action指针传入
    item->appendRow(actItem);
}

void SARibbonCustomizeWidget::onPushButtonDeleteClicked()
{
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    if (!isItemCanCustomize(item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 删除category
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemoveCategoryCustomizeData(d_ptr->itemObjectName(item));
        d_ptr->mCustomizeDatasCache.append(d);
    } else if (1 == level) {
        // 删除pannel
        QString catObjName      = d_ptr->itemObjectName(item->parent());
        QString pannelObjName   = d_ptr->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemovePannelCustomizeData(catObjName, pannelObjName);
        d_ptr->mCustomizeDatasCache.append(d);
    } else if (2 == level) {
        // 删除Action
        QString catObjName    = d_ptr->itemObjectName(item->parent()->parent());
        QString pannelObjName = d_ptr->itemObjectName(item->parent());
        QAction* act          = itemToAction(item);
        QString key           = d_ptr->mActionMgr->key(act);
        if (key.isEmpty() || catObjName.isEmpty() || pannelObjName.isEmpty()) {
            return;
        }

        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemoveActionCustomizeData(catObjName, pannelObjName, key, d_ptr->mActionMgr);
        d_ptr->mCustomizeDatasCache.append(d);
    }
    // 执行删除操作
    removeItem(item);
    // 删除后重新识别
    ui->pushButtonAdd->setEnabled(selectedAction() && isSelectedItemIsCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemIsCustomize());
}

void SARibbonCustomizeWidget::onListViewSelectClicked(const QModelIndex& index)
{
    // 每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    // 点击了listview，判断treeview的状态
    Q_UNUSED(index);
    ui->pushButtonAdd->setEnabled(isSelectedItemCanCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemCanCustomize());
}

void SARibbonCustomizeWidget::onTreeViewResultClicked(const QModelIndex& index)
{
    Q_UNUSED(index);
    // 每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    int level = itemLevel(item);

    ui->pushButtonAdd->setEnabled(selectedAction() && (level > 0) && isItemCanCustomize(item));
    ui->pushButtonDelete->setEnabled(isItemCanCustomize(item));  // 有CustomizeRole，必有CanCustomizeRole
    ui->pushButtonRename->setEnabled(level != 2 || isItemCanCustomize(item));  // QAction 不能改名 ， 有CustomizeRole，必有CanCustomizeRole
}

void SARibbonCustomizeWidget::onToolButtonUpClicked()
{
    QStandardItem* item = selectedItem();

    if ((nullptr == item) || (0 == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 移动category
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(d_ptr->itemObjectName(item), -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = d_ptr->mRibbonModel->takeItem(r);
        d_ptr->mRibbonModel->removeRow(r);
        d_ptr->mRibbonModel->insertRow(r - 1, item);
    } else if (1 == level) {
        QStandardItem* paritem = item->parent();
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePannelOrderCustomizeData(d_ptr->itemObjectName(paritem),
                                                                                            d_ptr->itemObjectName(item),
                                                                                            -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r - 1, item);
    } else if (2 == level) {
        QStandardItem* pannelItem   = item->parent();
        QStandardItem* categoryItem = pannelItem->parent();
        QAction* act                = itemToAction(item);
        if (!act) {
            return;
        }
        QString key = d_ptr->mActionMgr->key(act);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeActionOrderCustomizeData(d_ptr->itemObjectName(categoryItem),
                                                                                            d_ptr->itemObjectName(pannelItem),
                                                                                            key,
                                                                                            d_ptr->mActionMgr,
                                                                                            -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = pannelItem->takeChild(r);
        pannelItem->removeRow(r);
        pannelItem->insertRow(r - 1, item);
    }

    // 保持焦点，方便连续操作
    setSelectItem(item);
    onTreeViewResultClicked(item->index());
}

void SARibbonCustomizeWidget::onToolButtonDownClicked()
{
    QStandardItem* item = selectedItem();

    if (item == nullptr) {
        return;
    }
    int count = 0;

    if (item->parent()) {
        count = item->parent()->rowCount();
    } else {
        count = d_ptr->mRibbonModel->rowCount();
    }
    if ((nullptr == item) || ((count - 1) == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 移动category
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(d_ptr->itemObjectName(item), 1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = d_ptr->mRibbonModel->takeItem(item->row());
        d_ptr->mRibbonModel->removeRow(r);
        d_ptr->mRibbonModel->insertRow(r + 1, item);
    } else if (1 == level) {
        QStandardItem* paritem = item->parent();
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePannelOrderCustomizeData(d_ptr->itemObjectName(paritem),
                                                                                            d_ptr->itemObjectName(item),
                                                                                            1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r + 1, item);
    } else if (2 == level) {
        QStandardItem* pannelItem   = item->parent();
        QStandardItem* categoryItem = pannelItem->parent();
        QAction* act                = itemToAction(item);
        if (!act) {
            return;
        }
        QString key = d_ptr->mActionMgr->key(act);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeActionOrderCustomizeData(d_ptr->itemObjectName(categoryItem),
                                                                                            d_ptr->itemObjectName(pannelItem),
                                                                                            key,
                                                                                            d_ptr->mActionMgr,
                                                                                            -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = pannelItem->takeChild(r);
        pannelItem->removeRow(r);
        pannelItem->insertRow(r + 1, item);
    }

    // 保持焦点，方便连续操作
    setSelectItem(item);
    onTreeViewResultClicked(item->index());
}

void SARibbonCustomizeWidget::onItemChanged(QStandardItem* item)
{
    if (item == nullptr) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        if (item->isCheckable()) {
            QString objname = d_ptr->itemObjectName(item);
            SARibbonCustomizeData d = SARibbonCustomizeData::makeVisibleCategoryCustomizeData(objname, item->checkState() == Qt::Checked);
            d_ptr->mCustomizeDatasCache.append(d);
        }
    }
}

void SARibbonCustomizeWidget::onLineEditSearchActionTextEdited(const QString& text)
{
    d_ptr->mAcionModel->search(text);
}

void SARibbonCustomizeWidget::onPushButtonResetClicked()
{
    int btn = QMessageBox::warning(this,
                                   tr("Warning"),
                                   tr("Are you sure reset all customize setting?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);

    if (btn == QMessageBox::Yes) {
        clear();
    }
}

/*** End of inlined file: SARibbonCustomizeWidget.cpp ***/

/*** Start of inlined file: SARibbonCustomizeDialog.cpp ***/
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpacerItem>

/**
 * @brief The SARibbonCustomizeDialogUi class
 */
class SARibbonCustomizeDialogUi
{
public:
    SARibbonCustomizeWidget* customWidget;
    QVBoxLayout* verticalLayoutMain;
    QHBoxLayout* horizontalLayoutButtonGroup;
    QPushButton* pushButtonCancel;
    QPushButton* pushButtonOk;
    QSpacerItem* spacerItemleft;
    void setupUi(SARibbonMainWindow* ribbonWindow, QWidget* customizeDialog)
    {
        if (customizeDialog->objectName().isEmpty()) {
            customizeDialog->setObjectName(QStringLiteral("SARibbonCustomizeDialog"));
        }
        customizeDialog->resize(800, 600);
        verticalLayoutMain = new QVBoxLayout(customizeDialog);
        verticalLayoutMain->setObjectName(QStringLiteral("verticalLayoutMain"));

        customWidget = new SARibbonCustomizeWidget(ribbonWindow, customizeDialog);
        customWidget->setObjectName(QStringLiteral("customWidget"));
        verticalLayoutMain->addWidget(customWidget);

        horizontalLayoutButtonGroup = new QHBoxLayout();
        horizontalLayoutButtonGroup->setObjectName(QStringLiteral("horizontalLayoutButtonGroup"));

        spacerItemleft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        horizontalLayoutButtonGroup->addItem(spacerItemleft);

        pushButtonCancel = new QPushButton(customizeDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        horizontalLayoutButtonGroup->addWidget(pushButtonCancel);

        pushButtonOk = new QPushButton(customizeDialog);
        pushButtonOk->setObjectName(QStringLiteral("pushButtonCancel"));
        horizontalLayoutButtonGroup->addWidget(pushButtonOk);
        verticalLayoutMain->addItem(horizontalLayoutButtonGroup);
        retranslateUi(customizeDialog);
    }

    void retranslateUi(QWidget* customizeDialog)
    {
        customizeDialog->setWindowTitle(QApplication::translate("SARibbonCustomizeDialog", "Customize Dialog", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("SARibbonCustomizeDialog", "Cancel", Q_NULLPTR));
        pushButtonOk->setText(QApplication::translate("SARibbonCustomizeDialog", "OK", Q_NULLPTR));
    }
};

////////////////////////////////////////////////////////////////////

SARibbonCustomizeDialog::SARibbonCustomizeDialog(SARibbonMainWindow* ribbonWindow, QWidget* p, Qt::WindowFlags f)
    : QDialog(p, f), ui(new SARibbonCustomizeDialogUi)
{
    ui->setupUi(ribbonWindow, this);
    initConnection();
}

/**
 * @brief 设置action管理器
 *
 * 等同@ref SARibbonCustomizeWidget::setupActionsManager
 * @param mgr
 */
void SARibbonCustomizeDialog::setupActionsManager(SARibbonActionsManager* mgr)
{
    ui->customWidget->setupActionsManager(mgr);
}

/**
 * @brief //判断用户是否有要存储的内容，对应save动作
 * @return
 */
bool SARibbonCustomizeDialog::isApplied() const
{
    return ui->customWidget->isApplied();
}

/**
 * @brief 判断用户是否有改动内容，对应apply动作
 * @return
 */
bool SARibbonCustomizeDialog::isCached() const
{
    return ui->customWidget->isCached();
}

void SARibbonCustomizeDialog::initConnection()
{
    connect(ui->pushButtonOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::reject);
}

/**
 * @brief 等同SARibbonCustomizeWidget::applys
 *
 * @ref SARibbonCustomizeWidget::applys
 * @return
 */
bool SARibbonCustomizeDialog::applys()
{
    return ui->customWidget->applys();
}

/**
 * @brief 清除所有动作
 *
 * @ref SARibbonCustomizeWidget::clear
 */
void SARibbonCustomizeDialog::clear()
{
    ui->customWidget->clear();
}

/**
 * @brief 转换为xml
 *
 * @ref SARibbonCustomizeWidget::toXml
 * @param xml
 * @return
 */
bool SARibbonCustomizeDialog::toXml(QXmlStreamWriter* xml) const
{
    return (ui->customWidget->toXml(xml));
}

/**
 * @brief 等同SARibbonCustomizeWidget::toXml
 * @ref SARibbonCustomizeWidget::toXml
 * @param xmlpath
 * @return
 */
bool SARibbonCustomizeDialog::toXml(const QString& xmlpath) const
{
    return (ui->customWidget->toXml(xmlpath));
}

/**
 * @brief 等同SARibbonCustomizeWidget::fromXml
 * @param xml
 */
void SARibbonCustomizeDialog::fromXml(QXmlStreamReader* xml)
{
    ui->customWidget->fromXml(xml);
}

/**
 * @brief 等同SARibbonCustomizeWidget::fromXml
 * @param xmlpath
 */
void SARibbonCustomizeDialog::fromXml(const QString& xmlpath)
{
    ui->customWidget->fromXml(xmlpath);
}

/**
 * @brief 返回SARibbonCustomizeWidget窗口指针
 *
 * 通过SARibbonCustomizeWidget窗口可以操作更多的内容
 *
 * @return SARibbonCustomizeWidget指针，参考@ref SARibbonCustomizeWidget
 */
SARibbonCustomizeWidget* SARibbonCustomizeDialog::customizeWidget() const
{
    return (ui->customWidget);
}

/*** End of inlined file: SARibbonCustomizeDialog.cpp ***/

/*** Start of inlined file: SARibbonMainWindow.cpp ***/
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QWindowStateChangeEvent>
#include <QScreen>

#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include <QWKWidgets/widgetwindowagent.h>

#else

#endif

/**
 * @brief The SARibbonMainWindowPrivate class
 */
class SARibbonMainWindow::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonMainWindow)
public:
    PrivateData(SARibbonMainWindow* p);
    void installFrameless(SARibbonMainWindow* p);

public:
    SARibbonMainWindow::RibbonTheme mCurrentRibbonTheme { SARibbonMainWindow::RibbonThemeOffice2021Blue };
    SARibbonSystemButtonBar* mWindowButtonGroup { nullptr };
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    QWK::WidgetWindowAgent* mFramelessHelper { nullptr };
#else
    SAFramelessHelper* mFramelessHelper { nullptr };
#endif
};

SARibbonMainWindow::PrivateData::PrivateData(SARibbonMainWindow* p) : q_ptr(p)
{
}

void SARibbonMainWindow::PrivateData::installFrameless(SARibbonMainWindow* p)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    mFramelessHelper = new QWK::WidgetWindowAgent(p);
    mFramelessHelper->setup(p);
#else
    mFramelessHelper = new SAFramelessHelper(p);
#endif
}

//===================================================
// SARibbonMainWindow
//===================================================
SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, bool useRibbon, const Qt::WindowFlags flags)
    : QMainWindow(parent, flags), d_ptr(new SARibbonMainWindow::PrivateData(this))
{
    connect(qApp, &QApplication::primaryScreenChanged, this, &SARibbonMainWindow::onPrimaryScreenChanged);
    if (useRibbon) {
        d_ptr->installFrameless(this);
        setRibbonBar(createRibbonBar());
        setRibbonTheme(ribbonTheme());
    }
}

SARibbonMainWindow::~SARibbonMainWindow()
{
}

/**
 * @brief 返回ribbonbar，如果不是使用ribbon模式，返回nullptr
 * @return
 */
SARibbonBar* SARibbonMainWindow::ribbonBar() const
{
    return qobject_cast< SARibbonBar* >(menuWidget());
}

/**
 * @brief 设置ribbonbar
 * @param bar
 */
void SARibbonMainWindow::setRibbonBar(SARibbonBar* bar)
{
    QWidget* old = QMainWindow::menuWidget();
    if (old) {
        // 如果之前已经设置了menubar，要把之前的删除
        old->deleteLater();
    }
    QMainWindow::setMenuWidget(bar);
    const int th = bar->titleBarHeight();
    // 设置window按钮
    if (nullptr == d_ptr->mWindowButtonGroup) {
        d_ptr->mWindowButtonGroup = RibbonSubElementFactory->createWindowButtonGroup(this);
    }
    SARibbonSystemButtonBar* wg = d_ptr->mWindowButtonGroup;
    wg->setWindowStates(windowState());
    wg->setWindowTitleHeight(th);
    wg->show();
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    auto helper = d_ptr->mFramelessHelper;
    helper->setTitleBar(bar);
    // 以下这些窗口，需要允许点击
    helper->setHitTestVisible(wg);                          // IMPORTANT!
    helper->setHitTestVisible(bar->ribbonTabBar());         // IMPORTANT!
    helper->setHitTestVisible(bar->rightButtonGroup());     // IMPORTANT!
    helper->setHitTestVisible(bar->applicationButton());    // IMPORTANT!
    helper->setHitTestVisible(bar->quickAccessBar());       // IMPORTANT!
    helper->setHitTestVisible(bar->ribbonStackedWidget());  // IMPORTANT!
    if (wg->closeButton()) {
        helper->setSystemButton(QWK::WindowAgentBase::Close, wg->closeButton());
    }
    if (wg->minimizeButton()) {
        helper->setSystemButton(QWK::WindowAgentBase::Minimize, wg->minimizeButton());
    }
    if (wg->maximizeButton()) {
        helper->setSystemButton(QWK::WindowAgentBase::Maximize, wg->maximizeButton());
    }
#else
    bar->installEventFilter(this);
    // 设置窗体的标题栏高度
    d_ptr->mFramelessHelper->setTitleHeight(th);
    d_ptr->mFramelessHelper->setRubberBandOnResize(false);
#endif
}

#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER

/**
 * @brief 如果ribbon中有自定义的窗口在标题栏等非点击区域加入后，想能点击，需要调用此接口告知可点击
 * @param w
 * @param visible
 */
void SARibbonMainWindow::setFramelessHitTestVisible(const QWidget* w, bool visible)
{
    auto helper = d_ptr->mFramelessHelper;
    helper->setHitTestVisible(w, visible);
}
#else

SAFramelessHelper* SARibbonMainWindow::framelessHelper()
{
    return (d_ptr->mFramelessHelper);
}

#endif

bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#else
    // 这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
    // 由于ribbonbar会遮挡掉frameless的区域，导致frameless无法捕获这些消息
    // 因此必须ribbonBar()->installEventFilter(this);
    // 20240101发现installEventFilter后，SARibbonMainWindow没有执行这个回调
    if (obj == ribbonBar()) {
        switch (e->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Leave:
        case QEvent::HoverMove:
        case QEvent::MouseButtonDblClick:
            QApplication::sendEvent(this, e);

        default:
            break;
        }
    }
#endif
    return (QMainWindow::eventFilter(obj, e));
}

/**
 * @brief 获取系统按钮组，可以在此基础上添加其他按钮
 * @return
 */
SARibbonSystemButtonBar* SARibbonMainWindow::windowButtonBar() const
{
    return d_ptr->mWindowButtonGroup;
}

/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 */
void SARibbonMainWindow::updateWindowFlag(Qt::WindowFlags flags)
{
    if (SARibbonSystemButtonBar* g = d_ptr->mWindowButtonGroup) {
        g->updateWindowFlag(flags);
    }
}

/**
 * @brief SARibbonMainWindow::setRibbonTheme
 *
 * 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 * @param theme
 */
void SARibbonMainWindow::setRibbonTheme(SARibbonMainWindow::RibbonTheme theme)
{
    sa_set_ribbon_theme(this, theme);
    d_ptr->mCurrentRibbonTheme = theme;
    if (SARibbonBar* bar = ribbonBar()) {
        auto theme = ribbonTheme();
        // 尺寸修正
        switch (theme) {
        case RibbonThemeWindows7:
        case RibbonThemeOffice2013:
        case RibbonThemeOffice2016Blue:
        case RibbonThemeDark:
        case RibbonThemeDark2: {
            //! 在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
            //! office2013.qss的margin信息如下设置
            //! margin-top: 0px;
            //! margin-right: 0px;
            //! margin-left: 5px;
            //! margin-bottom: 0px;
            SARibbonTabBar* tab = bar->ribbonTabBar();
            if (!tab) {
                break;
            }
            tab->setTabMargin(QMargins(5, 0, 0, 0));
        } break;
        case RibbonThemeOffice2021Blue: {
            SARibbonTabBar* tab = bar->ribbonTabBar();
            if (!tab) {
                break;
            }
            //! 在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
            //! office2021.qss的margin信息如下设置
            //! margin-top: 0px;
            //! margin-right: 5px;
            //! margin-left: 5px;
            //! margin-bottom: 0px;
            tab->setTabMargin(QMargins(5, 0, 5, 0));
        }
        default:
            break;
        }
        // 上下文标签颜色设置,以及基线颜色设置
        switch (theme) {
        case RibbonThemeWindows7:
        case RibbonThemeOffice2013:
        case RibbonThemeDark:
            bar->setContextCategoryColorList(QList< QColor >());  //< 设置空颜色列表会重置为默认色系
            break;
        case RibbonThemeOffice2016Blue:
            bar->setContextCategoryColorList(QList< QColor >() << QColor(18, 64, 120));  //< 设置空颜色列表会重置为默认色系
            break;
        case RibbonThemeOffice2021Blue:
            bar->setContextCategoryColorList(QList< QColor >() << QColor(0, 0, 0, 0));  //< 设置空颜色列表会重置为默认色系
            break;
        default:
            break;
        }
        // 基线颜色设置
        if (RibbonThemeOffice2013 == theme) {
            bar->setTabBarBaseLineColor(QColor(186, 201, 219));
        } else {
            bar->setTabBarBaseLineColor(QColor());
        }
    }
}

SARibbonMainWindow::RibbonTheme SARibbonMainWindow::ribbonTheme() const
{
    return (d_ptr->mCurrentRibbonTheme);
}

bool SARibbonMainWindow::isUseRibbon() const
{
    return (nullptr != ribbonBar());
}

/**
 * @brief 创建ribbonbar的工厂函数
 * @return
 */
SARibbonBar* SARibbonMainWindow::createRibbonBar()
{
    SARibbonBar* bar = new SARibbonBar(this);
    bar->setContentsMargins(3, 0, 3, 0);
    return bar;
}

void SARibbonMainWindow::resizeEvent(QResizeEvent* e)
{
    QMainWindow::resizeEvent(e);
    SARibbonBar* bar            = ribbonBar();
    SARibbonSystemButtonBar* wg = d_ptr->mWindowButtonGroup;

    if (wg) {
        if (bar) {
            const int th = bar->titleBarHeight();
            if (th != wg->height()) {
                wg->setWindowTitleHeight(th);
            }
        }
        QSize wgSizeHint = wg->sizeHint();
        wg->setGeometry(frameGeometry().width() - wgSizeHint.width(), 0, wgSizeHint.width(), wgSizeHint.height());
    }
    if (bar) {
        if (wg) {
            bar->setWindowButtonGroupSize(wg->size());
        }
        if (bar->size().width() != (this->size().width())) {
            bar->setFixedWidth(this->size().width());
        }
    }
}

void SARibbonMainWindow::changeEvent(QEvent* e)
{
    if (e) {
        switch (e->type()) {
        case QEvent::WindowStateChange: {
            if (SARibbonSystemButtonBar* wg = d_ptr->mWindowButtonGroup) {
                wg->setWindowStates(windowState());
            }
        } break;

        default:
            break;
        }
    }
    QMainWindow::changeEvent(e);
}

/**
 * @brief 主屏幕切换触发的信号
 * @param screen
 */
void SARibbonMainWindow::onPrimaryScreenChanged(QScreen* screen)
{
    Q_UNUSED(screen);
    // 主屏幕切换后，从新计算所有尺寸
    if (SARibbonBar* bar = ribbonBar()) {
        qDebug() << "Primary Screen Changed";
        bar->updateRibbonGeometry();
    }
}

void sa_set_ribbon_theme(QWidget* w, SARibbonMainWindow::RibbonTheme theme)
{
    QFile file;
    switch (theme) {
    case SARibbonMainWindow::RibbonThemeWindows7:
        file.setFileName(":/theme/resource/theme-win7.qss");
        break;
    case SARibbonMainWindow::RibbonThemeOffice2013:
        file.setFileName(":/theme/resource/theme-office2013.qss");
        break;
    case SARibbonMainWindow::RibbonThemeOffice2016Blue:
        file.setFileName(":/theme/resource/theme-office2016-blue.qss");
        break;
    case SARibbonMainWindow::RibbonThemeOffice2021Blue:
        file.setFileName(":/theme/resource/theme-office2021-blue.qss");
        break;
    case SARibbonMainWindow::RibbonThemeDark:
        file.setFileName(":/theme/resource/theme-dark.qss");
        break;
    case SARibbonMainWindow::RibbonThemeDark2:
        file.setFileName(":/theme/resource/theme-dark2.qss");
        break;
    default:
        file.setFileName(":/theme/resource/theme-office2013.qss");
        break;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    // 有反馈用qstring接住文件内容，再设置进去才能生效（qt5.7版本）
    QString qss = QString::fromUtf8(file.readAll());
    w->setStyleSheet(qss);
}

/*** End of inlined file: SARibbonMainWindow.cpp ***/

#ifdef _MSC_VER
#pragma warning(pop)
#pragma pop_macro("_CRT_SECURE_NO_WARNINGS")
#endif
