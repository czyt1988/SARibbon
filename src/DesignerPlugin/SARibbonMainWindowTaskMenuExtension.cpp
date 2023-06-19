#include "SARibbonMainWindowTaskMenuExtension.h"
#include <QAction>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormEditorInterface>
#include <QDesignerWidgetFactoryInterface>
#include <QDesignerContainerExtension>
#include <QDesignerMetaDataBaseInterface>
#include <QExtensionManager>



#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"

#include "SARibbonPluginDebugHelper.h"
using namespace SA_PLUGIN;
SARibbonMainWindowTaskMenuExtension::SARibbonMainWindowTaskMenuExtension(QWidget *w, QObject *p)
    : QObject(p)
    , m_widget(w)
    , m_bar(nullptr)
    , m_useRibbon(nullptr)
{
    initActions();
}


QList<QAction *> SARibbonMainWindowTaskMenuExtension::taskActions() const
{
    return (m_actions);
}


/**
 * @brief 初始化所有actions
 */
void SARibbonMainWindowTaskMenuExtension::initActions()
{
    m_useRibbon = new QAction(tr("use ribbon"), this);

    m_useRibbon->setObjectName(QStringLiteral("use_ribbon"));
    connect(m_useRibbon, &QAction::triggered, this, &SARibbonMainWindowTaskMenuExtension::onUseRibbon);

    m_actions.append(m_useRibbon);
}


QDesignerFormWindowInterface *SARibbonMainWindowTaskMenuExtension::formWindowInterface() const
{
    QDesignerFormWindowInterface *result = QDesignerFormWindowInterface::findFormWindow(m_widget);

    return (result);
}


QDesignerFormEditorInterface *SARibbonMainWindowTaskMenuExtension::core() const
{
    QDesignerFormWindowInterface *fw = formWindowInterface();

    if (nullptr == fw) {
        return (nullptr);
    }
    return (fw->core());
}


/**
 * @brief SARibbonMainWindowTaskMenuExtension::onUseRibbon
 */
void SARibbonMainWindowTaskMenuExtension::onUseRibbon()
{
    SA_PLUGIN_MARK();
    QDesignerFormWindowInterface *fw = formWindowInterface();

    if (nullptr == fw) {
        return;
    }
    SARibbonMainWindow *mw = qobject_cast<SARibbonMainWindow *>(fw->mainContainer());

    if (!mw) {
        return;
    }
    if (mw->isUseRibbon()) {
        return;
    }
    QDesignerFormEditorInterface *core = fw->core();

    if (!core) {
        return;
    }
    SA_PLUGIN_LOG("add ribbon bar");
    //在QMainWindow下创建SARibbonBar，将调用
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension *>(core->extensionManager(), mw);

    //添加bar，这时候会触发SARibbonMainWindowContainerExtension::addWidget(QWidget *widget)函数
    m_bar = qobject_cast<SARibbonBar *>(core->widgetFactory()->createWidget("SARibbonBar", mw));

    core->widgetFactory()->initialize(m_bar);
    c->addWidget(m_bar);
    //添加一个子模块到QMainWindow下面
    m_bar->setObjectName(QLatin1String("ribbonbar"));
    //确保objname单一，不单一会自动后面加_1
    fw->ensureUniqueObjectName(m_bar);
    //添加metadata
    core->metaDataBase()->add(m_bar);
    //选中
    fw->emitSelectionChanged();
    //
    m_bar->setFocus();
    //
    m_useRibbon->setEnabled(false);
}
