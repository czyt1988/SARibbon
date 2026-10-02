#include <QtTest>
#include <QApplication>
#include <QBuffer>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QSignalSpy>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include <SARibbonQml/SARibbonQmlTypes.h>
#include <SARibbonQml/bar/RibbonBar.h>
#include <SARibbonQml/button/RibbonToolButton.h>
#include <SARibbonQml/category/RibbonCategory.h>
#include <SARibbonQml/customize/RibbonActionRegistry.h>
#include <SARibbonQml/customize/RibbonActionRegistryModel.h>
#include <SARibbonQml/customize/RibbonCustomizer.h>
#include <SARibbonQml/host/RibbonLayoutItemHost.h>
#include <SARibbonQml/panel/RibbonPanel.h>
#include <SARibbonQml/quickaccess/RibbonQuickAccessBar.h>
#include "SARibbonCustomizeData.h"
#include "SARibbonCustomizeWidget.h"

/**
 * @brief QML 定制系统测试（计划 WS-C2）
 * @details 定制系统在两个前端之间共享的只有记录层：core 的
 *          SARibbonCustomizeRecord（含 make* 工厂与 simplify）与
 *          SARibbonCustomizeXml（recordsToXml/recordsFromXml）。寻址层与执行层
 *          各自实现——widgets 走 SARibbonActionsManager + QAction 指针，QML 走
 *          RibbonActionRegistry 的稳定字符串 key + 宿主树查询/修改 API。本测试把
 *          这条分界钉死：
 *          1. 注册表能从声明式宿主树自动收集（autoRegister 对应 widgets
 *             autoRegisteActions），tag 划分、tag 名、搜索、key↔item 双向解析、
 *             命令模板（无活动项的描述符）与可定制标记都符合 widgets 语义；
 *          2. 列表模型按 filterTag/searchText 收窄，行 role 与 infoAt 的 map
 *             都是满 key 的（NOTES B60）；
 *          3. 记录经 core simplify 合并的规则与 widgets 相同（增删相消、改名只
 *             留最后一条、连续顺序调整相加、和为零的顺序调整被删）；
 *          4. apply 真的改动了宿主树（新增 category/panel/命令、改名、调序、
 *             显隐、快速访问栏增删移），reverse 按 widgets
 *             sa_customize_datas_reverse 的表恢复——包括它的不对称性：Remove* 与
 *             Rename* 没有逆操作；
 *          5. enforceCanCustomize 打开后语义与 widgets 的 isCanCustomize 闸门一致；
 *          6. XML 往返一致，且**跨前端兼容**：widgets
 *             sa_customize_datas_to_xml 写出的字节流 QML 能直接 apply，QML
 *             appliedToXml 写出的字节流 widgets sa_customize_datas_from_xml 能
 *             逐字段读回。
 */
namespace
{

/// 场景 QML：两个 category（其中一个两面板）+ 一个快速访问栏按钮，全部带 objectName
/// 以便按名寻址（与 widgets 侧定制记录依赖 objectName 的做法一致）
const char* kSceneQml = R"QML(import QtQuick 2.12
import SARibbon 3.0
Item {
    width: 900
    height: 320
    RibbonBar {
        objectName: "bar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        RibbonQuickAccessBar {
            objectName: "qab"
            RibbonToolButton { objectName: "q0"; text: "Save" }
        }
        RibbonCategory {
            objectName: "cat0"
            title: "Home"
            RibbonPanel {
                objectName: "p0"
                panelTitle: "Clip"
                RibbonToolButton { objectName: "b0"; text: "Paste" }
                RibbonToolButton { objectName: "b1"; text: "Cut" }
            }
            RibbonPanel {
                objectName: "p1"
                panelTitle: "Font"
                RibbonToolButton { objectName: "b2"; text: "Bold" }
            }
        }
        RibbonCategory {
            objectName: "cat1"
            title: "Insert"
            RibbonPanel {
                objectName: "p2"
                panelTitle: "Tables"
                RibbonToolButton { objectName: "b3"; text: "Table" }
            }
        }
    }
}
)QML";

/// 一个已布局的场景：引擎、组件、视图与 bar 宿主
/// @note 组件与根对象都挂在引擎上，活得比函数调用久；bar 由 QML 声明创建，
///       因此叶子在 componentComplete 里自动建立，不需要 ensureQmlLeaf
struct Scene
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickView view;
    SARibbonQml::RibbonBar* bar = nullptr;

    explicit Scene(const char* src = kSceneQml) : component(&engine), view(&engine, nullptr)
    {
        saRibbonRegisterQmlTypes(&engine);
        view.setResizeMode(QQuickView::SizeRootObjectToView);
        view.resize(900, 320);
        component.setData(QByteArray(src), QUrl());
        QObject* rootObj = component.create();
        QQuickItem* rootItem = qobject_cast< QQuickItem* >(rootObj);
        if (rootItem) {
            rootItem->setParentItem(view.contentItem());
            view.setContent(QUrl(), &component, rootObj);
            view.show();
            (void)QTest::qWaitForWindowExposed(&view);
            bar = qobject_cast< SARibbonQml::RibbonBar* >(rootItem->findChild< QQuickItem* >(QStringLiteral("bar")));
        }
    }
    Q_DISABLE_COPY(Scene)
};

/// 断言一个 map 含全部约定 key（NOTES B60：缺 key 会让 Qt 6.7 的 V4 绑定彻底失效）
void verifyFullKeys(const QVariantMap& m, const QStringList& keys)
{
    for (const QString& k : keys) {
        QVERIFY2(m.contains(k), qPrintable(QStringLiteral("map misses key: %1").arg(k)));
    }
}

/// 把记录列表写成 widgets 的字节流（跨前端兼容性测试的 widgets 侧）
QByteArray widgetsRecordsToXml(const QList< SARibbonCustomizeData >& cds)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QXmlStreamWriter xml(&buffer);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    xml.setCodec("utf-8");
#endif
    xml.writeStartDocument();
    sa_customize_datas_to_xml(&xml, cds);
    xml.writeEndDocument();
    return data;
}

}

class TestCustomizeQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void registryAutoRegisterAndModel();
    void recordsSimplifyRules();
    void applyAndReverseHostTree();
    void enforceCanCustomizeGate();
    void quickAccessRecords();
    void visibleCategoryRecord();
    void xmlRoundTripAndCrossFrontend();
};

/**
 * \if ENGLISH
 * @brief The registry collects a declarative host tree exactly like the widgets
 *        actions manager collects a widget tree
 * \endif
 *
 * \if CHINESE
 * @brief 注册表从声明式宿主树自动收集，语义与 widgets actions manager 一致
 * \endif
 */
void TestCustomizeQml::registryAutoRegisterAndModel()
{
    Scene scene;
    QVERIFY(scene.bar);
    QCOMPARE(scene.bar->categoryCount(), 2);

    SARibbonQml::RibbonActionRegistry registry;
    QSignalSpy tagSpy(&registry, &SARibbonQml::RibbonActionRegistry::actionTagChanged);
    const QMap< int, SARibbonQml::RibbonCategory* > tagMap = registry.autoRegister(scene.bar);

    // 4 个面板内命令 + 1 个快速访问栏按钮
    QCOMPARE(registry.count(), 5);
    const int tag0 = int(SARibbon::Core::AutoCategoryDistinguishBeginTag);
    QCOMPARE(tagMap.size(), 2);
    QCOMPARE(tagMap.value(tag0), scene.bar->categoryAt(0));
    QCOMPARE(tagMap.value(tag0 + 1), scene.bar->categoryAt(1));
    // tag 名取自 category 标题（widgets autoRegisteActions 的 setTagName 行为）
    QCOMPARE(registry.tagName(tag0), QStringLiteral("Home"));
    QCOMPARE(registry.tagName(tag0 + 1), QStringLiteral("Insert"));
    QCOMPARE(registry.tagName(SARibbonQml::RibbonActionRegistry::QuickAccessActionTag),
             QStringLiteral("quick access bar"));
    QCOMPARE(registry.actionTags().size(), 3);
    QVERIFY(tagSpy.count() >= 3);

    QCOMPARE(registry.filter(tag0).size(), 3);
    QCOMPARE(registry.filter(tag0 + 1).size(), 1);
    QCOMPARE(registry.filter(SARibbonQml::RibbonActionRegistry::QuickAccessActionTag).size(), 1);
    QCOMPARE(registry.allActions().size(), 5);

    // 搜索按显示名匹配（widgets search 的 contains 语义）
    const QList< SARibbonQml::RibbonActionDescriptor > hits = registry.search(QStringLiteral("Cut"));
    QCOMPARE(hits.size(), 1);
    QCOMPARE(hits.first().text, QStringLiteral("Cut"));
    QVERIFY(hits.first().hasItem());

    // key ↔ item 双向解析，且 key 稳定
    SARibbonQml::RibbonLayoutItemHost* item = scene.bar->categoryAt(0)->panelAt(0)->childItemAt(1);
    QVERIFY(item);
    const QString key = registry.key(item);
    QVERIFY(!key.isEmpty());
    QCOMPARE(registry.item(key), item);
    QCOMPARE(registry.descriptor(key).text, QStringLiteral("Cut"));
    QCOMPARE(registry.tagOf(item), tag0);

    // 描述符 map 满 key
    verifyFullKeys(registry.actionInfo(key),
                   QStringList{ QStringLiteral("key"), QStringLiteral("text"), QStringLiteral("iconSource"),
                                QStringLiteral("tag"), QStringLiteral("proportion"), QStringLiteral("hasItem") });
    // 越界/未知 key 也必须是满 key 的空描述（NOTES B60）
    verifyFullKeys(registry.actionInfo(QStringLiteral("no-such-key")),
                   QStringList{ QStringLiteral("key"), QStringLiteral("text"), QStringLiteral("iconSource"),
                                QStringLiteral("tag"), QStringLiteral("proportion"), QStringLiteral("hasItem") });
    QCOMPARE(registry.actionInfoList(tag0).size(), 3);
    QCOMPARE(registry.tagInfoList().size(), 3);
    QCOMPARE(registry.searchInfo(QStringLiteral("Cut")).size(), 1);

    // 命令模板：只有描述、没有活动项（widgets 侧对应尚未 addRibbonAction 的 QAction）
    QVERIFY(registry.registeCommand(int(SARibbon::Core::CommonlyUsedActionTag),
                                    QStringLiteral("cmd_template"),
                                    QStringLiteral("Template Command"),
                                    QString(),
                                    SARibbon::Core::SARibbonRowProportion::Large));
    QCOMPARE(registry.count(), 6);
    const SARibbonQml::RibbonActionDescriptor tpl = registry.descriptor(QStringLiteral("cmd_template"));
    QVERIFY(tpl.isValid());
    QVERIFY(!tpl.hasItem());
    QCOMPARE(registry.item(QStringLiteral("cmd_template")), nullptr);
    // 重复 key 被拒
    QVERIFY(!registry.registeCommand(int(SARibbon::Core::CommonlyUsedActionTag),
                                     QStringLiteral("cmd_template"),
                                     QStringLiteral("Again")));
    registry.unregisteKey(QStringLiteral("cmd_template"));
    QCOMPARE(registry.count(), 5);
    QVERIFY(!registry.descriptor(QStringLiteral("cmd_template")).isValid());

    // 可定制标记：widgets 侧由应用自己给 QAction 打标，QML 侧由注册表批量打
    QCOMPARE(registry.markCustomizable(true), 5);
    QVERIFY(SARibbon::Core::isCanCustomize(item));

    // ---- 列表模型 ----
    SARibbonQml::RibbonActionRegistryModel model;
    model.setRegistry(&registry);
    model.setFilter(tag0);
    QCOMPARE(model.rowCount(), 3);
    model.setSearchText(QStringLiteral("Cut"));
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.keyAt(0), key);
    QCOMPARE(model.rowOfKey(key), 0);
    verifyFullKeys(model.infoAt(0),
                   QStringList{ QStringLiteral("key"), QStringLiteral("text"), QStringLiteral("iconSource"),
                                QStringLiteral("tag"), QStringLiteral("proportion"), QStringLiteral("hasItem") });
    const QHash< int, QByteArray > roles = model.roleNames();
    QVERIFY(roles.values().contains(QByteArrayLiteral("text")));
    QVERIFY(roles.values().contains(QByteArrayLiteral("tagName")));
    QCOMPARE(model.data(model.index(0), SARibbonQml::RibbonActionRegistryModel::TextRole).toString(),
             QStringLiteral("Cut"));
    QCOMPARE(model.data(model.index(0), Qt::DisplayRole).toString(), QStringLiteral("Cut"));
    // 清空搜索 → 全 tag 视图（filterTag 设为 UnknowActionTag 表示不过滤）
    model.setSearchText(QString());
    model.setFilter(int(SARibbon::Core::UnknowActionTag));
    QCOMPARE(model.rowCount(), 5);
    QVERIFY(!model.descriptorAt(99).isValid());
    model.uninstallRegistry();
    QCOMPARE(model.rowCount(), 0);
}

/**
 * \if ENGLISH
 * @brief Pending records merge through the core simplify algorithm, so the two
 *        front ends cannot drift on which records survive
 * \endif
 *
 * \if CHINESE
 * @brief 待应用记录经 core simplify 合并，两个前端不会在"哪些记录留下"上分叉
 * \endif
 */
void TestCustomizeQml::recordsSimplifyRules()
{
    SARibbonQml::RibbonCustomizer cz;
    // 增删相消
    QVERIFY(cz.addCategory(QStringLiteral("Tmp"), -1, QStringLiteral("tmpcat")));
    QVERIFY(cz.removeCategory(QStringLiteral("tmpcat")));
    QCOMPARE(cz.recordCount(), 2);
    QCOMPARE(cz.simplifyRecords(), 0);

    // 同一对象连续改名只留最后一条
    QVERIFY(cz.renameCategory(QStringLiteral("First"), QStringLiteral("cat0")));
    QVERIFY(cz.renameCategory(QStringLiteral("Second"), QStringLiteral("cat0")));
    QCOMPARE(cz.simplifyRecords(), 1);
    QCOMPARE(cz.recordInfo(0).value(QStringLiteral("key")).toString(), QStringLiteral("Second"));
    QVERIFY(cz.recordInfo(0).contains(QStringLiteral("type")));
    cz.clearRecords();
    QCOMPARE(cz.recordCount(), 0);

    // 连续顺序调整相加，和为零则整条删除
    QVERIFY(cz.changeCategoryOrder(QStringLiteral("cat0"), 1));
    QVERIFY(cz.changeCategoryOrder(QStringLiteral("cat0"), 1));
    QCOMPARE(cz.simplifyRecords(), 1);
    QCOMPARE(cz.recordInfo(0).value(QStringLiteral("index")).toInt(), 2);
    QVERIFY(cz.changeCategoryOrder(QStringLiteral("cat0"), -1));
    QVERIFY(cz.changeCategoryOrder(QStringLiteral("cat0"), -1));
    QCOMPARE(cz.simplifyRecords(), 0);

    // 连续显隐只留最后一条
    QVERIFY(cz.visibleCategory(QStringLiteral("cat1"), false));
    QVERIFY(cz.visibleCategory(QStringLiteral("cat1"), true));
    QCOMPARE(cz.simplifyRecords(), 1);
    QCOMPARE(cz.recordInfo(0).value(QStringLiteral("index")).toInt(), 1);

    // 无 bar 时 apply 直接失败，不会半途改树
    QVERIFY(!cz.apply());
}

/**
 * \if ENGLISH
 * @brief apply mutates the real host tree and reverse restores it, with the
 *        widgets inverse table (the Remove and Rename record types have no inverse)
 * \endif
 *
 * \if CHINESE
 * @brief apply 真的改动宿主树，reverse 按 widgets 的逆操作表恢复
 *        （Remove 与 Rename 两类记录没有逆操作）
 * \endif
 */
void TestCustomizeQml::applyAndReverseHostTree()
{
    Scene scene;
    QVERIFY(scene.bar);
    SARibbonQml::RibbonActionRegistry registry;
    registry.autoRegister(scene.bar);
    QVERIFY(registry.registeCommand(int(SARibbon::Core::CommonlyUsedActionTag),
                                    QStringLiteral("cmd_new"),
                                    QStringLiteral("New Command"),
                                    QString(),
                                    SARibbon::Core::SARibbonRowProportion::Large));

    SARibbonQml::RibbonCustomizer cz;
    cz.setBar(scene.bar);
    cz.setRegistry(&registry);
    QSignalSpy failSpy(&cz, &SARibbonQml::RibbonCustomizer::applyFailed);

    QVERIFY(cz.addCategory(QStringLiteral("New Cat"), -1, QStringLiteral("newcat")));
    QVERIFY(cz.addPanel(QStringLiteral("New Panel"), -1, QStringLiteral("newcat"), QStringLiteral("newpanel")));
    QVERIFY(cz.addAction(QStringLiteral("cmd_new"),
                         int(SARibbon::Core::SARibbonRowProportion::Large),
                         QStringLiteral("newcat"),
                         QStringLiteral("newpanel")));
    QVERIFY(cz.renameCategory(QStringLiteral("Renamed"), QStringLiteral("cat0")));
    QVERIFY(cz.renamePanel(QStringLiteral("Clip2"), QStringLiteral("cat0"), QStringLiteral("p0")));
    QVERIFY(cz.changeCategoryOrder(QStringLiteral("cat1"), -1));
    QCOMPARE(cz.recordCount(), 6);

    QVERIFY(cz.apply());
    QCOMPARE(failSpy.count(), 0);
    QCOMPARE(cz.recordCount(), 0);
    QCOMPARE(cz.appliedCount(), 6);
    QVERIFY(cz.isApplied());
    QCOMPARE(cz.appliedInfoList().size(), 6);

    // 结构确实变了
    QCOMPARE(scene.bar->categoryCount(), 3);
    SARibbonQml::RibbonCategory* newCat = scene.bar->categoryByObjectName(QStringLiteral("newcat"));
    QVERIFY(newCat);
    QCOMPARE(newCat->title(), QStringLiteral("New Cat"));
    QCOMPARE(scene.bar->categoryIndex(newCat), 2);
    // cat1 前移到 0
    QCOMPARE(scene.bar->categoryIndex(scene.bar->categoryByObjectName(QStringLiteral("cat1"))), 0);
    QCOMPARE(scene.bar->categoryByObjectName(QStringLiteral("cat0"))->title(), QStringLiteral("Renamed"));
    QCOMPARE(newCat->panelCount(), 1);
    SARibbonQml::RibbonPanel* newPanel = newCat->panelByObjectName(QStringLiteral("newpanel"));
    QVERIFY(newPanel);
    QCOMPARE(newPanel->panelTitle(), QStringLiteral("New Panel"));
    QCOMPARE(scene.bar->categoryByObjectName(QStringLiteral("cat0"))->panelByObjectName(QStringLiteral("p0"))
                 ->panelTitle(),
             QStringLiteral("Clip2"));
    // 命令模板落地成了一个宿主，文字与比例都来自描述符
    QCOMPARE(newPanel->childItemCount(), 1);
    SARibbonQml::RibbonLayoutItemHost* made = newPanel->childItemAt(0);
    QVERIFY(made);
    QCOMPARE(made->property("text").toString(), QStringLiteral("New Command"));
    QCOMPARE(int(made->property("proportion").toInt()), int(SARibbon::Core::SARibbonRowProportion::Large));
    // 落地后 key 仍然指向同一个宿主（bindItem 修补了描述符）
    QCOMPARE(registry.item(QStringLiteral("cmd_new")), made);
    QVERIFY(made->qmlLeaf());
    // 新叶子真的挂进了视觉树（新面板自己的叶子已由 insertPanel 建立）
    QVERIFY(newPanel->qmlLeaf());
    QVERIFY(made->qmlLeaf()->parentItem());

    // 删除：只摘不销毁（对应 widgets removeAction 把 QAction 留在 manager 里）
    QVERIFY(cz.removeAction(QStringLiteral("newcat"), QStringLiteral("newpanel"), QStringLiteral("cmd_new")));
    QVERIFY(cz.apply());
    QCOMPARE(newPanel->childItemCount(), 0);
    QVERIFY(registry.item(QStringLiteral("cmd_new")) == made);  // 宿主仍活着
    QVERIFY(made->parentItem() == nullptr || !made->isVisible());

    // 撤销：结构恢复，改名与删除不恢复（widgets sa_customize_datas_reverse 的不对称性）
    QVERIFY(cz.reverse());
    QCOMPARE(cz.appliedCount(), 0);
    QVERIFY(!cz.isApplied());
    QCOMPARE(scene.bar->categoryCount(), 2);
    QVERIFY(!scene.bar->categoryByObjectName(QStringLiteral("newcat")));
    QCOMPARE(scene.bar->categoryIndex(scene.bar->categoryByObjectName(QStringLiteral("cat1"))), 1);
    // Rename* 无逆操作：标题保持改后的值
    QCOMPARE(scene.bar->categoryByObjectName(QStringLiteral("cat0"))->title(), QStringLiteral("Renamed"));
}

/**
 * \if ENGLISH
 * @brief The customizable gate is opt-in and, once on, refuses unmarked objects
 *        exactly like the widgets isCanCustomize check
 * \endif
 *
 * \if CHINESE
 * @brief 可定制闸门是可选的；打开后对未标记对象的拒绝与 widgets isCanCustomize 一致
 * \endif
 */
void TestCustomizeQml::enforceCanCustomizeGate()
{
    Scene scene;
    QVERIFY(scene.bar);
    SARibbonQml::RibbonCustomizer cz;
    cz.setBar(scene.bar);
    cz.setEnforceCanCustomize(true);
    QVERIFY(cz.isEnforceCanCustomize());
    QSignalSpy failSpy(&cz, &SARibbonQml::RibbonCustomizer::applyFailed);

    SARibbonQml::RibbonCategory* cat0 = scene.bar->categoryByObjectName(QStringLiteral("cat0"));
    QVERIFY(cat0);
    QVERIFY(!SARibbon::Core::isCanCustomize(cat0));  // core 标记默认 false
    const int panelsBefore = cat0->panelCount();

    QVERIFY(cz.addPanel(QStringLiteral("Locked"), -1, QStringLiteral("cat0"), QStringLiteral("lockedpanel")));
    QVERIFY(!cz.apply());
    QCOMPARE(failSpy.count(), 1);
    QCOMPARE(cat0->panelCount(), panelsBefore);  // 树没被改动
    QCOMPARE(cz.recordCount(), 1);               // 失败的记录留在待应用列表

    // 打标之后重试即通过（记录还在队列里，无需重新产出）
    cz.setCanCustomize(cat0, true);
    QVERIFY(SARibbon::Core::isCanCustomize(cat0));
    QVERIFY(cz.apply());
    QCOMPARE(cat0->panelCount(), panelsBefore + 1);
    QVERIFY(cat0->panelByObjectName(QStringLiteral("lockedpanel")));
    QCOMPARE(cz.recordCount(), 0);

    // 未知对象名的记录始终失败并给出诊断
    QVERIFY(cz.removePanel(QStringLiteral("nosuchcat"), QStringLiteral("nosuchpanel")));
    QVERIFY(!cz.apply());
    QCOMPARE(failSpy.count(), 2);
    QVERIFY(!failSpy.at(1).at(2).toString().isEmpty());
}

/**
 * \if ENGLISH
 * @brief Quick access bar records add, reorder and remove buttons, and a removed
 *        declarative button can be put back because detaching never destroys
 * \endif
 *
 * \if CHINESE
 * @brief 快速访问栏记录能增、移、删按钮；声明式按钮被删后还能加回来，
 *        因为摘除从不销毁宿主
 * \endif
 */
void TestCustomizeQml::quickAccessRecords()
{
    Scene scene;
    QVERIFY(scene.bar);
    SARibbonQml::RibbonQuickAccessBar* qab = scene.bar->quickAccessBar();
    QVERIFY(qab);
    QCOMPARE(qab->buttonCount(), 1);
    SARibbonQml::RibbonToolButton* q0 = qab->buttonAt(0);
    QVERIFY(q0);

    SARibbonQml::RibbonActionRegistry registry;
    registry.autoRegister(scene.bar);
    QVERIFY(registry.registeCommand(int(SARibbon::Core::CommonlyUsedActionTag),
                                    QStringLiteral("cmd_undo"),
                                    QStringLiteral("Undo")));
    const QString q0Key = registry.key(q0);
    QVERIFY(!q0Key.isEmpty());

    SARibbonQml::RibbonCustomizer cz;
    cz.setBar(scene.bar);
    cz.setRegistry(&registry);

    // 追加到末尾（index < 0）
    QVERIFY(cz.addQuickAction(QStringLiteral("cmd_undo"), -1));
    QVERIFY(cz.apply());
    QCOMPARE(qab->buttonCount(), 2);
    SARibbonQml::RibbonToolButton* undoBtn = qobject_cast< SARibbonQml::RibbonToolButton* >(
        registry.item(QStringLiteral("cmd_undo")));
    QVERIFY(undoBtn);
    QCOMPARE(qab->buttonIndex(undoBtn), 1);
    QVERIFY(undoBtn->qmlLeaf());

    // 顺序记录携带相对位移（widgets 语义）
    QVERIFY(cz.changeQuickActionOrder(QStringLiteral("cmd_undo"), -1));
    QVERIFY(cz.apply());
    QCOMPARE(qab->buttonIndex(undoBtn), 0);
    QCOMPARE(qab->buttonIndex(q0), 1);

    // 撤销：逆记录把顺序调回去，并把加进来的按钮摘掉
    QVERIFY(cz.reverse());
    QCOMPARE(qab->buttonCount(), 1);
    QCOMPARE(qab->buttonAt(0), q0);

    // 声明式按钮摘下再加回，仍是同一个宿主
    QVERIFY(cz.removeQuickAction(q0Key));
    QVERIFY(cz.apply());
    QCOMPARE(qab->buttonCount(), 0);
    QVERIFY(cz.addQuickAction(q0Key, -1));
    QVERIFY(cz.apply());
    QCOMPARE(qab->buttonCount(), 1);
    QCOMPARE(qab->buttonAt(0), q0);
}

/**
 * \if ENGLISH
 * @brief A VisibleCategory record hides and shows a category through the bar,
 *        and its inverse restores the previous state
 * \endif
 *
 * \if CHINESE
 * @brief VisibleCategory 记录经 bar 隐藏/显示 category，其逆记录恢复原状态
 * \endif
 */
void TestCustomizeQml::visibleCategoryRecord()
{
    Scene scene;
    QVERIFY(scene.bar);
    SARibbonQml::RibbonCategory* cat1 = scene.bar->categoryByObjectName(QStringLiteral("cat1"));
    QVERIFY(cat1);
    QVERIFY(!scene.bar->isCategoryHidden(cat1));

    SARibbonQml::RibbonCustomizer cz;
    cz.setBar(scene.bar);
    QVERIFY(cz.visibleCategory(QStringLiteral("cat1"), false));
    QVERIFY(cz.apply());
    QVERIFY(scene.bar->isCategoryHidden(cat1));
    QTRY_VERIFY(!cat1->isVisible());

    QVERIFY(cz.reverse());
    QVERIFY(!scene.bar->isCategoryHidden(cat1));
    QVERIFY(!cz.isApplied());

    // 再隐藏一次，确认状态稳定可重复（simplify 只留最后一条显隐记录）
    QVERIFY(cz.visibleCategory(QStringLiteral("cat1"), false));
    QVERIFY(cz.visibleCategory(QStringLiteral("cat1"), false));
    QCOMPARE(cz.recordCount(), 2);
    QCOMPARE(cz.simplifyRecords(), 1);
    QVERIFY(cz.apply());
    QVERIFY(scene.bar->isCategoryHidden(cat1));
}

/**
 * \if ENGLISH
 * @brief XML persistence round trips, and the byte stream is interchangeable
 *        between the two front ends
 * \endif
 *
 * \if CHINESE
 * @brief XML 持久化可往返，且字节流在两个前端之间可以互换
 * \endif
 */
void TestCustomizeQml::xmlRoundTripAndCrossFrontend()
{
    // ---- QML 写出 → QML 读回 ----
    QByteArray xmlData;
    {
        Scene scene;
        QVERIFY(scene.bar);
        SARibbonQml::RibbonCustomizer cz;
        cz.setBar(scene.bar);
        QVERIFY(cz.addCategory(QStringLiteral("\u65b0\u5efa"), -1, QStringLiteral("newcat")));
        QVERIFY(cz.addPanel(QStringLiteral("\u9762\u677f"), -1, QStringLiteral("newcat"), QStringLiteral("newpanel")));
        QVERIFY(cz.renameCategory(QStringLiteral("Home2"), QStringLiteral("cat0")));
        QVERIFY(cz.apply());
        QCOMPARE(cz.appliedCount(), 3);
        xmlData = cz.appliedToXml();
        QVERIFY(!xmlData.isEmpty());
        QVERIFY(xmlData.contains("sa-ribbon-customize"));
        // 非 ASCII 标题必须以 UTF-8 落到字节流里（writer 建在 QIODevice 上的原因）
        QVERIFY(xmlData.contains(QString(QStringLiteral("\u65b0\u5efa")).toUtf8()));
    }

    {
        Scene scene;
        QVERIFY(scene.bar);
        QCOMPARE(scene.bar->categoryCount(), 2);
        SARibbonQml::RibbonCustomizer cz;
        cz.setBar(scene.bar);
        QVERIFY(cz.applyFromXml(xmlData));
        QCOMPARE(scene.bar->categoryCount(), 3);
        SARibbonQml::RibbonCategory* newCat = scene.bar->categoryByObjectName(QStringLiteral("newcat"));
        QVERIFY(newCat);
        QCOMPARE(newCat->title(), QStringLiteral("\u65b0\u5efa"));
        QCOMPARE(newCat->panelCount(), 1);
        QVERIFY(newCat->panelByObjectName(QStringLiteral("newpanel")));
        QCOMPARE(newCat->panelByObjectName(QStringLiteral("newpanel"))->panelTitle(), QStringLiteral("\u9762\u677f"));
        QCOMPARE(scene.bar->categoryByObjectName(QStringLiteral("cat0"))->title(), QStringLiteral("Home2"));
        QCOMPARE(cz.appliedCount(), 3);
        // 空字节流与坏字节流都被拒
        QVERIFY(!cz.applyFromXml(QByteArray()));
        QVERIFY(!cz.applyFromXml(QByteArrayLiteral("<not-a-customize-file/>")));
    }

    // ---- widgets 写出 → QML 读回 ----
    QList< SARibbonCustomizeData > cds;
    cds.append(SARibbonCustomizeData::makeAddCategoryCustomizeData(QStringLiteral("\u63d2\u5165"),
                                                                  -1,
                                                                  QStringLiteral("wcat")));
    cds.append(SARibbonCustomizeData::makeRenameCategoryCustomizeData(QStringLiteral("Insert2"),
                                                                     QStringLiteral("cat1")));
    cds.append(SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(QStringLiteral("cat1"), -1));
    cds.append(SARibbonCustomizeData::makeVisibleCategoryCustomizeData(QStringLiteral("cat0"), false));
    const QByteArray fromWidgets = widgetsRecordsToXml(cds);
    QVERIFY(!fromWidgets.isEmpty());
    {
        Scene scene;
        QVERIFY(scene.bar);
        SARibbonQml::RibbonCustomizer cz;
        cz.setBar(scene.bar);
        QVERIFY(cz.applyFromXml(fromWidgets));
        QCOMPARE(cz.appliedCount(), 4);
        QCOMPARE(scene.bar->categoryCount(), 3);
        QVERIFY(scene.bar->categoryByObjectName(QStringLiteral("wcat")));
        QCOMPARE(scene.bar->categoryByObjectName(QStringLiteral("wcat"))->title(), QStringLiteral("\u63d2\u5165"));
        QCOMPARE(scene.bar->categoryByObjectName(QStringLiteral("cat1"))->title(), QStringLiteral("Insert2"));
        QCOMPARE(scene.bar->categoryIndex(scene.bar->categoryByObjectName(QStringLiteral("cat1"))), 0);
        QVERIFY(scene.bar->isCategoryHidden(scene.bar->categoryByObjectName(QStringLiteral("cat0"))));
    }

    // ---- QML 写出 → widgets 读回（逐字段对照）----
    QXmlStreamReader reader(xmlData);
    const QList< SARibbonCustomizeData > back = sa_customize_datas_from_xml(&reader, nullptr);
    QVERIFY(!reader.hasError());
    QCOMPARE(back.size(), 3);
    QCOMPARE(int(back.at(0).actionType()), int(SARibbon::Core::SARibbonCustomizeRecord::AddCategoryActionType));
    QCOMPARE(back.at(0).keyValue, QStringLiteral("\u65b0\u5efa"));
    QCOMPARE(back.at(0).categoryObjNameValue, QStringLiteral("newcat"));
    QCOMPARE(back.at(0).indexValue, -1);
    QCOMPARE(int(back.at(1).actionType()), int(SARibbon::Core::SARibbonCustomizeRecord::AddPanelActionType));
    QCOMPARE(back.at(1).panelObjNameValue, QStringLiteral("newpanel"));
    QCOMPARE(back.at(1).categoryObjNameValue, QStringLiteral("newcat"));
    QCOMPARE(int(back.at(2).actionType()), int(SARibbon::Core::SARibbonCustomizeRecord::RenameCategoryActionType));
    QCOMPARE(back.at(2).keyValue, QStringLiteral("Home2"));
}

QTEST_MAIN(TestCustomizeQml)
#include "tst_customize_qml.moc"
