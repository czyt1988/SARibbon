#ifndef RIBBONMENUITEM_H
#define RIBBONMENUITEM_H
#include "SARibbonQmlGlobal.h"
#include <QObject>
#include <QQmlListProperty>
#include <QVariantList>
#include <QVector>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Declarative menu entry attached to a tool button / bar menu
 * @details Pure data object: the owning host renders the popup (leaf side)
 *          and mediates activation through its menuTriggered signal, so the
 *          item itself carries no popup logic.
 *          The checkable/checked pair mirrors QAction: an entry declared
 *          checkable carries a mark and toggles itself when activated, which
 *          is what the widgets side gets for free from QAction::trigger.
 *          shortcut is the caption drawn right-aligned on the row; wiring it
 *          to a real key binding needs the QAction bridge (deferred), so the
 *          string is presentational in this front end.
 *          submenu holds nested RibbonMenuItem objects (widgets
 *          SARibbonMenu::addRibbonMenu parity): the leaf renders them as a
 *          hover-opened sibling popup, and the owning host resolves an index
 *          path down to the activated leaf entry.
 * \endif
 *
 * \if CHINESE
 * @brief 挂在工具按钮 / bar 菜单上的声明式菜单项
 * @details 纯数据对象：弹出渲染由持有它的宿主完成（叶子侧），激活经宿主的
 *          menuTriggered 信号中转，菜单项自身不携带弹出逻辑。
 *          checkable/checked 一对对照 QAction：声明为可勾选的菜单项带勾选标记，
 *          被激活时自行翻转状态——这正是 widgets 侧从 QAction::trigger 白拿到的
 *          行为。shortcut 是右对齐绘制在行尾的文本；把它接到真正的按键绑定需要
 *          QAction 抽象桥（已延后），故本前端里它是纯展示字段。
 *          submenu 承载嵌套的 RibbonMenuItem（对应 widgets
 *          SARibbonMenu::addRibbonMenu）：叶子把它们渲染为悬停弹出的同级弹窗，
 *          持有它的宿主按索引路径一路解析到被激活的末端菜单项。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonMenuItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool separator READ isSeparator WRITE setSeparator NOTIFY separatorChanged)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonMenuItem > submenu READ submenu NOTIFY submenuChanged)
    Q_PROPERTY(bool hasSubmenu READ hasSubmenu NOTIFY submenuChanged)
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

    // QAction checkable parity: clearing checkable also clears checked
    bool isCheckable() const;
    void setCheckable(bool on);

    bool isChecked() const;
    void setChecked(bool on);

    // Right-aligned row caption (presentational; the QAction bridge is deferred)
    QString shortcut() const;
    void setShortcut(const QString& s);

    // Nested entries (widgets SARibbonMenu::addRibbonMenu parity)
    QQmlListProperty< SARibbonQml::RibbonMenuItem > submenu();
    int submenuCount() const;
    RibbonMenuItem* submenuItemAt(int index) const;
    bool hasSubmenu() const;

    // Toggle + triggered(); called by the owning host while it resolves an
    // activation path (separators and disabled entries are refused there).
    // Deliberately NOT invokable: activation must stay mediated by the host so
    // menuTriggered / applicationMenuTriggered keep being the single entry point
    void activate();

    // Walk an index path from a top-level list down to the entry it addresses
    // (the leaf publishes one path per activation); nullptr on any bad step
    static RibbonMenuItem* resolvePath(const QVector< RibbonMenuItem* >& roots, const QVariantList& indexPath);

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void enabledChanged();
    void separatorChanged();
    void checkableChanged();
    void checkedChanged();
    void toggled(bool checked);
    void shortcutChanged();
    void submenuChanged();

    /**
     * \if ENGLISH
     * @brief The entry was activated (through the owning host or trigger())
     * \endif
     *
     * \if CHINESE
     * @brief 菜单项被激活（经持有它的宿主或 trigger()）
     * \endif
     */
    void triggered();

private:
    // QQmlListProperty callback types differ between Qt5 (int) and Qt6 (qsizetype)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ListIndex = qsizetype;
#else
    using ListIndex = int;
#endif
    static void appendSubmenuEntry(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop, SARibbonQml::RibbonMenuItem* item);
    static ListIndex submenuCountCb(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop);
    static SARibbonQml::RibbonMenuItem* submenuItemAtCb(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop, ListIndex index);
    static void clearSubmenu(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop);

    void emitSubmenuChanged();

    QString mText;
    QString mIconSource;
    QString mShortcut;
    bool mEnabled   = true;
    bool mSeparator = false;
    bool mCheckable = false;
    bool mChecked   = false;
    QVector< RibbonMenuItem* > mSubmenu;
};

}

#endif  // RIBBONMENUITEM_H
