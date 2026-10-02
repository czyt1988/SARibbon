#include <QtTest>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include <SARibbonCore/SARibbonCustomizeXml.h>

using SARibbon::Core::SARibbonCustomizeRecord;

/**
 * \if ENGLISH
 * @brief Contract tests of the customize record model sunk into core (plan 04 WS-C1)
 * @details Three things are load-bearing for both front ends and are locked here:
 *          the numeric values of ActionType and SARibbonActionTag (they travel inside
 *          persisted files), the exact XML attribute names/order produced by
 *          recordsToXml (2.x configuration files must stay readable), and the field
 *          mapping of the fifteen make* factories. The templates are additionally
 *          proven not to slice, so the widgets SARibbonCustomizeData and the QML
 *          customizer can keep their own extra state across a round trip.
 * \endif
 *
 * \if CHINESE
 * @brief 下沉 core 的定制记录模型契约测试（计划 04 WS-C1）
 * @details 两个前端共同依赖、必须在此钉死的有三处：ActionType 与
 *          SARibbonActionTag 的数值（它们会写进持久化文件）、recordsToXml 产出的
 *          XML 属性名与顺序（2.x 配置文件必须继续可读）、以及十五个 make* 工厂的
 *          字段映射。另外验证模板不发生切片，使 widgets 的 SARibbonCustomizeData
 *          与 QML 定制器能在往返中保留各自的额外状态。
 * \endif
 */
namespace {

// A record type with extra state, used to prove the XML templates do not slice
struct DerivedRecord : public SARibbonCustomizeRecord
{
    DerivedRecord() : marker(0)
    {
    }
    int marker;  ///< Field that only the derived type has
};

// Compare every persisted field of two records
bool sameRecord(const SARibbonCustomizeRecord& a, const SARibbonCustomizeRecord& b)
{
    return ((a.actionType() == b.actionType()) && (a.indexValue == b.indexValue) && (a.keyValue == b.keyValue)
            && (a.categoryObjNameValue == b.categoryObjNameValue) && (a.panelObjNameValue == b.panelObjNameValue)
            && (a.actionRowProportionValue == b.actionRowProportionValue));
}

// Serialize a record list to a UTF-8 byte array
QByteArray toXmlBytes(const QList< SARibbonCustomizeRecord >& records)
{
    QByteArray buffer;
    QXmlStreamWriter xml(&buffer);

    xml.setAutoFormatting(false);
    SARibbon::Core::recordsToXml(&xml, records);
    return (buffer);
}

// Parse a record list back from a UTF-8 byte array
QList< SARibbonCustomizeRecord > fromXmlBytes(const QByteArray& buffer)
{
    QXmlStreamReader xml(buffer);

    return (SARibbon::Core::recordsFromXml< SARibbonCustomizeRecord >(&xml));
}

}

class TestCustomizeRecord : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void enumValuesPinned();
    void defaultRecordState();
    void factoriesPopulateFields();
    void xmlAttributeOrderPinned();
    void xmlRoundTrip();
    void xmlSkipsBrokenEntries();
    void xmlGuardsRejectBadInput();
    void xmlKeepsDerivedRecordType();
    void canCustomizeProperty();
    void simplifyMergesRecords();
};

/**
 * \if ENGLISH
 * @brief Pin the numeric values of ActionType and SARibbonActionTag
 * @details Both enums are serialized as integers, so any reordering silently
 *          invalidates existing configuration files.
 * \endif
 *
 * \if CHINESE
 * @brief 钉死 ActionType 与 SARibbonActionTag 的数值
 * @details 两个枚举都以整数写入文件，任何重排都会静默作废已有配置。
 * \endif
 */
void TestCustomizeRecord::enumValuesPinned()
{
    QCOMPARE(int(SARibbonCustomizeRecord::UnknowActionType), 0);
    QCOMPARE(int(SARibbonCustomizeRecord::AddCategoryActionType), 1);
    QCOMPARE(int(SARibbonCustomizeRecord::AddPanelActionType), 2);
    QCOMPARE(int(SARibbonCustomizeRecord::AddActionActionType), 3);
    QCOMPARE(int(SARibbonCustomizeRecord::RemoveCategoryActionType), 4);
    QCOMPARE(int(SARibbonCustomizeRecord::RemovePanelActionType), 5);
    QCOMPARE(int(SARibbonCustomizeRecord::RemoveActionActionType), 6);
    QCOMPARE(int(SARibbonCustomizeRecord::ChangeCategoryOrderActionType), 7);
    QCOMPARE(int(SARibbonCustomizeRecord::ChangePanelOrderActionType), 8);
    QCOMPARE(int(SARibbonCustomizeRecord::ChangeActionOrderActionType), 9);
    QCOMPARE(int(SARibbonCustomizeRecord::RenameCategoryActionType), 10);
    QCOMPARE(int(SARibbonCustomizeRecord::RenamePanelActionType), 11);
    QCOMPARE(int(SARibbonCustomizeRecord::VisibleCategoryActionType), 12);
    QCOMPARE(int(SARibbonCustomizeRecord::AddQuickActionActionType), 13);
    QCOMPARE(int(SARibbonCustomizeRecord::RemoveQuickActionActionType), 14);
    QCOMPARE(int(SARibbonCustomizeRecord::ChangeQuickActionOrderActionType), 15);

    QCOMPARE(int(SARibbon::Core::UnknowActionTag), 0);
    QCOMPARE(int(SARibbon::Core::CommonlyUsedActionTag), 0x01);
    QCOMPARE(int(SARibbon::Core::NotInFunctionalAreaActionTag), 0x02);
    QCOMPARE(int(SARibbon::Core::AutoCategoryDistinguishBeginTag), 0x1000);
    QCOMPARE(int(SARibbon::Core::AutoCategoryDistinguishEndTag), 0x2000);
    QCOMPARE(int(SARibbon::Core::NotInRibbonCategoryTag), 0x2001);
    QCOMPARE(int(SARibbon::Core::UserDefineActionTag), 0x8000);
}

/**
 * \if ENGLISH
 * @brief Check the constructors leave the record in the documented state
 * \endif
 *
 * \if CHINESE
 * @brief 检查构造函数把记录置为文档所述状态
 * \endif
 */
void TestCustomizeRecord::defaultRecordState()
{
    const SARibbonCustomizeRecord empty;

    QCOMPARE(empty.actionType(), SARibbonCustomizeRecord::UnknowActionType);
    QCOMPARE(empty.indexValue, -1);
    QCOMPARE(int(empty.actionRowProportionValue), int(SARibbon::Core::SARibbonRowProportion::Large));
    QVERIFY(empty.keyValue.isEmpty());
    QVERIFY(empty.categoryObjNameValue.isEmpty());
    QVERIFY(empty.panelObjNameValue.isEmpty());
    QVERIFY(!empty.isValid());

    const SARibbonCustomizeRecord typed(SARibbonCustomizeRecord::RenamePanelActionType);

    QCOMPARE(typed.actionType(), SARibbonCustomizeRecord::RenamePanelActionType);
    QVERIFY(typed.isValid());
}

/**
 * \if ENGLISH
 * @brief Lock the field mapping of all fifteen make* factories
 * \endif
 *
 * \if CHINESE
 * @brief 钉死十五个 make* 工厂的字段映射
 * \endif
 */
void TestCustomizeRecord::factoriesPopulateFields()
{
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeAddCategory(QStringLiteral("Tab"), 2, QStringLiteral("cat"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::AddCategoryActionType);
        QCOMPARE(d.keyValue, QStringLiteral("Tab"));
        QCOMPARE(d.indexValue, 2);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QVERIFY(d.panelObjNameValue.isEmpty());
    }
    {
        const SARibbonCustomizeRecord d =
            SARibbonCustomizeRecord::makeAddPanel(QStringLiteral("Panel"), 1, QStringLiteral("cat"), QStringLiteral("pan"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::AddPanelActionType);
        QCOMPARE(d.keyValue, QStringLiteral("Panel"));
        QCOMPARE(d.indexValue, 1);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.panelObjNameValue, QStringLiteral("pan"));
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeAddAction(QStringLiteral("act"),
                                                                                 SARibbon::Core::SARibbonRowProportion::Small,
                                                                                 QStringLiteral("cat"),
                                                                                 QStringLiteral("pan"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::AddActionActionType);
        QCOMPARE(d.keyValue, QStringLiteral("act"));
        QCOMPARE(int(d.actionRowProportionValue), int(SARibbon::Core::SARibbonRowProportion::Small));
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.panelObjNameValue, QStringLiteral("pan"));
        // 工厂不设置 indexValue，保持构造函数的 -1
        QCOMPARE(d.indexValue, -1);
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeRemoveCategory(QStringLiteral("cat"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::RemoveCategoryActionType);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeRemovePanel(QStringLiteral("cat"), QStringLiteral("pan"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::RemovePanelActionType);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.panelObjNameValue, QStringLiteral("pan"));
    }
    {
        const SARibbonCustomizeRecord d =
            SARibbonCustomizeRecord::makeRemoveAction(QStringLiteral("cat"), QStringLiteral("pan"), QStringLiteral("act"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::RemoveActionActionType);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.panelObjNameValue, QStringLiteral("pan"));
        QCOMPARE(d.keyValue, QStringLiteral("act"));
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeChangeCategoryOrder(QStringLiteral("cat"), -1);

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::ChangeCategoryOrderActionType);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.indexValue, -1);
    }
    {
        const SARibbonCustomizeRecord d =
            SARibbonCustomizeRecord::makeChangePanelOrder(QStringLiteral("cat"), QStringLiteral("pan"), 1);

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::ChangePanelOrderActionType);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.panelObjNameValue, QStringLiteral("pan"));
        QCOMPARE(d.indexValue, 1);
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeChangeActionOrder(
            QStringLiteral("cat"), QStringLiteral("pan"), QStringLiteral("act"), -1);

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::ChangeActionOrderActionType);
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.panelObjNameValue, QStringLiteral("pan"));
        QCOMPARE(d.keyValue, QStringLiteral("act"));
        QCOMPARE(d.indexValue, -1);
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeRenameCategory(QStringLiteral("New"), QStringLiteral("cat"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::RenameCategoryActionType);
        QCOMPARE(d.keyValue, QStringLiteral("New"));
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
    }
    {
        const SARibbonCustomizeRecord d =
            SARibbonCustomizeRecord::makeRenamePanel(QStringLiteral("New"), QStringLiteral("cat"), QStringLiteral("pan"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::RenamePanelActionType);
        QCOMPARE(d.keyValue, QStringLiteral("New"));
        QCOMPARE(d.categoryObjNameValue, QStringLiteral("cat"));
        QCOMPARE(d.panelObjNameValue, QStringLiteral("pan"));
    }
    {
        const SARibbonCustomizeRecord show = SARibbonCustomizeRecord::makeVisibleCategory(QStringLiteral("cat"), true);
        const SARibbonCustomizeRecord hide = SARibbonCustomizeRecord::makeVisibleCategory(QStringLiteral("cat"), false);

        QCOMPARE(show.actionType(), SARibbonCustomizeRecord::VisibleCategoryActionType);
        QCOMPARE(show.indexValue, 1);
        QCOMPARE(hide.indexValue, 0);
    }
    {
        const SARibbonCustomizeRecord append = SARibbonCustomizeRecord::makeAddQuickAction(QStringLiteral("act"));
        const SARibbonCustomizeRecord at     = SARibbonCustomizeRecord::makeAddQuickAction(QStringLiteral("act"), 3);

        QCOMPARE(append.actionType(), SARibbonCustomizeRecord::AddQuickActionActionType);
        QCOMPARE(append.indexValue, -1);
        QCOMPARE(at.indexValue, 3);
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeRemoveQuickAction(QStringLiteral("act"));

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::RemoveQuickActionActionType);
        QCOMPARE(d.keyValue, QStringLiteral("act"));
        QCOMPARE(d.indexValue, -1);
    }
    {
        const SARibbonCustomizeRecord d = SARibbonCustomizeRecord::makeChangeQuickActionOrder(QStringLiteral("act"), 1);

        QCOMPARE(d.actionType(), SARibbonCustomizeRecord::ChangeQuickActionOrderActionType);
        QCOMPARE(d.keyValue, QStringLiteral("act"));
        QCOMPARE(d.indexValue, 1);
    }
}

/**
 * \if ENGLISH
 * @brief Lock the XML element name, attribute names, attribute order and encoding
 * @details Compared as a literal prefix rather than a whole document so the
 *          assertion does not depend on how QXmlStreamWriter closes empty
 *          elements; the attribute sequence is what 2.x readers key on.
 * \endif
 *
 * \if CHINESE
 * @brief 钉死 XML 元素名、属性名、属性顺序与编码
 * @details 以字面前缀而非整篇文档比较，使断言不依赖 QXmlStreamWriter 关闭空元素的
 *          写法；2.x 读取端依赖的正是属性序列。
 * \endif
 */
void TestCustomizeRecord::xmlAttributeOrderPinned()
{
    QList< SARibbonCustomizeRecord > records;

    records << SARibbonCustomizeRecord::makeAddCategory(QStringLiteral("Tab"), 0, QStringLiteral("cat1"));
    const QByteArray buffer = toXmlBytes(records);
    const QString text      = QString::fromUtf8(buffer);

    QVERIFY2(text.startsWith(QLatin1String("<sa-ribbon-customize>")), qPrintable(text));
    QVERIFY2(text.contains(QLatin1String("<customize-data type=\"1\" index=\"0\" key=\"Tab\" category=\"cat1\" panel=\"\" row-prop=\"1\"")),
             qPrintable(text));
    QVERIFY2(text.endsWith(QLatin1String("</sa-ribbon-customize>")), qPrintable(text));

    // 非 ASCII 内容以 UTF-8 落盘（QByteArray 版 writer 不接受 setCodec，默认即 UTF-8）
    QList< SARibbonCustomizeRecord > cnRecords;
    // "新建页" / "面板"：源码不带 /utf-8 编译选项，用转义写码点避免受本地代码页影响
    cnRecords << SARibbonCustomizeRecord::makeAddCategory(QStringLiteral("\u65B0\u5EFA\u9875"), 0, QStringLiteral("cat1"))
              << SARibbonCustomizeRecord::makeAddPanel(QStringLiteral("\u9762\u677F"), 1, QStringLiteral("cat1"), QStringLiteral("pan1"));
    const QByteArray cnBuffer = toXmlBytes(cnRecords);

    QVERIFY(cnBuffer.contains(QByteArray("\xE6\x96\xB0\xE5\xBB\xBA\xE9\xA1\xB5")));
    QVERIFY(cnBuffer.contains(QByteArray("\xE9\x9D\xA2\xE6\x9D\xBF")));
    const QList< SARibbonCustomizeRecord > cnBack = fromXmlBytes(cnBuffer);

    QCOMPARE(cnBack.size(), 2);
    QCOMPARE(cnBack[ 0 ].keyValue, QStringLiteral("\u65B0\u5EFA\u9875"));
    QCOMPARE(cnBack[ 1 ].keyValue, QStringLiteral("\u9762\u677F"));
}

/**
 * \if ENGLISH
 * @brief Round-trip every action type through XML without loss
 * \endif
 *
 * \if CHINESE
 * @brief 所有操作类型经 XML 往返后无损
 * \endif
 */
void TestCustomizeRecord::xmlRoundTrip()
{
    QList< SARibbonCustomizeRecord > records;

    records << SARibbonCustomizeRecord::makeAddCategory(QStringLiteral("Tab"), 0, QStringLiteral("cat1"))
            << SARibbonCustomizeRecord::makeAddPanel(QStringLiteral("Panel"), 1, QStringLiteral("cat1"), QStringLiteral("pan1"))
            << SARibbonCustomizeRecord::makeAddAction(
                   QStringLiteral("act1"), SARibbon::Core::SARibbonRowProportion::Medium, QStringLiteral("cat1"), QStringLiteral("pan1"))
            << SARibbonCustomizeRecord::makeRemoveAction(QStringLiteral("cat1"), QStringLiteral("pan1"), QStringLiteral("act2"))
            << SARibbonCustomizeRecord::makeRemovePanel(QStringLiteral("cat1"), QStringLiteral("pan2"))
            << SARibbonCustomizeRecord::makeRemoveCategory(QStringLiteral("cat2"))
            << SARibbonCustomizeRecord::makeChangeCategoryOrder(QStringLiteral("cat1"), -1)
            << SARibbonCustomizeRecord::makeChangePanelOrder(QStringLiteral("cat1"), QStringLiteral("pan1"), 1)
            << SARibbonCustomizeRecord::makeChangeActionOrder(QStringLiteral("cat1"), QStringLiteral("pan1"), QStringLiteral("act1"), -1)
            << SARibbonCustomizeRecord::makeRenameCategory(QStringLiteral("Renamed"), QStringLiteral("cat1"))
            << SARibbonCustomizeRecord::makeRenamePanel(QStringLiteral("Renamed"), QStringLiteral("cat1"), QStringLiteral("pan1"))
            << SARibbonCustomizeRecord::makeVisibleCategory(QStringLiteral("cat1"), false)
            << SARibbonCustomizeRecord::makeAddQuickAction(QStringLiteral("act1"), 2)
            << SARibbonCustomizeRecord::makeRemoveQuickAction(QStringLiteral("act3"))
            << SARibbonCustomizeRecord::makeChangeQuickActionOrder(QStringLiteral("act1"), -1);
    QCOMPARE(records.size(), 15);

    const QList< SARibbonCustomizeRecord > back = fromXmlBytes(toXmlBytes(records));

    QCOMPARE(back.size(), records.size());
    for (int i = 0; i < records.size(); ++i) {
        QVERIFY2(sameRecord(records[ i ], back[ i ]), qPrintable(QStringLiteral("record %1 differs").arg(i)));
    }
}

/**
 * \if ENGLISH
 * @brief Malformed customize-data entries are skipped, the rest still parse
 * \endif
 *
 * \if CHINESE
 * @brief 异常的 customize-data 条目被跳过，其余仍可解析
 * \endif
 */
void TestCustomizeRecord::xmlSkipsBrokenEntries()
{
    const QByteArray buffer = QByteArrayLiteral("<sa-ribbon-customize>"
                                                "<customize-data index=\"0\" key=\"no-type\"/>"
                                                "<customize-data type=\"12\" index=\"1\" key=\"\" category=\"cat1\" panel=\"\" row-prop=\"1\"/>"
                                                "<customize-data type=\"not-a-number\"/>"
                                                "</sa-ribbon-customize>");
    const QList< SARibbonCustomizeRecord > back = fromXmlBytes(buffer);

    QCOMPARE(back.size(), 1);
    QCOMPARE(back[ 0 ].actionType(), SARibbonCustomizeRecord::VisibleCategoryActionType);
    QCOMPARE(back[ 0 ].indexValue, 1);
    QCOMPARE(back[ 0 ].categoryObjNameValue, QStringLiteral("cat1"));

    // 缺少可选属性时保持构造函数的默认值
    const QByteArray sparse = QByteArrayLiteral("<sa-ribbon-customize><customize-data type=\"14\"/></sa-ribbon-customize>");
    const QList< SARibbonCustomizeRecord > sparseBack = fromXmlBytes(sparse);

    QCOMPARE(sparseBack.size(), 1);
    QCOMPARE(sparseBack[ 0 ].actionType(), SARibbonCustomizeRecord::RemoveQuickActionActionType);
    QCOMPARE(sparseBack[ 0 ].indexValue, -1);
    QCOMPARE(int(sparseBack[ 0 ].actionRowProportionValue), int(SARibbon::Core::SARibbonRowProportion::Large));
    QVERIFY(sparseBack[ 0 ].keyValue.isEmpty());
}

/**
 * \if ENGLISH
 * @brief Null writer, null reader and empty lists are rejected, not crashed on
 * \endif
 *
 * \if CHINESE
 * @brief 空 writer、空 reader 与空列表被拒绝而非崩溃
 * \endif
 */
void TestCustomizeRecord::xmlGuardsRejectBadInput()
{
    QList< SARibbonCustomizeRecord > records;

    records << SARibbonCustomizeRecord::makeRemoveCategory(QStringLiteral("cat1"));
    QVERIFY(!SARibbon::Core::recordsToXml< SARibbonCustomizeRecord >(nullptr, records));

    QByteArray buffer;
    QXmlStreamWriter xml(&buffer);

    QVERIFY(!SARibbon::Core::recordsToXml(&xml, QList< SARibbonCustomizeRecord >()));
    QVERIFY(buffer.isEmpty());

    const QList< SARibbonCustomizeRecord > none = SARibbon::Core::recordsFromXml< SARibbonCustomizeRecord >(nullptr);

    QVERIFY(none.isEmpty());

    // 不含根元素的文档解析为空列表
    QByteArray unrelated = QByteArrayLiteral("<other-root><item/></other-root>");
    QXmlStreamReader reader(unrelated);
    const QList< SARibbonCustomizeRecord > empty = SARibbon::Core::recordsFromXml< SARibbonCustomizeRecord >(&reader);

    QVERIFY(empty.isEmpty());
}

/**
 * \if ENGLISH
 * @brief The XML templates keep the caller's record type (no slicing)
 * @details The widgets SARibbonCustomizeData carries a manager pointer and the QML
 *          customizer will carry its registry binding; neither may be lost when a
 *          derived record list goes through a round trip.
 * \endif
 *
 * \if CHINESE
 * @brief XML 模板保持调用方的记录类型（不切片）
 * @details widgets 的 SARibbonCustomizeData 带 manager 指针，QML 定制器将带注册表绑定，
 *          派生记录列表往返后都不能丢失这些额外状态。
 * \endif
 */
void TestCustomizeRecord::xmlKeepsDerivedRecordType()
{
    QVERIFY(sizeof(DerivedRecord) > sizeof(SARibbonCustomizeRecord));

    QList< DerivedRecord > records;
    DerivedRecord a;

    a.setActionType(SARibbonCustomizeRecord::AddCategoryActionType);
    a.keyValue             = QStringLiteral("Tab");
    a.categoryObjNameValue = QStringLiteral("cat1");
    a.indexValue           = 0;
    a.marker               = 11;
    DerivedRecord b;
    b.setActionType(SARibbonCustomizeRecord::RemoveCategoryActionType);
    b.categoryObjNameValue = QStringLiteral("cat1");
    b.marker               = 22;
    records << a << b;

    QByteArray buffer;
    QXmlStreamWriter xml(&buffer);

    xml.setAutoFormatting(false);
    QVERIFY(SARibbon::Core::recordsToXml(&xml, records));

    QXmlStreamReader reader(buffer);
    QList< DerivedRecord > back = SARibbon::Core::recordsFromXml< DerivedRecord >(&reader);

    QCOMPARE(back.size(), 2);
    // 返回的确实是 DerivedRecord：额外字段可用（若被切片则此处无法编译/赋值）
    for (DerivedRecord& d : back) {
        d.marker = 7;
    }
    QCOMPARE(back[ 0 ].marker, 7);
    QCOMPARE(back[ 0 ].actionType(), SARibbonCustomizeRecord::AddCategoryActionType);
    QCOMPARE(back[ 0 ].keyValue, QStringLiteral("Tab"));
    QCOMPARE(back[ 1 ].actionType(), SARibbonCustomizeRecord::RemoveCategoryActionType);

    // simplify 同样是模板，派生类型可用
    const QList< DerivedRecord > simplified = SARibbonCustomizeRecord::simplify(back);

    QVERIFY(simplified.isEmpty());
}

/**
 * \if ENGLISH
 * @brief The customizable marking helpers work on any QObject
 * \endif
 *
 * \if CHINESE
 * @brief 可定制标记辅助函数对任意 QObject 生效
 * \endif
 */
void TestCustomizeRecord::canCustomizeProperty()
{
    QVERIFY(!SARibbon::Core::isCanCustomize(nullptr));
    SARibbon::Core::setCanCustomize(nullptr);  // 不崩溃

    QObject obj;

    QVERIFY(!obj.property(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE).isValid());
    QVERIFY(!SARibbon::Core::isCanCustomize(&obj));
    SARibbon::Core::setCanCustomize(&obj);
    QVERIFY(obj.property(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE).toBool());
    QVERIFY(SARibbon::Core::isCanCustomize(&obj));
    SARibbon::Core::setCanCustomize(&obj, false);
    QVERIFY(!SARibbon::Core::isCanCustomize(&obj));
}

/**
 * \if ENGLISH
 * @brief simplify() cancels, collapses and merges records as documented
 * \endif
 *
 * \if CHINESE
 * @brief simplify() 按文档所述抵消、归并与合并记录
 * \endif
 */
void TestCustomizeRecord::simplifyMergesRecords()
{
    // 单条与空列表原样返回
    QVERIFY(SARibbonCustomizeRecord::simplify(QList< SARibbonCustomizeRecord >()).isEmpty());
    {
        QList< SARibbonCustomizeRecord > one;

        one << SARibbonCustomizeRecord::makeAddCategory(QStringLiteral("Tab"), 0, QStringLiteral("cat"));
        QCOMPARE(SARibbonCustomizeRecord::simplify(one).size(), 1);
    }
    // 连续的添加 + 删除同名 category 相互抵消
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeAddCategory(QStringLiteral("Tab"), 0, QStringLiteral("cat"))
            << SARibbonCustomizeRecord::makeRemoveCategory(QStringLiteral("cat"));
        QVERIFY(SARibbonCustomizeRecord::simplify(csd).isEmpty());
        // 不同名则不抵消
        csd << SARibbonCustomizeRecord::makeRemoveCategory(QStringLiteral("other"));
        QCOMPARE(SARibbonCustomizeRecord::simplify(csd).size(), 1);
    }
    // 连续的添加 + 删除同名 panel 相互抵消
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeAddPanel(QStringLiteral("P"), 0, QStringLiteral("cat"), QStringLiteral("pan"))
            << SARibbonCustomizeRecord::makeRemovePanel(QStringLiteral("cat"), QStringLiteral("pan"));
        QVERIFY(SARibbonCustomizeRecord::simplify(csd).isEmpty());
    }
    // 连续的添加 + 删除同名 action 相互抵消
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeAddAction(
                   QStringLiteral("act"), SARibbon::Core::SARibbonRowProportion::Large, QStringLiteral("cat"), QStringLiteral("pan"))
            << SARibbonCustomizeRecord::makeRemoveAction(QStringLiteral("cat"), QStringLiteral("pan"), QStringLiteral("act"));
        QVERIFY(SARibbonCustomizeRecord::simplify(csd).isEmpty());
    }
    // 连续显示/隐藏同一 category 只保留最后一步
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeVisibleCategory(QStringLiteral("cat"), true)
            << SARibbonCustomizeRecord::makeVisibleCategory(QStringLiteral("cat"), false);
        const QList< SARibbonCustomizeRecord > res = SARibbonCustomizeRecord::simplify(csd);

        QCOMPARE(res.size(), 1);
        QCOMPARE(res[ 0 ].indexValue, 0);
    }
    // 同一 category 多次改名只保留最后一次
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeRenameCategory(QStringLiteral("A"), QStringLiteral("cat"))
            << SARibbonCustomizeRecord::makeRenameCategory(QStringLiteral("B"), QStringLiteral("cat"));
        const QList< SARibbonCustomizeRecord > res = SARibbonCustomizeRecord::simplify(csd);

        QCOMPARE(res.size(), 1);
        QCOMPARE(res[ 0 ].keyValue, QStringLiteral("B"));
    }
    // 同一 panel 多次改名只保留最后一次，不同 panel 互不影响
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeRenamePanel(QStringLiteral("A"), QStringLiteral("cat"), QStringLiteral("pan1"))
            << SARibbonCustomizeRecord::makeRenamePanel(QStringLiteral("B"), QStringLiteral("cat"), QStringLiteral("pan2"))
            << SARibbonCustomizeRecord::makeRenamePanel(QStringLiteral("C"), QStringLiteral("cat"), QStringLiteral("pan1"));
        const QList< SARibbonCustomizeRecord > res = SARibbonCustomizeRecord::simplify(csd);

        QCOMPARE(res.size(), 2);
        QCOMPARE(res[ 0 ].keyValue, QStringLiteral("B"));
        QCOMPARE(res[ 1 ].keyValue, QStringLiteral("C"));
    }
    // 连续同向移动合并位移，位移为 0 时整条删除
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeChangeCategoryOrder(QStringLiteral("cat"), -1)
            << SARibbonCustomizeRecord::makeChangeCategoryOrder(QStringLiteral("cat"), -1);
        const QList< SARibbonCustomizeRecord > merged = SARibbonCustomizeRecord::simplify(csd);

        QCOMPARE(merged.size(), 1);
        QCOMPARE(merged[ 0 ].indexValue, -2);

        QList< SARibbonCustomizeRecord > cancel;

        cancel << SARibbonCustomizeRecord::makeChangeCategoryOrder(QStringLiteral("cat"), -1)
            << SARibbonCustomizeRecord::makeChangeCategoryOrder(QStringLiteral("cat"), 1);
        QVERIFY(SARibbonCustomizeRecord::simplify(cancel).isEmpty());

        QList< SARibbonCustomizeRecord > panelOrder;

        panelOrder << SARibbonCustomizeRecord::makeChangePanelOrder(QStringLiteral("cat"), QStringLiteral("pan"), 1)
            << SARibbonCustomizeRecord::makeChangePanelOrder(QStringLiteral("cat"), QStringLiteral("pan"), 1);
        const QList< SARibbonCustomizeRecord > panelMerged = SARibbonCustomizeRecord::simplify(panelOrder);

        QCOMPARE(panelMerged.size(), 1);
        QCOMPARE(panelMerged[ 0 ].indexValue, 2);

        QList< SARibbonCustomizeRecord > actionOrder;

        actionOrder << SARibbonCustomizeRecord::makeChangeActionOrder(
                           QStringLiteral("cat"), QStringLiteral("pan"), QStringLiteral("act"), 1)
            << SARibbonCustomizeRecord::makeChangeActionOrder(QStringLiteral("cat"), QStringLiteral("pan"), QStringLiteral("act"), -1);
        QVERIFY(SARibbonCustomizeRecord::simplify(actionOrder).isEmpty());
    }
    // 无关记录不受影响
    {
        QList< SARibbonCustomizeRecord > csd;

        csd << SARibbonCustomizeRecord::makeAddQuickAction(QStringLiteral("act"), 0)
            << SARibbonCustomizeRecord::makeRemoveQuickAction(QStringLiteral("other"));
        const QList< SARibbonCustomizeRecord > res = SARibbonCustomizeRecord::simplify(csd);

        QCOMPARE(res.size(), 2);
        QVERIFY(sameRecord(res[ 0 ], csd[ 0 ]));
        QVERIFY(sameRecord(res[ 1 ], csd[ 1 ]));
    }
}

QTEST_MAIN(TestCustomizeRecord)
#include "tst_customizeRecord.moc"
