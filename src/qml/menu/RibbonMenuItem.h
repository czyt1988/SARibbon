#ifndef RIBBONMENUITEM_H
#define RIBBONMENUITEM_H
#include "SARibbonQmlGlobal.h"
#include <QObject>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Declarative menu entry attached to a tool button / bar menu
 * @details Pure data object: the owning host renders the popup (leaf side)
 *          and mediates activation through its menuTriggered signal, so the
 *          item itself carries no popup logic.
 * \endif
 *
 * \if CHINESE
 * @brief 挂在工具按钮 / bar 菜单上的声明式菜单项
 * @details 纯数据对象：弹出渲染由持有它的宿主完成（叶子侧），激活经宿主的
 *          menuTriggered 信号中转，菜单项自身不携带弹出逻辑。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonMenuItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool separator READ isSeparator WRITE setSeparator NOTIFY separatorChanged)
public:
    explicit RibbonMenuItem(QObject* parent = nullptr);

    QString text() const;
    void setText(const QString& t);

    QString iconSource() const;
    void setIconSource(const QString& s);

    bool isEnabled() const;
    void setEnabled(bool on);

    bool isSeparator() const;
    void setSeparator(bool on);

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void enabledChanged();
    void separatorChanged();

private:
    QString mText;
    QString mIconSource;
    bool mEnabled   = true;
    bool mSeparator = false;
};

}

#endif  // RIBBONMENUITEM_H
