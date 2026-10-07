#ifndef SARIBBONQMLACTION_H
#define SARIBBONQMLACTION_H
#include "SARibbonQmlGlobal.h"
// the single QAction include face of the module (contract D5, plan-05 S1)
#include "SARibbonQmlActionCompat.h"
#include <QUrl>
#include <QVariantList>

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
    Q_PROPERTY(QVariantList menuActions READ menuActions WRITE setMenuActions NOTIFY menuActionsChanged)
public:
    explicit RibbonAction(QObject* parent = nullptr);

    QUrl iconSource() const;
    void setIconSource(const QUrl& url);

    // String convenience for QAction::shortcut (S0-V4: QML string literals
    // cannot reach a QKeySequence property directly); empty clears it
    QString shortcutText() const;
    void setShortcutText(const QString& text);

    // Submenu container: a flat list of QAction* (nested levels come from
    // RibbonAction entries carrying their own menuActions). Pure data here;
    // the menu rendering is plan-06 S3
    QVariantList menuActions() const;
    void setMenuActions(const QVariantList& actions);

Q_SIGNALS:
    void iconSourceChanged();
    void shortcutTextChanged();
    void menuActionsChanged();

private:
    // re-emit the convenience notifies off the coarse QAction::changed
    void syncNotifies();

    QUrl mIconSource;
    QVariantList mMenuActions;
};

}
#endif  // SARIBBONQMLACTION_H
