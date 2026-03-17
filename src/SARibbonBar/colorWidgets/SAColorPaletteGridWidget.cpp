#include "SAColorPaletteGridWidget.h"
// Qt
#include <QMenu>
#include <QWidgetAction>
#include <QVBoxLayout>
#include <QColorDialog>
// SA
#include "SAColorGridWidget.h"
/**
 * \if ENGLISH
 * @brief Private data class for SAColorPaletteGridWidget
 * \endif
 *
 * \if CHINESE
 * @brief SAColorPaletteGridWidget的私有数据类
 * \endif
 */
class SAColorPaletteGridWidget::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorPaletteGridWidget)
public:
    /**
     * \if ENGLISH
     * @brief Constructor for PrivateData
     * @param p Parent SAColorPaletteGridWidget
     * \endif
     *
     * \if CHINESE
     * @brief PrivateData构造函数
     * @param p 父SAColorPaletteGridWidget
     * \endif
     */
    PrivateData(SAColorPaletteGridWidget* p);
    
    /**
     * \if ENGLISH
     * @brief Generate color palette
     * @param clrList Color list
     * @return Generated color palette
     * \endif
     *
     * \if CHINESE
     * @brief 生成color palette
     * @param clrList 颜色列表
     * @return 生成的颜色面板
     * \endif
     */
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
    for (int f : sacolor_as_const(mFactor)) {
        for (const QColor& c : sacolor_as_const(clrList)) {
            res.append(c.lighter(f));
        }
    }
    return res;
}
//==============================================================
// SAColorPaletteGridWidget
//==============================================================

/**
 * \if ENGLISH
 * @brief Constructor for SAColorPaletteGridWidget with default standard colors
 * @param par Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 使用默认的标准颜色生成一个color palette
 * @param par 父部件
 * \endif
 */
SAColorPaletteGridWidget::SAColorPaletteGridWidget(QWidget* par) : QWidget(par), d_ptr(new PrivateData(this))
{
    init();
    setColorList(SA::getStandardColorList());
}

/**
 * \if ENGLISH
 * @brief Constructor for SAColorPaletteGridWidget with specified colors
 * @param cls Color list
 * @param par Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 根据指定的颜色生成一个color palette
 * @param cls 颜色列表
 * @param par 父部件
 * \endif
 */
SAColorPaletteGridWidget::SAColorPaletteGridWidget(const QList< QColor >& cls, QWidget* par)
    : QWidget(par), d_ptr(new PrivateData(this))
{
    init();
    setColorList(cls);
}

/**
 * \if ENGLISH
 * @brief Destructor for SAColorPaletteGridWidget
 * \endif
 *
 * \if CHINESE
 * @brief SAColorPaletteGridWidget析构函数
 * \endif
 */
SAColorPaletteGridWidget::~SAColorPaletteGridWidget()
{
}

/**
 * \if ENGLISH
 * @brief Initialize widget
 * \endif
 *
 * \if CHINESE
 * @brief 初始化部件
 * \endif
 */
void SAColorPaletteGridWidget::init()
{
    connect(d_ptr->mMainColorList, &SAColorGridWidget::colorClicked, this, &SAColorPaletteGridWidget::onMainColorClicked);
    connect(d_ptr->mPaletteColorGrid, &SAColorGridWidget::colorClicked, this, &SAColorPaletteGridWidget::onPaletteColorClicked);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setSizePolicy(sizePolicy);
    setColorIconSize(QSize(10, 10));
}

/**
 * \if ENGLISH
 * @brief Set color list, the number of colors determines the number of main color cards
 * @details Palette color cards will be generated based on factor
 * @param cls Color list
 * \endif
 *
 * \if CHINESE
 * @brief 设置颜色系列，颜色系列的个数决定了主色卡的数量，palette色卡会根据factor进行生成
 * @param cls 颜色列表
 * \endif
 */
void SAColorPaletteGridWidget::setColorList(const QList< QColor >& cls)
{
    d_ptr->mMainColorList->setColumnCount(0);
    d_ptr->mMainColorList->setColorList(cls);
    d_ptr->mPaletteColorGrid->setColumnCount(cls.size());
    d_ptr->mPaletteColorGrid->setColorList(d_ptr->makeColorPalette(cls));
}

/**
 * \if ENGLISH
 * @brief Get color list
 * @return Color list
 * \endif
 *
 * \if CHINESE
 * @brief 获取颜色系列
 * @return 颜色列表
 * \endif
 */
QList< QColor > SAColorPaletteGridWidget::colorList() const
{
    return d_ptr->mMainColorList->getColorList();
}

/**
 * \if ENGLISH
 * @brief Set color shade factor, factor determines the number of palette rows
 * @details Default factor is { 180, 160, 140, 75, 50 }, which will generate 5 rows
 * @details Each factor will apply QColor::lighter operation to standard colors
 * @param factor Factor list
 * \endif
 *
 * \if CHINESE
 * @brief 设置颜色深浅比例factor，factor决定了palette的行数，factor有5个，就有5行
 * @details 默认为{ 180, 160, 140, 75, 50 },相当于会有5行，每个系数会对标准颜色执行QColor::lighter操作
 * @param factor 因子列表
 * \endif
 */
void SAColorPaletteGridWidget::setFactor(const QList< int >& factor)
{
    d_ptr->mFactor = factor;
    d_ptr->mPaletteColorGrid->setColorList(d_ptr->makeColorPalette(d_ptr->mMainColorList->getColorList()));
}

/**
 * \if ENGLISH
 * @brief Get factor list
 * @return Factor list
 * \endif
 *
 * \if CHINESE
 * @brief 获取色卡
 * @return 因子列表
 * \endif
 */
QList< int > SAColorPaletteGridWidget::factor() const
{
    return d_ptr->mFactor;
}

/**
 * \if ENGLISH
 * @brief Set color icon size, default is 10*10
 * @param s Icon size
 * \endif
 *
 * \if CHINESE
 * @brief 设置颜色块的尺寸，默认为10*10
 * @param s 图标大小
 * \endif
 */
void SAColorPaletteGridWidget::setColorIconSize(const QSize& s)
{
    d_ptr->mMainColorList->setColorIconSize(s);
    d_ptr->mPaletteColorGrid->setColorIconSize(s);
}

/**
 * \if ENGLISH
 * @brief Get color icon size
 * @return Icon size
 * \endif
 *
 * \if CHINESE
 * @brief 获取颜色块尺寸
 * @return 图标大小
 * \endif
 */
QSize SAColorPaletteGridWidget::colorIconSize() const
{
    return d_ptr->mMainColorList->colorIconSize();
}

/**
 * \if ENGLISH
 * @brief Set whether color blocks are checkable
 * @param on Checkable state
 * \endif
 *
 * \if CHINESE
 * @brief 设置颜色块是否能被checked
 * @param on 可选中状态
 * \endif
 */
void SAColorPaletteGridWidget::setColorCheckable(bool on)
{
    d_ptr->mMainColorList->setColorCheckable(on);
    d_ptr->mPaletteColorGrid->setColorCheckable(on);
}

/**
 * \if ENGLISH
 * @brief Check if color blocks are checkable
 * @return Checkable state
 * \endif
 *
 * \if CHINESE
 * @brief 检查颜色块是否可选中
 * @return 可选中状态
 * \endif
 */
bool SAColorPaletteGridWidget::isColorCheckable() const
{
    return d_ptr->mMainColorList->isColorCheckable();
}

/**
 * \if ENGLISH
 * @brief Handle main color clicked
 * @param c Clicked color
 * \endif
 *
 * \if CHINESE
 * @brief 处理主颜色点击
 * @param c 点击的颜色
 * \endif
 */
void SAColorPaletteGridWidget::onMainColorClicked(const QColor& c)
{
    d_ptr->mPaletteColorGrid->clearCheckedState();
    Q_EMIT colorClicked(c);
}

/**
 * \if ENGLISH
 * @brief Handle palette color clicked
 * @param c Clicked color
 * \endif
 *
 * \if CHINESE
 * @brief 处理调色板颜色点击
 * @param c 点击的颜色
 * \endif
 */
void SAColorPaletteGridWidget::onPaletteColorClicked(const QColor& c)
{
    d_ptr->mMainColorList->clearCheckedState();
    Q_EMIT colorClicked(c);
}
