#ifndef RIBBONGALLERYVIEWPORTWIDGET_H
#define RIBBONGALLERYVIEWPORTWIDGET_H
#include "SARibbonQmlGlobal.h"
#include <QQuickItem>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Custom content section of the gallery expanded viewport
 * @details The QML counterpart of handing a QWidget to the widgets
 *          SARibbonGalleryViewport::addWidget: arbitrary QQuickItem content
 *          declared as this section's children renders below the gallery
 *          groups inside the expanded viewport popup. The gallery host owns
 *          the section (QObject parent); the leaf visually attaches it into
 *          the popup's section slot through the host invokable, and the
 *          slot pushes the section width as rendering context (contract §6).
 *          The section's implicitHeight follows its childrenRect from C++
 *          (every direct child's geometry change re-derives it), which is
 *          what the leaf's slot height reads — a plain property write has no
 *          place in the QML binding graph, so no binding loop can form the
 *          way a QML-side childrenRect binding would.
 *          The section itself is pure content: no leaf, no panel-layout
 *          participation, no command semantics (the customizer ignores it
 *          exactly like the widgets customizer ignores addWidget content).
 * \endif
 *
 * \if CHINESE
 * @brief 画廊展开视口的自定义内容节
 * @details 对应 widgets 侧把 QWidget 交给 SARibbonGalleryViewport::addWidget：
 *          任意 QQuickItem 内容以本节的子项形式声明，渲染于展开视口弹窗中
 *          画廊组的下方。画廊宿主持有该节（QObject 父子关系）；叶子通过宿主
 *          的可调用方法把节视觉挂载进弹窗的节槽位，槽位宽度作为渲染上下文
 *          压入（契约 §6）。节的 implicitHeight 由 C++ 侧跟随 childrenRect
 *          （任一直接子项几何变化即重推导），叶子的节槽高度读取它——普通
 *          属性写入不在 QML 绑定图中，不会像 QML 侧 childrenRect 绑定那样
 *          构成绑定循环。本节自身是纯内容：无叶子、不参与面板布局、无
 *          命令语义（定制器不纳管它，正如 widgets 定制器不纳管 addWidget 的
 *          内容）。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonGalleryViewportWidget : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString sectionTitle READ sectionTitle WRITE setSectionTitle NOTIFY sectionTitleChanged)
public:
    explicit RibbonGalleryViewportWidget(QQuickItem* parent = nullptr);

    // Section caption rendered above the content (widgets addWidget title parity)
    QString sectionTitle() const;
    void setSectionTitle(const QString& t);

protected:
    // wire / unwire the geometry signals of joining / leaving children so
    // implicitHeight keeps following childrenRect
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& data) override;

Q_SIGNALS:
    void sectionTitleChanged();

private:
    // re-derive implicitHeight off the direct children's bounding rect
    void updateImplicitHeight();
    void watchChild(QQuickItem* child, bool on);

    QString mSectionTitle;
};

}

#endif  // RIBBONGALLERYVIEWPORTWIDGET_H
