#include "SARibbonCustomizeDialog.h"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpacerItem>

/**
 * \if ENGLISH
 * @brief UI class for SARibbonCustomizeDialog
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCustomizeDialogUi 类
 * \endif
 */
class SARibbonCustomizeDialogUi
{
public:
    SARibbonCustomizeWidget* customWidget;
    QVBoxLayout* verticalLayoutMain;
    QHBoxLayout* horizontalLayoutButtonGroup;
    QPushButton* pushButtonCancel;
    QPushButton* pushButtonOk;
    QSpacerItem* spacerItemleft;
    void setupUi(SARibbonMainWindow* ribbonWindow, QWidget* customizeDialog)
    {
        if (customizeDialog->objectName().isEmpty()) {
            customizeDialog->setObjectName(QStringLiteral("SARibbonCustomizeDialog"));
        }
        customizeDialog->resize(800, 600);
        verticalLayoutMain = new QVBoxLayout(customizeDialog);
        verticalLayoutMain->setObjectName(QStringLiteral("verticalLayoutMain"));

        customWidget = new SARibbonCustomizeWidget(ribbonWindow, customizeDialog);
        customWidget->setObjectName(QStringLiteral("customWidget"));
        verticalLayoutMain->addWidget(customWidget);

        horizontalLayoutButtonGroup = new QHBoxLayout();
        horizontalLayoutButtonGroup->setObjectName(QStringLiteral("horizontalLayoutButtonGroup"));

        spacerItemleft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        horizontalLayoutButtonGroup->addItem(spacerItemleft);

        pushButtonCancel = new QPushButton(customizeDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        horizontalLayoutButtonGroup->addWidget(pushButtonCancel);

        pushButtonOk = new QPushButton(customizeDialog);
        pushButtonOk->setObjectName(QStringLiteral("pushButtonCancel"));
        horizontalLayoutButtonGroup->addWidget(pushButtonOk);
        verticalLayoutMain->addItem(horizontalLayoutButtonGroup);
        retranslateUi(customizeDialog);
    }

    void retranslateUi(QWidget* customizeDialog)
    {
        customizeDialog->setWindowTitle(QApplication::translate("SARibbonCustomizeDialog", "Customize Dialog", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("SARibbonCustomizeDialog", "Cancel", Q_NULLPTR));
        pushButtonOk->setText(QApplication::translate("SARibbonCustomizeDialog", "OK", Q_NULLPTR));
    }
};

////////////////////////////////////////////////////////////////////

SARibbonCustomizeDialog::SARibbonCustomizeDialog(SARibbonMainWindow* ribbonWindow, QWidget* p, Qt::WindowFlags f)
    : QDialog(p, f), ui(new SARibbonCustomizeDialogUi)
{
    ui->setupUi(ribbonWindow, this);
    initConnection();
}

/**
 * \if ENGLISH
 * @brief Set the action manager
 * @details Equivalent to @ref SARibbonCustomizeWidget::setupActionsManager
 * @param mgr SARibbonActionsManager pointer
 * \endif
 *
 * \if CHINESE
 * @brief 设置action管理器
 * @details 等同@ref SARibbonCustomizeWidget::setupActionsManager
 * @param mgr SARibbonActionsManager 指针
 * \endif
 */
SARibbonCustomizeDialog::~SARibbonCustomizeDialog()
{
}
void SARibbonCustomizeDialog::setupActionsManager(SARibbonActionsManager* mgr)
{
    ui->customWidget->setupActionsManager(mgr);
}

/**
 * \if ENGLISH
 * @brief Check if there is content to store, corresponding to save action
 * @return true if there is content to save
 * \endif
 *
 * \if CHINESE
 * @brief 判断用户是否有要存储的内容，对应save动作
 * @return 是否有内容可保存
 * \endif
 */
bool SARibbonCustomizeDialog::isApplied() const
{
    return ui->customWidget->isApplied();
}

/**
 * \if ENGLISH
 * @brief Check if there are modified contents, corresponding to apply action
 * @return true if there are cached changes
 * \endif
 *
 * \if CHINESE
 * @brief 判断用户是否有改动内容，对应apply动作
 * @return 是否有缓存的改动
 * \endif
 */
bool SARibbonCustomizeDialog::isCached() const
{
    return ui->customWidget->isCached();
}

void SARibbonCustomizeDialog::initConnection()
{
    connect(ui->pushButtonOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::reject);
}

/**
 * \if ENGLISH
 * @brief Equivalent to SARibbonCustomizeWidget::applys
 * @return true if apply succeeded
 * \endif
 *
 * \if CHINESE
 * @brief 等同SARibbonCustomizeWidget::applys
 * @return 应用是否成功
 * \endif
 */
bool SARibbonCustomizeDialog::applys()
{
    return ui->customWidget->applys();
}

/**
 * \if ENGLISH
 * @brief Clear all actions
 * \endif
 *
 * \if CHINESE
 * @brief 清除所有动作
 * \endif
 */
void SARibbonCustomizeDialog::clear()
{
    ui->customWidget->clear();
}

/**
 * \if ENGLISH
 * @brief Convert to XML
 * @param xml XML stream writer
 * @return true if conversion succeeded
 * \endif
 *
 * \if CHINESE
 * @brief 转换为xml
 * @param xml XML 流写入器
 * @return 转换是否成功
 * \endif
 */
bool SARibbonCustomizeDialog::toXml(QXmlStreamWriter* xml) const
{
    return (ui->customWidget->toXml(xml));
}

/**
 * \if ENGLISH
 * @brief Equivalent to SARibbonCustomizeWidget::toXml
 * @param xmlpath XML file path
 * @return true if conversion succeeded
 * \endif
 *
 * \if CHINESE
 * @brief 等同SARibbonCustomizeWidget::toXml
 * @param xmlpath XML 文件路径
 * @return 转换是否成功
 * \endif
 */
bool SARibbonCustomizeDialog::toXml(const QString& xmlpath) const
{
    return (ui->customWidget->toXml(xmlpath));
}

/**
 * \if ENGLISH
 * @brief Equivalent to SARibbonCustomizeWidget::fromXml
 * @param xml XML stream reader
 * \endif
 *
 * \if CHINESE
 * @brief 等同SARibbonCustomizeWidget::fromXml
 * @param xml XML 流读取器
 * \endif
 */
void SARibbonCustomizeDialog::fromXml(QXmlStreamReader* xml)
{
    ui->customWidget->fromXml(xml);
}

/**
 * \if ENGLISH
 * @brief Equivalent to SARibbonCustomizeWidget::fromXml
 * @param xmlpath XML file path
 * \endif
 *
 * \if CHINESE
 * @brief 等同SARibbonCustomizeWidget::fromXml
 * @param xmlpath XML 文件路径
 * \endif
 */
void SARibbonCustomizeDialog::fromXml(const QString& xmlpath)
{
    ui->customWidget->fromXml(xmlpath);
}

/**
 * \if ENGLISH
 * @brief Return the SARibbonCustomizeWidget pointer
 * @details Through SARibbonCustomizeWidget, you can operate more content
 * @return SARibbonCustomizeWidget pointer
 * \endif
 *
 * \if CHINESE
 * @brief 返回SARibbonCustomizeWidget窗口指针
 * @details 通过SARibbonCustomizeWidget窗口可以操作更多的内容
 * @return SARibbonCustomizeWidget指针
 * \endif
 */
SARibbonCustomizeWidget* SARibbonCustomizeDialog::customizeWidget() const
{
    return (ui->customWidget);
}
