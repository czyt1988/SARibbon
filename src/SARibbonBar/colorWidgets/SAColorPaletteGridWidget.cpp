#include "SAColorPaletteGridWidget.h"
#include "SAColorGridWidget.h"
#include "QVBoxLayout"
class SAColorPaletteGridWidget::PrivateData
{
    SA_DECLARE_PUBLIC(SAColorPaletteGridWidget)
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
SAColorPaletteGridWidget::SAColorPaletteGridWidget(QWidget* par) : QWidget(par), d_ptr(new PrivateData(this))
{
    setColorIconSize(QSize(10, 10));
    setColorList(SA::getStandardColorList());
    connect(d_ptr->mMainColorList, &SAColorGridWidget::colorClicked, this, &SAColorPaletteGridWidget::onMainColorClicked);
    connect(d_ptr->mPaletteColorGrid, &SAColorGridWidget::colorClicked, this, &SAColorPaletteGridWidget::onPaletteColorClicked);
}

SAColorPaletteGridWidget::~SAColorPaletteGridWidget()
{
}

void SAColorPaletteGridWidget::setColorList(const QList< QColor >& cls)
{
    d_ptr->mMainColorList->setColorList(cls);
    d_ptr->mPaletteColorGrid->setColumnCount(cls.size());
    d_ptr->mPaletteColorGrid->setColorList(d_ptr->makeColorPalette(cls));
}

/**
 * @brief 设置颜色深浅比例factor
 *
 * 默认为{ 180, 160, 140, 75, 50 },相当于会有5行
 * @param factor
 */
void SAColorPaletteGridWidget::setFactor(const QList< int >& factor)
{
    d_ptr->mFactor = factor;
    d_ptr->mPaletteColorGrid->setColorList(d_ptr->makeColorPalette(d_ptr->mMainColorList->getColorList()));
}

QList< int > SAColorPaletteGridWidget::getFactor() const
{
    return d_ptr->mFactor;
}

void SAColorPaletteGridWidget::setColorIconSize(const QSize& s)
{
    d_ptr->mMainColorList->setColorIconSize(s);
    d_ptr->mPaletteColorGrid->setColorIconSize(s);
}

QSize SAColorPaletteGridWidget::getColorIconSize() const
{
    return d_ptr->mMainColorList->getColorIconSize();
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
