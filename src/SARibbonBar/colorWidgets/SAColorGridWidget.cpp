#include "SAColorGridWidget.h"
#include "SAColorToolButton.h"
#include <QGridLayout>
#include <cmath>
#include <QButtonGroup>

/**
 * \if ENGLISH
 * @brief Private data class for SAColorGridWidget
 * \endif
 *
 * \if CHINESE
 * @brief SAColorGridWidget的私有数据类
 * \endif
 */
class SAColorGridWidget::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorGridWidget)
public:
    /**
     * \if ENGLISH
     * @brief Constructor for PrivateData
     * @param p Parent SAColorGridWidget
     * \endif
     *
     * \if CHINESE
     * @brief PrivateData构造函数
     * @param p 父SAColorGridWidget
     * \endif
     */
    PrivateData(SAColorGridWidget* p);
    
    /**
     * \if ENGLISH
     * @brief Get ColorToolButton by index
     * @param index Index of the button
     * @return SAColorToolButton at the specified index
     * \endif
     *
     * \if CHINESE
     * @brief 根据索引获取ColorToolButton
     * @param index 按钮的索引
     * @return 指定索引处的SAColorToolButton
     * \endif
     */
    SAColorToolButton* getColorToolButtonAt(int index);
    
    /**
     * \if ENGLISH
     * @brief Get ColorToolButton by row and column
     * @param r Row index
     * @param c Column index
     * @return SAColorToolButton at the specified position
     * \endif
     *
     * \if CHINESE
     * @brief 根据行列获取ColorToolButton
     * @param r 行索引
     * @param c 列索引
     * @return 指定位置的SAColorToolButton
     * \endif
     */
    SAColorToolButton* getColorToolButtonAt(int r, int c);
    
    /**
     * \if ENGLISH
     * @brief Get the currently checked button
     * @return Currently checked SAColorToolButton
     * \endif
     *
     * \if CHINESE
     * @brief 获取当前选中的按钮
     * @return 当前选中的SAColorToolButton
     * \endif
     */
    SAColorToolButton* getCheckedButton() const;
    
    /**
     * \if ENGLISH
     * @brief Update grid layout based on mColors
     * @param isRemoveSpacer Whether to remove spacer items
     * \endif
     *
     * \if CHINESE
     * @brief 根据mColors更新布局
     * @param isRemoveSpacer 是否移除间隔项
     * \endif
     */
    void updateGridColor(bool isRemoveSpacer = false);
    
    /**
     * \if ENGLISH
     * @brief Update color button sizes
     * \endif
     *
     * \if CHINESE
     * @brief 更新颜色按钮大小
     * \endif
     */
    void updateGridColorSize();
    
    /**
     * \if ENGLISH
     * @brief Update color button checkable state
     * \endif
     *
     * \if CHINESE
     * @brief 更新颜色按钮的可选中状态
     * \endif
     */
    void updateGridColorCheckable();
    
    /**
     * \if ENGLISH
     * @brief Iterate through all color buttons
     * @param fn Function to apply to each button
     * \endif
     *
     * \if CHINESE
     * @brief 遍历所有颜色按钮
     * @param fn 应用于每个按钮的函数
     * \endif
     */
    void iterationColorBtns(SAColorGridWidget::FunColorBtn fn);
    
    /**
     * \if ENGLISH
     * @brief Remove widget at specified position
     * @param r Row index
     * @param c Column index
     * \endif
     *
     * \if CHINESE
     * @brief 删除指定位置的部件
     * @param r 行索引
     * @param c 列索引
     * \endif
     */
    void removeAt(int r, int c);
    
    /**
     * \if ENGLISH
     * @brief Set color at specified position
     * @param clr Color to set
     * @param r Row index
     * @param c Column index
     * \endif
     *
     * \if CHINESE
     * @brief 在指定位置设置颜色
     * @param clr 要设置的颜色
     * @param r 行索引
     * @param c 列索引
     * \endif
     */
    void setColorAt(const QColor& clr, int r, int c);
    
    /**
     * \if ENGLISH
     * @brief Check if specified position is a spacer
     * @param r Row index
     * @param c Column index
     * @return true if the position is a spacer, false otherwise
     * \endif
     *
     * \if CHINESE
     * @brief 检查指定位置是否是间隔
     * @param r 行索引
     * @param c 列索引
     * @return 如果位置是间隔返回true，否则返回false
     * \endif
     */
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

/**
 * \if ENGLISH
 * @brief Constructor for SAColorGridWidget
 * @param par Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SAColorGridWidget构造函数
 * @param par 父部件
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Destructor for SAColorGridWidget
 * \endif
 *
 * \if CHINESE
 * @brief SAColorGridWidget析构函数
 * \endif
 */
SAColorGridWidget::~SAColorGridWidget()
{
}

/**
 * \if ENGLISH
 * @brief Set column count, row count will be determined by column count
 * @details If set to -1 or 0, column count is not limited, resulting in a single row
 * @param c Column count
 * \endif
 *
 * \if CHINESE
 * @brief 设置列数，行数量会根据列数量来匹配,如果设置-1或者0，说明不限定列数量，这样会只有一行
 * @param c 列数
 * \endif
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
 * \if ENGLISH
 * @brief Set color list
 * @param cls Color list
 * \endif
 *
 * \if CHINESE
 * @brief 设置颜色列表
 * @param cls 颜色列表
 * \endif
 */
void SAColorGridWidget::setColorList(const QList< QColor >& cls)
{
    d_ptr->mColors = cls;
    d_ptr->updateGridColor();
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Get color list
 * @return Color list
 * \endif
 *
 * \if CHINESE
 * @brief 获取颜色列表
 * @return 颜色列表
 * \endif
 */
QList< QColor > SAColorGridWidget::getColorList() const
{
    return d_ptr->mColors;
}

/**
 * \if ENGLISH
 * @brief Get spacing
 * @return Spacing value
 * \endif
 *
 * \if CHINESE
 * @brief 获取间隔
 * @return 间隔值
 * \endif
 */
int SAColorGridWidget::spacing() const
{
    return d_ptr->mGridLayout->spacing();
}

/**
 * \if ENGLISH
 * @brief Set spacing
 * @param v Spacing value
 * \endif
 *
 * \if CHINESE
 * @brief 设置间隔
 * @param v 间隔值
 * \endif
 */
void SAColorGridWidget::setSpacing(int v)
{
    d_ptr->mGridLayout->setSpacing(v);
}

/**
 * \if ENGLISH
 * @brief Get color count
 * @return Number of colors
 * \endif
 *
 * \if CHINESE
 * @brief 获取颜色的数量
 * @return 颜色数量
 * \endif
 */
int SAColorGridWidget::colorCount() const
{
    return d_ptr->mColors.size();
}

/**
 * \if ENGLISH
 * @brief Set color icon size
 * @param s Icon size
 * \endif
 *
 * \if CHINESE
 * @brief 设置图标 size
 * @param s 图标大小
 * \endif
 */
void SAColorGridWidget::setColorIconSize(const QSize& s)
{
    d_ptr->mIconSize = s;
    setMinimumHeight(s.height());
    setMinimumWidth(s.width());
    d_ptr->updateGridColorSize();
}

/**
 * \if ENGLISH
 * @brief Get color icon size
 * @return Icon size
 * \endif
 *
 * \if CHINESE
 * @brief 获取图标 size
 * @return 图标大小
 * \endif
 */
QSize SAColorGridWidget::colorIconSize() const
{
    return d_ptr->mIconSize;
}

/**
 * \if ENGLISH
 * @brief Set whether colors are checkable
 * @details Checkable color buttons can be checked
 * @param on Checkable state
 * \endif
 *
 * \if CHINESE
 * @brief 设置颜色是否是checkable
 * @details checkable的颜色按钮是可checked的
 * @param on 可选中状态
 * \endif
 */
void SAColorGridWidget::setColorCheckable(bool on)
{
    d_ptr->mColorCheckable = on;
    d_ptr->updateGridColorCheckable();
}

/**
 * \if ENGLISH
 * @brief Check if colors are checkable
 * @return Checkable state
 * \endif
 *
 * \if CHINESE
 * @brief 颜色是否是checkable
 * @return 可选中状态
 * \endif
 */
bool SAColorGridWidget::isColorCheckable() const
{
    return d_ptr->mColorCheckable;
}

/**
 * \if ENGLISH
 * @brief Get currently checked color
 * @return Currently checked color
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前选中的颜色
 * @return 当前选中的颜色
 * \endif
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
 * \if ENGLISH
 * @brief Get color button by index
 * @param index Button index
 * @return SAColorToolButton at the specified index, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取index对应的colorbutton
 * @param index 按钮索引
 * @return 如果没有返回nullptr
 * \endif
 */
SAColorToolButton* SAColorGridWidget::colorButton(int index) const
{
    return d_ptr->getColorToolButtonAt(index);
}

/**
 * \if ENGLISH
 * @brief Set vertical spacing (equivalent to GridLayout's VerticalSpacing property)
 * @param v Vertical spacing value
 * \endif
 *
 * \if CHINESE
 * @brief 等同GridLayout的VerticalSpacing属性
 * @param v 垂直间隔值
 * \endif
 */
void SAColorGridWidget::setVerticalSpacing(int v)
{
    d_ptr->mGridLayout->setVerticalSpacing(v);
}

/**
 * \if ENGLISH
 * @brief Get vertical spacing (equivalent to GridLayout's VerticalSpacing property)
 * @return Vertical spacing value
 * \endif
 *
 * \if CHINESE
 * @brief 等同GridLayout的VerticalSpacing属性
 * @return 垂直间隔值
 * \endif
 */
int SAColorGridWidget::verticalSpacing() const
{
    return d_ptr->mGridLayout->verticalSpacing();
}

/**
 * \if ENGLISH
 * @brief Set horizontal spacing (equivalent to GridLayout's HorizontalSpacing property)
 * @param v Horizontal spacing value
 * \endif
 *
 * \if CHINESE
 * @brief 等同GridLayout的HorizontalSpacing属性
 * @param v 水平间隔值
 * \endif
 */
void SAColorGridWidget::setHorizontalSpacing(int v)
{
    d_ptr->mGridLayout->setHorizontalSpacing(v);
}

/**
 * \if ENGLISH
 * @brief Get horizontal spacing (equivalent to GridLayout's HorizontalSpacing property)
 * @return Horizontal spacing value
 * \endif
 *
 * \if CHINESE
 * @brief 等同GridLayout的HorizontalSpacing属性
 * @return 水平间隔值
 * \endif
 */
int SAColorGridWidget::horizontalSpacing() const
{
    return d_ptr->mGridLayout->horizontalSpacing();
}

/**
 * \if ENGLISH
 * @brief Get column count
 * @return Column count
 * \endif
 *
 * \if CHINESE
 * @brief 获取列数
 * @return 列数
 * \endif
 */
int SAColorGridWidget::columnCount() const
{
    return d_ptr->mColumnCount;
}

/**
 * \if ENGLISH
 * @brief Clear checked state, no color will be selected
 * \endif
 *
 * \if CHINESE
 * @brief 清除选中状态，这时没有颜色是选中的
 * \endif
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

/**
 * \if ENGLISH
 * @brief Iterate through all color buttons
 * @param fn Function to apply to each button
 * \endif
 *
 * \if CHINESE
 * @brief 遍历所有颜色按钮
 * @param fn 应用于每个按钮的函数
 * \endif
 */
void SAColorGridWidget::iterationColorBtns(SAColorGridWidget::FunColorBtn fn)
{
    d_ptr->iterationColorBtns(fn);
}

/**
 * \if ENGLISH
 * @brief Set minimum height for a specific row
 * @param row Row index
 * @param minSize Minimum height
 * \endif
 *
 * \if CHINESE
 * @brief 设置指定行的最小高度
 * @param row 行索引
 * @param minSize 最小高度
 * \endif
 */
void SAColorGridWidget::setRowMinimumHeight(int row, int minSize)
{
    d_ptr->mGridLayout->setRowMinimumHeight(row, minSize);
}

/**
 * \if ENGLISH
 * @brief Set horizontal spacer to the right
 * @param on Whether to enable horizontal spacer
 * \endif
 *
 * \if CHINESE
 * @brief 设置右侧水平间隔
 * @param on 是否启用水平间隔
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Handle button clicked event
 * @param btn Clicked button
 * \endif
 *
 * \if CHINESE
 * @brief 处理按钮点击事件
 * @param btn 被点击的按钮
 * \endif
 */
void SAColorGridWidget::onButtonClicked(QAbstractButton* btn)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        Q_EMIT colorClicked(t->color());
    }
}

/**
 * \if ENGLISH
 * @brief Handle button pressed event
 * @param btn Pressed button
 * \endif
 *
 * \if CHINESE
 * @brief 处理按钮按下事件
 * @param btn 被按下的按钮
 * \endif
 */
void SAColorGridWidget::onButtonPressed(QAbstractButton* btn)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        Q_EMIT colorPressed(t->color());
    }
}

/**
 * \if ENGLISH
 * @brief Handle button toggled event
 * @param btn Toggled button
 * @param on Toggle state
 * \endif
 *
 * \if CHINESE
 * @brief 处理按钮切换事件
 * @param btn 被切换的按钮
 * @param on 切换状态
 * \endif
 */
void SAColorGridWidget::onButtonToggled(QAbstractButton* btn, bool on)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        Q_EMIT colorToggled(t->color(), on);
    }
}

/**
 * \if ENGLISH
 * @brief Get size hint
 * @return Recommended size
 * \endif
 *
 * \if CHINESE
 * @brief 获取尺寸提示
 * @return 推荐尺寸
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Handle button released event
 * @param btn Released button
 * \endif
 *
 * \if CHINESE
 * @brief 处理按钮释放事件
 * @param btn 被释放的按钮
 * \endif
 */
void SAColorGridWidget::onButtonReleased(QAbstractButton* btn)
{
    SAColorToolButton* t = qobject_cast< SAColorToolButton* >(btn);
    if (t) {
        Q_EMIT colorReleased(t->color());
    }
}

namespace SA
{

/**
 * \if ENGLISH
 * @brief Get standard color list
 * @return Standard color list
 * \endif
 *
 * \if CHINESE
 * @brief 获取标准颜色列表
 * @return 标准颜色列表
 * \endif
 */
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
