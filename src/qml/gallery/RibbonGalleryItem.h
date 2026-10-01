#ifndef RIBBONGALLERYITEM_H
#define RIBBONGALLERYITEM_H
#include "SARibbonQmlGlobal.h"
#include <QObject>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Declarative gallery entry: one grid cell of a gallery group
 * @details Pure data object mirroring the widgets SARibbonGalleryItem's
 *          action face (text + icon + enabled); activation is mediated by
 *          the gallery host's triggered signal.
 * \endif
 *
 * \if CHINESE
 * @brief 声明式画廊条目：画廊组的一个网格单元
 * @details 纯数据对象，对应 widgets SARibbonGalleryItem 的 action 面
 *          （文本 + 图标 + 可用性）；激活经画廊宿主的 triggered 信号中转。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonGalleryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
public:
    explicit RibbonGalleryItem(QObject* parent = nullptr);

    QString text() const;
    void setText(const QString& t);

    QString iconSource() const;
    void setIconSource(const QString& s);

    bool isEnabled() const;
    void setEnabled(bool on);

    QString toolTip() const;
    void setToolTip(const QString& t);

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void enabledChanged();
    void toolTipChanged();

private:
    QString mText;
    QString mIconSource;
    bool mEnabled = true;
    QString mToolTip;
};

}

#endif  // RIBBONGALLERYITEM_H
