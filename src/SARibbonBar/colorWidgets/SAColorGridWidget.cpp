#include "SAColorGridWidget.h"

class SAColorGridWidget::PrivateData
{
    SA_DECLARE_PUBLIC(SAColorGridWidget)
public:
    PrivateData(SAColorGridWidget* p);
    QList< QColor > mColors;
    QSize mGridSize;
    int mSpacing;
    QSize mIconSize;
};

SAColorGridWidget::PrivateData::PrivateData(SAColorGridWidget* p) : q_ptr(p)
{
}

//==============================================================
// SAColorGridWidget
//==============================================================

SAColorGridWidget::SAColorGridWidget(QWidget* par) : QWidget(par), d_ptr(new SAColorGridWidget::PrivateData(this))
{
}

SAColorGridWidget::~SAColorGridWidget()
{
}

/**
 * @brief 设置颜色列表
 * @param c
 */
void SAColorGridWidget::setColorList(const QList< QColor >& c)
{
    d_ptr->mColors = c;
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
 * @brief 设置grid尺寸
 * @param row
 * @param col
 */
void SAColorGridWidget::setGridSize(int row, int col)
{
    d_ptr->mGridSize = QSize(row, col);
}

/**
 * @brief 设置grid尺寸
 * @param row
 * @param col
 */
void SAColorGridWidget::setGridSize(const QSize& s)
{
    d_ptr->mGridSize = s;
}

/**
 * @brief 获取grid size
 * @return
 */
QSize SAColorGridWidget::getGridSize() const
{
    return d_ptr->mGridSize;
}

/**
 * @brief 获取间隔
 * @return
 */
int SAColorGridWidget::getSpacing() const
{
    return d_ptr->mSpacing;
}

/**
 * @brief 设置间隔
 * @param v
 */
void SAColorGridWidget::setSpacing(int v)
{
    d_ptr->mSpacing = v;
}

/**
 * @brief 设置图标 size
 * @return
 */
void SAColorGridWidget::setIconSize(const QSize& s)
{
}

/**
 * @brief 获取图标 size
 * @return
 */
QSize SAColorGridWidget::getIconSize() const
{
}
