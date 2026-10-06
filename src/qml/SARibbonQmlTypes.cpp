#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlTypes.h"
#include "SARibbonQmlTheme.h"
#include "SARibbonQmlMetrics.h"
#include "SARibbonQmlQuickHost.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlMenuItem.h"
#include "SARibbonQmlPanel.h"
#include "SARibbonQmlToolButton.h"
#include "SARibbonQmlControlContainer.h"
#include "SARibbonQmlCategory.h"
#include "SARibbonQmlContextCategory.h"
#include "SARibbonQmlGallery.h"
#include "SARibbonQmlGalleryGroup.h"
#include "SARibbonQmlGalleryItem.h"
#include "SARibbonQmlSeparator.h"
#include "SARibbonQmlColorGrid.h"
#include "SARibbonQmlColorMenu.h"
#include "SARibbonQmlColorToolButton.h"
#include "SARibbonQmlQuickAccessBar.h"
#include "SARibbonQmlButtonGroup.h"
#include "SARibbonQmlApplicationWindow.h"
#include "SARibbonQmlTab.h"
#include "SARibbonQmlBar.h"
#include "SARibbonQmlActionRegistry.h"
#include "SARibbonQmlActionRegistryModel.h"
#include "SARibbonQmlCustomizer.h"
#include "SARibbonQmlCustomizeTreeModel.h"
#include "SARibbonQmlWindowAgent.h"
#include <QQmlEngine>
#include <QMetaType>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickItem>
#include <QFile>
#include <QDebug>
#include <QtQml>

namespace SARibbonQml {

QQuickItem* createVisualLeaf(QQuickItem* host, const QUrl& leafUrl)
{
    if (!host) {
        return nullptr;
    }
    QQmlEngine* engine = qmlEngine(host);
    if (!engine) {
        // C++-created hosts (e.g. the auto tabs the bar builds for categories
        // without an explicit RibbonTab) carry no QML context of their own:
        // walk the parentItem chain to the nearest QML-created ancestor
        // (KDDW View.cpp same fallback shape, plan-04 S3 note)
        QQuickItem* p = host->parentItem();
        while (p && !engine) {
            engine = qmlEngine(p);
            p = p->parentItem();
        }
    }
    if (!engine) {
        qWarning() << "SARibbonQml: no QML engine reachable from host, cannot create leaf" << leafUrl;
        return nullptr;
    }
    // QFile understands the ":/..." form but not "qrc:/..." URLs (exists() would
    // always report false for the URL form) — convert for the existence check
    QString resourcePath = leafUrl.toString();
    if (leafUrl.scheme() == QLatin1String("qrc")) {
        resourcePath = QLatin1Char(':') + leafUrl.path();
    }
    if (!QFile::exists(resourcePath)) {
        qWarning() << "SARibbonQml: leaf resource missing (static build without Q_INIT_RESOURCE?)" << leafUrl;
        return nullptr;
    }
    // The leaf must outlive this function, so its creation context must be
    // parented to a context that outlives it too. A stack QQmlComponent owns
    // the contexts created from it — creating with the default context makes
    // the leaf's bindings reference a context that dies as soon as this
    // function returns, and ANY later touch of the leaf (setParentItem at
    // teardown included) then frees V4 blocks of the dead context
    // (_CrtIsValidHeapPointer assert / QV4 access violations — the round-4/5
    // crash family, NOTES B44). Creating in the engine's ROOT context keeps
    // the leaf's context alive for the engine's lifetime.
    QQmlComponent component(engine, leafUrl);
    QObject* obj = component.create(engine->rootContext());
    if (!obj) {
        qWarning() << "SARibbonQml: leaf create() failed:" << leafUrl << component.errorString();
        return nullptr;
    }
    QQuickItem* leaf = qobject_cast< QQuickItem* >(obj);
    if (!leaf) {
        qWarning() << "SARibbonQml: leaf root is not a QQuickItem:" << leafUrl;
        obj->deleteLater();
        return nullptr;
    }
    // trilogy step 2: handshake injection — the leaf's onCppHostChanged handler
    // assigns itself back into the host's inherited qmlLeaf property
    leaf->setProperty("cppHost", QVariant::fromValue(host));
    // trilogy step 3: reparent onto the host (both parents, KDDW Group.cpp rule)
    leaf->setParentItem(host);
    leaf->setParent(host);
    leaf->setZ(-1);  // background layer under any structural children
    return leaf;
}

}  // namespace SARibbonQml

void saRibbonRegisterQmlTypes(QQmlEngine* engine)
{
    Q_UNUSED(engine);  // registration goes into the global QQmlMetaType registry
    static bool once = false;
    if (once) {
        return;
    }
    once = true;

#if defined(SA_RIBBON_QML_STATIC) || defined(QT_STATIC)
    Q_INIT_RESOURCE(saribbon_qml);  // static library qrc does not auto-load
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Qt5: the RibbonEnums enums are Q_ENUM'd in a class other than the one
    // owning the properties, and Qt5 never registers such enums with the
    // metatype registry — every enum-typed property is then opaque to the
    // QML engine ("Unable to assign int to [unknown property type]") and to
    // the property system ("Unable to handle unregistered datatype"). The
    // named qRegisterMetaType form registers each enum under its
    // moc-normalized property name (no SARibbonQml:: qualifier — that is
    // the name QMetaProperty looks up) as a typedef of the auto-named
    // metatype. Qt6 registers Q_ENUM metatypes lazily and needs none of this
    qRegisterMetaType< SARibbonQml::RibbonEnums::RowProportion >("RibbonEnums::RowProportion");
    qRegisterMetaType< SARibbonQml::RibbonEnums::LayoutMode >("RibbonEnums::LayoutMode");
    qRegisterMetaType< SARibbonQml::RibbonEnums::Alignment >("RibbonEnums::Alignment");
    qRegisterMetaType< SARibbonQml::RibbonEnums::PopupMode >("RibbonEnums::PopupMode");
    qRegisterMetaType< SARibbonQml::RibbonEnums::ToolButtonStyle >("RibbonEnums::ToolButtonStyle");
    qRegisterMetaType< SARibbonQml::RibbonEnums::RibbonStyle >("RibbonEnums::RibbonStyle");
    qRegisterMetaType< SARibbonQml::RibbonEnums::GalleryCaptionStyle >("RibbonEnums::GalleryCaptionStyle");
    qRegisterMetaType< SARibbonQml::RibbonEnums::ColorStyle >("RibbonEnums::ColorStyle");
    qRegisterMetaType< SARibbonQml::RibbonEnums::ColorMenuStyle >("RibbonEnums::ColorMenuStyle");
#endif

    // ---- singletons (callback form; CppOwnership set inside, plan-04 S1-3) ----
    qmlRegisterSingletonType< SARibbonQml::RibbonTheme >(
        "SARibbon", 3, 0, "RibbonTheme",
        [](QQmlEngine* qmlEngine, QJSEngine*) -> QObject* {
            QObject* o = SARibbonQml::RibbonTheme::instance();
            QQmlEngine::setObjectOwnership(o, QQmlEngine::CppOwnership);
            return o;
        });
    qmlRegisterSingletonType< SARibbonQml::RibbonMetrics >(
        "SARibbon", 3, 0, "RibbonMetrics",
        [](QQmlEngine* qmlEngine, QJSEngine*) -> QObject* {
            QObject* o = SARibbonQml::RibbonMetrics::instance();
            QQmlEngine::setObjectOwnership(o, QQmlEngine::CppOwnership);
            return o;
        });

    // ---- types ----
    qmlRegisterType< SARibbonQml::RibbonBar >("SARibbon", 3, 0, "RibbonBar");
    qmlRegisterType< SARibbonQml::RibbonCategory >("SARibbon", 3, 0, "RibbonCategory");
    qmlRegisterType< SARibbonQml::RibbonTab >("SARibbon", 3, 0, "RibbonTab");
    qmlRegisterType< SARibbonQml::RibbonPanel >("SARibbon", 3, 0, "RibbonPanel");
    qmlRegisterType< SARibbonQml::RibbonToolButton >("SARibbon", 3, 0, "RibbonToolButton");
    qmlRegisterType< SARibbonQml::RibbonControlContainer >("SARibbon", 3, 0, "RibbonControlContainer");
    // basic input controls (widgets QSS `SARibbonPanel > Q{CheckBox,RadioButton,
    // ComboBox,LineEdit}` specializations): pure QML documents registered by
    // URL — they carry no layout authority of their own, so unlike the
    // structural types above they need no C++ host class; embedded into a
    // panel through RibbonControlContainer.control (or usable standalone)
    qmlRegisterType(QUrl(QStringLiteral("qrc:/SARibbon/RibbonCheckBox.qml")),
                    "SARibbon", 3, 0, "RibbonCheckBox");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/SARibbon/RibbonRadioButton.qml")),
                    "SARibbon", 3, 0, "RibbonRadioButton");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/SARibbon/RibbonComboBox.qml")),
                    "SARibbon", 3, 0, "RibbonComboBox");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/SARibbon/RibbonSpinBox.qml")),
                    "SARibbon", 3, 0, "RibbonSpinBox");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/SARibbon/RibbonTextField.qml")),
                    "SARibbon", 3, 0, "RibbonTextField");
    qmlRegisterType< SARibbonQml::RibbonMenuItem >("SARibbon", 3, 0, "RibbonMenuItem");
    qmlRegisterType< SARibbonQml::RibbonContextCategory >("SARibbon", 3, 0, "RibbonContextCategory");
    qmlRegisterType< SARibbonQml::RibbonGallery >("SARibbon", 3, 0, "RibbonGallery");
    qmlRegisterType< SARibbonQml::RibbonGalleryGroup >("SARibbon", 3, 0, "RibbonGalleryGroup");
    qmlRegisterType< SARibbonQml::RibbonGalleryItem >("SARibbon", 3, 0, "RibbonGalleryItem");
    qmlRegisterType< SARibbonQml::RibbonSeparator >("SARibbon", 3, 0, "RibbonSeparator");
    qmlRegisterType< SARibbonQml::RibbonQuickAccessBar >("SARibbon", 3, 0, "RibbonQuickAccessBar");
    qmlRegisterType< SARibbonQml::RibbonButtonGroup >("SARibbon", 3, 0, "RibbonButtonGroup");
    qmlRegisterType< SARibbonQml::RibbonApplicationWindow >("SARibbon", 3, 0, "RibbonApplicationWindow");
    // frameless window agent (QWindowKit Quick route): declare as a child of
    // RibbonBar; the leaf renders the system button row automatically
    qmlRegisterType< SARibbonQml::RibbonWindowAgent >("SARibbon", 3, 0, "RibbonWindowAgent");
    // color widget family (widgets SAColorGridWidget / SAColorMenu counterparts)
    qmlRegisterType< SARibbonQml::RibbonColorGrid >("SARibbon", 3, 0, "RibbonColorGrid");
    qmlRegisterType< SARibbonQml::RibbonColorMenu >("SARibbon", 3, 0, "RibbonColorMenu");
    qmlRegisterType< SARibbonQml::RibbonColorToolButton >("SARibbon", 3, 0, "RibbonColorToolButton");
    // customization subsystem (widgets SARibbonActionsManager / SARibbonCustomizeWidget counterparts)
    static_assert(SARibbonQml::RibbonActionRegistry::QuickAccessActionTag > int(SARibbon::Core::UserDefineActionTag),
                  "QML-only quick access tag must live above the user-define tag base");
    qmlRegisterType< SARibbonQml::RibbonActionRegistry >("SARibbon", 3, 0, "RibbonActionRegistry");
    qmlRegisterType< SARibbonQml::RibbonActionRegistryModel >("SARibbon", 3, 0, "RibbonActionRegistryModel");
    qmlRegisterType< SARibbonQml::RibbonCustomizer >("SARibbon", 3, 0, "RibbonCustomizer");
    qmlRegisterType< SARibbonQml::RibbonCustomizeTreeModel >("SARibbon", 3, 0, "RibbonCustomizeTreeModel");
    // shared host bases: reachable from QML only through their subclasses,
    // registered for tooling/metaobject access (not creatable from QML)
    qmlRegisterUncreatableType< SARibbonQml::RibbonQuickHost >("SARibbon", 3, 0, "RibbonQuickHost", "Base class only");
    qmlRegisterUncreatableType< SARibbonQml::RibbonLayoutItemHost >(
        "SARibbon", 3, 0, "RibbonLayoutItemHost", "Base class only");

    // Uncreatable enum holder (enums mirrored via Q_ENUM on registered classes;
    // Q_NAMESPACE route is closed: 3.0 enums stay in the global namespace, plan-02 S1)
    qmlRegisterUncreatableType< SARibbonQml::RibbonEnums >(
        "SARibbon", 3, 0, "Ribbon", "Enum access only");

    qmlRegisterModule("SARibbon", 3, 0);
}
