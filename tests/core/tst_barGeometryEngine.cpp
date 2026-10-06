#include <QtTest>
#include <SARibbonCore/SARibbonBarGeometryEngine.h>

/**
 * @brief core SARibbonBarGeometryEngine::calcMinimumWidth 单元测试
 * @details 最小宽度算法是两个前端共用的屏幕规则载体（计划 04：QML 首个消费方，
 *          widgets 后续可复用）。本测试把以下约定钉死：
 *          1. 行区组合：宽松 = max(tab 行, 标题行 + 窗口标题)；紧凑 = 共享行 +
 *             窗口标题（标题骑在 tab 行右侧剩余空间里）；
 *          2. 屏幕未知（headless / 无窗口）：只做组合，不做钳制；
 *          3. 规则 1：含标题总宽不超过屏幕（cap = 2/3 屏宽）；
 *          4. 规则 2：超过 cap 先牺牲标题（标题预留归零）；
 *          5. 规则 3：去掉标题仍超过 cap，允许交叠，最小宽度 = cap。
 */
using Engine = SARibbon::Core::SARibbonBarGeometryEngine;

class TestBarGeometryEngine : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void looseComposition();
    void compactComposition();
    void unknownScreenSkipsRules();
    void titleSacrificedBeforeOverlap();
    void overlapCappedAtTwoThirds();
    void compactTitleSacrifice();
    void degenerateInputs();
};

void TestBarGeometryEngine::looseComposition()
{
    // 宽松样式两行独立：tab 行 600（右侧组在其中），标题行 400（快速访问栏 +
    // 系统条在其中）；标题 120 只加在标题行上 —— max(600, 400+120) = 600
    Engine::MinimumWidthInput in;
    in.isCompactStyle      = false;
    in.tabRowWidth         = 600;
    in.titleRowWidth       = 400;
    in.titleTextWidth      = 120;
    in.screenAvailableWidth = 0;
    QCOMPARE(Engine::calcMinimumWidth(in), 600);

    // 标题行占优：max(500, 400+120) = 520
    in.tabRowWidth = 500;
    QCOMPARE(Engine::calcMinimumWidth(in), 520);

    // 标题骑不进 tab 行的富余：两行都不含标题时布局下限是 600
    in.tabRowWidth   = 600;
    in.titleTextWidth = 0;
    QCOMPARE(Engine::calcMinimumWidth(in), 600);
}

void TestBarGeometryEngine::compactComposition()
{
    // 紧凑样式单行共享：标题宽度直接加进整行（标题在 tab 行右侧的剩余空间）
    Engine::MinimumWidthInput in;
    in.isCompactStyle      = true;
    in.tabRowWidth         = 700;
    in.titleRowWidth       = 0;  // 紧凑样式无独立标题行
    in.titleTextWidth      = 100;
    in.screenAvailableWidth = 0;
    QCOMPARE(Engine::calcMinimumWidth(in), 800);

    // 无标题：纯行宽
    in.titleTextWidth = 0;
    QCOMPARE(Engine::calcMinimumWidth(in), 700);
}

void TestBarGeometryEngine::unknownScreenSkipsRules()
{
    // 屏幕未知：任何宽度都不钳制（headless / 无窗口场景）。宽松组合
    // max(4000, 3000+500) = 4000 —— tab 行占优
    Engine::MinimumWidthInput in;
    in.isCompactStyle      = false;
    in.tabRowWidth         = 4000;
    in.titleRowWidth       = 3000;
    in.titleTextWidth      = 500;
    in.screenAvailableWidth = 0;
    QCOMPARE(Engine::calcMinimumWidth(in), 4000);

    // 标题行占优：max(2000, 3000+500) = 3500
    in.tabRowWidth = 2000;
    QCOMPARE(Engine::calcMinimumWidth(in), 3500);

    // 负数同样按未知处理（防御）
    in.screenAvailableWidth = -1;
    QCOMPARE(Engine::calcMinimumWidth(in), 3500);
}

void TestBarGeometryEngine::titleSacrificedBeforeOverlap()
{
    // 规则 2：含标题 700 > cap(400)，但无标题布局 380 <= 400 —— 牺牲标题，
    // 最小宽度 = 380（窗口保住缩小空间，标题自由区自然塌缩为不显示）
    Engine::MinimumWidthInput in;
    in.isCompactStyle      = false;
    in.tabRowWidth         = 380;
    in.titleRowWidth       = 300;
    in.titleTextWidth      = 320;
    in.screenAvailableWidth = 600;
    QCOMPARE(Engine::calcMinimumWidth(in), 380);

    // 边界：含标题恰好等于 cap —— 不牺牲。标题行 300 + 标题 100 = 400
    // 盖过 tab 行 380，full = 400 == cap
    in.titleTextWidth = 100;
    QCOMPARE(Engine::calcMinimumWidth(in), 400);
}

void TestBarGeometryEngine::overlapCappedAtTwoThirds()
{
    // 规则 3：无标题布局 900 > cap(400) —— 允许控件交叠，最小宽度 = cap
    Engine::MinimumWidthInput in;
    in.isCompactStyle      = false;
    in.tabRowWidth         = 900;
    in.titleRowWidth       = 500;
    in.titleTextWidth      = 100;
    in.screenAvailableWidth = 600;
    QCOMPARE(Engine::calcMinimumWidth(in), 400);

    // 无标题布局超过屏幕整宽（900 > 600）同样落进 cap —— 规则 1 天然满足
    in.screenAvailableWidth = 500;  // cap = 333
    QCOMPARE(Engine::calcMinimumWidth(in), 333);

    // 屏宽不能被 3 整除时向下取整（600*2/3 = 400, 601*2/3 = 400）
    in.screenAvailableWidth = 601;
    QCOMPARE(Engine::calcMinimumWidth(in), 400);
}

void TestBarGeometryEngine::compactTitleSacrifice()
{
    // 紧凑样式同样先牺牲标题：行宽 350、标题 100、屏宽 600（cap 400）
    // 含标题 450 > 400，无标题 350 <= 400 → 350
    Engine::MinimumWidthInput in;
    in.isCompactStyle      = true;
    in.tabRowWidth         = 350;
    in.titleTextWidth      = 100;
    in.screenAvailableWidth = 600;
    QCOMPARE(Engine::calcMinimumWidth(in), 350);

    // 行宽本身超 cap：450 > 400 → 交叠钳制 400
    in.tabRowWidth = 450;
    QCOMPARE(Engine::calcMinimumWidth(in), 400);
}

void TestBarGeometryEngine::degenerateInputs()
{
    // 空输入：至少 1（绝不返回 0/负数，窗口最小宽度必须有意义）
    Engine::MinimumWidthInput in;
    QCOMPARE(Engine::calcMinimumWidth(in), 1);

    // 屏幕极小（cap 计算为 0）也保住 1
    in.screenAvailableWidth = 1;
    QCOMPARE(Engine::calcMinimumWidth(in), 1);

    // 负标题宽度按 0 处理
    in.screenAvailableWidth = 0;
    in.titleTextWidth       = -50;
    in.tabRowWidth          = 100;
    QCOMPARE(Engine::calcMinimumWidth(in), 100);
}

QTEST_MAIN(TestBarGeometryEngine)
#include "tst_barGeometryEngine.moc"
