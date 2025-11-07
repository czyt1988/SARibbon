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
 * @brief 带名称的构造函数
 * @param name Category名称
 * @param p 父级控件指针
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
 * @brief Get the category name/获取Category名称
 * @return Current category name (i.e., windowTitle)/当前Category的名称（即windowTitle）
 */
QString SARibbonCategory::categoryName() const
{
    return (windowTitle());
}

/**
 * @brief Set the category name/设置Category名称
 * @param title New name (equivalent to setWindowTitle)/新名称（等价于setWindowTitle）
 */
void SARibbonCategory::setCategoryName(const QString& title)
{
    setWindowTitle(title);
}

/**
 * @brief Get the panel layout mode/获取面板布局模式
 * @return Current layout mode for all panels/当前所有面板的布局模式
 */
SARibbonPanel::PanelLayoutMode SARibbonCategory::panelLayoutMode() const
{
    return (d_ptr->defaultPanelLayoutMode);
}

/**
 * @brief Set the panel layout mode/设置面板布局模式
 *
 *  * When @ref SARibbonBar calls the @ref SARibbonBar::setRibbonStyle function, this function will be called for all
 * SARibbonCategory objects to set the new SARibbonPanel::PanelLayoutMode
 *
 * 在@ref SARibbonBar 调用@ref SARibbonBar::setRibbonStyle 函数时，会对所有的SARibbonCategory调用此函数
 * 把新的SARibbonPanel::PanelLayoutMode设置进去
 * @param m
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
 * @brief Add a panel (panel)/添加面板(panel)
 *
 * @note The ownership of the panel (panel) is managed by SARibbonCategory, please do not destroy it
 * externally/面板(panel)的所有权由SARibbonCategory来管理，请不要在外部对其进行销毁
 *
 * @param title Panel (panel) title, which will be displayed below the panel (panel) in the three-row mode of
 * office/wps / 面板(panel)的标题，在office/wps的三行模式下会显示在面板(panel)的下方
 *
 * @return Returns a pointer to the generated @ref SARibbonPanel / 返回生成的@ref SARibbonPanel 指针
 *
 * @see For other operations on SARibbonPanel, refer to @ref SARibbonCategory::takePanel
 *
 * 对SARibbonPanel的其他操作，参考 @ref SARibbonCategory::takePanel
 */
SARibbonPanel* SARibbonCategory::addPanel(const QString& title)
{
    return (d_ptr->addPanel(title));
}

/**
 * @brief Add panel/添加panel
 * @param panel panel ownership is managed by SARibbonCategory
 * panel的所有权SARibbonCategory来管理
 */
void SARibbonCategory::addPanel(SARibbonPanel* panel)
{
    d_ptr->addPanel(panel);
}

/**
 * @brief Qt Designer specific/Qt Designer专用
 * @param panel
 */
void SARibbonCategory::addPanel(QWidget* panel)
{
    SARibbonPanel* p = qobject_cast< SARibbonPanel* >(panel);
    if (p) {
        addPanel(p);
    }
}

/**
 * @brief Create a new panel and insert it at the index position/新建一个panel，并插入到index位置
 * @param title panel title/panel的title
 * @param index Insertion position, if index exceeds the number of panels in the category, it will be inserted at the
 * end/插入的位置，如果index超出category里panel的个数，将插入到最后
 * @return Returns a pointer to the generated @ref SARibbonPanel
 */
SARibbonPanel* SARibbonCategory::insertPanel(const QString& title, int index)
{
    return (d_ptr->insertPanel(title, index));
}

/**
 * @brief Find panel by name/通过名字查找panel
 * @param title
 * @return If there are duplicate names, only the first one that meets the condition will be returned/如果有重名，只会返回第一个符合条件的
 */
SARibbonPanel* SARibbonCategory::panelByName(const QString& title) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelByName(title);
    }
    return nullptr;
}

/**
 * @brief Find panel by ObjectName/通过ObjectName查找panel
 * @param objname
 * @return
 */
SARibbonPanel* SARibbonCategory::panelByObjectName(const QString& objname) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelByObjectName(objname);
    }
    return nullptr;
}

/**
 * @brief Find panel by index, returns nullptr if the index is out of range/通过索引找到panel，如果超过索引范围，会返回nullptr
 * @param index
 * @return Returns nullptr if the index is out of range/如果超过索引范围，会返回nullptr
 */
SARibbonPanel* SARibbonCategory::panelByIndex(int index) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelByIndex(index);
    }
    return nullptr;
}

/**
 * @brief Find the index corresponding to panel/查找panel对应的索引
 * @param p
 * @return Returns -1 if not found/如果找不到，返回-1
 */
int SARibbonCategory::panelIndex(SARibbonPanel* p) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelIndex(p);
    }
    return -1;
}

/**
 * @brief Move a Panel from from index to to index/移动一个Panel从from index到to index
 * @param from Index of the panel to move/要移动panel的index
 * @param to Position to move to/要移动到的位置
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
 * @brief Detach panel from SARibbonCategory management/把panel脱离SARibbonCategory的管理
 * @param panel Panel to extract/需要提取的panel
 * @return Returns true on success, otherwise false/成功返回true，否则返回false
 */
bool SARibbonCategory::takePanel(SARibbonPanel* panel)
{
    return (d_ptr->takePanel(panel));
}

/**
 * @brief Remove Panel, Category will directly recycle SARibbonPanel memory/移除Panel，Category会直接回收SARibbonPanel内存
 * @param panel Panel to remove/需要移除的panel
 * @note After removal, panel becomes a wild pointer. It is generally recommended to set the panel pointer to nullptr after the operation
 *
 * This operation is equivalent to:
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
 * 移除后panel为野指针，一般操作完建议把panel指针设置为nullptr
 */
bool SARibbonCategory::removePanel(SARibbonPanel* panel)
{
    return (d_ptr->removePanel(panel));
}

/**
 * @brief Remove panel/移除panel
 * @param index Index of panel, returns false if it exceeds the limit/panel的索引，如果超出会返回false
 * @return Returns true on success/成功返回true
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
 * @brief Return all panels under Category/返回Category下的所有panel
 * @return List of all panels in the category/分类中所有panel的列表
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
 * @brief Returns true if it is a ContextCategory/如果是ContextCategory，此函数返回true
 * @return True if this is a context category, false otherwise/如果是上下文分类返回true，否则返回false
 */
bool SARibbonCategory::isContextCategory() const
{
    return (d_ptr->isContextCategory);
}

/**
 * @brief Return the number of panels/返回panel的个数
 * @return Number of panels in the category/分类中panel的数量
 */
int SARibbonCategory::panelCount() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->panelCount();
    }
    return -1;
}

/**
 * @brief Determine if customization is allowed/判断是否可以自定义
 * @return True if customization is allowed, false otherwise/如果允许自定义返回true，否则返回false
 */
bool SARibbonCategory::isCanCustomize() const
{
    return (d_ptr->isCanCustomize);
}

/**
 * @brief Set whether customization is allowed/设置是否可以自定义
 * @param b True to allow customization, false to disallow/为true允许自定义，为false禁止自定义
 */
void SARibbonCategory::setCanCustomize(bool b)
{
    d_ptr->isCanCustomize = b;
}

/**
 * @brief Height of panel title bar/panel标题栏的高度
 * @return Current height of panel title bars/当前panel标题栏的高度
 */
int SARibbonCategory::panelTitleHeight() const
{
    return d_ptr->panelTitleHeight;
}

/**
 * @brief Set the height of panel/设置panel的高度
 * @param h New height for panel title bars/panel标题栏的新高度
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
 * @brief Whether the panel title bar is displayed/是否panel显示标题栏
 * @return True if panel title bars are displayed, false otherwise/如果显示panel标题栏返回true，否则返回false
 */
bool SARibbonCategory::isEnableShowPanelTitle() const
{
    return d_ptr->enableShowPanelTitle;
}

/**
 * @brief Set to display panel title/设置显示panel标题
 * @param on True to display panel title bars, false to hide them/为true显示panel标题栏，为false隐藏它们
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
 * @brief Get the corresponding ribbonbar, returns null if not managed by ribbonbar/获取对应的ribbonbar，如果没有加入ribbonbar的管理，此值为null
 * @return Returns null if not managed by ribbonbar/如果没有加入ribbonbar的管理，此值为null
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
 * @brief Refresh the category layout, suitable for calling after changing the ribbon mode/刷新category的布局，适用于改变ribbon的模式之后调用
 */
void SARibbonCategory::updateItemGeometry()
{
#if SARIBBONCATEGORY_DEBUG_PRINT
    qDebug() << "SARibbonCategory name=" << categoryName() << " updateItemGeometry";
#endif
    d_ptr->updateItemGeometry();
}

/**
 * @brief Set whether to use animation when scrolling/设置滚动时是否使用动画
 * @param useAnimating True to use animation, false to scroll directly/为true使用动画，为false直接滚动
 */
void SARibbonCategory::setUseAnimatingScroll(bool useAnimating)
{
    d_ptr->isUseAnimating = useAnimating;
}

/**
 * @brief Whether to use animation when scrolling/滚动时是否使用动画
 * @return True if animation is used, false otherwise/如果使用动画返回true，否则返回false
 */
bool SARibbonCategory::isUseAnimatingScroll() const
{
    return d_ptr->isUseAnimating;
}

/**
 * @brief Set wheel scroll step (px)/设置滚轮滚动步长（px）
 * @param step Scroll step in pixels/以像素为单位的滚动步长
 */
void SARibbonCategory::setWheelScrollStep(int step)
{
    d_ptr->wheelScrollStep = qMax(10, step);  // 最小10像素
}

/**
 * @brief Wheel scroll step/滚轮的滚动步长
 * @return Current scroll step in pixels/当前以像素为单位的滚动步长
 */
int SARibbonCategory::wheelScrollStep() const
{
    return d_ptr->wheelScrollStep;
}

/**
 * @brief Set animation duration/设置动画持续时间
 * @param duration Animation duration in milliseconds/以毫秒为单位的动画持续时间
 */
void SARibbonCategory::setAnimationDuration(int duration)
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        lay->setAnimationDuration(duration);
    }
}

/**
 * @brief Animation duration/动画持续时间
 * @return Current animation duration in milliseconds, or -1 if not available/当前以毫秒为单位的动画持续时间，如果不可用则返回-1
 */
int SARibbonCategory::animationDuration() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->animationDuration();
    }
    return -1;
}

/**
 * @brief Determine whether the text of panel is allowed to wrap/判断panel的文字是否允许换行
 * @return True if text wrapping is enabled, false otherwise/如果启用文字换行返回true，否则返回false
 */
bool SARibbonCategory::isEnableWordWrap() const
{
    return d_ptr->enableWordWrap;
}

/**
 * @brief Set whether the text of panel buttons is allowed to wrap/设置panel的按钮文字允许换行
 * @param on True to enable text wrapping, false to disable/为true启用文字换行，为false禁用
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
 * @brief Set button maximum aspect ratio, this coefficient determines the maximum width of the button/设置按钮最大宽高比，这个系数决定按钮的最大宽度
 *
 * The maximum width of the button is button height * this coefficient. For example, if the button height is h,
 * then the maximum button width maxw = h * buttonMaximumAspectRatio
 * If the text cannot be fully displayed at this width, the button will not continue to expand horizontally,
 * and ... will be used instead of the incompletely displayed text
 *
 * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
 * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
 *
 * @see buttonMaximumAspectRatio
 *
 * @note Users should not call @ref SARibbonCategory::setButtonMaximumAspectRatio to set,
 * but call @ref SARibbonBar::setButtonMaximumAspectRatio to set the aspect ratio
 *
 * 用户不应该调用@ref SARibbonCategory::setButtonMaximumAspectRatio 来设置，
 * 而是调用@ref SARibbonBar::setButtonMaximumAspectRatio 设置宽高比
 *
 * @param fac New maximum aspect ratio/新的最大宽高比
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
 * @brief Button maximum aspect ratio, this coefficient determines the maximum width of the button/按钮最大宽高比，这个系数决定按钮的最大宽度
 * @return Current button maximum aspect ratio/当前按钮最大宽高比
 * @see setButtonMaximumAspectRatio
 */
qreal SARibbonCategory::buttonMaximumAspectRatio() const
{
    return d_ptr->buttonMaximumAspectRatio;
}

/**
 * @brief This function will iterate through all panels under Category and execute the function pointer/此函数会遍历Category下的所有panel,执行函数指针
 * @param fp Function pointer returns false to stop iteration/函数指针返回false则停止迭代
 * @return Returns false indicating that all categories have not been iterated. The iteration was interrupted by receiving
 * a false return from the callback function/返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
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
 * @brief Set the alignment of Category/设置Category的对齐方式
 * @param al Alignment for the category/分类的对齐方式
 */
void SARibbonCategory::setCategoryAlignment(SARibbonAlignment al)
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(layout());
    if (lay) {
        lay->setCategoryAlignment(al);
    }
}

/**
 * @brief Category alignment/Category的对齐方式
 * @return Current alignment of the category/分类的当前对齐方式
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
 * @brief Set the spacing of panel/设置panel的spacing
 * @param n Spacing between panels/panel之间的间距
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
 * @brief panel spacing/panel的spacing
 * @return Current spacing between panels/当前panel之间的间距
 */
int SARibbonCategory::panelSpacing() const
{
    return d_ptr->panelSpacing;
}

/**
 * @brief 设置panel的大按钮图标尺寸
 * @param largeSize
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
 * @brief panel的大按钮图标尺寸
 * @return
 */
QSize SARibbonCategory::panelLargeIconSize() const
{
    return d_ptr->panelLargeToolButtonIconSize;
}

/**
 * @brief 设置panel的小按钮图标尺寸
 * @param smallSize
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
 * @brief panel的小按钮图标尺寸
 * @return
 */
QSize SARibbonCategory::panelSmallIconSize() const
{
    return d_ptr->panelSmallToolButtonIconSize;
}

/**
 * @brief Set the icon size of panel buttons, large action is not affected/设置panel按钮的icon尺寸，large action不受此尺寸影响
 * @param smallSize New icon size for panel buttons/panel按钮的新图标大小
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
 * @brief Icon size of panel buttons, large action is not affected/panel按钮的icon尺寸，large action不受此尺寸影响
 * @return Current icon size for panel buttons/当前panel按钮的图标大小
 */
QPair< QSize, QSize > SARibbonCategory::panelToolButtonIconSize() const
{
    return qMakePair(d_ptr->panelSmallToolButtonIconSize, d_ptr->panelLargeToolButtonIconSize);
}

/**
 * @brief 滚动事件
 *
 * 在内容超出category的宽度情况下，滚轮可滚动panel
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
#if SARIBBONCATEGORY_DEBUG_PRINT
            qDebug() << "SARibbonCategory changeEvent(StyleChange),categoryName=" << categoryName();
#endif
            layout()->invalidate();
        }
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
 * @brief Mark this as a context label/标记这个是上下文标签
 * @param isContextCategory True to mark as context category, false otherwise/为true标记为上下文分类，否则为false
 */
void SARibbonCategory::markIsContextCategory(bool isContextCategory)
{
    d_ptr->isContextCategory = isContextCategory;
}

/**
 * @brief Get SARibbonCategoryLayoutlayout/获取SARibbonCategoryLayoutlayout
 * @return Pointer to the category layout, or nullptr if not available/指向分类布局的指针，如果不可用则返回nullptr
 */
SARibbonCategoryLayout* SARibbonCategory::categoryLayout() const
{
    return qobject_cast< SARibbonCategoryLayout* >(layout());
}

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
 * @brief Constructor with arrow type and optional parent/带箭头类型和可选父级的构造函数
 * @param arr Arrow type for the button/按钮的箭头类型
 * @param p Parent widget/父级控件
 */
SARibbonCategoryScrollButton::SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p) : QToolButton(p)
{
    setArrowType(arr);
}

/**
 * @brief Destructor/析构函数
 */
SARibbonCategoryScrollButton::~SARibbonCategoryScrollButton()
{
}
