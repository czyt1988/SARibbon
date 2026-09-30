#include <QtTest>
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonMainWindow.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonActionsManager.h"
#include "SARibbonCustomizeWidget.h"
#include "SARibbonCustomizeData.h"

/**
 * @brief 快速访问栏自定义测试（GitHub #67 / Gitee IA62AX）
 *
 * 覆盖：数据层 apply（增/删/移）、序列化往返、旧 XML 兼容、反向（重置）。
 */
namespace {
QByteArray datasToXmlBytes(const QList< SARibbonCustomizeData >& cds)
{
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    QXmlStreamWriter xml(&buf);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");                  // set the encoding before writeStartDocument
#endif
    xml.writeStartDocument();
    if (!sa_customize_datas_to_xml(&xml, cds)) {
        return QByteArray();
    }
    xml.writeEndDocument();
    buf.close();
    return bytes;
}
}  // namespace

class SARibbonQuickAccessCustomizeTest : public QObject
{
    Q_OBJECT
private slots:
    void testAddRemoveMoveApply();
    void testXmlRoundTrip();
    void testOldXmlCompatible();
    void testReverse();
};

void SARibbonQuickAccessCustomizeTest::testAddRemoveMoveApply()
{
    SARibbonMainWindow w;
    SARibbonBar* bar     = w.ribbonBar();
    SARibbonCategory* c  = bar->addCategoryPage(QStringLiteral("main"));
    SARibbonPanel* p     = c->addPanel(QStringLiteral("panel1"));
    QAction* a1          = new QAction(QStringLiteral("act1"), &w);
    QAction* a2          = new QAction(QStringLiteral("act2"), &w);
    p->addLargeAction(a1);
    p->addLargeAction(a2);

    SARibbonActionsManager mgr(bar);
    mgr.registeAction(a1, SARibbonActionsManager::CommonlyUsedActionTag);
    mgr.registeAction(a2, SARibbonActionsManager::CommonlyUsedActionTag);

    SARibbonQuickAccessBar* quickBar = bar->quickAccessBar();
    QVERIFY(quickBar);
    QVERIFY(!quickBar->actions().contains(a1));

    // 增加到快速访问栏
    QString key1 = mgr.key(a1);
    QVERIFY(!key1.isEmpty());
    SARibbonCustomizeData add1 = SARibbonCustomizeData::makeAddQuickActionCustomizeData(key1, &mgr);
    QVERIFY(add1.apply(bar));
    QCOMPARE(quickBar->actions(), QList< QAction* >({ a1 }));

    // 再加第二个
    QString key2 = mgr.key(a2);
    SARibbonCustomizeData add2 = SARibbonCustomizeData::makeAddQuickActionCustomizeData(key2, &mgr);
    QVERIFY(add2.apply(bar));
    QCOMPARE(quickBar->actions(), QList< QAction* >({ a1, a2 }));

    // 移动 a2 到 a1 之前（-1）
    SARibbonCustomizeData mv = SARibbonCustomizeData::makeChangeQuickActionOrderCustomizeData(key2, &mgr, -1);
    QVERIFY(mv.apply(bar));
    QCOMPARE(quickBar->actions(), QList< QAction* >({ a2, a1 }));

    // 移除 a1
    SARibbonCustomizeData rm = SARibbonCustomizeData::makeRemoveQuickActionCustomizeData(key1, &mgr);
    QVERIFY(rm.apply(bar));
    QCOMPARE(quickBar->actions(), QList< QAction* >({ a2 }));

    // 无效 key 返回 false
    SARibbonCustomizeData bad = SARibbonCustomizeData::makeAddQuickActionCustomizeData("nonexist", &mgr);
    QVERIFY(!bad.apply(bar));
}

void SARibbonQuickAccessCustomizeTest::testXmlRoundTrip()
{
    SARibbonMainWindow w;
    SARibbonActionsManager mgr(w.ribbonBar());
    QAction* a1 = new QAction(QStringLiteral("act1"), &w);
    mgr.registeAction(a1, SARibbonActionsManager::CommonlyUsedActionTag);
    const QString key1 = mgr.key(a1);

    QList< SARibbonCustomizeData > cds;
    cds.append(SARibbonCustomizeData::makeAddQuickActionCustomizeData(key1, &mgr));
    cds.append(SARibbonCustomizeData::makeChangeQuickActionOrderCustomizeData(key1, &mgr, -1));
    cds.append(SARibbonCustomizeData::makeRemoveQuickActionCustomizeData(key1, &mgr));

    const QByteArray bytes = datasToXmlBytes(cds);
    QVERIFY(!bytes.isEmpty());
    QVERIFY(bytes.contains("sa-ribbon-customize"));

    // 读回
    QBuffer readBuf;
    readBuf.setData(bytes);
    readBuf.open(QIODevice::ReadOnly);
    QXmlStreamReader xml(&readBuf);
    SARibbonActionsManager mgr2(w.ribbonBar());
    const QList< SARibbonCustomizeData > loaded = sa_customize_datas_from_xml(&xml, &mgr2);
    QCOMPARE(loaded.size(), 3);
    QCOMPARE(loaded.at(0).actionType(), SARibbonCustomizeData::AddQuickActionActionType);
    QCOMPARE(loaded.at(1).actionType(), SARibbonCustomizeData::ChangeQuickActionOrderActionType);
    QCOMPARE(loaded.at(2).actionType(), SARibbonCustomizeData::RemoveQuickActionActionType);
    QCOMPARE(loaded.at(0).keyValue, key1);
    QCOMPARE(loaded.at(1).indexValue, -1);
    // manager 指针在读取时被设置
    SARibbonCustomizeData& firstData = const_cast< SARibbonCustomizeData& >(loaded.first());
    QCOMPARE(firstData.actionManager(), &mgr2);
}

void SARibbonQuickAccessCustomizeTest::testOldXmlCompatible()
{
    // 不含 QuickAccessBar 操作的旧 XML（type 1/6/7）必须能正常读入
    const QByteArray oldXml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<sa-ribbon-customize>"
        "<customize-data type=\"1\" index=\"0\" key=\"newtab\" category=\"cat-obj\" panel=\"\" row-prop=\"0\"/>"
        "<customize-data type=\"6\" index=\"-1\" key=\"someaction\" category=\"cat-obj\" panel=\"panel-obj\" row-prop=\"0\"/>"
        "<customize-data type=\"7\" index=\"-1\" key=\"\" category=\"cat-obj\" panel=\"\" row-prop=\"0\"/>"
        "</sa-ribbon-customize>";
    SARibbonMainWindow w;
    QBuffer buf;
    buf.setData(oldXml);
    buf.open(QIODevice::ReadOnly);
    QXmlStreamReader xml(&buf);
    SARibbonActionsManager mgr(w.ribbonBar());
    const QList< SARibbonCustomizeData > loaded = sa_customize_datas_from_xml(&xml, &mgr);
    QCOMPARE(loaded.size(), 3);
    QCOMPARE(loaded.at(0).actionType(), SARibbonCustomizeData::AddCategoryActionType);
    QCOMPARE(loaded.at(1).actionType(), SARibbonCustomizeData::RemoveActionActionType);
    QCOMPARE(loaded.at(2).actionType(), SARibbonCustomizeData::ChangeCategoryOrderActionType);
    QCOMPARE(loaded.at(0).keyValue, QStringLiteral("newtab"));
}

void SARibbonQuickAccessCustomizeTest::testReverse()
{
    // 反向操作（重置）必须覆盖快速访问栏的新 ActionType
    SARibbonMainWindow w;
    SARibbonBar* bar = w.ribbonBar();
    SARibbonCategory* c = bar->addCategoryPage(QStringLiteral("main"));
    SARibbonPanel* p = c->addPanel(QStringLiteral("panel1"));
    QAction* a1 = new QAction(QStringLiteral("act1"), &w);
    p->addLargeAction(a1);
    SARibbonActionsManager mgr(bar);
    mgr.registeAction(a1, SARibbonActionsManager::CommonlyUsedActionTag);
    const QString key1 = mgr.key(a1);

    QList< SARibbonCustomizeData > cds;
    cds.append(SARibbonCustomizeData::makeAddQuickActionCustomizeData(key1, &mgr));
    // 应用
    QVERIFY(sa_customize_datas_apply(cds, bar) > 0);
    QCOMPARE(bar->quickAccessBar()->actions(), QList< QAction* >({ a1 }));
    // 反向后快速访问栏恢复为空
    QVERIFY(sa_customize_datas_reverse(cds, bar) > 0);
    QVERIFY(bar->quickAccessBar()->actions().isEmpty());
}

QTEST_MAIN(SARibbonQuickAccessCustomizeTest)

#include "SARibbonQuickAccessCustomizeTest.moc"
