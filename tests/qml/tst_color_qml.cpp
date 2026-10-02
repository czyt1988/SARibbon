#include <QtTest>
#include <QApplication>
#include <QGridLayout>
#include <QImage>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QSignalSpy>
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include <SARibbonQml/SARibbonQmlTypes.h>
#include <SARibbonQml/button/RibbonToolButton.h>
#include <SARibbonQml/color/RibbonColorGrid.h>
#include <SARibbonQml/color/RibbonColorMenu.h>
#include <SARibbonQml/color/RibbonColorToolButton.h>
#include "colorWidgets/SAColorGridWidget.h"
#include "colorWidgets/SAColorMenu.h"
#include "colorWidgets/SAColorPaletteGridWidget.h"
#include "colorWidgets/SAColorToolButton.h"

/**
 * @brief QML 颜色控件族测试（计划 WS-D）
 * @details 颜色控件族的共享算法（标准色表、色板深浅行、色带几何、"无颜色"斜线）
 *          已下沉 core，两个前端都只是「喂输入 + 取输出」。本测试把这条约定钉死：
 *          1. QML 颜色网格发布的几何必须与 widgets SAColorGridWidget 的真实布局
 *             逐像素相同（sizeHint 与每个色块按钮的 geometry），行最小高、列数不
 *             限定、尾部弹簧三种形变都各自对照一次；
 *          2. 互斥勾选的语义与 widgets QButtonGroup 一致：先报告取消的色块再报告
 *             新选中的，重复点击已选单元不改变选中，clearCheckedState 清空；
 *          3. 叶子确实按发布的 cellRects 摆放色块，真实鼠标点击能驱动宿主的
 *             colorClicked / checkedIndex；
 *          4. 无效 QColor 的单元画出"无颜色"标记（红色斜线），斜线内缩量来自 core
 *             SA::noneColorSlashLine，用 grabWindow 客观判定（项目既有 headless
 *             验证法，不依赖人眼看图）；
 *          5. 颜色菜单（RibbonColorMenu）的默认数据与 widgets SAColorMenu 逐项相同，
 *             自定义颜色的记录规则（先追加、满了左移、缩容量从前面裁）与
 *             recordCustomColor 一致，叶子按 widgets 的条目顺序摆出三个网格与两行
 *             文本，真实点击把颜色报告出来并关掉弹窗。菜单外框几何是刻意不同的一
 *             处（QMenu 的 sizeHint 脱离 widgets 无法复现），因此只对照数据与顺序，
 *             不逐像素对照外框。
 */
namespace
{
/// 取一个已布局的 widgets 颜色网格，供几何对照
std::unique_ptr< SAColorGridWidget > makeWidgetsGrid(const QList< QColor >& colors, int columnCount, const QSize& iconSize)
{
    std::unique_ptr< SAColorGridWidget > w(new SAColorGridWidget());
    w->setColumnCount(columnCount);
    w->setColorIconSize(iconSize);
    w->setColorList(colors);
    w->resize(w->sizeHint());
    if (QLayout* l = w->layout()) {
        l->activate();
    }
    return w;
}

/// 把一个 C++ 创建的宿主挂进视图并创建叶子（宿主不经 QML 声明时不会跑
/// componentComplete，必须显式 ensureQmlLeaf）
/// @note 两个坑：
///       1. C++ new 出来的宿主自己没有 QML 上下文，QQuickView 的 contentItem
///          也没有，createVisualLeaf 沿 parentItem 链找不到引擎就会放弃建叶子；
///       2. 视图没有由 QML 创建的根对象时（没走 setContent），窗口按透明合成，
///          grabWindow 出来的像素 alpha 近乎 0，预乘后颜色全被压平，判色失效。
///          所以这里先用引擎造一个空 Item 当根对象。
bool attachHost(QQuickView& view, SARibbonQml::RibbonColorGrid* host, int w, int h)
{
    QQmlEngine* engine = view.engine();
    if (!engine) {
        return false;
    }
    // 组件挂在引擎上，活得比视图久（NOTES B44 的上下文生命周期族）
    QQmlComponent* rootComp = new QQmlComponent(engine, engine);
    rootComp->setData(QByteArrayLiteral("import QtQuick 2.12\nItem {\n}\n"), QUrl());
    QObject* rootObj = rootComp->create();
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    if (!rootItem) {
        return false;
    }
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), rootComp, rootObj);

    if (!QQmlEngine::contextForObject(host)) {
        QQmlEngine::setContextForObject(host, engine->rootContext());
    }
    host->setParentItem(rootItem);
    host->setSize(QSize(w, h));
    host->ensureQmlLeaf();
    if (!host->qmlLeaf()) {
        return false;
    }
    view.show();
    return QTest::qWaitForWindowExposed(&view);
}

/**
 * @brief 收集 QML 视觉子树的全部项
 * @details Repeater 的委托对象在 QObject 父子关系上挂在别处，findChildren() 会漏掉，
 *          因此按 childItems() 走（NOTES B50）。
 */
void collectVisualItems(QQuickItem* item, QList< QQuickItem* >* out)
{
    if (!item) {
        return;
    }
    out->append(item);
    const QList< QQuickItem* > kids = item->childItems();
    for (QQuickItem* kid : kids) {
        collectVisualItems(kid, out);
    }
}

/**
 * @brief 在视觉子树里按 objectName 找一项
 * @details Popup 打开后其内容项会被重挂到窗口 Overlay 下，因此菜单内部的项从宿主
 *          叶子上是找不到的，必须从窗口 contentItem 起走视觉树（仍是 NOTES B50 的
 *          childItems 走法）。
 */
QQuickItem* findVisualItem(QQuickItem* item, const QString& name)
{
    if (!item) {
        return nullptr;
    }
    if (item->objectName() == name) {
        return item;
    }
    const QList< QQuickItem* > kids = item->childItems();
    for (QQuickItem* kid : kids) {
        if (QQuickItem* hit = findVisualItem(kid, name)) {
            return hit;
        }
    }
    return nullptr;
}

/// 视觉树里按 objectName 找一个颜色网格宿主
SARibbonQml::RibbonColorGrid* findGrid(QQuickItem* root, const char* name)
{
    return qobject_cast< SARibbonQml::RibbonColorGrid* >(findVisualItem(root, QLatin1String(name)));
}

/// 项中心的窗口坐标（mapToItem(nullptr) 即映射到场景）
QPoint sceneCenter(QQuickItem* item)
{
    return item->mapToItem(nullptr, QPointF(item->width() / 2.0, item->height() / 2.0)).toPoint();
}

/// 某个单元中心的窗口坐标
QPoint sceneCellCenter(SARibbonQml::RibbonColorGrid* grid, int index)
{
    return grid->mapToItem(nullptr, grid->cellRects().at(index).toRect().center()).toPoint();
}

/// 统计接近某个颜色的像素数（headless 客观判定）
int countPixelsNear(const QImage& img, const QColor& color, int tolerance = 8)
{
    int n = 0;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            const QColor c = img.pixelColor(x, y);
            if (qAbs(c.red() - color.red()) <= tolerance && qAbs(c.green() - color.green()) <= tolerance
                && qAbs(c.blue() - color.blue()) <= tolerance) {
                ++n;
            }
        }
    }
    return n;
}

/// 统计"偏红"的像素数。抗锯齿斜线绝大多数像素是红与白的混合色，
/// 用固定容差匹配纯红只会数到线芯的两三个像素，因此这里判定的是红色通道
/// 明显压过另外两个通道，而不是逼近某个具体色值
int countReddishPixels(const QImage& img)
{
    int n = 0;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            const QColor c = img.pixelColor(x, y);
            if (c.red() > 100 && c.red() - c.green() > 40 && c.red() - c.blue() > 40) {
                ++n;
            }
        }
    }
    return n;
}

/// 叶子委托的 objectName（RibbonColorGrid.qml 约定），便于在视觉树里定位单元
const char* const kCellName = "colorGridCell";

/**
 * @brief 用 QML 源码造一个场景并显示出来
 * @details 颜色按钮这类宿主必须经 QML 声明才会跑 componentComplete（叶子创建、
 *          菜单叶子创建、sizeHint 用派生版 layoutInput 重算都挂在那里），因此不能
 *          像网格那样 C++ new 出来再 attachHost。组件挂在引擎上，活得比视图久
 *          （NOTES B44 的上下文生命周期族）。
 */
QQuickItem* attachQmlSource(QQuickView& view, const QByteArray& src)
{
    QQmlEngine* engine = view.engine();
    if (!engine) {
        return nullptr;
    }
    QQmlComponent* comp = new QQmlComponent(engine, engine);
    comp->setData(src, QUrl());
    QObject* obj = comp->create();
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(obj);
    if (!rootItem) {
        qWarning("%s", qPrintable(comp->errorString()));
        return nullptr;
    }
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), comp, obj);
    view.show();
    if (!QTest::qWaitForWindowExposed(&view)) {
        return nullptr;
    }
    return rootItem;
}

/// 面板里放若干个颜色按钮的场景骨架（面板负责按 sizeHint 摆位置，按钮不自己写死宽高）
/// @param panelHeight 面板高度；0 表示撑满窗口。默认给一个接近真实 ribbon 的高度，
///                    否则大按钮的图标槽会被拉到两百多像素，几何虽然仍由 core 算出，
///                    但弹窗会掉到窗口外面（QTest 的鼠标事件就废了）
QByteArray colorButtonScene(const QByteArray& buttons, int w = 600, int h = 300, int panelHeight = 100)
{
    const QByteArray panelGeom = panelHeight > 0
                                     ? QByteArrayLiteral("        anchors.top: parent.top\n"
                                                         "        anchors.left: parent.left\n"
                                                         "        anchors.right: parent.right\n"
                                                         "        height: ")
                                           + QByteArray::number(panelHeight) + QByteArrayLiteral("\n")
                                     : QByteArrayLiteral("        anchors.fill: parent\n");
    return QByteArrayLiteral("import QtQuick 2.12\n"
                             "import SARibbon 3.0\n"
                             "Item {\n"
                             "    width: ")
           + QByteArray::number(w) + QByteArrayLiteral("\n    height: ") + QByteArray::number(h)
           + QByteArrayLiteral("\n    RibbonPanel {\n"
                               "        objectName: \"panel\"\n")
           + panelGeom + QByteArrayLiteral("        panelTitle: \"P\"\n") + buttons
           + QByteArrayLiteral("    }\n}\n");
}
}  // namespace

class TestColorQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void gridGeometryMatchesWidgets();
    void unlimitedColumnsSingleRow();
    void rowMinimumHeightAndSpacer();
    void exclusiveCheckSemantics();
    void leafPlacesCellsAndClicks();
    void noneColorMarkRenders();
    void menuDataMatchesWidgets();
    void customColorRecordRules();
    void menuLeafOpensAndPicks();
    void colorButtonGeometryFollowsCore();
    void colorButtonRendersSwatch();
    void colorButtonMenuPicksColor();
};

/**
 * @brief 网格几何与 widgets 逐像素一致
 * @details 单元即 icon-only 的 SAColorToolButton。QGridLayout 取 sizeHint（图标
 *          尺寸加四周边距）与 minimumSizeHint 中较大的那个，两者之差由 core
 *          SA::colorGridCellSize 统一给出；排布用 contentsMargins(1,1,1,1)、
 *          spacing 0。
 */
void TestColorQml::gridGeometryMatchesWidgets()
{
    const QList< QColor > colors = SA::getStandardColorList();
    QVERIFY(!colors.isEmpty());
    const int cols = 4;
    const QSize iconSize(16, 16);
    const QSize cellSize = SA::colorGridCellSize(iconSize, 4);
    // 单元确实比图标盒大：QToolButton 的最小尺寸提示说了算
    QVERIFY(cellSize.width() > iconSize.width() + 8);

    std::unique_ptr< SAColorGridWidget > w = makeWidgetsGrid(colors, cols, iconSize);

    SARibbonQml::RibbonColorGrid host;
    host.setColumnCount(cols);
    host.setColorIconSize(iconSize);
    host.setColorList(colors);

    QCOMPARE(host.colorCount(), colors.size());
    QCOMPARE(host.gridColumns(), cols);
    QCOMPARE(host.gridRows(), (colors.size() + cols - 1) / cols);
    QCOMPARE(host.cellWidth(), cellSize.width());
    QCOMPARE(host.cellHeight(), cellSize.height());
    QCOMPARE(host.implicitWidth(), w->sizeHint().width());
    QCOMPARE(host.implicitHeight(), w->sizeHint().height());

    const QVariantList rects = host.cellRects();
    QCOMPARE(rects.size(), colors.size());
    const QVariantList noneFlags = host.cellNoneColor();
    QCOMPARE(noneFlags.size(), colors.size());
    for (int i = 0; i < colors.size(); ++i) {
        SAColorToolButton* btn = w->colorButton(i);
        QVERIFY2(btn, qPrintable(QStringLiteral("widgets cell %1 missing").arg(i)));
        QCOMPARE(rects.at(i).toRect(), btn->geometry());
        QCOMPARE(btn->color(), colors.at(i));
        QCOMPARE(noneFlags.at(i).toBool(), !colors.at(i).isValid());
        QCOMPARE(host.colorAt(i), colors.at(i));
    }
    QVERIFY(!host.colorAt(colors.size()).isValid());

    // 色块数少于列数：widgets 的空列不占宽度，宿主按实际占用的列数算
    QList< QColor > few;
    few << QColor(Qt::red) << QColor(Qt::green) << QColor(Qt::blue) << QColor(Qt::cyan) << QColor(Qt::magenta);
    const int fewCols = 10;
    std::unique_ptr< SAColorGridWidget > wFew = makeWidgetsGrid(few, fewCols, iconSize);

    SARibbonQml::RibbonColorGrid hostFew;
    hostFew.setColumnCount(fewCols);
    hostFew.setColorIconSize(iconSize);
    hostFew.setColorList(few);
    QCOMPARE(hostFew.gridColumns(), few.size());
    QCOMPARE(hostFew.gridRows(), 1);
    QCOMPARE(hostFew.implicitWidth(), wFew->sizeHint().width());
    QCOMPARE(hostFew.implicitHeight(), wFew->sizeHint().height());
    for (int i = 0; i < few.size(); ++i) {
        QCOMPARE(hostFew.cellRects().at(i).toRect(), wFew->colorButton(i)->geometry());
    }
}

/**
 * @brief 列数不限定时只有一行（widgets columnCount <= 0 分支）
 */
void TestColorQml::unlimitedColumnsSingleRow()
{
    QList< QColor > colors;
    for (int i = 0; i < 7; ++i) {
        colors.append(QColor(i * 20, 100, 200));
    }
    const QSize iconSize(12, 12);

    for (int unlimited : { 0, -1 }) {
        std::unique_ptr< SAColorGridWidget > w = makeWidgetsGrid(colors, unlimited, iconSize);

        SARibbonQml::RibbonColorGrid host;
        host.setColumnCount(unlimited);
        host.setColorIconSize(iconSize);
        host.setColorList(colors);

        QCOMPARE(host.gridRows(), 1);
        QCOMPARE(host.gridColumns(), colors.size());
        QCOMPARE(host.implicitWidth(), w->sizeHint().width());
        QCOMPARE(host.implicitHeight(), w->sizeHint().height());
        for (int i = 0; i < colors.size(); ++i) {
            QCOMPARE(host.cellRects().at(i).toRect(), w->colorButton(i)->geometry());
        }
    }

    // 空列表：两端都退化为只剩内容边距
    SARibbonQml::RibbonColorGrid empty;
    empty.setColumnCount(8);
    QCOMPARE(empty.gridRows(), 0);
    QCOMPARE(empty.gridColumns(), 0);
    QCOMPARE(empty.cellRects().size(), 0);
    QCOMPARE(empty.implicitWidth(), 2);
    QCOMPARE(empty.implicitHeight(), 2);
}

/**
 * @brief 行最小高与尾部弹簧（widgets setRowMinimumHeight / setHorizontalSpacerToRight）
 */
void TestColorQml::rowMinimumHeightAndSpacer()
{
    QList< QColor > colors;
    for (int i = 0; i < 12; ++i) {
        colors.append(QColor(10 * i, 40, 220));
    }
    const int cols     = 6;
    const QSize iconSize(16, 16);

    std::unique_ptr< SAColorGridWidget > w = makeWidgetsGrid(colors, cols, iconSize);
    w->setRowMinimumHeight(0, 40);
    w->setHorizontalSpacerToRight(true);
    w->resize(w->sizeHint());
    if (QLayout* l = w->layout()) {
        l->activate();
    }

    SARibbonQml::RibbonColorGrid host;
    host.setColumnCount(cols);
    host.setColorIconSize(iconSize);
    host.setColorList(colors);
    host.setRowMinimumHeight(0, 40);
    host.setRowMinimumHeight(0, 40);  // 幂等
    QCOMPARE(host.rowMinimumHeight(0), 40);
    QCOMPARE(host.rowMinimumHeight(1), 0);
    host.setHorizontalSpacerToRight(true);

    QCOMPARE(host.implicitWidth(), w->sizeHint().width());
    QCOMPARE(host.implicitHeight(), w->sizeHint().height());
    for (int i = 0; i < colors.size(); ++i) {
        QCOMPARE(host.cellRects().at(i).toRect(), w->colorButton(i)->geometry());
    }
    // 单元自身高度不变（QToolButton 纵向固定），被撑高的行里单元居中放置，
    // 因此第一行单元的 y 比行起点下移 (40 - 单元高) / 2
    const int cellH = SA::colorGridCellSize(iconSize, 4).height();
    QCOMPARE(host.cellRects().at(0).toRect().height(), cellH);
    QCOMPARE(host.cellRects().at(0).toRect().y(), 1 + (40 - cellH) / 2);
    QCOMPARE(host.cellRects().at(cols).toRect().height(), cellH);
    QCOMPARE(host.cellRects().at(cols).toRect().y(), 1 + 40 + host.verticalSpacing());

    // 间隔非零时两端仍然一致（弹簧独占一列，只贡献自身宽度，不额外占一个间隔）
    std::unique_ptr< SAColorGridWidget > w2 = makeWidgetsGrid(colors, cols, iconSize);
    w2->setSpacing(2);
    w2->setHorizontalSpacerToRight(true);
    w2->resize(w2->sizeHint());
    if (QLayout* l = w2->layout()) {
        l->activate();
    }

    SARibbonQml::RibbonColorGrid host2;
    host2.setColumnCount(cols);
    host2.setColorIconSize(iconSize);
    host2.setColorList(colors);
    host2.setSpacing(2);
    QCOMPARE(host2.horizontalSpacing(), 2);
    QCOMPARE(host2.verticalSpacing(), 2);
    host2.setHorizontalSpacerToRight(true);
    QCOMPARE(host2.implicitWidth(), w2->sizeHint().width());
    QCOMPARE(host2.implicitHeight(), w2->sizeHint().height());
    for (int i = 0; i < colors.size(); ++i) {
        QCOMPARE(host2.cellRects().at(i).toRect(), w2->colorButton(i)->geometry());
    }

    // 关掉弹簧回到无弹簧宽度（弹簧只贡献自身宽度，不额外占一个间隔）
    const int widthWithSpacer = host2.implicitWidth();
    host2.setHorizontalSpacerToRight(false);
    QCOMPARE(host2.implicitWidth(), widthWithSpacer - host2.spacerWidth());
}

/**
 * @brief 互斥勾选语义（widgets QButtonGroup::setExclusive(true) 对照）
 */
void TestColorQml::exclusiveCheckSemantics()
{
    QList< QColor > colors;
    colors << QColor(Qt::red) << QColor(Qt::green) << QColor(Qt::blue) << QColor();

    SARibbonQml::RibbonColorGrid host;
    host.setColumnCount(4);
    host.setColorList(colors);

    QSignalSpy toggled(&host, &SARibbonQml::RibbonColorGrid::colorToggled);
    QSignalSpy clicked(&host, &SARibbonQml::RibbonColorGrid::colorClicked);
    QSignalSpy pressed(&host, &SARibbonQml::RibbonColorGrid::colorPressed);
    QSignalSpy released(&host, &SARibbonQml::RibbonColorGrid::colorReleased);
    QVERIFY(toggled.isValid() && clicked.isValid() && pressed.isValid() && released.isValid());

    // 非 checkable：点击报告 colorClicked，但不进入勾选态
    host.activateCell(1);
    QCOMPARE(clicked.count(), 1);
    QCOMPARE(clicked.at(0).at(0).value< QColor >(), QColor(Qt::green));
    QCOMPARE(host.checkedIndex(), -1);
    QCOMPARE(toggled.count(), 0);
    QVERIFY(!host.currentCheckedColor().isValid());

    // 越界索引一律忽略
    clicked.clear();
    host.activateCell(colors.size());
    host.activateCell(-1);
    host.notifyCellPressed(colors.size());
    QCOMPARE(clicked.count(), 0);
    QCOMPARE(pressed.count(), 0);

    host.notifyCellPressed(2);
    host.notifyCellReleased(2);
    QCOMPARE(pressed.count(), 1);
    QCOMPARE(pressed.at(0).at(0).value< QColor >(), QColor(Qt::blue));
    QCOMPARE(released.count(), 1);

    host.setColorCheckable(true);
    host.activateCell(1);
    QCOMPARE(host.checkedIndex(), 1);
    QCOMPARE(host.currentCheckedColor(), QColor(Qt::green));
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.at(0).at(0).value< QColor >(), QColor(Qt::green));
    QCOMPARE(toggled.at(0).at(1).toBool(), true);

    // 切换：先报告取消的，再报告新选中的（互斥按钮组的顺序）
    toggled.clear();
    host.activateCell(2);
    QCOMPARE(toggled.count(), 2);
    QCOMPARE(toggled.at(0).at(0).value< QColor >(), QColor(Qt::green));
    QCOMPARE(toggled.at(0).at(1).toBool(), false);
    QCOMPARE(toggled.at(1).at(0).value< QColor >(), QColor(Qt::blue));
    QCOMPARE(toggled.at(1).at(1).toBool(), true);

    // 重复点击已选单元保持勾选
    toggled.clear();
    host.activateCell(2);
    QCOMPARE(toggled.count(), 0);
    QCOMPARE(host.checkedIndex(), 2);

    // "无颜色"单元同样可选中，报告的是无效色
    toggled.clear();
    host.activateCell(3);
    QCOMPARE(host.checkedIndex(), 3);
    QVERIFY(!host.currentCheckedColor().isValid());
    QCOMPARE(toggled.count(), 2);
    QVERIFY(!toggled.at(1).at(0).value< QColor >().isValid());
    QCOMPARE(toggled.at(1).at(1).toBool(), true);

    // 直接写 checkedIndex 与 clearCheckedState
    toggled.clear();
    host.setCheckedIndex(0);
    QCOMPARE(host.checkedIndex(), 0);
    QCOMPARE(toggled.count(), 2);
    host.clearCheckedState();
    QCOMPARE(host.checkedIndex(), -1);
    QCOMPARE(toggled.count(), 3);
    QCOMPARE(toggled.at(2).at(1).toBool(), false);
    host.clearCheckedState();  // 已无选中，幂等
    QCOMPARE(toggled.count(), 3);

    // 关掉 checkable 会把选中一并清掉
    host.setCheckedIndex(1);
    QCOMPARE(host.checkedIndex(), 1);
    toggled.clear();
    host.setColorCheckable(false);
    QCOMPARE(host.checkedIndex(), -1);
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.at(0).at(1).toBool(), false);
    toggled.clear();
    host.activateCell(2);
    QCOMPARE(toggled.count(), 0);
    QCOMPARE(host.checkedIndex(), -1);

    // 颜色表缩短后越界的选中被丢弃
    host.setColorCheckable(true);
    host.setColorList(colors);
    host.setCheckedIndex(3);
    QCOMPARE(host.checkedIndex(), 3);
    host.setColorList(colors.mid(0, 2));
    QCOMPARE(host.checkedIndex(), -1);
    QCOMPARE(host.gridColumns(), 2);
}

/**
 * @brief 叶子按发布的 cellRects 摆放色块，真实点击驱动宿主
 */
void TestColorQml::leafPlacesCellsAndClicks()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    const char* src = "import QtQuick 2.12\n"
                      "import SARibbon 3.0\n"
                      "RibbonColorGrid {\n"
                      "    width: 200\n"
                      "    height: 80\n"
                      "    columnCount: 3\n"
                      "    colorCheckable: true\n"
                      "    colorIconSize: Qt.size(16, 16)\n"
                      "    colorList: [\"#ff0000\", \"#00ff00\", \"#0000ff\", \"#ffff00\", \"#00ffff\"]\n"
                      "}\n";

    QQmlComponent component(&engine);
    component.setData(QByteArray(src), QUrl());
    QObject* rootObj = component.create();
    QVERIFY2(rootObj, qPrintable(component.errorString()));

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(240, 120);
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    QVERIFY(rootItem);
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), &component, rootObj);
    view.show();
    QVERIFY(QTest::qWaitForWindowExposed(&view));

    auto* host = qobject_cast< SARibbonQml::RibbonColorGrid* >(rootObj);
    QVERIFY(host);
    QCOMPARE(host->gridColumns(), 3);
    QCOMPARE(host->gridRows(), 2);
    QVERIFY(host->qmlLeaf());

    // 叶子的单元委托与宿主发布的矩形一一对应
    QList< QQuickItem* > items;
    collectVisualItems(host->qmlLeaf(), &items);
    QList< QQuickItem* > cells;
    for (QQuickItem* it : items) {
        if (it->objectName() == QLatin1String(kCellName)) {
            cells.append(it);
        }
    }
    QCOMPARE(cells.size(), 5);
    const QVariantList rects = host->cellRects();
    for (int i = 0; i < cells.size(); ++i) {
        const QRect r   = rects.at(i).toRect();
        QQuickItem* cell = cells.at(i);
        QCOMPARE(qRound(cell->x()), r.x());
        QCOMPARE(qRound(cell->y()), r.y());
        QCOMPARE(qRound(cell->width()), r.width());
        QCOMPARE(qRound(cell->height()), r.height());
    }

    // 色块颜色：单元里的第一个可见填充矩形即色块
    const QImage shot = view.grabWindow();
    const QRect first = rects.at(0).toRect().marginsRemoved(QMargins(host->cellMargin(), host->cellMargin(), host->cellMargin(), host->cellMargin()));
    QCOMPARE(shot.pixelColor(first.center()), QColor(0xff, 0x00, 0x00));

    // 真实点击第二行第一个单元 -> checkedIndex 与 colorClicked
    QSignalSpy clicked(host, &SARibbonQml::RibbonColorGrid::colorClicked);
    const QRect target = rects.at(3).toRect();
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, target.center());
    QCOMPARE(clicked.count(), 1);
    QCOMPARE(clicked.at(0).at(0).value< QColor >(), QColor(0xff, 0xff, 0x00));
    QCOMPARE(host->checkedIndex(), 3);

    // implicit 尺寸随颜色表变化重新发布
    const int w0 = host->implicitWidth();
    host->setColorList(QList< QColor >() << QColor(Qt::black));
    QVERIFY(host->implicitWidth() < w0);
}

/**
 * @brief 无效 QColor 单元画出"无颜色"标记（红色斜线，core 内缩量）
 */
void TestColorQml::noneColorMarkRenders()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    auto* host = new SARibbonQml::RibbonColorGrid();
    host->setColumnCount(2);
    host->setColorIconSize(QSize(24, 24));
    host->setColorList(QList< QColor >() << QColor(Qt::red) << QColor());
    QVERIFY(host->cellNoneColor().at(1).toBool());
    QVERIFY(!host->cellNoneColor().at(0).toBool());
    // 斜线内缩量就是 core 在同一个色块矩形（单元四周内缩 cellMargin）上算出来的量
    const QSize swatchSize(host->cellWidth() - 2 * host->cellMargin(), host->cellHeight() - 2 * host->cellMargin());
    QCOMPARE(host->noneColorSlashInset(), SA::noneColorSlashLine(QRect(QPoint(0, 0), swatchSize)).x1());

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(200, 100);
    QVERIFY(attachHost(view, host, host->implicitWidth(), host->implicitHeight()));

    QList< QQuickItem* > items;
    collectVisualItems(host->qmlLeaf(), &items);
    int cellCount = 0;
    for (QQuickItem* it : items) {
        if (it->objectName() == QLatin1String(kCellName)) {
            ++cellCount;
        }
    }
    QCOMPARE(cellCount, 2);

    // 无颜色单元：白底 + 红色斜线（widgets paintNoneColor 的两笔）。斜线由
    // Canvas 绘制，首帧才落笔，因此轮询到画出来为止。斜线是抗锯齿的，除了线芯
    // 两三个像素外都是红白混合色，所以数"偏红"像素而不是数纯红像素
    const QRect noneCell = host->cellRects().at(1).toRect().marginsRemoved(QMargins(host->cellMargin(), host->cellMargin(), host->cellMargin(), host->cellMargin()));
    QImage shot;
    QImage noneArea;
    auto grabRed = [&]() -> int {
        shot     = view.grabWindow();
        noneArea = shot.copy(noneCell);
        return countReddishPixels(noneArea);
    };
    int redPixels = 0;
    QTRY_VERIFY_WITH_TIMEOUT((redPixels = grabRed()) > 8, 3000);
    // 斜线只在两个内缩端点之间出现：色块最左侧一列不应有红色像素
    QCOMPARE(countReddishPixels(noneArea.copy(0, 0, 1, noneArea.height())), 0);
    // 底色是白填充（斜线以外的绝大多数像素都接近白）
    QVERIFY(countPixelsNear(noneArea, QColor(Qt::white), 24) * 4 > noneArea.width() * noneArea.height());
    // 相邻的有效色块仍然是纯色填充
    const QRect validCell = host->cellRects().at(0).toRect().marginsRemoved(QMargins(host->cellMargin(), host->cellMargin(), host->cellMargin(), host->cellMargin()));
    QCOMPARE(shot.pixelColor(validCell.center()), QColor(0xff, 0x00, 0x00));
}

/**
 * @brief 颜色菜单的默认数据与 widgets SAColorMenu 逐项相同
 * @details 菜单宿主自己不造任何色值：标准色表与深浅因子取自 core，深浅行由 core
 *          SA::colorPaletteShades 推导，因此与 widgets SAColorPaletteGridWidget 的
 *          paletteColorList() 是同一份算式。这里对着一个真实的 widgets SAColorMenu
 *          逐项核对，顺带钉住"无颜色"项在自定义颜色之上这条条目顺序（enableNoneColorAction
 *          用的是 insertAction(mCustomColorAction, ...)）以及标记斜线的内缩算式。
 */
void TestColorQml::menuDataMatchesWidgets()
{
    SAColorMenu wmenu;
    wmenu.enableNoneColorAction(true);
    SAColorPaletteGridWidget* palette = wmenu.colorPaletteGridWidget();
    QVERIFY(palette);
    SAColorGridWidget* customGrid = wmenu.customColorsWidget();
    QVERIFY(customGrid);

    const QList< QAction* > acts = wmenu.actions();
    const int noneIdx             = acts.indexOf(wmenu.noneColorAction());
    const int customIdx           = acts.indexOf(wmenu.customColorAction());
    QVERIFY(noneIdx >= 0);
    QVERIFY2(noneIdx < customIdx, "widgets puts the none color action above the custom color one");

    SARibbonQml::RibbonColorMenu host;
    QCOMPARE(host.standardColors(), palette->colorList());
    QCOMPARE(host.paletteFactors(), palette->factor());
    QCOMPARE(host.paletteColors(), SA::colorPaletteShades(palette->colorList(), palette->factor()));
    QCOMPARE(host.paletteColumns(), palette->colorList().size());
    QCOMPARE(host.paletteColors().size(), palette->colorList().size() * palette->factor().size());
    QCOMPARE(host.maxCustomColorCount(), customGrid->columnCount());
    QCOMPARE(host.colorIconSize(), customGrid->colorIconSize());
    QCOMPARE(host.customColorText(), wmenu.customColorAction()->text());
    QCOMPARE(host.noneColorText(), wmenu.noneColorAction()->text());
    QVERIFY(!host.themeColorsTitle().isEmpty());
    // widgets 里两个网格都关掉了勾选，菜单宿主也按同样的形状把它们摆出来
    QVERIFY(!customGrid->isColorCheckable());
    // "无颜色"默认不开，与 enableNoneColorAction 必须显式调用一致
    QVERIFY(!host.isNoneColorEnabled());

    // 标记斜线：widgets 在 32x32 的盒子上内缩 1px 再画（createNoneColorIcon），
    // 宿主发布的是同一算式作用在自己标记盒上的内缩量
    QCOMPARE(host.noneMarkSlashInset(), SA::noneColorSlashLine(QRect(0, 0, host.noneMarkSide(), host.noneMarkSide()).adjusted(1, 1, -1, -1)).x1());
    host.setNoneMarkSide(32);
    QCOMPARE(host.noneMarkSlashInset(), SA::noneColorSlashLine(QRect(0, 0, 32, 32).adjusted(1, 1, -1, -1)).x1());

    // 两个输入喂一个输出：任一变化都重算深浅行，并经同一个信号报告（B48 单依赖）
    QSignalSpy shades(&host, &SARibbonQml::RibbonColorMenu::paletteColorsChanged);
    QVERIFY(shades.isValid());
    host.setPaletteFactors(QList< int >() << 150);
    QCOMPARE(shades.count(), 1);
    QCOMPARE(host.paletteColors().size(), host.standardColors().size());
    QCOMPARE(host.paletteColors(), SA::colorPaletteShades(host.standardColors(), host.paletteFactors()));
    host.setStandardColors(QList< QColor >() << QColor(Qt::red) << QColor(Qt::green));
    QCOMPARE(shades.count(), 2);
    QCOMPARE(host.paletteColumns(), 2);
    QCOMPARE(host.paletteColors().size(), 2);
    // 同样的值不再触发（属性写回自身不产生通知）
    host.setStandardColors(host.standardColors());
    host.setPaletteFactors(host.paletteFactors());
    QCOMPARE(shades.count(), 2);
}

/**
 * @brief 自定义颜色记录规则与 widgets recordCustomColor 一致
 * @details 追加、满了整体左移、缩小容量从前面裁掉保留最新，无效色一律拒绝（"无
 *          颜色"是一个选项，不是自定义颜色）。同时钉住菜单宿主对 widgets
 *          onCustomColorActionTriggered / onNoneColorActionTriggered /
 *          emitSelectedColor 三条路径的拆分：requestCustomColor 只发信号且菜单不关
 *          （widgets 那边是模态对话框盖在菜单上），addCustomColor 与 selectNoneColor
 *          都会报告颜色并关菜单。
 */
void TestColorQml::customColorRecordRules()
{
    SARibbonQml::RibbonColorMenu host;
    host.setMaxCustomColorCount(3);
    QCOMPARE(host.maxCustomColorCount(), 3);

    QSignalSpy changed(&host, &SARibbonQml::RibbonColorMenu::customColorsChanged);
    QSignalSpy requested(&host, &SARibbonQml::RibbonColorMenu::customColorRequested);
    QSignalSpy selected(&host, &SARibbonQml::RibbonColorMenu::selectedColor);
    QVERIFY(changed.isValid() && requested.isValid() && selected.isValid());

    host.recordCustomColor(QColor());
    QCOMPARE(host.customColors().size(), 0);
    QCOMPARE(changed.count(), 0);

    host.recordCustomColor(QColor(Qt::red));
    host.recordCustomColor(QColor(Qt::green));
    host.recordCustomColor(QColor(Qt::blue));
    QCOMPARE(changed.count(), 3);
    QList< QColor > expect;
    expect << QColor(Qt::red) << QColor(Qt::green) << QColor(Qt::blue);
    QCOMPARE(host.customColors(), expect);

    // 满了以后整体左移，新色落在最后
    host.recordCustomColor(QColor(Qt::cyan));
    expect.clear();
    expect << QColor(Qt::green) << QColor(Qt::blue) << QColor(Qt::cyan);
    QCOMPARE(host.customColors(), expect);

    // 缩小容量：从前面裁掉，保留最新的
    host.setMaxCustomColorCount(2);
    expect.clear();
    expect << QColor(Qt::blue) << QColor(Qt::cyan);
    QCOMPARE(host.customColors(), expect);
    // 容量为 0：记录被裁空，且此后再也记不进去（记录功能整体关掉）
    host.setMaxCustomColorCount(0);
    QVERIFY(host.customColors().isEmpty());
    host.recordCustomColor(QColor(Qt::magenta));
    QVERIFY(host.customColors().isEmpty());
    host.setMaxCustomColorCount(2);
    host.setCustomColors(expect);
    QCOMPARE(host.customColors(), expect);

    // 取色请求：只发信号，不动记录也不关菜单
    host.setMenuVisible(true);
    host.requestCustomColor();
    QCOMPARE(requested.count(), 1);
    QCOMPARE(selected.count(), 0);
    QVERIFY(host.isMenuVisible());

    // 回灌：记录 + 报告 + 关菜单
    host.addCustomColor(QColor(0x11, 0x22, 0x33));
    QCOMPARE(selected.count(), 1);
    QCOMPARE(selected.at(0).at(0).value< QColor >(), QColor(0x11, 0x22, 0x33));
    QVERIFY(!host.isMenuVisible());
    QCOMPARE(host.customColors().size(), 2);
    QCOMPARE(host.customColors().at(1), QColor(0x11, 0x22, 0x33));
    host.addCustomColor(QColor());
    QCOMPARE(selected.count(), 1);
    QCOMPARE(host.customColors().size(), 2);

    // 无颜色：报告无效色并关菜单
    host.setMenuVisible(true);
    host.selectNoneColor();
    QCOMPARE(selected.count(), 2);
    QVERIFY(!selected.at(1).at(0).value< QColor >().isValid());
    QVERIFY(!host.isMenuVisible());

    // emitSelectedColor 原样报告传入的颜色（宿主不做有效性过滤）
    host.emitSelectedColor(QColor());
    QCOMPARE(selected.count(), 3);
    QVERIFY(!selected.at(2).at(0).value< QColor >().isValid());

    host.clearCustomColors();
    QVERIFY(host.customColors().isEmpty());
    host.clearCustomColors();  // 已空，幂等

    // 直接整表写入
    host.setCustomColors(expect);
    QCOMPARE(host.customColors(), expect);
}

/**
 * @brief 菜单叶子按 widgets 的条目顺序摆出内容，真实点击报告颜色并关菜单
 * @details 叶子内部是三个 RibbonColorGrid（标准色一行、深浅行、自定义颜色）加两行
 *          文本，全部数字来自宿主。这里把弹窗打开后从窗口 Overlay 起走视觉树，逐项
 *          核对网格的输入与几何，再用真实鼠标点击走一遍三条路径：深浅色块 ->
 *          selectedColor 且弹窗关闭；"无颜色"行 -> 无效色；"自定义颜色"行 ->
 *          customColorRequested 且弹窗保持打开（widgets 那边此时正被模态对话框盖着），
 *          回灌后弹窗才关、记录进网格。
 */
void TestColorQml::menuLeafOpensAndPicks()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    // 宿主不能当 QML 根对象：QQuickView 默认 SizeRootObjectToView 会把它撑到窗口
    // 大小，弹窗按 root.height 落到窗口外面，于是包一层带尺寸的 Item
    const char* src = "import QtQuick 2.12\n"
                      "import SARibbon 3.0\n"
                      "Item {\n"
                      "    width: 420\n"
                      "    height: 520\n"
                      "    RibbonColorMenu {\n"
                      "        objectName: \"colorMenuHost\"\n"
                      "        x: 8\n"
                      "        y: 8\n"
                      "        width: 60\n"
                      "        height: 24\n"
                      "        noneColorEnabled: true\n"
                      "        colorIconSize: Qt.size(12, 12)\n"
                      "    }\n"
                      "}\n";

    QQmlComponent component(&engine);
    component.setData(QByteArray(src), QUrl());
    QObject* rootObj = component.create();
    QVERIFY2(rootObj, qPrintable(component.errorString()));

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(420, 520);
    QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
    QVERIFY(rootItem);
    rootItem->setParentItem(view.contentItem());
    view.setContent(QUrl(), &component, rootObj);
    view.show();
    QVERIFY(QTest::qWaitForWindowExposed(&view));

    auto* host = rootItem->findChild< SARibbonQml::RibbonColorMenu* >(QStringLiteral("colorMenuHost"));
    QVERIFY(host);
    QVERIFY(host->qmlLeaf());
    QVERIFY(!host->isMenuVisible());

    host->openMenu();
    QTRY_VERIFY(host->isMenuVisible());

    auto* mainGrid   = findGrid(view.contentItem(), "colorMenuMainGrid");
    auto* shadeGrid  = findGrid(view.contentItem(), "colorMenuShadeGrid");
    auto* customGrid = findGrid(view.contentItem(), "colorMenuCustomGrid");
    QVERIFY(mainGrid && shadeGrid && customGrid);

    // 标准色一行；深浅行按因子数排布且行间不留缝（SAColorPaletteGridWidget）
    QCOMPARE(mainGrid->colorList(), host->standardColors());
    QCOMPARE(mainGrid->gridRows(), 1);
    QCOMPARE(mainGrid->gridColumns(), host->standardColors().size());
    QCOMPARE(shadeGrid->colorList(), host->paletteColors());
    QCOMPARE(shadeGrid->gridColumns(), host->paletteColumns());
    QCOMPARE(shadeGrid->gridRows(), host->paletteFactors().size());
    QCOMPARE(shadeGrid->verticalSpacing(), 0);
    // 自定义颜色：列数即容量、带尾部弹簧、行最小高跟着色块盒
    QCOMPARE(customGrid->columnCount(), host->maxCustomColorCount());
    QVERIFY(customGrid->isHorizontalSpacerToRight());
    QCOMPARE(customGrid->rowMinimumHeight(0), host->colorIconSize().height());
    QCOMPARE(customGrid->colorCount(), 0);
    // 菜单里的网格一律不可勾选
    QVERIFY(!mainGrid->isColorCheckable());
    QVERIFY(!shadeGrid->isColorCheckable());
    QVERIFY(!customGrid->isColorCheckable());
    // 色块盒传到了每个网格
    QCOMPARE(mainGrid->colorIconSize(), host->colorIconSize());
    QCOMPARE(shadeGrid->colorIconSize(), host->colorIconSize());
    QCOMPARE(customGrid->colorIconSize(), host->colorIconSize());

    QQuickItem* noneRow   = findVisualItem(view.contentItem(), QLatin1String("colorMenuNoneRow"));
    QQuickItem* customRow = findVisualItem(view.contentItem(), QLatin1String("colorMenuCustomRow"));
    QVERIFY(noneRow && customRow);
    QTRY_VERIFY(noneRow->isVisible() && customRow->isVisible());
    QCOMPARE(qRound(noneRow->height()), host->actionRowHeight());
    QCOMPARE(qRound(customRow->height()), host->actionRowHeight());
    // 条目顺序：深浅色板在"无颜色"之上，"无颜色"又在"自定义颜色"之上。
    // Column 要等一次 polish 才摆放子项，因此轮询到摆好为止
    QTRY_VERIFY(shadeGrid->mapToItem(nullptr, QPointF(0, 0)).y() < noneRow->mapToItem(nullptr, QPointF(0, 0)).y());
    QTRY_VERIFY(noneRow->mapToItem(nullptr, QPointF(0, 0)).y() < customRow->mapToItem(nullptr, QPointF(0, 0)).y());

    QSignalSpy selected(host, &SARibbonQml::RibbonColorMenu::selectedColor);
    QSignalSpy requested(host, &SARibbonQml::RibbonColorMenu::customColorRequested);
    QVERIFY(selected.isValid() && requested.isValid());

    // 点一个深浅色块：报告颜色并关菜单
    const QColor shade = shadeGrid->colorAt(12);
    QVERIFY(shade.isValid());
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, sceneCellCenter(shadeGrid, 12));
    QCOMPARE(selected.count(), 1);
    QCOMPARE(selected.at(0).at(0).value< QColor >(), shade);
    QTRY_VERIFY(!host->isMenuVisible());

    // "无颜色"行：报告无效色（斜线本身已由 noneColorMarkRenders 客观判过）
    host->openMenu();
    QTRY_VERIFY(host->isMenuVisible());
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, sceneCenter(noneRow));
    QCOMPARE(selected.count(), 2);
    QVERIFY(!selected.at(1).at(0).value< QColor >().isValid());
    QTRY_VERIFY(!host->isMenuVisible());

    // "自定义颜色"行：只请求取色，菜单不关
    host->openMenu();
    QTRY_VERIFY(host->isMenuVisible());
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, sceneCenter(customRow));
    QCOMPARE(requested.count(), 1);
    QCOMPARE(selected.count(), 2);
    QVERIFY(host->isMenuVisible());

    // 回灌取色结果：关菜单，颜色进记录，网格随之长出一个色块
    host->addCustomColor(QColor(0x11, 0x22, 0x33));
    QCOMPARE(selected.count(), 3);
    QCOMPARE(selected.at(2).at(0).value< QColor >(), QColor(0x11, 0x22, 0x33));
    QTRY_VERIFY(!host->isMenuVisible());
    QCOMPARE(host->customColors().size(), 1);

    host->openMenu();
    QTRY_VERIFY(host->isMenuVisible());
    QTRY_COMPARE(customGrid->colorCount(), 1);
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, sceneCellCenter(customGrid, 0));
    QCOMPARE(selected.count(), 4);
    QCOMPARE(selected.at(3).at(0).value< QColor >(), QColor(0x11, 0x22, 0x33));
    QTRY_VERIFY(!host->isMenuVisible());

    // 关掉"无颜色"后该行不再出现
    host->setNoneColorEnabled(false);
    host->openMenu();
    QTRY_VERIFY(host->isMenuVisible());
    QTRY_VERIFY(!noneRow->isVisible());
    host->closeMenu();
    QTRY_VERIFY(!host->isMenuVisible());
}

/**
 * @brief 颜色按钮的绘制几何与 sizeHint 全部由 core 算出，叶子一个像素也不重算
 * @details 色带矩形来自 core SA::calcColorUnderIconMetrics（与 widgets
 *          SARibbonColorToolButton::PrivateData::createIconPixmap 同一套算式），
 *          高度即 SA::colorBandHeight；ColorFillToIcon 走的是 widgets createColorIcon
 *          的规则——在 32x32 的颜色图标里留 1px 边，图标被缩放到 side 后边也跟着缩。
 *          按钮永远占着图标槽（layoutInput 里 hasIcon 恒真），因此没有菜单时
 *          sizeHint 只差指示箭头那一条。这里同时钉住一处已记录的刻意差异：没有
 *          iconSource 时 widgets 整块不画，QML 保留槽位并让色带占满槽宽。
 */
void TestColorQml::colorButtonGeometryFollowsCore()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    const QByteArray src = colorButtonScene(QByteArrayLiteral(
        "        RibbonColorToolButton { objectName: \"underBtn\"; text: \"Font\" }\n"
        "        RibbonColorToolButton { objectName: \"fillBtn\"; text: \"Font\"; colorStyle: Ribbon.ColorFillToIcon }\n"
        "        RibbonColorToolButton { objectName: \"noMenuBtn\"; text: \"Font\"; colorMenuStyle: Ribbon.NoColorMenu }\n"
        "        RibbonColorToolButton { objectName: \"smallBtn\"; text: \"Font\"; proportion: Ribbon.Small }\n"
        "        RibbonColorToolButton { objectName: \"smallNoMenuBtn\"; text: \"Font\"; proportion: Ribbon.Small; colorMenuStyle: Ribbon.NoColorMenu }\n"),
                                            900, 300);

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(900, 300);
    QQuickItem* rootItem = attachQmlSource(view, src);
    QVERIFY(rootItem);

    auto* under   = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("underBtn"));
    auto* fill    = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("fillBtn"));
    auto* noMenu  = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("noMenuBtn"));
    auto* smallBtn = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("smallBtn"));
    auto* smallNo  = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("smallNoMenuBtn"));
    QVERIFY(under && fill && noMenu && smallBtn && smallNo);
    // 面板布局要等一次 polish 才把宽高发下来
    QTRY_VERIFY(under->width() > 0 && fill->width() > 0 && noMenu->width() > 0 && smallBtn->width() > 0);

    // ---- 默认形态：WithColorMenu + MenuButtonPopup（widgets setupStandardColorMenu）----
    QCOMPARE(int(under->colorMenuStyle()), int(SARibbonQml::RibbonEnums::WithColorMenu));
    QCOMPARE(int(under->popupMode()), int(SARibbonQml::RibbonEnums::MenuButtonPopup));
    QVERIFY(under->hasMenu());
    QVERIFY(under->colorMenu());
    QVERIFY(under->indicatorGeometry().width() > 0);
    QVERIFY(!under->menuRect().isEmpty());
    QVERIFY(!under->actionRect().isEmpty());
    // 默认颜色是无效色，即 widgets 的"无颜色"标记
    QVERIFY(!under->hasValidColor());
    QVERIFY(under->isNoneColorEnabled());

    // ---- ColorUnderIcon：色带矩形 = core 在图标槽上算出来的那一块 ----
    const QRectF slot = under->iconGeometry();
    QVERIFY(slot.width() > 0 && slot.height() > 0);
    const QSize slotSize(int(slot.width()), int(slot.height()));
    const SA::ColorUnderIconMetrics m = SA::calcColorUnderIconMetrics(slotSize, QSize());
    QVERIFY(m.colorRect.isValid());
    QCOMPARE(under->colorRect(), QRectF(m.colorRect).translated(slot.x(), slot.y()));
    QCOMPARE(int(under->colorRect().height()), SA::colorBandHeight(slotSize.height()));
    // 没有 iconSource：图标矩形无效，色带占满槽宽（已记录的刻意差异）
    QVERIFY(!under->iconDrawRect().isValid());
    QCOMPARE(int(under->colorRect().width()), slotSize.width());
    // 斜线内缩量跟着色带矩形走，与 core 一致
    QCOMPARE(under->colorSlashInset(),
             SA::noneColorSlashLine(QRect(0, 0, int(under->colorRect().width()), int(under->colorRect().height()))).x1());

    // ---- ColorFillToIcon：颜色铺满自然图标盒，四周留等比缩放的 1px 边 ----
    const QRectF fslot = fill->iconGeometry();
    const int side     = fill->iconSide();
    QVERIFY(fslot.width() > 0 && side > 0);
    const QRectF natural(fslot.x() + (fslot.width() - side) / 2.0, fslot.y() + (fslot.height() - side) / 2.0, side, side);
    const qreal inset = SA::ColorToolButtonConstants::COLOR_BLOCK_MARGIN * qreal(side)
                        / qreal(SA::ColorToolButtonConstants::DEFAULT_COLOR_ICON_SIZE);
    QCOMPARE(fill->colorRect(), natural.adjusted(inset, inset, -inset, -inset));
    QVERIFY(!fill->iconDrawRect().isValid());

    // ---- NoColorMenu：菜单对象被销毁，指示箭头与菜单热区一并消失，宽度收窄 ----
    QVERIFY(!noMenu->hasMenu());
    QVERIFY(!noMenu->colorMenu());
    QVERIFY(noMenu->indicatorGeometry().width() <= 0);
    QVERIFY(noMenu->menuRect().isEmpty());
    // 大按钮的箭头画在文本条里，不额外占宽（core sizeHint 只在小按钮分支加
    // indicatorLen），因此同为 "Font" 的大按钮有无菜单宽度相同；小按钮则窄一条
    QCOMPARE(noMenu->width(), under->width());
    QVERIFY(smallNo->width() < smallBtn->width());
    // 图标槽仍然保留：颜色按钮从来不因为"没图标"而塌成纯文本按钮
    QVERIFY(noMenu->iconGeometry().width() > 0);
    QVERIFY(noMenu->colorRect().width() > 0);
}

/**
 * @brief 叶子按宿主发布的矩形上色：有效色填充、无效色画"无颜色"标记
 * @details 判定方式沿用项目既有的 headless 做法——grabWindow 后直接读像素，不看图。
 *          色带是纯色 Rectangle，中心像素必须逐分量相等；"无颜色"标记的斜线由
 *          Canvas 抗锯齿绘制，绝大多数像素是红白混合色，因此数"偏红"像素而不是
 *          逼近某个色值。ColorFillToIcon 下整个自然图标盒都是颜色，取盒心与色带
 *          中心两处一起判。
 */
void TestColorQml::colorButtonRendersSwatch()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    const QByteArray src = colorButtonScene(QByteArrayLiteral(
        "        RibbonColorToolButton { objectName: \"redBtn\"; text: \"Font\"; color: \"#e02020\" }\n"
        "        RibbonColorToolButton { objectName: \"noneBtn\"; text: \"Font\" }\n"
        "        RibbonColorToolButton { objectName: \"fillBtn\"; text: \"Font\"; colorStyle: Ribbon.ColorFillToIcon; color: \"#20e020\" }\n"));

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(600, 300);
    QQuickItem* rootItem = attachQmlSource(view, src);
    QVERIFY(rootItem);

    auto* redBtn  = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("redBtn"));
    auto* noneBtn = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("noneBtn"));
    auto* fillBtn = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("fillBtn"));
    QVERIFY(redBtn && noneBtn && fillBtn);
    QTRY_VERIFY(redBtn->colorRect().width() > 0 && noneBtn->colorRect().width() > 0 && fillBtn->colorRect().width() > 0);

    const QColor bandColor(0xe0, 0x20, 0x20);
    const QColor fillColor(0x20, 0xe0, 0x20);
    // 色带中心的窗口坐标
    const QPoint redCenter  = redBtn->mapToItem(nullptr, redBtn->colorRect().center()).toPoint();
    const QPoint fillCenter = fillBtn->mapToItem(nullptr, fillBtn->colorRect().center()).toPoint();
    const QRectF fillBand   = fillBtn->colorRect();
    const QPoint fillTop    = fillBtn->mapToItem(nullptr, QPointF(fillBand.center().x(), fillBand.top() + 2)).toPoint();

    QImage shot;
    int redPixels = 0;
    QRect noneArea;
    auto grab = [&]() -> bool {
        shot = view.grabWindow();
        if (shot.pixelColor(redCenter) != bandColor || shot.pixelColor(fillCenter) != fillColor) {
            return false;
        }
        noneArea  = QRect(noneBtn->mapToItem(nullptr, noneBtn->colorRect().topLeft()).toPoint(),
                          QSize(int(noneBtn->colorRect().width()), int(noneBtn->colorRect().height())));
        redPixels = countReddishPixels(shot.copy(noneArea));
        return redPixels > 8;
    };
    QTRY_VERIFY_WITH_TIMEOUT(grab(), 3000);
    // ColorFillToIcon：颜色铺满自然图标盒（widgets createColorIcon 的 32x32 减 1px 边，
    // 缩放后边也跟着缩），而不是图标下方那一条色带
    const int fillSide   = fillBtn->iconSide();
    const qreal fillInset = SA::ColorToolButtonConstants::COLOR_BLOCK_MARGIN * qreal(fillSide)
                            / qreal(SA::ColorToolButtonConstants::DEFAULT_COLOR_ICON_SIZE);
    QVERIFY(fillSide > 0);
    QCOMPARE(int(fillBand.height() + 0.5), fillSide - 2 * int(fillInset));
    QVERIFY(fillBand.height() > SA::colorBandHeight(fillSide));
    QCOMPARE(shot.pixelColor(fillTop), fillColor);
    // "无颜色"格是白底红斜线：斜线两端内缩，最左一列不该有红色像素
    QCOMPARE(countReddishPixels(shot.copy(noneArea.x(), noneArea.y(), 1, noneArea.height())), 0);
    QVERIFY(countPixelsNear(shot.copy(noneArea), QColor(Qt::white), 24) * 4 > noneArea.width() * noneArea.height());
    // 有效色的色带是一整块纯色填充，没有斜线也没有留白（边缘抗锯齿像素除外）
    const QRect redArea(redBtn->mapToItem(nullptr, redBtn->colorRect().topLeft()).toPoint(),
                        QSize(int(redBtn->colorRect().width()), int(redBtn->colorRect().height())));
    QVERIFY(countPixelsNear(shot.copy(redArea), bandColor, 8) * 100 > redArea.width() * redArea.height() * 95);
}

/**
 * @brief 真实点击走完颜色按钮的两条路径：动作区报色，菜单区取色
 * @details 动作区点击发 colorClicked(颜色, 勾选态)，与 widgets
 *          SARibbonColorToolButton::onButtonClicked 一致；菜单区打开的是宿主自己
 *          那份 RibbonColorMenu，选色后按钮颜色跟着变并发 colorChanged（不额外发
 *          colorClicked），选"无颜色"则回到无效色。点"自定义颜色"行时按钮转发菜单的
 *          customColorRequested 且菜单保持打开，回灌 addCustomColor 才改颜色。
 *          切成 NoColorMenu 后菜单对象销毁，
 *          openMenu 不再有任何效果。
 */
void TestColorQml::colorButtonMenuPicksColor()
{
    QQmlEngine engine;
    saRibbonRegisterQmlTypes(&engine);

    const QByteArray src = colorButtonScene(QByteArrayLiteral(
        "        RibbonColorToolButton { objectName: \"btn\"; text: \"Font\"; color: \"#112233\" }\n"),
                                            600, 520);

    QQuickView view(&engine, nullptr);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(600, 520);
    QQuickItem* rootItem = attachQmlSource(view, src);
    QVERIFY(rootItem);

    auto* btn = rootItem->findChild< SARibbonQml::RibbonColorToolButton* >(QStringLiteral("btn"));
    QVERIFY(btn);
    QTRY_VERIFY(btn->width() > 0 && !btn->menuRect().isEmpty());
    QVERIFY(btn->colorMenu());
    QVERIFY(!btn->isMenuVisible());

    QSignalSpy clicked(btn, SIGNAL(colorClicked(QColor, bool)));
    QSignalSpy changed(btn, SIGNAL(colorChanged(QColor)));
    QVERIFY(clicked.isValid() && changed.isValid());

    // ---- 动作区：报当前颜色与勾选态 ----
    const QPoint actionCenter = btn->mapToItem(nullptr, btn->actionRect().center()).toPoint();
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, actionCenter);
    QCOMPARE(clicked.count(), 1);
    QCOMPARE(clicked.at(0).at(0).value< QColor >(), QColor(0x11, 0x22, 0x33));
    QCOMPARE(clicked.at(0).at(1).toBool(), false);
    QVERIFY(changed.isEmpty());

    // ---- 菜单区：打开颜色菜单，选一个深浅色块 ----
    const QPoint menuCenter = btn->mapToItem(nullptr, btn->menuRect().center()).toPoint();
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, menuCenter);
    QTRY_VERIFY(btn->isMenuVisible());
    QVERIFY(btn->colorMenu()->isMenuVisible());

    // Popup 的内容项被重挂到窗口 Overlay 下，只能从窗口 contentItem 起走视觉树（NOTES B59）
    auto* shadeGrid = findGrid(view.contentItem(), "colorMenuShadeGrid");
    QVERIFY(shadeGrid);
    QTRY_VERIFY(shadeGrid->colorCount() > 12);
    const QColor shade = shadeGrid->colorAt(12);
    QVERIFY(shade.isValid());
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, sceneCellCenter(shadeGrid, 12));
    QTRY_VERIFY(!btn->isMenuVisible());
    QCOMPARE(btn->color(), shade);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(changed.at(0).at(0).value< QColor >(), shade);
    QCOMPARE(clicked.count(), 1);  // 取色不算点击按钮

    // ---- "无颜色"行：回到无效色，宿主与它那份菜单的开关是同一个 ----
    btn->openMenu();
    QTRY_VERIFY(btn->isMenuVisible());
    QQuickItem* noneRow = findVisualItem(view.contentItem(), QLatin1String("colorMenuNoneRow"));
    QVERIFY(noneRow);
    QTRY_VERIFY(noneRow->isVisible());
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, sceneCenter(noneRow));
    QTRY_VERIFY(!btn->isMenuVisible());
    QVERIFY(!btn->hasValidColor());
    QCOMPARE(changed.count(), 2);
    QVERIFY(!changed.at(1).at(0).value< QColor >().isValid());
    QVERIFY(btn->colorMenu()->isNoneColorEnabled());

    // ---- "自定义颜色"行：按钮转发菜单的 customColorRequested，菜单保持打开 ----
    // 宿主不含 QColorDialog，转发信号让 QML 侧不必伸手进 colorMenu
    QSignalSpy customReq(btn, &SARibbonQml::RibbonColorToolButton::customColorRequested);
    QVERIFY(customReq.isValid());
    btn->openMenu();
    QTRY_VERIFY(btn->isMenuVisible());
    QQuickItem* customRow = findVisualItem(view.contentItem(), QLatin1String("colorMenuCustomRow"));
    QVERIFY(customRow);
    QTRY_VERIFY(customRow->isVisible());
    QTest::mouseClick(&view, Qt::LeftButton, Qt::NoModifier, sceneCenter(customRow));
    QCOMPARE(customReq.count(), 1);
    QVERIFY(btn->isMenuVisible());  // 与菜单自己的语义一致：只发信号，不关闭

    // 回灌：记录到自定义色并选中
    const QColor picked(0x12, 0x34, 0x56);
    btn->colorMenu()->addCustomColor(picked);
    QCOMPARE(btn->color(), picked);
    QCOMPARE(changed.count(), 3);
    QCOMPARE(btn->colorMenu()->customColors().size(), 1);
    btn->closeMenu();
    QTRY_VERIFY(!btn->isMenuVisible());

    // ---- NoColorMenu：菜单销毁，openMenu 无效 ----
    btn->setColorMenuStyle(SARibbonQml::RibbonEnums::NoColorMenu);
    QVERIFY(!btn->hasMenu());
    QVERIFY(!btn->colorMenu());
    QVERIFY(!btn->isMenuVisible());
    btn->openMenu();
    QVERIFY(!btn->isMenuVisible());
    QVERIFY(btn->menuRect().isEmpty());
    // 无菜单时不存在转发路径
    QCOMPARE(customReq.count(), 1);
    // 颜色本身不受菜单样式影响
    QCOMPARE(btn->color(), picked);

    // 切回来：菜单重新建立，且能再打开
    btn->setColorMenuStyle(SARibbonQml::RibbonEnums::WithColorMenu);
    QVERIFY(btn->hasMenu());
    QVERIFY(btn->colorMenu());
    btn->openMenu();
    QTRY_VERIFY(btn->isMenuVisible());
    btn->closeMenu();
    QTRY_VERIFY(!btn->isMenuVisible());
}

QTEST_MAIN(TestColorQml)
#include "tst_color_qml.moc"
