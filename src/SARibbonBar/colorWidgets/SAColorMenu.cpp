#include "SAColorMenu.h"
#include <QWidgetAction>
#include <QColorDialog>
#include <QDebug>
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

public:
    SAColorPaletteGridWidget* mThemeColors { nullptr };    ///< 主题颜色
    QWidgetAction* mThemeColorPaletteAction { nullptr };   ///< ThemeColorsPalette对应的action
    QAction* mCustomColorAction { nullptr };               ///< 自定义颜色action
    SAColorGridWidget* mCustomColorsWidget { nullptr };    ///< 自定义颜色记录
    QWidgetAction* mCustomColorsWidgetAction { nullptr };  ///< 自定义颜色窗口对应的action
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
    if (btn->menu() != this) {
        btn->setMenu(this);
    }
    connect(this, &SAColorMenu::selectedColor, btn, &SAColorToolButton::setColor);
}

/**
 * @brief ThemeColorsPalette对应的action
 * @return
 */
QAction* SAColorMenu::getThemeColorsPaletteAction() const
{
    return d_ptr->mThemeColorPaletteAction;
}

/**
 * @brief 自定义颜色action
 * @return
 */
QAction* SAColorMenu::getCustomColorAction() const
{
    return d_ptr->mCustomColorAction;
}

/**
 * @brief 获取ThemeColorsPalette
 * @return
 */
SAColorPaletteGridWidget* SAColorMenu::getColorPaletteGridWidget() const
{
    return d_ptr->mThemeColors;
}

void SAColorMenu::init(const QList< QColor >& themeCls)
{
    d_ptr->mThemeColors             = new SAColorPaletteGridWidget(themeCls, this);
    d_ptr->mThemeColorPaletteAction = new QWidgetAction(this);
    d_ptr->mThemeColorPaletteAction->setDefaultWidget(d_ptr->mThemeColors);
    addAction(d_ptr->mThemeColorPaletteAction);

    d_ptr->mCustomColorAction = new QAction(tr("Custom Color"), this);  // cn:自定义颜色
    addAction(d_ptr->mCustomColorAction);
    d_ptr->mCustomColorsWidget = new SAColorGridWidget(this);
    d_ptr->mCustomColorsWidget->setColorIconSize(d_ptr->mThemeColors->getColorIconSize());
    d_ptr->mCustomColorsWidget->setColumnCount(d_ptr->mMaxCustomColorSize);
    d_ptr->mCustomColorsWidgetAction = new QWidgetAction(this);
    d_ptr->mCustomColorsWidgetAction->setDefaultWidget(d_ptr->mCustomColorsWidget);
    addAction(d_ptr->mCustomColorsWidgetAction);

    connect(d_ptr->mCustomColorAction, &QAction::triggered, this, &SAColorMenu::onCustomColorActionTriggered);
    connect(d_ptr->mThemeColors, &SAColorPaletteGridWidget::colorClicked, this, &SAColorMenu::onThemeColorsSelected);
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
        emit selectedColor(c);
        hide();
    }
}

/**
 * @brief 主题颜色选中
 * @param c
 */
void SAColorMenu::onThemeColorsSelected(const QColor& c)
{
    emit selectedColor(c);
    hide();
}
