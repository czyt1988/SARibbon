#ifndef SARIBBONQMLACTION_H
#define SARIBBONQMLACTION_H
#include "SARibbonQmlGlobal.h"
// the single QAction include face of the module (contract D5, plan-05 S1)
#include "SARibbonQmlActionCompat.h"
#include <QUrl>
#include <QVariantList>
#include <QQmlListProperty>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Thin QML-friendly command object on top of QAction
 * @details Contract D1/D5: QAction is the only command abstraction both front
 *          ends consume. A bare QAction created in C++ binds to QML views
 *          directly (the `action` property of RibbonToolButton takes QAction*),
 *          so RibbonAction adds only what plain QAction cannot express in QML:
 *          an `iconSource` url mirrored into QAction::icon (the same action
 *          keeps feeding the widgets world), a `shortcutText` string convenience
 *          (S0-V4: QML cannot assign a string literal to a QKeySequence
 *          property), and the `menuActions` submenu container consumed by
 *          plan-06. QAction already provides toolTip natively — it is not
 *          shadowed here (single-authority rule). Adding any placement
 *          semantics (proportion etc.) is forbidden (contract D3).
 * \endif
 *
 * \if CHINESE
 * @brief QAction 之上的薄 QML 友好命令对象
 * @details 契约 D1/D5：QAction 是两个前端共同消费的唯一命令抽象。C++ 创建的
 *          裸 QAction 可直接绑定 QML 视图（RibbonToolButton 的 `action` 属性
 *          取 QAction*），RibbonAction 只补裸 QAction 无法在 QML 表达的部分：
 *          镜像到 QAction::icon 的 `iconSource` url（同一 action 继续喂给
 *          widgets 世界）、`shortcutText` 字符串便捷属性（S0-V4：QML 无法把
 *          字符串字面量赋给 QKeySequence 属性）、以及计划 06 消费的
 *          `menuActions` 子菜单容器。toolTip 是 QAction 原生属性，不在此遮蔽
 *          （唯一权威规则）。禁止在其上添加任何放置语义（契约 D3）。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonAction : public QAction
{
    Q_OBJECT
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QString shortcutText READ shortcutText WRITE setShortcutText NOTIFY shortcutTextChanged)
    // QAction has isSeparator/setSeparator but no Q_PROPERTY for it — the QML
    // authoring face needs one (menu separators are declared, not code)
    Q_PROPERTY(bool separator READ isSeparator WRITE setSeparator NOTIFY separatorChanged)
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonAction > menuActions READ menuActions NOTIFY menuActionsChanged)
public:
    explicit RibbonAction(QObject* parent = nullptr);

    QUrl iconSource() const;
    void setIconSource(const QUrl& url);

    // String convenience for QAction::shortcut (S0-V4: QML string literals
    // cannot reach a QKeySequence property directly); empty clears it
    QString shortcutText() const;
    void setShortcutText(const QString& text);

    // QAction::isSeparator/setSeparator QML face (NOTIFY rides changed())
    bool isSeparator() const;
    void setSeparator(bool on);

    // Submenu container: a QAction list (nested levels come from
    // RibbonAction entries carrying their own menuActions — contract §5).
    // A QQmlListProperty so QML object-literal lists assign natively; a
    // plain QAction joins from C++ through addMenuAction
    QQmlListProperty< SARibbonQml::RibbonAction > menuActions();
    int menuActionCount() const;
    QAction* menuActionAt(int index) const;
    // C++ append of a plain QAction (any QAction, registered or not) into the submenu
    void addMenuAction(QAction* action);
    // the plain C++ face: the submenu as a QVariantList (live view)
    QVariantList menuActionList() const;

Q_SIGNALS:
    void iconSourceChanged();
    void shortcutTextChanged();
    void separatorChanged();
    void menuActionsChanged();

private:
    // re-emit the convenience notifies off the coarse QAction::changed
    void syncNotifies();

    // QQmlListProperty callback types differ between Qt5 (int) and Qt6 (qsizetype)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ListIndex = qsizetype;
#else
    using ListIndex = int;
#endif
    static void appendMenuActionCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop, SARibbonQml::RibbonAction* action);
    static ListIndex menuActionCountCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop);
    static SARibbonQml::RibbonAction* menuActionAtCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop, ListIndex index);
    static void clearMenuActionsCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop);

    QUrl mIconSource;
    QVector< QAction* > mMenuActions;
};

/**
 * \if ENGLISH
 * @brief Menu-tree derivation over a QVariantList of QAction entries (plan-06 S3)
 * @details Hosts (tool button, bar application menu) keep a plain QAction list
 *          as the authoring surface; this helper turns it into the row-map list
 *          the shared leaf renders (text/iconSource/shortcut/checkable/checked/
 *          enabled/separator/submenu/hasSubmenu — the same row keys 2.x
 *          carried) and keeps it LIVE: every action change
 *          rebuilds the rows. Nested levels come from RibbonAction.menuActions
 *          (contract §5: bare QAction entries are flat-menu only). Activation
 *          resolves an index path back to the QAction so the host can
 *          action->trigger() it.
 * \endif
 *
 * \if CHINESE
 * @brief QAction 菜单树列表的菜单行派生器（计划 06 S3）
 * @details 宿主（工具按钮、bar 应用菜单）持有一列 QAction 作为编写面；本工具
 *          把它转换成共享叶子渲染的行 map 列表（text/iconSource/shortcut/
 *          checkable/checked/enabled/separator/submenu/hasSubmenu——与 2.x
 *          同键名），并保持活性：任一 action 变化即重建行。
 *          嵌套层级来自 RibbonAction.menuActions（契约 §5：裸 QAction 条目仅
 *          平面菜单）。激活按索引路径解析回 QAction，宿主对它 action->trigger()。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonMenuModel : public QObject
{
    Q_OBJECT
public:
    explicit RibbonMenuModel(QObject* parent = nullptr);
    ~RibbonMenuModel() override;

    // (re)set the QAction list; wires the live update chain
    void setActions(const QVariantList& actions);
    const QVariantList& actions() const;

    // Derived row maps for the leaf (NOTIFY rowsChanged on every rebuild)
    QVariantList rows() const;

    // Resolve an activation index path back to the QAction it addresses
    // (nullptr on a bad path, a separator or a disabled entry)
    QAction* resolvePath(const QVariantList& indexPath) const;

Q_SIGNALS:
    void rowsChanged();

private:
    void rewatch();
    void rebuild();

    QVariantList mActions;
    QVariantList mRows;
    QVector< QMetaObject::Connection > mWatched;
};

// Build the row-map list of an action list (pure derivation, no watching —
// RibbonMenuModel wraps this for the live path)
SA_RIBBON_QML_EXPORT QVariantList saRibbonMenuRows(const QVariantList& actions);

}
#endif  // SARIBBONQMLACTION_H
