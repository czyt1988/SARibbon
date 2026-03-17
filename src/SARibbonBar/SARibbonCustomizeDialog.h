#ifndef SARIBBONCUSTOMIZEDIALOG_H
#define SARIBBONCUSTOMIZEDIALOG_H
#include "SARibbonGlobal.h"
#include <QDialog>
#include "SARibbonCustomizeWidget.h"
class SARibbonActionsManager;
class SARibbonCustomizeDialogUi;
class QXmlStreamWriter;


/**
 * \if ENGLISH
 * @brief Dialog wrapper for SARibbonCustomizeWidget
 * @details This function depends on @ref SARibbonActionsManager. SARibbonActionsManager is recommended to be a member variable of mainwindow.
 *          SARibbonActionsManager can quickly bind all QActions, see SARibbonActionsManager documentation for details.
 * @note SARibbon customization is step-based. If @ref sa_apply_customize_from_xml_file or similar functions are called before the window is generated,
 *       you need to call @ref SARibbonCustomizeDialog::fromXml before the dialog is generated to synchronize the configuration file,
 *       so that the modified configuration file is consistent.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCustomizeWidget的对话框封装
 * @details 此功能依赖于@ref SARibbonActionsManager ，SARibbonActionsManager建议作为mianwindow的成员变量，
 *          SARibbonActionsManager可以快速绑定所有QAction，详细见SARibbonActionsManager的说明
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 *       那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeDialog::fromXml 同步配置文件，这样再次修改后的配置文件就一致
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    /// Constructor for SARibbonCustomizeDialog
    explicit SARibbonCustomizeDialog(SARibbonMainWindow *ribbonWindow, QWidget *p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    /// Destructor for SARibbonCustomizeDialog
    ~SARibbonCustomizeDialog();
    /// Set the action manager
    void setupActionsManager(SARibbonActionsManager *mgr);

	/// Check if there is content to store, corresponding to save action
	bool isApplied() const;

	/// Check if there are modified contents, corresponding to apply action
	bool isCached() const;

    /// Apply all settings
    bool applys();

    /// Clear all actions
    void clear();

    /// Convert to XML
    bool toXml(QXmlStreamWriter *xml) const;
    /// Convert to XML file
    bool toXml(const QString& xmlpath) const;

    /// Load from XML, for file-based settings, it is recommended to call this function before the dialog is displayed
    void fromXml(QXmlStreamReader *xml);
    /// Load from XML file
    void fromXml(const QString& xmlpath);

    /// Return the SARibbonCustomizeWidget pointer
    SARibbonCustomizeWidget *customizeWidget() const;

private:
    void initConnection();

    SARibbonCustomizeDialogUi *ui;
};

#endif // SARIBBONCUSTOMIZEDIALOG_H
