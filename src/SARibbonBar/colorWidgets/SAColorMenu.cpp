#include "SAColorMenu.h"
#include <QWidgetAction>
#include <QColorDialog>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include "SAColorGridWidget.h"
#include "SAColorPaletteGridWidget.h"
#include "SAColorToolButton.h"
/**
 * \if ENGLISH
 * @brief Private data class for SAColorMenu
 * \endif
 *
 * \if CHINESE
 * @brief SAColorMenu的私有数据类
 * \endif
 */
class SAColorMenu::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorMenu)
public:
    /**
     * \if ENGLISH
     * @brief Constructor for PrivateData
     * @param p Parent SAColorMenu
     * \endif
     *
     * \if CHINESE
     * @brief PrivateData构造函数
     * @param p 父SAColorMenu
     * \endif
     */
    PrivateData(SAColorMenu* p);
    
    /**
     * \if ENGLISH
     * @brief Get color from color dialog
     * @return Selected color, or invalid color if dialog is canceled
     * \endif
     *
     * \if CHINESE
     * @brief 通过对话框获取颜色
     * @return 选中的颜色，如果对话框被取消则返回无效颜色
     * \endif
     */
    QColor getColorByDialog();
    
    /**
     * \if ENGLISH
     * @brief Record custom color
     * @param c Color to record
     * \endif
     *
     * \if CHINESE
     * @brief 记录自定义颜色
     * @param c 要记录的颜色
     * \endif
     */
    void recordCustomColor(const QColor& c);
    
    /**
     * \if ENGLISH
     * @brief Add widget to menu
     * @param w Widget to add
     * @return Created QWidgetAction
     * \endif
     *
     * \if CHINESE
     * @brief 向菜单添加部件
     * @param w 要添加的部件
     * @return 创建的QWidgetAction
     * \endif
     */
    QWidgetAction* addWidget(QWidget* w);
    
    /**
     * \if ENGLISH
     * @brief Create a none color icon
     * @param baseSize Icon base size
     * @return Created icon
     * \endif
     *
     * \if CHINESE
     * @brief 创建一个无颜色的icon
     * @param baseSize 图标基础大小
     * @return 创建的图标
     * \endif
     */
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
    QScopedPointer< QColorDialog > mColorDlg { nullptr };  ///<颜色对话框
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
        //超过数量，就左移动
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

/**
 * \if ENGLISH
 * @brief Constructor for SAColorMenu
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SAColorMenu构造函数
 * @param parent 父部件
 * \endif
 */
SAColorMenu::SAColorMenu(QWidget* parent) : QMenu(parent), d_ptr(new SAColorMenu::PrivateData(this))
{
    init(SA::getStandardColorList());
}

/**
 * \if ENGLISH
 * @brief Constructor for SAColorMenu with title
 * @param title Menu title
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SAColorMenu构造函数（带标题）
 * @param title 菜单标题
 * @param parent 父部件
 * \endif
 */
SAColorMenu::SAColorMenu(const QString& title, QWidget* parent)
    : QMenu(title, parent), d_ptr(new SAColorMenu::PrivateData(this))
{
    init(SA::getStandardColorList());
}

/**
 * \if ENGLISH
 * @brief Destructor for SAColorMenu
 * \endif
 *
 * \if CHINESE
 * @brief SAColorMenu析构函数
 * \endif
 */
SAColorMenu::~SAColorMenu()
{
}

/**
 * \if ENGLISH
 * @brief Quick bind to SAColorToolButton
 * @param btn SAColorToolButton to bind
 * \endif
 *
 * \if CHINESE
 * @brief 快速绑定ColorToolButton
 * @param btn 要绑定的SAColorToolButton
 * \endif
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
 * \if ENGLISH
 * @brief Get theme colors palette action
 * @return Theme colors palette action
 * \endif
 *
 * \if CHINESE
 * @brief ThemeColorsPalette对应的action
 * @return 主题颜色面板对应的action
 * \endif
 */
QWidgetAction* SAColorMenu::themeColorsPaletteAction() const
{
    return d_ptr->mThemeColorPaletteAction;
}

/**
 * \if ENGLISH
 * @brief Get custom colors widget action
 * @return Custom colors widget action
 * \endif
 *
 * \if CHINESE
 * @brief CustomColorsWidget对应的action
 * @return 自定义颜色窗口对应的action
 * \endif
 */
QWidgetAction* SAColorMenu::getCustomColorsWidgetAction() const
{
    return d_ptr->mCustomColorsWidgetAction;
}

/**
 * \if ENGLISH
 * @brief Get custom color action
 * @return Custom color action
 * \endif
 *
 * \if CHINESE
 * @brief 自定义颜色action
 * @return 自定义颜色action
 * \endif
 */
QAction* SAColorMenu::customColorAction() const
{
    return d_ptr->mCustomColorAction;
}

/**
 * \if ENGLISH
 * @brief Get theme colors palette widget
 * @return Theme colors palette widget
 * \endif
 *
 * \if CHINESE
 * @brief 获取ThemeColorsPalette
 * @return 主题颜色面板部件
 * \endif
 */
SAColorPaletteGridWidget* SAColorMenu::colorPaletteGridWidget() const
{
    return d_ptr->mThemeColorsWidget;
}

/**
 * \if ENGLISH
 * @brief Get custom colors widget
 * @return Custom colors widget
 * \endif
 *
 * \if CHINESE
 * @brief 获取自定义颜色grid
 * @return 自定义颜色网格部件
 * \endif
 */
SAColorGridWidget* SAColorMenu::customColorsWidget() const
{
    return d_ptr->mCustomColorsWidget;
}

/**
 * \if ENGLISH
 * @brief Enable none color action
 * @details None color selection will emit selectedColor(QColor())
 * @param on Whether to enable none color action
 * \endif
 *
 * \if CHINESE
 * @brief 建立没有颜色的action，可以选择无颜色
 * @details 无颜色选中会发射selectedColor(QColor())
 * @param on 是否启用无颜色action
 * \endif
 */
void SAColorMenu::enableNoneColorAction(bool on)
{
    //无颜色默认是在自定义颜色的上方
    if (on) {
        if (d_ptr->mNoneColorAction) {
            if (actions().contains(d_ptr->mNoneColorAction)) {
                //已经包含了NoneColorAction,退出
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
 * \if ENGLISH
 * @brief Get none color action
 * @note Note that enableNoneColorAction(true) must be called before this returns non-nullptr
 * @return None color action, or nullptr if not created
 * \endif
 *
 * \if CHINESE
 * @brief 获取None Color Action
 * @note 注意，enableNoneColorAction(true),之后才不是nullptr
 * @return 如果没有建立NoneColorAction，会返回nullptr
 * \endif
 */
QAction* SAColorMenu::noneColorAction() const
{
    return d_ptr->mNoneColorAction;
}

/**
 * \if ENGLISH
 * @brief Helper slot function to emit selectedColor signal and hide menu
 * @details This function allows other custom actions to be associated with the menu
 * @param c Selected color
 * \endif
 *
 * \if CHINESE
 * @brief 这是一个辅助槽函数，为了让用户自定义的其他action也能关联menu，可以调用此槽函数，实现selectedColor信号以及menu的隐藏
 * @param c 选中的颜色
 * \endif
 */
void SAColorMenu::emitSelectedColor(const QColor& c)
{
    Q_EMIT selectedColor(c);
    hide();
}

/**
 * \if ENGLISH
 * @brief Initialize menu
 * @param themeCls Theme color list
 * \endif
 *
 * \if CHINESE
 * @brief 初始化菜单
 * @param themeCls 主题颜色列表
 * \endif
 */
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
 * \if ENGLISH
 * @brief Handle custom color action triggered
 * @param on Triggered state
 * \endif
 *
 * \if CHINESE
 * @brief 自定义颜色
 * @param on 触发状态
 * \endif
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
 * \if ENGLISH
 * @brief Handle none color action triggered
 * @param on Triggered state
 * \endif
 *
 * \if CHINESE
 * @brief 无颜色
 * @param on 触发状态
 * \endif
 */
void SAColorMenu::onNoneColorActionTriggered(bool on)
{
    Q_UNUSED(on);
    emitSelectedColor(QColor());
}
