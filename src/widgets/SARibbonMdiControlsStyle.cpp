#include "SARibbonMdiControlsStyle.h"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

/**
 * \if ENGLISH
 * @brief Collect the visible MDI sub controls in logical (LTR) order: minimize, restore, close
 * @details This matches the order used by the Windows style's subControlRect for CC_MdiControls
 * \endif
 *
 * \if CHINESE
 * @brief 收集可见的MDI子控件，按逻辑顺序（LTR）排列：最小化、还原、关闭
 * @details 与Windows样式CC_MdiControls的subControlRect顺序一致
 * \endif
 */
static QList< QStyle::SubControl > saVisibleMdiSubControls(QStyle::SubControls subControls)
{
    QList< QStyle::SubControl > res;
    if (subControls.testFlag(QStyle::SC_MdiMinButton)) {
        res.append(QStyle::SC_MdiMinButton);
    }
    if (subControls.testFlag(QStyle::SC_MdiNormalButton)) {
        res.append(QStyle::SC_MdiNormalButton);
    }
    if (subControls.testFlag(QStyle::SC_MdiCloseButton)) {
        res.append(QStyle::SC_MdiCloseButton);
    }
    return res;
}

/**
 * \if ENGLISH
 * @brief Draw one MDI button glyph (minimize line, restore rectangles or close cross)
 * \endif
 *
 * \if CHINESE
 * @brief 绘制单个MDI按钮图元（最小化横线、还原双框或关闭交叉线）
 * \endif
 */
static void saDrawMdiButtonGlyph(QPainter* painter, QStyle::SubControl subControl, const QRect& buttonRect,
                                 const QColor& glyphColor)
{
    // 图元大小按按钮矩形比例计算，保证任意DPI下协调且清晰
    const qreal glyphSize = qMin(buttonRect.width(), buttonRect.height()) * 0.36;
    const QPointF center  = QRectF(buttonRect).center();
    QPen pen(glyphColor);
    pen.setWidthF(qMax(qreal(1.0), glyphSize / 8.0));
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    switch (subControl) {
    case QStyle::SC_MdiMinButton: {
        // 最小化：位于中部偏下的横线
        const qreal y = center.y() + glyphSize * 0.25;
        painter->drawLine(QPointF(center.x() - glyphSize / 2, y), QPointF(center.x() + glyphSize / 2, y));
    } break;
    case QStyle::SC_MdiNormalButton: {
        // 还原：两个错位的方框，只绘制背框露出的部分，避免线条交叉
        const qreal s  = glyphSize;
        const qreal x0 = center.x() - s / 2;
        const qreal y0 = center.y() - s / 2;
        QPainterPath path;
        path.moveTo(x0 + s * 0.25, y0 + s * 0.25);
        path.lineTo(x0 + s * 0.25, y0);
        path.lineTo(x0 + s, y0);
        path.lineTo(x0 + s, y0 + s * 0.75);
        path.lineTo(x0 + s * 0.75, y0 + s * 0.75);
        path.addRect(QRectF(x0, y0 + s * 0.25, s * 0.75, s * 0.75));
        painter->drawPath(path);
    } break;
    case QStyle::SC_MdiCloseButton: {
        // 关闭：交叉线
        const qreal half = glyphSize / 2;
        painter->drawLine(QPointF(center.x() - half, center.y() - half), QPointF(center.x() + half, center.y() + half));
        painter->drawLine(QPointF(center.x() + half, center.y() - half), QPointF(center.x() - half, center.y() + half));
    } break;
    default:
        break;
    }
}

/**
 * \if ENGLISH
 * @brief Paint all visible MDI buttons with hover/pressed feedback
 * \endif
 *
 * \if CHINESE
 * @brief 绘制所有可见的MDI按钮，带悬停/按下状态反馈
 * \endif
 */
static void saDrawMdiControls(const QStyleOptionComplex* option, QPainter* painter)
{
    const QList< QStyle::SubControl > controls = saVisibleMdiSubControls(option->subControls);
    const int count                            = controls.size();
    if (0 == count) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    const int buttonWidth = option->rect.width() / count;
    const bool isSunken   = option->state.testFlag(QStyle::State_Sunken);
    const bool isHover    = option->state.testFlag(QStyle::State_MouseOver);
    for (int i = 0; i < count; ++i) {
        const QStyle::SubControl sc = controls.at(i);
        const QRect logicalRect(option->rect.left() + i * buttonWidth, option->rect.top(), buttonWidth,
                                option->rect.height());
        const QRect buttonRect = QStyle::visualRect(option->direction, option->rect, logicalRect);
        const bool isActive    = option->activeSubControls.testFlag(sc);
        const bool pressed     = isSunken && isActive;
        const bool hovered     = !pressed && isHover && isActive;
        const bool isClose     = (QStyle::SC_MdiCloseButton == sc);
        QColor backgroundColor;
        QColor glyphColor = option->palette.windowText().color();
        if (isClose && (hovered || pressed)) {
            // 关闭按钮悬停/按下用红色背景，与Windows系统按钮观感一致
            backgroundColor = pressed ? QColor(0xf1, 0x70, 0x7a) : QColor(0xe8, 0x11, 0x23);
            glyphColor      = Qt::white;
        } else if (hovered || pressed) {
            QColor highlight = option->palette.highlight().color();
            highlight.setAlpha(pressed ? 64 : 32);
            backgroundColor = highlight;
        }
        if (backgroundColor.isValid()) {
            painter->fillRect(buttonRect, backgroundColor);
        }
        saDrawMdiButtonGlyph(painter, sc, buttonRect, glyphColor);
    }
    painter->restore();
}

SARibbonMdiControlsStyle::SARibbonMdiControlsStyle() : QProxyStyle()
{
}

void SARibbonMdiControlsStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option,
                                                  QPainter* painter, const QWidget* widget) const
{
    if (CC_MdiControls != control || nullptr == option || nullptr == painter) {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        return;
    }
    saDrawMdiControls(option, painter);
}

QRect SARibbonMdiControlsStyle::subControlRect(ComplexControl control, const QStyleOptionComplex* option,
                                               SubControl subControl, const QWidget* widget) const
{
    if (CC_MdiControls != control || nullptr == option) {
        return QProxyStyle::subControlRect(control, option, subControl, widget);
    }
    const QList< QStyle::SubControl > controls = saVisibleMdiSubControls(option->subControls);
    const int count                            = controls.size();
    if (0 == count || !controls.contains(subControl)) {
        return QRect();
    }
    const int buttonWidth = option->rect.width() / count;
    const int offset      = option->rect.left() + controls.indexOf(subControl) * buttonWidth;
    const QRect logicalRect(offset, option->rect.top(), buttonWidth, option->rect.height());
    return visualRect(option->direction, option->rect, logicalRect);
}

QStyle::SubControl SARibbonMdiControlsStyle::hitTestComplexControl(ComplexControl control,
                                                                   const QStyleOptionComplex* option,
                                                                   const QPoint& position, const QWidget* widget) const
{
    if (CC_MdiControls != control || nullptr == option) {
        return QProxyStyle::hitTestComplexControl(control, option, position, widget);
    }
    const QList< QStyle::SubControl > controls = saVisibleMdiSubControls(option->subControls);
    for (QStyle::SubControl sc : controls) {
        if (subControlRect(control, option, sc, widget).contains(position)) {
            return sc;
        }
    }
    return SC_None;
}

void SARibbonMdiControlsStyle::applyToMdiControllerWidget(QWidget* widget)
{
    if (nullptr == widget) {
        return;
    }
    // QMdi::ControllerWidget是Qt私有类（QMdiSubWindow最大化时移入菜单栏角落的自绘控件），通过类名识别
    if (QLatin1String(widget->metaObject()->className()) != QLatin1String("QMdi::ControllerWidget")) {
        return;
    }
    // 通过动态属性标记已应用状态：存在应用级样式表时setStyle会把自定义样式
    // 包进私有的QStyleSheetStyle，外部无法通过类型识别base，属性标记可跨Qt版本稳定工作
    if (widget->property("sa_mdiControlsStyleApplied").toBool()) {
        return;
    }
    static SARibbonMdiControlsStyle* sMdiControlsStyle = nullptr;
    if (nullptr == sMdiControlsStyle) {
        // QProxyStyle无参构造会自行创建并持有桌面样式实例作为base，不涉及应用级样式对象的所有权；
        // QStyleSheetStyle仅以裸指针持有base，不接管其生命周期，单例挂到qApp下与应用程序同生命周期
        sMdiControlsStyle = new SARibbonMdiControlsStyle();
        sMdiControlsStyle->setParent(QApplication::instance());
    }
    widget->setStyle(sMdiControlsStyle);
    widget->setProperty("sa_mdiControlsStyleApplied", true);
}
