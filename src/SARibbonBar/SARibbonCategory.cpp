#include "SARibbonCategory.h"
#include <QList>
#include <QResizeEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QApplication>
#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QResizeEvent>
#include "SARibbonCategoryLayout.h"
#include "SARibbonElementManager.h"

#ifndef SARIBBONCATEGORY_DEBUG_PRINT
#define SARIBBONCATEGORY_DEBUG_PRINT 0
#endif
#if SARIBBONCATEGORY_DEBUG_PRINT
#include <QDebug>
#endif

/**
 * @brief SARibbonCategory::PrivateData
 */
class SARibbonCategory::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCategory)
public:
    PrivateData(SARibbonCategory* p);

    SARibbonPanel* addPanel(const QString& title);
    SARibbonPanel* insertPanel(const QString& title, int index);
    void addPanel(SARibbonPanel* panel);
    void insertPanel(int index, SARibbonPanel* panel);

    // 把panel从Category中移除，不会销毁，此时panel的所有权归还操作者
    bool takePanel(SARibbonPanel* panel);

    // 移除Panel，Category会直接回收SARibbonPanel内存
    bool removePanel(SARibbonPanel* panel);
    SARibbonCategory* ribbonCategory();
    const SARibbonCategory* ribbonCategory() const;

    // 返回所有的Panel
    QList< SARibbonPanel* > panelList();

    // 更新item的布局,此函数会调用doItemLayout
    void updateItemGeometry();

    void doWheelEvent(QWheelEvent* event);
    // 初始化
    void init(SARibbonCategory* c);

public:
    bool enableShowPanelTitle { true };             ///< 是否运行panel的标题栏显示
    int panelTitleHeight { 15 };                    ///< panel的标题栏默认高度
    bool isContextCategory { false };               ///< 标记是否是上下文标签
    bool isCanCustomize { true };                   ///< 标记是否可以自定义
    int panelSpacing { 0 };                         ///< panel的spacing
    bool isUseAnimating { true };                   ///< 默认使用动画滚动
    bool enableWordWrap { true };                   ///< 是否文字换行
    QSize panelSmallToolButtonIconSize { 20, 20 };  ///< 记录panel的默认小图标大小
    QSize panelLargeToolButtonIconSize { 32, 32 };  ///< 记录panel的默认大图标大小
    int wheelScrollStep { 400 };                    ///< 默认滚轮滚动步长
    qreal buttonMaximumAspectRatio { 1.4 };         ///< 按钮最大宽高比，这个系数决定按钮的最大宽度
    SARibbonPanel::PanelLayoutMode defaultPanelLayoutMode { SARibbonPanel::ThreeRowMode };
};
SARibbonCategory::PrivateData::PrivateData(SARibbonCategory* p) : q_ptr(p)
{
}

SARibbonPanel* SARibbonCategory::PrivateData::addPanel(const QString& title)
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        SARibbonPanel* p = insertPanel(title, lay->panelCount());
        return p;
    }
    return nullptr;
}

SARibbonPanel* SARibbonCategory::PrivateData::insertPanel(const QString& title, int index)
{
    SARibbonPanel* panel = RibbonSubElementFactory->createRibbonPanel(ribbonCategory());

    panel->setPanelName(title);
    panel->setObjectName(title);
    insertPanel(index, panel);
    return (panel);
}

void SARibbonCategory::PrivateData::addPanel(SARibbonPanel* panel)
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        insertPanel(lay->panelCount(), panel);
    }
}

/**
 * @brief 插入panel到layout
 *
 * 所有的添加操作最终会调用此函数
 * @param index
 * @param panel
 */
void SARibbonCategory::PrivateData::insertPanel(int index, SARibbonPanel* panel)
{
    if (nullptr == panel) {
        return;
    }
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (nullptr == lay) {
        return;
    }
    if (panel->parentWidget() != q_ptr) {
        panel->setParent(q_ptr);
    }
    // 同步状态
    panel->setEnableShowTitle(this->enableShowPanelTitle);
    panel->setTitleHeight(this->panelTitleHeight);
    panel->setPanelLayoutMode(this->defaultPanelLayoutMode);
    panel->setSpacing(this->panelSpacing);
    panel->setToolButtonIconSize(this->panelSmallToolButtonIconSize, this->panelLargeToolButtonIconSize);
    panel->setEnableWordWrap(this->enableWordWrap);

    index = qMax(0, index);
    index = qMin(lay->panelCount(), index);
    lay->insertPanel(index, panel);
    panel->setVisible(true);

    QObject::connect(panel, &SARibbonPanel::actionTriggered, ribbonCategory(), &SARibbonCategory::actionTriggered);
    q_ptr->updateGeometry();  // 通知父布局这个控件的尺寸提示(sizeHint())可能已改变
}

bool SARibbonCategory::PrivateData::takePanel(SARibbonPanel* panel)
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (nullptr == lay) {
        return false;
    }
    bool res = lay->takePanel(panel);
    q_ptr->updateGeometry();  // 通知父布局这个控件的尺寸提示(sizeHint())可能已改变
    return res;
}

bool SARibbonCategory::PrivateData::removePanel(SARibbonPanel* panel)
{
    if (takePanel(panel)) {
        panel->hide();
        panel->deleteLater();
        return (true);
    }
    return (false);
}

QList< SARibbonPanel* > SARibbonCategory::PrivateData::panelList()
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        return lay->panelList();
    }
    return QList< SARibbonPanel* >();
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
#if SARIBBONCATEGORY_DEBUG_PRINT
    qDebug() << "SARibbonCategory::PrivateData::updateItemGeometry,categoryName=" << q_ptr->categoryName();
#endif
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (!lay) {
        return;
    }
    const QList< SARibbonPanel* > panels = lay->panelList();
    for (auto panel : panels) {
        panel->updateItemGeometry();
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

    // 如果动画正在进行，忽略新的事件
    if (isUseAnimating && lay->isAnimatingScroll()) {
        event->ignore();
        return;
    }

    QSize contentSize = lay->categoryContentSize();
    int totalWidth    = lay->categoryTotalWidth();

    if (totalWidth > contentSize.width()) {
        int scrollStep = wheelScrollStep;

        // 根据滚轮方向确定滚动方向
        QPoint numPixels  = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;

        if (!numPixels.isNull()) {
            scrollStep = (numPixels.y() < 0) ? -scrollStep : scrollStep;
        } else if (!numDegrees.isNull()) {
            scrollStep = (numDegrees.y() < 0) ? -scrollStep : scrollStep;
        }

        // 动态调整步长 - 滚动越快步长越大
        const int absDelta = qMax(qAbs(numPixels.y()), qAbs(numDegrees.y()));
        if (absDelta > 60) {
            scrollStep *= 2;
        } else if (absDelta < 20) {
            scrollStep /= 2;
        }

        // 根据设置选择滚动方式
        if (isUseAnimating) {
            lay->scrollByAnimate(scrollStep);
        } else {
            lay->scroll(scrollStep);
        }
    } else {
        event->ignore();
        if (lay->isScrolled()) {
            // 根据设置选择复位方式
            if (isUseAnimating) {
                lay->scrollToByAnimate(0);
            } else {
                lay->scroll(0);
            }
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

/**
 * \if ENGLISH
 * @brief Constructor with name
 * @param name Category name
 * @param p Parent widget pointer
 * \endif
 *
 * \if CHINESE
 * @brief 带名称的构造函数
 * @param name Category名称
 * @param p 父级控件指针
 * \endif
 */
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
 * \if ENGLISH
 * @brief Get the category name
 * @return Current category name (i.e., windowTitle)
 * \endif
 *
 * \if CHINESE
 * @brief 获取Category名称
 * @return 当前Category的名称（即windowTitle）
 * \endif
 */
QString SARibbonCategory::categoryName() const
{
    return (windowTitle());
}

/**
 * \if ENGLISH
 * @brief Set the category name
 * @param title New name (equivalent to setWindowTitle)
 * \endif
 *
 * \if CHINESE
 * @brief 设置Category名称
 * @param title 新名称（等价于setWindowTitle）
 * \endif
 */
void SARibbonCategory::setCategoryName(const QString& title)
{
    setWindowTitle(title);
}

/**
 * \if ENGLISH
 * @brief Get the panel layout mode
 * @return Current layout mode for all panels
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板布局模式
 * @return 当前所有面板的布局模式
 * \endif
 */
SARibbonPanel::PanelLayoutMode SARibbonCategory::panelLayoutMode() const
{
    return (d_ptr->defaultPanelLayoutMode);
}

/**
 * \if ENGLISH
 * @brief Set the panel layout mode
 *
 * When @ref SARibbonBar calls the @ref SARibbonBar::setRibbonStyle function, this function will be called for all
 * SARibbonCategory objects to set the new SARibbonPanel::PanelLayoutMode
 * @param m Panel layout mode
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板布局模式
 *
 * 在@ref SARibbonBar 调用@ref SARibbonBar::setRibbonStyle 函数时，会对所有的SARibbonCategory调用此函数
 * 把新的SARibbonPanel::PanelLayoutMode设置进去
 * @param m 面板布局模式
 * \endif
 */
void SARibbonCategory::setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m)
{
    d_ptr->defaultPanelLayoutMode = m;
    iteratePanel([ m ](SARibbonPanel* p) -> bool {
        p->setPanelLayoutMode(m);
        return true;
    });
    updateItemGeometry();
}

/**
 * \if ENGLISH
 * @brief Add a panel
 *
 * @note The ownership of the panel is managed by SARibbonCategory, please do not destroy it externally
 * @param title Panel title, which will be displayed below the panel in the three-row mode of office/wps
 * @return Returns a pointer to the generated @ref SARibbonPanel
 * @see For other operations on SARibbonPanel, refer to @ref SARibbonCategory::takePanel
 * \endif
 *
 * \if CHINESE
 * @brief 添加面板(panel)
 *
 * @note 面板(panel)的所有权由SARibbonCategory来管理，请不要在外部对其进行销毁
 * @param title 面板(panel)的标题，在office/wps的三行模式下会显示在面板(panel)的下方
 * @return 返回生成的@ref SARibbonPanel 指针
 * @see 对SARibbonPanel的其他操作，参考 @ref SARibbonCategory::takePanel
 * \endif
 */
SARibbonPanel* SARibbonCategory::addPanel(const QString& title)
{
    return (d_ptr->addPanel(title));
}

/**
 * \if ENGLISH
 * @brief Add panel
 * @param panel Panel ownership is managed by SARibbonCategory
 * \endif
 *
 * \if CHINESE
 * @brief 添加panel
 * @param panel panel的所有权SARibbonCategory来管理
 * \endif
 */
void SARibbonCategory::addPanel(SARibbonPanel* panel)
{
    d_ptr->addPanel(panel);
}

/**
 * \if ENGLISH
 * @brief Qt Designer specific
 * @param panel Widget to add
 * \endif
 *
 * \if CHINESE
 * @brief Qt Designer专用
 * @param panel 要添加的窗口部件
 * \endif
 */
void SARibbonCategory::addPanel(QWidget* panel)
{
    SARibbonPanel* p = qobject_cast< SARibbonPanel* >(panel);
    if (p) {
        addPanel(p);
    }
}

/**
 * \if ENGLISH
 * @brief Create a new panel and insert it at the index position
 * @param title Panel title
 * @param index Insertion position, if index exceeds the number of panels in the category, it will be inserted at the end
 * @return Returns a pointer to the generated @ref SARibbonPanel
 * \endif
 *
 * \if CHINESE
 * @brief 新建一个panel，并插入到index位置
 * @param title panel的title
 * @param index 插入的位置，如果index超出category里panel的个数，将插入到最后
 * @return 返回生成的@ref SARibbonPanel 指针
 * \endif
 */
SARibbonPanel* SARibbonCategory::insertPanel(const QString& title, int index)
{
    return (d_ptr->insertPanel(title, index));
}

/**
 * \if ENGLISH
 * @brief Insert an existing panel at the specified index position
 *
 * This function inserts an existing SARibbonPanel object at the specified index position.
 * The panel's ownership will be transferred to SARibbonCategory.
 *
 * @param panel Pointer to the SARibbonPanel to insert, if nullptr the function does nothing
 * @param index Insertion position index
 *              - If index < 0, the panel will be inserted at the beginning (index 0)
 *              - If index >= panelCount(), the panel will be appended at the end
 *
 * @note The panel's parent widget will be set to this SARibbonCategory
 * @note The panel will inherit the category's display settings (title height, layout mode, etc.)
 *
 * @see addPanel(SARibbonPanel*), takePanel(SARibbonPanel*)
 * \endif
 *
 * \if CHINESE
 * @brief 在指定索引位置插入已存在的面板
 *
 * 此函数将一个已存在的SARibbonPanel对象插入到指定的索引位置。
 * 面板的所有权将转移给SARibbonCategory管理。
 *
 * @param panel 要插入的SARibbonPanel指针，如果为nullptr则函数不执行任何操作
 * @param index 插入位置的索引
 *              - 如果index < 0，面板将被插入到开头（索引0）
 *              - 如果index >= panelCount()，面板将被追加到末尾
 *
 * @note 面板的父窗口将被设置为此SARibbonCategory
 * @note 面板将继承Category的显示设置（标题高度、布局模式等）
 *
 * @see addPanel(SARibbonPanel*), takePanel(SARibbonPanel*)
 * \endif
 */
void SARibbonCategory::insertPanel(SARibbonPanel* panel, int index)
{
    d_ptr->insertPanel(index, panel);
}

/**
 * \if ENGLISH
 * @brief Find panel by name
 * @param title Panel title to search for
 * @return If there are duplicate names, only the first one that meets the condition will be returned
 * \endif
 *
 * \if CHINESE
 * @brief 通过名字查找panel
 * @param title 要查找的panel标题
 * @return 如果有重名，只会返回第一个符合条件的
 * \endif
 */
SARibbonPanel* SARibbonCategory::panelByName(const QString& title) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelByName(title);
    }
    return nullptr;
}

/**
 * \if ENGLISH
 * @brief Find panel by ObjectName
 * @param objname Object name to search for
 * @return Panel with the specified object name, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 通过ObjectName查找panel
 * @param objname 要查找的对象名
 * @return 找到的panel指针，如果没找到返回nullptr
 * \endif
 */
SARibbonPanel* SARibbonCategory::panelByObjectName(const QString& objname) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelByObjectName(objname);
    }
    return nullptr;
}

/**
 * \if ENGLISH
 * @brief Find panel by index, returns nullptr if the index is out of range
 * @param index Panel index
 * @return Returns nullptr if the index is out of range
 * \endif
 *
 * \if CHINESE
 * @brief 通过索引找到panel，如果超过索引范围，会返回nullptr
 * @param index panel的索引
 * @return 如果超过索引范围，会返回nullptr
 * \endif
 */
SARibbonPanel* SARibbonCategory::panelByIndex(int index) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelByIndex(index);
    }
    return nullptr;
}

/**
 * \if ENGLISH
 * @brief Find the index corresponding to panel
 * @param p Panel to find index for
 * @return Returns -1 if not found
 * \endif
 *
 * \if CHINESE
 * @brief 查找panel对应的索引
 * @param p 要查找索引的panel
 * @return 如果找不到，返回-1
 * \endif
 */
int SARibbonCategory::panelIndex(SARibbonPanel* p) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelIndex(p);
    }
    return -1;
}

/**
 * \if ENGLISH
 * @brief Move a Panel from from index to to index
 * @param from Index of the panel to move
 * @param to Position to move to
 * \endif
 *
 * \if CHINESE
 * @brief 移动一个Panel从from index到to index
 * @param from 要移动panel的index
 * @param to 要移动到的位置
 * \endif
 */
void SARibbonCategory::movePanel(int from, int to)
{
    if (from == to) {
        return;
    }
    if (to < 0) {
        to = 0;
    }
    if (to >= panelCount()) {
        to = panelCount() - 1;
    }
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        lay->movePanel(from, to);
    }
}

/**
 * \if ENGLISH
 * @brief Detach panel from SARibbonCategory management
 * @param panel Panel to extract
 * @return Returns true on success, otherwise false
 * \endif
 *
 * \if CHINESE
 * @brief 把panel脱离SARibbonCategory的管理
 * @param panel 需要提取的panel
 * @return 成功返回true，否则返回false
 * \endif
 */
bool SARibbonCategory::takePanel(SARibbonPanel* panel)
{
    return (d_ptr->takePanel(panel));
}

/**
 * \if ENGLISH
 * @brief Remove Panel, Category will directly recycle SARibbonPanel memory
 * @param panel Panel to remove
 * @note After removal, panel becomes a wild pointer. It is generally recommended to set the panel pointer to nullptr after the operation
 *
 * This operation is equivalent to:
 *
 * @code
 * SARibbonPanel* panel;
 * ...
 * category->takePanel(panel);
 * panel->hide();
 * panel->deleteLater();
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief 移除Panel，Category会直接回收SARibbonPanel内存
 * @param panel 需要移除的panel
 * @note 移除后panel为野指针，一般操作完建议把panel指针设置为nullptr
 *
 * 此操作等同于：
 *
 * @code
 * SARibbonPanel* panel;
 * ...
 * category->takePanel(panel);
 * panel->hide();
 * panel->deleteLater();
 * @endcode
 * \endif
 */
bool SARibbonCategory::removePanel(SARibbonPanel* panel)
{
    return (d_ptr->removePanel(panel));
}

/**
 * \if ENGLISH
 * @brief Remove panel
 * @param index Index of panel, returns false if it exceeds the limit
 * @return Returns true on success
 * \endif
 *
 * \if CHINESE
 * @brief 移除panel
 * @param index panel的索引，如果超出会返回false
 * @return 成功返回true
 * \endif
 */
bool SARibbonCategory::removePanel(int index)
{
    SARibbonPanel* p = panelByIndex(index);

    if (nullptr == p) {
        return (false);
    }
    return (removePanel(p));
}

/**
 * \if ENGLISH
 * @brief Return all panels under Category
 * @return List of all panels in the category
 * \endif
 *
 * \if CHINESE
 * @brief 返回Category下的所有panel
 * @return 分类中所有panel的列表
 * \endif
 */
QList< SARibbonPanel* > SARibbonCategory::panelList() const
{
    return (d_ptr->panelList());
}

QSize SARibbonCategory::sizeHint() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->sizeHint();
    }
    return QSize(1000, 100);
}

/**
 * \if ENGLISH
 * @brief Returns true if it is a ContextCategory
 * @return True if this is a context category, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 如果是ContextCategory，此函数返回true
 * @return 如果是上下文分类返回true，否则返回false
 * \endif
 */
bool SARibbonCategory::isContextCategory() const
{
    return (d_ptr->isContextCategory);
}

/**
 * \if ENGLISH
 * @brief Return the number of panels
 * @return Number of panels in the category
 * \endif
 *
 * \if CHINESE
 * @brief 返回panel的个数
 * @return 分类中panel的数量
 * \endif
 */
int SARibbonCategory::panelCount() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelCount();
    }
    return -1;
}

/**
 * \if ENGLISH
 * @brief Determine if customization is allowed
 * @return True if customization is allowed, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 判断是否可以自定义
 * @return 如果允许自定义返回true，否则返回false
 * \endif
 */
bool SARibbonCategory::isCanCustomize() const
{
    return (d_ptr->isCanCustomize);
}

/**
 * \if ENGLISH
 * @brief Set whether customization is allowed
 * @param b True to allow customization, false to disallow
 * \endif
 *
 * \if CHINESE
 * @brief 设置是否可以自定义
 * @param b 为true允许自定义，为false禁止自定义
 * \endif
 */
void SARibbonCategory::setCanCustomize(bool b)
{
    d_ptr->isCanCustomize = b;
}

/**
 * \if ENGLISH
 * @brief Height of panel title bar
 * @return Current height of panel title bars
 * \endif
 *
 * \if CHINESE
 * @brief panel标题栏的高度
 * @return 当前panel标题栏的高度
 * \endif
 */
int SARibbonCategory::panelTitleHeight() const
{
    return d_ptr->panelTitleHeight;
}

/**
 * \if ENGLISH
 * @brief Set the height of panel
 * @param h New height for panel title bars
 * \endif
 *
 * \if CHINESE
 * @brief 设置panel的高度
 * @param h panel标题栏的新高度
 * \endif
 */
void SARibbonCategory::setPanelTitleHeight(int h)
{
    d_ptr->panelTitleHeight = h;
    iteratePanel([ h ](SARibbonPanel* p) -> bool {
        p->setTitleHeight(h);
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Whether the panel title bar is displayed
 * @return True if panel title bars are displayed, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 是否panel显示标题栏
 * @return 如果显示panel标题栏返回true，否则返回false
 * \endif
 */
bool SARibbonCategory::isEnableShowPanelTitle() const
{
    return d_ptr->enableShowPanelTitle;
}

/**
 * \if ENGLISH
 * @brief Set to display panel title
 * @param on True to display panel title bars, false to hide them
 * \endif
 *
 * \if CHINESE
 * @brief 设置显示panel标题
 * @param on 为true显示panel标题栏，为false隐藏它们
 * \endif
 */
void SARibbonCategory::setEnableShowPanelTitle(bool on)
{
    d_ptr->enableShowPanelTitle = on;
    iteratePanel([ on ](SARibbonPanel* p) -> bool {
        p->setEnableShowTitle(on);
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Get the corresponding ribbonbar, returns null if not managed by ribbonbar
 * @return Returns null if not managed by ribbonbar
 * \endif
 *
 * \if CHINESE
 * @brief 获取对应的ribbonbar，如果没有加入ribbonbar的管理，此值为null
 * @return 如果没有加入ribbonbar的管理，此值为null
 * \endif
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
 * \if ENGLISH
 * @brief Refresh the category layout, suitable for calling after changing the ribbon mode
 * \endif
 *
 * \if CHINESE
 * @brief 刷新category的布局，适用于改变ribbon的模式之后调用
 * \endif
 */
void SARibbonCategory::updateItemGeometry()
{
#if SARIBBONCATEGORY_DEBUG_PRINT
    qDebug() << "SARibbonCategory name=" << categoryName() << " updateItemGeometry";
#endif
    d_ptr->updateItemGeometry();
}

/**
 * \if ENGLISH
 * @brief Set whether to use animation when scrolling
 * @param useAnimating True to use animation, false to scroll directly
 * \endif
 *
 * \if CHINESE
 * @brief 设置滚动时是否使用动画
 * @param useAnimating 为true使用动画，为false直接滚动
 * \endif
 */
void SARibbonCategory::setUseAnimatingScroll(bool useAnimating)
{
    d_ptr->isUseAnimating = useAnimating;
}

/**
 * \if ENGLISH
 * @brief Whether to use animation when scrolling
 * @return True if animation is used, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 滚动时是否使用动画
 * @return 如果使用动画返回true，否则返回false
 * \endif
 */
bool SARibbonCategory::isUseAnimatingScroll() const
{
    return d_ptr->isUseAnimating;
}

/**
 * \if ENGLISH
 * @brief Set wheel scroll step (px)
 * @param step Scroll step in pixels
 * \endif
 *
 * \if CHINESE
 * @brief 设置滚轮滚动步长（px）
 * @param step 以像素为单位的滚动步长
 * \endif
 */
void SARibbonCategory::setWheelScrollStep(int step)
{
    d_ptr->wheelScrollStep = qMax(10, step);  // 最小10像素
}

/**
 * \if ENGLISH
 * @brief Wheel scroll step
 * @return Current scroll step in pixels
 * \endif
 *
 * \if CHINESE
 * @brief 滚轮的滚动步长
 * @return 当前以像素为单位的滚动步长
 * \endif
 */
int SARibbonCategory::wheelScrollStep() const
{
    return d_ptr->wheelScrollStep;
}

/**
 * \if ENGLISH
 * @brief Set animation duration
 * @param duration Animation duration in milliseconds
 * \endif
 *
 * \if CHINESE
 * @brief 设置动画持续时间
 * @param duration 以毫秒为单位的动画持续时间
 * \endif
 */
void SARibbonCategory::setAnimationDuration(int duration)
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        lay->setAnimationDuration(duration);
    }
}

/**
 * \if ENGLISH
 * @brief Animation duration
 * @return Current animation duration in milliseconds, or -1 if not available
 * \endif
 *
 * \if CHINESE
 * @brief 动画持续时间
 * @return 当前以毫秒为单位的动画持续时间，如果不可用则返回-1
 * \endif
 */
int SARibbonCategory::animationDuration() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->animationDuration();
    }
    return -1;
}

/**
 * \if ENGLISH
 * @brief Determine whether the text of panel is allowed to wrap
 * @return True if text wrapping is enabled, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 判断panel的文字是否允许换行
 * @return 如果启用文字换行返回true，否则返回false
 * \endif
 */
bool SARibbonCategory::isEnableWordWrap() const
{
    return d_ptr->enableWordWrap;
}

/**
 * \if ENGLISH
 * @brief Set whether the text of panel buttons is allowed to wrap
 * @param on True to enable text wrapping, false to disable
 * \endif
 *
 * \if CHINESE
 * @brief 设置panel的按钮文字允许换行
 * @param on 为true启用文字换行，为false禁用
 * \endif
 */
void SARibbonCategory::setEnableWordWrap(bool on)
{
    d_ptr->enableWordWrap = on;
    iteratePanel([ on ](SARibbonPanel* panel) -> bool {
        if (panel) {
            panel->setEnableWordWrap(on);
        }
        return true;
    });
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Set button maximum aspect ratio, this coefficient determines the maximum width of the button
 *
 * The maximum width of the button is button height * this coefficient. For example, if the button height is h,
 * then the maximum button width maxw = h * buttonMaximumAspectRatio
 * If the text cannot be fully displayed at this width, the button will not continue to expand horizontally,
 * and ... will be used instead of the incompletely displayed text
 *
 * @see buttonMaximumAspectRatio
 *
 * @note Users should not call @ref SARibbonCategory::setButtonMaximumAspectRatio to set,
 * but call @ref SARibbonBar::setButtonMaximumAspectRatio to set the aspect ratio
 *
 * @param fac New maximum aspect ratio
 * \endif
 *
 * \if CHINESE
 * @brief 设置按钮最大宽高比，这个系数决定按钮的最大宽度
 *
 * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
 * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
 *
 * @see buttonMaximumAspectRatio
 *
 * @note 用户不应该调用@ref SARibbonCategory::setButtonMaximumAspectRatio 来设置，
 * 而是调用@ref SARibbonBar::setButtonMaximumAspectRatio 设置宽高比
 *
 * @param fac 新的最大宽高比
 * \endif
 */
void SARibbonCategory::setButtonMaximumAspectRatio(qreal fac)
{
    d_ptr->buttonMaximumAspectRatio = fac;
    iteratePanel([ fac ](SARibbonPanel* panel) -> bool {
        if (panel) {
            panel->setButtonMaximumAspectRatio(fac);
        }
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Button maximum aspect ratio, this coefficient determines the maximum width of the button
 * @return Current button maximum aspect ratio
 * @see setButtonMaximumAspectRatio
 * \endif
 *
 * \if CHINESE
 * @brief 按钮最大宽高比，这个系数决定按钮的最大宽度
 * @return 当前按钮最大宽高比
 * @see setButtonMaximumAspectRatio
 * \endif
 */
qreal SARibbonCategory::buttonMaximumAspectRatio() const
{
    return d_ptr->buttonMaximumAspectRatio;
}

/**
 * \if ENGLISH
 * @brief This function will iterate through all panels under Category and execute the function pointer
 * @param fp Function pointer returns false to stop iteration
 * @return Returns false indicating that all categories have not been iterated. The iteration was interrupted by
 * receiving a false return from the callback function
 * \endif
 *
 * \if CHINESE
 * @brief 此函数会遍历Category下的所有panel,执行函数指针
 * @param fp 函数指针返回false则停止迭代
 * @return 返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
 * \endif
 */
bool SARibbonCategory::iteratePanel(FpPanelIterate fp) const
{
    const QList< SARibbonPanel* > ps = panelList();
    for (SARibbonPanel* p : ps) {
        if (!fp(p)) {
            return false;
        }
    }
    return true;
}

/**
 * \if ENGLISH
 * @brief Set the alignment of Category
 * @param al Alignment for the category
 * \endif
 *
 * \if CHINESE
 * @brief 设置Category的对齐方式
 * @param al 分类的对齐方式
 * \endif
 */
void SARibbonCategory::setCategoryAlignment(SARibbonAlignment al)
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(layout());
    if (lay) {
        lay->setCategoryAlignment(al);
    }
}

/**
 * \if ENGLISH
 * @brief Category alignment
 * @return Current alignment of the category
 * \endif
 *
 * \if CHINESE
 * @brief Category的对齐方式
 * @return 分类的当前对齐方式
 * \endif
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
 * \if ENGLISH
 * @brief Set the spacing of panel
 * @param n Spacing between panels
 * \endif
 *
 * \if CHINESE
 * @brief 设置panel的spacing
 * @param n panel之间的间距
 * \endif
 */
void SARibbonCategory::setPanelSpacing(int n)
{
    d_ptr->panelSpacing = n;
    iteratePanel([ n ](SARibbonPanel* panel) -> bool {
        if (panel) {
            panel->setSpacing(n);
        }
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief panel spacing
 * @return Current spacing between panels
 * \endif
 *
 * \if CHINESE
 * @brief panel的spacing
 * @return 当前panel之间的间距
 * \endif
 */
int SARibbonCategory::panelSpacing() const
{
    return d_ptr->panelSpacing;
}

/**
 * \if ENGLISH
 * @brief Set the large button icon size of panel
 * @param largeSize New large icon size for panel buttons
 * \endif
 *
 * \if CHINESE
 * @brief 设置panel的大按钮图标尺寸
 * @param largeSize 新的panel大按钮图标尺寸
 * \endif
 */
void SARibbonCategory::setPanelLargeIconSize(const QSize& largeSize)
{
    d_ptr->panelLargeToolButtonIconSize = largeSize;
    iteratePanel([ largeSize ](SARibbonPanel* panel) -> bool {
        if (panel) {
            panel->setLargeIconSize(largeSize);
        }
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Large button icon size of panel
 * @return Current large icon size for panel buttons
 * \endif
 *
 * \if CHINESE
 * @brief panel的大按钮图标尺寸
 * @return 当前panel大按钮图标尺寸
 * \endif
 */
QSize SARibbonCategory::panelLargeIconSize() const
{
    return d_ptr->panelLargeToolButtonIconSize;
}

/**
 * \if ENGLISH
 * @brief Set the small button icon size of panel
 * @param smallSize New small icon size for panel buttons
 * \endif
 *
 * \if CHINESE
 * @brief 设置panel的小按钮图标尺寸
 * @param smallSize 新的panel小按钮图标尺寸
 * \endif
 */
void SARibbonCategory::setPanelSmallIconSize(const QSize& smallSize)
{
    d_ptr->panelSmallToolButtonIconSize = smallSize;
    iteratePanel([ smallSize ](SARibbonPanel* panel) -> bool {
        if (panel) {
            panel->setSmallIconSize(smallSize);
        }
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Small button icon size of panel
 * @return Current small icon size for panel buttons
 * \endif
 *
 * \if CHINESE
 * @brief panel的小按钮图标尺寸
 * @return 当前panel小按钮图标尺寸
 * \endif
 */
QSize SARibbonCategory::panelSmallIconSize() const
{
    return d_ptr->panelSmallToolButtonIconSize;
}

/**
 * \if ENGLISH
 * @brief Set the icon size of panel buttons, large action is not affected
 * @param smallSize New small icon size for panel buttons
 * @param largeSize New large icon size for panel buttons
 * \endif
 *
 * \if CHINESE
 * @brief 设置panel按钮的icon尺寸，large action不受此尺寸影响
 * @param smallSize panel按钮的新小图标大小
 * @param largeSize panel按钮的新大图标大小
 * \endif
 */
void SARibbonCategory::setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize)
{
    d_ptr->panelSmallToolButtonIconSize = smallSize;
    d_ptr->panelLargeToolButtonIconSize = largeSize;
    iteratePanel([ smallSize, largeSize ](SARibbonPanel* panel) -> bool {
        if (panel) {
            panel->setToolButtonIconSize(smallSize, largeSize);
        }
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Icon size of panel buttons, large action is not affected
 * @return Current icon size for panel buttons (small size, large size)
 * \endif
 *
 * \if CHINESE
 * @brief panel按钮的icon尺寸，large action不受此尺寸影响
 * @return 当前panel按钮的图标大小（小图标尺寸，大图标尺寸）
 * \endif
 */
QPair< QSize, QSize > SARibbonCategory::panelToolButtonIconSize() const
{
    return qMakePair(d_ptr->panelSmallToolButtonIconSize, d_ptr->panelLargeToolButtonIconSize);
}

/**
 * \if ENGLISH
 * @brief Wheel event
 *
 * When content exceeds the width of the category, the wheel can scroll the panel
 * @param event Wheel event object
 * \endif
 *
 * \if CHINESE
 * @brief 滚动事件
 *
 * 在内容超出category的宽度情况下，滚轮可滚动panel
 * @param event 滚轮事件对象
 * \endif
 */
void SARibbonCategory::wheelEvent(QWheelEvent* event)
{
    d_ptr->doWheelEvent(event);
}

/**
 * \if ENGLISH
 * @brief Change event handler
 * @param event Change event object
 * \endif
 *
 * \if CHINESE
 * @brief 变更事件处理函数
 * @param event 变更事件对象
 * \endif
 */
void SARibbonCategory::changeEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::StyleChange: {
        if (layout()) {
#if SARIBBONCATEGORY_DEBUG_PRINT
            qDebug() << "SARibbonCategory changeEvent(StyleChange),categoryName=" << categoryName();
#endif
            layout()->invalidate();
        }
    } break;
    case QEvent::LayoutDirectionChange: {
        // 布局方向改变（如 LTR→RTL），失效布局并触发重绘
        if (layout()) {
            layout()->invalidate();
        }
        update();
    } break;
    case QEvent::FontChange: {
#if SARIBBONCATEGORY_DEBUG_PRINT
        qDebug() << "SARibbonCategory changeEvent(FontChange),categoryName=" << categoryName();
#endif
        QFont f = font();
        iteratePanel([ f ](SARibbonPanel* p) -> bool {
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
 * \if ENGLISH
 * @brief Mark this as a context label
 * @param isContextCategory True to mark as context category, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 标记这个是上下文标签
 * @param isContextCategory 为true标记为上下文分类，否则为false
 * \endif
 */
void SARibbonCategory::markIsContextCategory(bool isContextCategory)
{
    d_ptr->isContextCategory = isContextCategory;
}

/**
 * \if ENGLISH
 * @brief Get SARibbonCategoryLayout
 * @return Pointer to the category layout, or nullptr if not available
 * \endif
 *
 * \if CHINESE
 * @brief 获取SARibbonCategoryLayout
 * @return 指向分类布局的指针，如果不可用则返回nullptr
 * \endif
 */
SARibbonCategoryLayout* SARibbonCategory::categoryLayout() const
{
    return qobject_cast< SARibbonCategoryLayout* >(layout());
}

/**
 * \if ENGLISH
 * @brief Event handler
 * @param e Event object
 * @return True if the event was handled, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 事件处理函数
 * @param e 事件对象
 * @return 如果事件被处理返回true，否则返回false
 * \endif
 */
bool SARibbonCategory::event(QEvent* e)
{
#if SARIBBONCATEGORY_DEBUG_PRINT
    if (e->type() != QEvent::Paint) {
        qDebug() << "SARibbonCategory event(" << e->type() << "),name=" << categoryName();
    }
#endif
    return QWidget::event(e);
}

//===================================================
// SARibbonCategoryScrollButton
//===================================================

/**
 * \if ENGLISH
 * @brief Constructor with arrow type and optional parent
 * @param arr Arrow type for the button
 * @param p Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 带箭头类型和可选父级的构造函数
 * @param arr 按钮的箭头类型
 * @param p 父级控件
 * \endif
 */
SARibbonCategoryScrollButton::SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p) : QToolButton(p)
{
    setArrowType(arr);
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
SARibbonCategoryScrollButton::~SARibbonCategoryScrollButton()
{
}
