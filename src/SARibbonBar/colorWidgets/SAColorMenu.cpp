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
class SAColorMenu::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorMenu)
public:
    PrivateData(SAColorMenu* p);
    QColor getColorByDialog();
    void recordCustomColor(const QColor& c);
    QWidgetAction* addWidget(QWidget* w);
    //创建一个无颜色的icon
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
