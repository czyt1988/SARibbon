#include "SAColorPaletteGridWidget.h"
// Qt
#include <QMenu>
#include <QWidgetAction>
#include <QVBoxLayout>
#include <QColorDialog>
// SA
#include "SAColorGridWidget.h"
class SAColorPaletteGridWidget::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorPaletteGridWidget)
public:
    PrivateData(SAColorPaletteGridWidget* p);
    //生成color palette
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
