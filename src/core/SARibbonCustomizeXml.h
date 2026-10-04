#ifndef SARIBBONCUSTOMIZEXML_H
#define SARIBBONCUSTOMIZEXML_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include <QList>
#include <QString>
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief XML persistence of customize records (plan 04 WS-C1)
 * @details Lifted from the sa_customize_datas_to_xml/sa_customize_datas_from_xml
 *          free functions of SARibbonCustomizeWidget.cpp so the QML customizer
 *          writes and reads exactly the same file format: element names,
 *          attribute names and attribute order are unchanged, which keeps every
 *          2.x configuration file loadable. Both functions are templates for the
 *          same reason SARibbonCustomizeRecord::simplify is one — the caller's
 *          record type (the plain core record or the widgets SARibbonCustomizeData)
 *          must survive the round trip, so no slicing happens here.
 * @note The reader cannot restore anything the record type keeps outside the file
 *       format. The widgets SARibbonCustomizeData attaches its SARibbonActionsManager
 *       after reading; the QML customizer binds its own registry the same way.
 * @note QXmlStreamWriter cannot pick an encoding when built on a QString, and
 *       customize data routinely carries non-ASCII titles, so build the writer on
 *       a QByteArray or a QIODevice, exactly as the 2.x API documented.
 * \endif
 *
 * \if CHINESE
 * @brief 定制记录的 XML 持久化（计划 04 WS-C1）
 * @details 自 SARibbonCustomizeWidget.cpp 的自由函数
 *          sa_customize_datas_to_xml/sa_customize_datas_from_xml 下沉，使 QML
 *          定制器读写完全相同的文件格式：元素名、属性名与属性顺序一律不变，
 *          2.x 产出的配置文件继续可用。两个函数与
 *          SARibbonCustomizeRecord::simplify 同样是模板——调用方的记录类型
 *          （core 纯记录或 widgets 的 SARibbonCustomizeData）必须在往返中保持，
 *          因此这里不发生切片。
 * @note 读取端无法恢复记录类型在文件格式之外持有的东西。widgets 的
 *       SARibbonCustomizeData 在读取之后再挂上 SARibbonActionsManager，
 *       QML 定制器以同样方式绑定自己的注册表。
 * @note QXmlStreamWriter 以 QString 为 io 时不支持编码，而定制数据常含非 ASCII
 *       标题，因此请像 2.x 文档要求的那样，用 QByteArray 或 QIODevice 构造 writer。
 * \endif
 */

// Write records to a QXmlStreamWriter (see the implementations below the declarations)
template< typename RecordT >
bool recordsToXml(QXmlStreamWriter* xml, const QList< RecordT >& records);

// Read records from a QXmlStreamReader (see the implementations below the declarations)
template< typename RecordT >
QList< RecordT > recordsFromXml(QXmlStreamReader* xml);

template< typename RecordT >
bool recordsToXml(QXmlStreamWriter* xml, const QList< RecordT >& records)
{
    if (nullptr == xml) {
        return (false);
    }
    if (records.size() <= 0) {
        return (false);
    }

    xml->writeStartElement(QStringLiteral("sa-ribbon-customize"));
    for (const RecordT& d : records) {
        xml->writeStartElement(QStringLiteral("customize-data"));
        xml->writeAttribute(QStringLiteral("type"), QString::number(d.actionType()));
        xml->writeAttribute(QStringLiteral("index"), QString::number(d.indexValue));
        xml->writeAttribute(QStringLiteral("key"), d.keyValue);
        xml->writeAttribute(QStringLiteral("category"), d.categoryObjNameValue);
        xml->writeAttribute(QStringLiteral("panel"), d.panelObjNameValue);
        xml->writeAttribute(QStringLiteral("row-prop"), QString::number(d.actionRowProportionValue));

        xml->writeEndElement();
    }
    xml->writeEndElement();
    if (xml->hasError()) {
        qWarning() << "write has error";
    }
    return (true);
}

template< typename RecordT >
QList< RecordT > recordsFromXml(QXmlStreamReader* xml)
{
    QList< RecordT > res;

    if (nullptr == xml) {
        return (res);
    }
    // 先找到"sa-ribbon-customize"
    while (!xml->atEnd()) {
        if (xml->isStartElement() && (xml->name().toString() == QLatin1String("sa-ribbon-customize"))) {
            break;
        }
        xml->readNext();
    }

    // 开始遍历"customize-data"
    while (!xml->atEnd()) {
        if (xml->isStartElement() && (xml->name().toString() == QLatin1String("customize-data"))) {
            // 首先读取属性type
            RecordT d;
            const QXmlStreamAttributes attrs = xml->attributes();
            if (!attrs.hasAttribute(QLatin1String("type"))) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            bool isOk = false;
            int v     = attrs.value(QLatin1String("type")).toInt(&isOk);
            if (!isOk) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            d.setActionType(static_cast< typename RecordT::ActionType >(v));
            if (attrs.hasAttribute(QLatin1String("index"))) {
                v = attrs.value(QLatin1String("index")).toInt(&isOk);
                if (isOk) {
                    d.indexValue = v;
                }
            }
            if (attrs.hasAttribute(QLatin1String("key"))) {
                d.keyValue = attrs.value(QLatin1String("key")).toString();
            }
            if (attrs.hasAttribute(QLatin1String("category"))) {
                d.categoryObjNameValue = attrs.value(QLatin1String("category")).toString();
            }
            if (attrs.hasAttribute(QLatin1String("panel"))) {
                d.panelObjNameValue = attrs.value(QLatin1String("panel")).toString();
            }
            if (attrs.hasAttribute(QLatin1String("row-prop"))) {
                v = attrs.value(QLatin1String("row-prop")).toInt(&isOk);
                if (isOk) {
                    d.actionRowProportionValue = static_cast< SARibbonRowProportion >(v);
                }
            }
            res.append(d);
        }
        xml->readNext();
    }
    if (xml->hasError()) {
        qWarning() << xml->errorString();
    }
    return (res);
}

}
}

#endif  // SARIBBONCUSTOMIZEXML_H
