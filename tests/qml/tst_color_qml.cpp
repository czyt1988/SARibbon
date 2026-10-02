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
#include <SARibbonQml/color/RibbonColorGrid.h>
#include "colorWidgets/SAColorGridWidget.h"
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
 *             验证法，不依赖人眼看图）。
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

QTEST_MAIN(TestColorQml)
#include "tst_color_qml.moc"
