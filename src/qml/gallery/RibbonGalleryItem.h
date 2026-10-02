#ifndef RIBBONGALLERYITEM_H
#define RIBBONGALLERYITEM_H
#include "SARibbonQmlGlobal.h"
#include <QObject>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Declarative gallery entry: one grid cell of a gallery group
 * @details Pure data object mirroring the widgets SARibbonGalleryItem's
 *          action face (text + icon + enabled + selectable); activation is
 *          mediated by the gallery host's triggered signal.
 *          selectable mirrors the widgets Qt::ItemIsSelectable flag, i.e. it
 *          gates whether a cell may BECOME CURRENT — not whether it may be
 *          activated. That split is the widgets behaviour (QAbstractItemView
 *          emits clicked for a non-selectable index while the selection stays
 *          put), so a non-selectable cell still fires triggered on click.
 * \endif
 *
 * \if CHINESE
 * @brief 声明式画廊条目：画廊组的一个网格单元
 * @details 纯数据对象，对应 widgets SARibbonGalleryItem 的 action 面
 *          （文本 + 图标 + 可用性 + 可选择性）；激活经画廊宿主的 triggered
 *          信号中转。
 *          selectable 对应 widgets 的 Qt::ItemIsSelectable 标志，它约束的是
 *          "能否成为当前项"，而不是"能否被激活"。这一拆分正是 widgets 的行为
 *          （QAbstractItemView 对不可选择的索引照样发 clicked，选择位置不动），
 *          因此不可选择的单元被点击时依然发 triggered。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonGalleryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_PROPERTY(bool selectable READ isSelectable WRITE setSelectable NOTIFY selectableChanged)
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

    /// Whether the cell may become the gallery's current item (Qt::ItemIsSelectable parity)
    bool isSelectable() const;
    void setSelectable(bool on);

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void enabledChanged();
    void toolTipChanged();
    void selectableChanged();

private:
    QString mText;
    QString mIconSource;
    bool mEnabled = true;
    QString mToolTip;
    bool mSelectable = true;
};

}

#endif  // RIBBONGALLERYITEM_H
