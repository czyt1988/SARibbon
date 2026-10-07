#ifndef RIBBONBAR_H
#define RIBBONBAR_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlQuickHost.h"
#include "SARibbonQmlTypes.h"
// the single QAction include face of the module (contract D5, plan-05 S1);
// full type: the shortcut collection API carries QAction*; RibbonAction is
// the applicationMenuActions QQmlListProperty template parameter
#include "SARibbonQmlActionCompat.h"
#include "SARibbonQmlAction.h"
// full definitions, not forward declarations: the pointer types below appear
// in Q_PROPERTY / signal signatures, so
// the moc output instantiates QMetaType::fromType and silently loses the
// QObject specialization if the types are incomplete where that moc file
// happens to be compiled (NOTES B64)
#include "SARibbonQmlCategory.h"
// full definition: the windowAgent Q_PROPERTY needs the type complete in
// every TU including this header (moc pointer metatype, same B64 family)
#include "SARibbonQmlWindowAgent.h"
#include <SARibbonCore/SARibbonBarGeometryEngine.h>
#include <SARibbonCore/SARibbonToolButtonLayout.h>
#include <QHash>
#include <QQmlListProperty>
#include <QRectF>
#include <QVariantList>
#include <QVector>

namespace SARibbonQml {

class RibbonCategory;
class RibbonTab;
class RibbonContextCategory;
class RibbonQuickAccessBar;
class RibbonButtonGroup;
class RibbonMenuModel;
class RibbonApplicationWindow;
class RibbonWindowAgent;
class RibbonShortcutMatcher;

/**
 * \if ENGLISH
 * @brief Bar structural host (plan-04 S4)
 * @details Bar height from RibbonMetrics; the tab row is laid out by this
 * host (plain sequence, no Repeater — geometry authority stays in C++).
 * Categories declared without a matching RibbonTab get an auto tab whose
 * text follows the category title (the QML counterpart of the widgets
 * addCategoryPage pairing); clicking a tab — host-side mouse handling —
 * switches currentIndex which shows the paired category. The optional
 * application button (label + geometry + click signal, office-2021 look)
 * is rendered by the bar leaf from the rect published here.
 * \endif
 *
 * \if CHINESE
 * @brief Bar 结构宿主（计划 04 S4）
 * @details bar 高度取自 RibbonMetrics；tab 行由本宿主排布（平铺序列，不用
 *          Repeater——几何权威留在 C++）。声明时未配对 RibbonTab 的 category
 *          会得到一个自动 tab（文字跟随 category 标题，对应 widgets 侧
 *          addCategoryPage 的配对语义）；点击 tab（宿主侧鼠标处理）切换
 *          currentIndex 并显示配对的 category。可选的应用按钮（文字+几何+
 *          点击信号，office-2021 外观）由 bar 叶子按本宿主发布的矩形渲染。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonBar : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString applicationLabel READ applicationLabel WRITE setApplicationLabel NOTIFY applicationLabelChanged)
    Q_PROPERTY(RibbonEnums::RibbonStyle ribbonStyle READ ribbonStyle WRITE setRibbonStyle NOTIFY ribbonStyleChanged)
    Q_PROPERTY(RibbonEnums::Alignment tabAlignment READ tabAlignment WRITE setTabAlignment NOTIFY tabAlignmentChanged)
    Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode NOTIFY minimumModeChanged)
    Q_PROPERTY(qreal buttonMaximumAspectRatio READ buttonMaximumAspectRatio WRITE setButtonMaximumAspectRatio NOTIFY buttonMaximumAspectRatioChanged)
    Q_PROPERTY(qreal largeButtonMinimumWidthRatio READ largeButtonMinimumWidthRatio WRITE setLargeButtonMinimumWidthRatio NOTIFY largeButtonMinimumWidthRatioChanged)
    Q_PROPERTY(int systemButtonStripWidth READ systemButtonStripWidth WRITE setSystemButtonStripWidth NOTIFY systemButtonStripWidthChanged)
    Q_PROPERTY(int tabBarHeight READ tabBarHeight NOTIFY layoutChanged)
    Q_PROPERTY(int titleBarHeight READ titleBarHeight NOTIFY layoutChanged)
    Q_PROPERTY(int categoryRowY READ categoryRowY NOTIFY layoutChanged)
    Q_PROPERTY(QRectF applicationButtonRect READ applicationButtonRect NOTIFY layoutChanged)
    Q_PROPERTY(QVariantList contextBands READ contextBands NOTIFY layoutChanged)
    Q_PROPERTY(int minimumWidth READ minimumWidth NOTIFY minimumWidthChanged)
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonAction > applicationMenuActions READ applicationMenuActions NOTIFY applicationMenuActionsChanged)
    Q_PROPERTY(QVariantList applicationMenuModel READ applicationMenuModel NOTIFY applicationMenuModelChanged)
    Q_PROPERTY(bool hasApplicationMenu READ hasApplicationMenu NOTIFY applicationMenuModelChanged)
    Q_PROPERTY(QQuickItem* applicationWindowItem READ applicationWindowItem NOTIFY applicationWindowChanged)
    Q_PROPERTY(bool hasApplicationWindow READ hasApplicationWindow NOTIFY applicationWindowChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle NOTIFY windowTitleChanged)
    Q_PROPERTY(bool framelessActive READ isFramelessActive NOTIFY framelessActiveChanged)
    Q_PROPERTY(SARibbonQml::RibbonWindowAgent* windowAgent READ windowAgent WRITE setWindowAgent NOTIFY windowAgentChanged)
public:
    explicit RibbonBar(QQuickItem* parent = nullptr);
    ~RibbonBar() override;

    int currentIndex() const;
    void setCurrentIndex(int idx);

    QString applicationLabel() const;
    void setApplicationLabel(const QString& label);

    // Six styles (Loose/Compact x 3/2/1 rows) mirroring the widgets
    // SARibbonBar::RibbonStyleFlag values; propagation follows widgets
    // setRibbonStyle: compact = tabs on the title row, single-row hides panel
    // titles + enables icon-right-text, three-row enables word wrap
    RibbonEnums::RibbonStyle ribbonStyle() const;
    void setRibbonStyle(RibbonEnums::RibbonStyle style);

    // Tab row alignment inside the free strip (widgets setRibbonAlignment
    // parity: left = after the app button, center/right = shifted)
    RibbonEnums::Alignment tabAlignment() const;
    void setTabAlignment(RibbonEnums::Alignment alignment);

    // Minimum (collapsed) mode: the category row hides, only title + tabs
    // remain (widgets setMinimumMode parity)
    bool isMinimumMode() const;
    void setMinimumMode(bool on);

    // Button width tuning pushed down the whole host tree (widgets
    // SARibbonBar::setButtonMaximumAspectRatio parity — the documented entry
    // point; the category/panel/button copies only relay it)
    qreal buttonMaximumAspectRatio() const;
    void setButtonMaximumAspectRatio(qreal fac);
    qreal largeButtonMinimumWidthRatio() const;
    void setLargeButtonMinimumWidthRatio(qreal fac);

    // Reserved right edge for frameless window system buttons (min/max/
    // close). Native-frame windows need no reservation, so the default is 0
    // and the right button group hugs the window edge (widgets
    // resizeInLooseStyle/resizeInCompactStyle only subtract the system strip
    // when isUseRibbonFrame() is on). A declared RibbonWindowAgent child
    // overrides this automatically with the actual system button strip width
    int systemButtonStripWidth() const;
    void setSystemButtonStripWidth(int w);

    // Window title text the leaf paints into the title free area (widgets
    // SARibbonBar::paintWindowTitle parity; the source is the attached
    // QQuickWindow's title, kept in sync through windowTitleChanged)
    QString windowTitle() const;
    void setWindowTitle(const QString& title);

    // True while the frameless agent is attached to a window and enabled;
    // the leaf renders the system button row and hit-test registration runs
    bool isFramelessActive() const;

    // The frameless agent (optional; declare one through the `windowAgent`
    // property: `windowAgent: RibbonWindowAgent { }` — setup wires this bar
    // as the title bar and syncs the strip reservation)
    RibbonWindowAgent* windowAgent() const;
    void setWindowAgent(RibbonWindowAgent* agent);

    // Register a QML-rendered system button (kind: minimize/maximize/close)
    // with the frameless agent; no-op without an agent
    Q_INVOKABLE bool setSystemButton(const QString& kind, QQuickItem* item);

    // layout values consumed by the visual leaf (re-published on relayout)
    int tabBarHeight() const;
    int titleBarHeight() const;
    int categoryRowY() const;
    QRectF applicationButtonRect() const;

    // Auto-computed minimum width of the hosting window (core
    // SARibbonBarGeometryEngine::calcMinimumWidth): the no-overlap width of
    // the title-row / tab-row content (application button, quick access bar,
    // effective tab row, right button group, system button strip) plus the
    // window title, capped by the screen rules — never above 2/3 of the
    // available screen width; the title is sacrificed first when over the
    // cap, and beyond that overlap is accepted. Bind the hosting window:
    // `ApplicationWindow { minimumWidth: ribbonBar.minimumWidth }`. The bar
    // deliberately does not write the window property itself (QML windows
    // derive no minimum from content, and a C++ write would fight user
    // bindings)
    int minimumWidth() const;

    // Tab pairing: categories without an explicit RibbonTab at their index
    // get an auto-created tab bound to the category title
    void registerCategory(RibbonCategory* category);
    void unregisterCategory(RibbonCategory* category);

    // Registered category queries (WS-C2: the customizer addresses categories
    // through them, widgets SARibbonBar::categoryIndex/categoryByObjectName
    // parity). The queries walk the DECLARED row, hidden categories included —
    // visibility is a separate flag, exactly as on the widgets side
    int categoryCount() const;
    SARibbonQml::RibbonCategory* categoryAt(int index) const;
    int categoryIndex(RibbonCategory* category) const;
    SARibbonQml::RibbonCategory* categoryByObjectName(const QString& objName) const;

    // Runtime category creation / removal / reordering (WS-C2, widgets
    // insertCategoryPage / removeCategory / moveCategory parity). insertCategory
    // keeps the tab row paired: an all-auto tab row grows a tab at the same
    // slot, an explicitly declared tab row is left to syncTabCount
    Q_INVOKABLE SARibbonQml::RibbonCategory* insertCategory(const QString& title, int index);
    Q_INVOKABLE bool removeCategory(RibbonCategory* category);
    Q_INVOKABLE bool moveCategory(int from, int to);

    // User-driven category visibility (widgets showCategory/hideCategory
    // parity): a hidden category drops out of the effective tab row AND out of
    // the category row, so it is neither clickable nor shown
    Q_INVOKABLE void showCategory(RibbonCategory* category);
    Q_INVOKABLE void hideCategory(RibbonCategory* category);
    bool isCategoryHidden(RibbonCategory* category) const;

    // Title-row host, reachable for the customizer (quick access entries are
    // part of the customize record set)
    SARibbonQml::RibbonQuickAccessBar* quickAccessBar() const;

    // ---- shortcut fallback (plan-05 S5, contract D8-1) ----
    // The bar-level matcher owning QAction::shortcut real triggering: the
    // action-bound buttons collect into it (attach/detach/property change),
    // it filters the bar's QQuickWindow key presses. Same code path on both
    // lanes — QAction shortcuts never auto-trigger in a Quick scene
    SARibbonQml::RibbonShortcutMatcher* shortcutMatcher();
    // Called by the action-bound buttons through the parent chain
    void collectShortcut(QObject* button, QAction* action);
    void dropShortcut(QObject* button);

    void registerTab(RibbonTab* tab);
    void unregisterTab(RibbonTab* tab);

    // Context categories: registered automatically when declared as children;
    // active ones append colored tabs (one per page) after the normal tabs
    // and publish their bands through the contextBands property
    void registerContext(RibbonContextCategory* ctx);
    void unregisterContext(RibbonContextCategory* ctx);

    // Context category queries (WS-C3): the customize tree brackets a context
    // page title and hides it from the "main category" scope, which is the
    // widgets SARibbonCategory::isContextCategory test it has no QML equivalent
    // for — a QML page belongs to a context by declaration, not by a flag
    QVector< RibbonCategory* > contextCategories() const;
    bool isContextCategory(RibbonCategory* category) const;

    // title free area (engine-computed); QML side binds the window title text
    QRectF titleRect() const;

    // Active context bands for the visual leaf: list of maps
    // {x, width, title, color, highlight, textColor}; y/height derive from
    // the published tabBar/titleBar metrics (band spans them, under the tabs)
    QVariantList contextBands() const;

    // Application button menu (widgets menu-mode app button): when the list
    // is non-empty the app button click opens it; activation is mediated by
    // applicationMenuTriggered (tests drive it without a windowed popup)
    // Application menu entries as QAction objects (plan-06 S3, contract §5:
    // submenus via RibbonAction.menuActions, bare QAction entries flat-only).
    // Object literals, id references and C++ appends all land here; the leaf
    // renders the derived applicationMenuModel rows
    QQmlListProperty< SARibbonQml::RibbonAction > applicationMenuActions();
    int applicationMenuActionCount() const;
    QAction* applicationMenuActionAt(int index) const;
    // C++ append of any QAction (bare ones included)
    void addApplicationMenuAction(QAction* action);
    QVariantList applicationMenuModel() const;
    bool hasApplicationMenu() const;
    Q_INVOKABLE void activateApplicationMenuItem(int index);
    Q_INVOKABLE void activateApplicationMenuItemPath(const QVariantList& indexPath);

    // Application window (widgets ApplicationWidget mode): a
    // RibbonApplicationWindow declared as a child opens as an office-backstage
    // overlay on click (priority over the menu, coverage/animation follow the
    // item's properties); close() from inner content routes through here
    QQuickItem* applicationWindowItem() const;
    bool hasApplicationWindow() const;
    Q_INVOKABLE void requestApplicationWindowClose();

Q_SIGNALS:
    void currentIndexChanged();
    void applicationLabelChanged();
    void ribbonStyleChanged();
    void tabAlignmentChanged();
    void minimumModeChanged();
    void buttonMaximumAspectRatioChanged();
    void largeButtonMinimumWidthRatioChanged();
    void systemButtonStripWidthChanged();
    void layoutChanged();
    void minimumWidthChanged();
    void applicationButtonClicked();
    void applicationMenuActionsChanged();
    void applicationMenuModelChanged();
    void applicationMenuTriggered(QAction* item);
    void applicationWindowChanged();
    void windowTitleChanged();
    void framelessActiveChanged();
    void windowAgentChanged();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void updatePolish() override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private:
    void relayout();
    void syncTabCount();
    RibbonTab* createAutoTab(int index);
    void rebuildContextTabs(RibbonContextCategory* ctx);
    void syncContextSignals(RibbonContextCategory* ctx);
    bool isOwnedContextTab(RibbonTab* tab) const;
    void propagateRibbonStyle();
    void propagateLayoutFactors();
    static int styleRowCount(RibbonEnums::RibbonStyle style);
    static bool styleIsCompact(RibbonEnums::RibbonStyle style);
    void placeTitleRowHosts(int titleH, int tabBarY, int tabH, int appBtnW, int systemStripW);
    // frameless: attach/detach the agent to the hosting QQuickWindow and
    // keep the strip reservation in sync
    void attachWindowAgent();
    void detachWindowAgent();
    // frameless: (re-)register every interactive child with the QWK agent so
    // they receive Qt mouse events inside the HTCAPTION bar rect
    void syncHitTestVisible();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ListIndex = qsizetype;
#else
    using ListIndex = int;
#endif
    int effectiveTabCount() const;
    QVector< RibbonTab* > effectiveTabs() const;
    QVector< RibbonCategory* > effectiveCategories() const;
    // Move a tab row slot (the paired auto tab follows its category) and keep
    // mAutoTabs in row order, which syncTabCount's tail-shrink test relies on
    void moveTabSlot(int from, int to);

    int mCurrentIndex = 0;
    QString mApplicationLabel;
    RibbonEnums::RibbonStyle mRibbonStyle = RibbonEnums::RibbonStyleLooseThreeRow;
    RibbonEnums::Alignment mTabAlignment = RibbonEnums::AlignLeft;
    bool mTabOnTitle = false;
    bool mMinimumMode = false;
    int mSystemButtonStripWidth = 0;  ///< frameless system-button strip reservation (native frame: 0)
    qreal mButtonMaximumAspectRatio     = SARibbon::Core::ToolButtonLayoutConstants::BUTTON_MAX_ASPECT_RATIO_DEFAULT;
    qreal mLargeButtonMinimumWidthRatio = SARibbon::Core::ToolButtonLayoutConstants::LARGE_BUTTON_MIN_WIDTH_RATIO;
    QVector< RibbonCategory* > mCategories;
    QVector< RibbonCategory* > mHiddenCategories;  ///< user-hidden subset (customize visibility records)
    QVector< RibbonTab* > mTabs;      ///< explicit + auto tabs in row order
    QVector< RibbonTab* > mAutoTabs;  ///< subset owned (and destroyed) by this bar
    QVector< RibbonContextCategory* > mContexts;  ///< declared context categories
    QHash< RibbonContextCategory*, QVector< RibbonTab* > > mContextTabs;  ///< per-context page tabs (owned)
    RibbonQuickAccessBar* mQuickAccessBar = nullptr;  ///< declared quick access row (single)
    RibbonButtonGroup* mRightButtonGroup  = nullptr;  ///< declared right group (single)
    RibbonApplicationWindow* mApplicationWindow = nullptr;  ///< declared app window (single)
    RibbonWindowAgent* mWindowAgent = nullptr;  ///< declared frameless agent (single)
    RibbonShortcutMatcher* mShortcutMatcher = nullptr;  ///< plan-05 S5: QAction shortcut real triggering
    QString mWindowTitle;  ///< mirrored window title for the leaf
    RibbonMenuModel* mAppMenuModel = nullptr;  ///< plan-06 S3: QAction application menu derivation
    QVector< QAction* > mAppMenuActions;
    void syncAppMenuModel();
    static void appendAppMenuActionCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop, SARibbonQml::RibbonAction* action);
    static ListIndex appMenuActionCountCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop);
    static SARibbonQml::RibbonAction* appMenuActionAtCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop, ListIndex index);
    static void clearAppMenuActionsCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop);
    QVariantList mBands;
    QRect mTitleRect;
    QRectF mApplicationButtonRect;
    int mTabBarHeight   = 0;
    int mTitleBarHeight = 0;
    int mCategoryRowY   = 0;
    int mMinimumWidth   = 0;  ///< core-derived window minimum (screen rules applied)
};

}

#endif  // RIBBONBAR_H
