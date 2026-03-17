#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonActionsManager.h"
#include "SARibbonPanel.h"
#include "SARibbonCustomizeData.h"

// SARibbonCustomizeWidget 特有
class SARibbonCustomizeWidgetUi;
class SARibbonMainWindow;
class SARibbonBar;
//
class QStandardItemModel;
class QStandardItem;
class QAbstractButton;
//
class QXmlStreamWriter;
class QXmlStreamReader;

/**
 * \if ENGLISH
 * @brief Customization interface widget
 * @note SARibbon customization is step-based. If @ref sa_apply_customize_from_xml_file or similar functions are called
 * before the window is generated, you need to call @ref SARibbonCustomizeWidget::fromXml before the dialog is generated
 * to synchronize the configuration file, so that the modified configuration file is consistent.
 * \endif
 *
 * \if CHINESE
 * @brief 自定义界面窗口
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 *       那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeWidget::fromXml
 * 同步配置文件，这样再次修改后的配置文件就一致
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCustomizeWidget)
public:
    /// Constructor using SARibbonMainWindow
    explicit SARibbonCustomizeWidget(SARibbonMainWindow* ribbonWindow,
                                     QWidget* parent   = nullptr,
                                     Qt::WindowFlags f = Qt::WindowFlags());
    /// Constructor using SARibbonBar directly
    explicit SARibbonCustomizeWidget(SARibbonBar* ribbonbar,
                                     QWidget* parent   = nullptr,
                                     Qt::WindowFlags f = Qt::WindowFlags());
    ~SARibbonCustomizeWidget();

    /**
     * \if ENGLISH
     * @brief Ribbon tree display type
     * \endif
     *
     * \if CHINESE
     * @brief 定义ribbon树的显示类型
     * \endif
     */
    enum RibbonTreeShowType
    {
        ShowAllCategory,  ///< Show all categories including context categories
        ShowMainCategory  ///< Show main categories only, excluding context categories
    };

    /**
     * \if ENGLISH
     * @brief Item roles for QStandardItem
     * \endif
     *
     * \if CHINESE
     * @brief QStandardItem对应的role
     * \endif
     */
    enum ItemRole
    {
        LevelRole        = Qt::UserRole + 1,  ///< Level: 0=category, 1=panel, 2=item
        PointerRole      = Qt::UserRole + 2,  ///< Pointer storage, cast based on LevelRole
        CanCustomizeRole = Qt::UserRole + 3,  ///< Whether this item can be customized (bool)
        CustomizeRole = Qt::UserRole + 4,  ///< Whether this is a custom item (bool), mainly for self-added tabs and panels
        CustomizeObjNameRole = Qt::UserRole + 5  ///< Temporary custom content object name (QString)
    };

    /// Set the action manager
    void setupActionsManager(SARibbonActionsManager* mgr);

    /// Check if there is content to store, corresponding to save action
    bool isApplied() const;

    /// Check if there are modified contents, corresponding to apply action
    bool isCached() const;

    /// Get the model
    const QStandardItemModel* model() const;

    /// Update model based on current radiobutton selection
    void updateModel();

    /// Update model with specified type
    void updateModel(RibbonTreeShowType type);

    /// Apply all settings
    bool applys();

    /// Convert to XML
    bool toXml(QXmlStreamWriter* xml) const;
    /// Convert to XML file
    bool toXml(const QString& xmlpath) const;

    /// Load from XML, for file-based settings, it is recommended to call this function before the dialog is displayed
    void fromXml(QXmlStreamReader* xml);
    /// Load from XML file
    void fromXml(const QString& xmlpath);

    /// Apply XML configuration
    static bool fromXml(QXmlStreamReader* xml, SARibbonBar* bar, SARibbonActionsManager* mgr);

    /// Cache applied actions, these actions will not be cleared by clear(), used for local storage
    void makeActionsApplied();

    /// Clear applied actions, need to clear applied actions after cancel operation
    void clearApplied();
    /// Clear cached actions, after executing applys function, if you want to continue calling, you should clear, otherwise it will cause exceptions
    void clearCache();
    /// Clear all actions, excluding locally read data
    void clear();

protected:
    /// Simplify QList<SARibbonCustomizeData>, merge some actions
    void simplify();

    /// Get the row proportion selected in the current interface
    SARibbonPanelItem::RowProportion selectedRowProportion() const;

    /// Get the action selected in listview
    QAction* selectedAction() const;
    /// Convert item to action
    QAction* itemToAction(QStandardItem* item) const;

    /// Get the selected item in ribbon tree
    QStandardItem* selectedItem() const;

    /// Get the level of selected ribbon tree item
    int selectedRibbonLevel() const;

    /// Get the level based on selected item
    int itemLevel(QStandardItem* item) const;

    /// Set an item to be selected
    void setSelectItem(QStandardItem* item, bool ensureVisible = true);

    /// Check if item can be customized
    bool isItemCanCustomize(QStandardItem* item) const;
    bool isSelectedItemCanCustomize() const;

    /// Check if item is a customize item
    bool isCustomizeItem(QStandardItem* item) const;
    bool isSelectedItemIsCustomize() const;

    /// Remove an item
    void removeItem(QStandardItem* item);

private Q_SLOTS:
    void onComboBoxActionIndexCurrentIndexChanged(int index);
    void onRadioButtonGroupButtonClicked(QAbstractButton* b);
    void onPushButtonNewCategoryClicked();
    void onPushButtonNewPanelClicked();
    void onPushButtonRenameClicked();
    void onPushButtonAddClicked();
    void onPushButtonDeleteClicked();
    void onListViewSelectClicked(const QModelIndex& index);
    void onTreeViewResultClicked(const QModelIndex& index);
    void onToolButtonUpClicked();
    void onToolButtonDownClicked();
    void onItemChanged(QStandardItem* item);
    void onLineEditSearchActionTextEdited(const QString& text);
    void onPushButtonResetClicked();

private:
    void init(SARibbonBar* ribbonbar);
    void initConnection();

private:
    SARibbonCustomizeWidgetUi* ui;
};

/**
 * \if ENGLISH
 * @brief Convert to XML
 * @details This function only writes elements, not document related content.
 * @param xml QXmlStreamWriter pointer
 * @param cds Steps generated based on QList<SARibbonCustomizeData>
 * @return Returns false if exception occurs, also returns false if there is no customization data
 * \endif
 *
 * \if CHINESE
 * @brief 转换为xml
 * @details 此函数仅会写element，不会写document相关内容
 * @param xml QXmlStreamWriter指针
 * @param cds 基于QList<SARibbonCustomizeData>生成的步骤
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 * \endif
 */
bool SA_RIBBON_EXPORT sa_customize_datas_to_xml(QXmlStreamWriter* xml, const QList< SARibbonCustomizeData >& cds);

/**
 * \if ENGLISH
 * @brief Get QList<SARibbonCustomizeData> from XML
 * @param xml XML stream reader
 * @param mgr Action manager
 * @return QList<SARibbonCustomizeData>
 * \endif
 *
 * \if CHINESE
 * @brief 通过xml获取QList<SARibbonCustomizeData>
 * @param xml XML流读取器
 * @param mgr Action管理器
 * @return QList<SARibbonCustomizeData>
 * \endif
 */
QList< SARibbonCustomizeData > SA_RIBBON_EXPORT sa_customize_datas_from_xml(QXmlStreamReader* xml,
                                                                            SARibbonActionsManager* mgr);

/**
 * \if ENGLISH
 * @brief Apply QList<SARibbonCustomizeData>
 * @param cds Customize data list
 * @param w SARibbonBar pointer
 * @return Number of successfully applied items
 * \endif
 *
 * \if CHINESE
 * @brief 应用QList<SARibbonCustomizeData>
 * @param cds 自定义数据列表
 * @param w SARibbonBar指针
 * @return 成功应用的个数
 * \endif
 */
int SA_RIBBON_EXPORT sa_customize_datas_apply(const QList< SARibbonCustomizeData >& cds, SARibbonBar* w);

/**
 * \if ENGLISH
 * @brief Reverse apply QList<SARibbonCustomizeData>
 * @param cds Customize data list
 * @param w SARibbonBar pointer
 * @return Number of successfully reversed items
 * \endif
 *
 * \if CHINESE
 * @brief 反向取消应用
 * @param cds 自定义数据列表
 * @param w SARibbonBar指针
 * @return 成功应用的个数
 * \endif
 */
int SA_RIBBON_EXPORT sa_customize_datas_reverse(const QList< SARibbonCustomizeData >& cds, SARibbonBar* w);

/**
 * \if ENGLISH
 * @brief Directly load XML customization file for ribbon customization display
 * @param filePath XML configuration file path
 * @param bar SARibbonBar pointer
 * @param mgr Action manager
 * @return Returns true if successful
 * @note Repeating loading a configuration file will cause exceptions. To avoid this, generally use a variable to ensure it is only loaded once.
 * \endif
 *
 * \if CHINESE
 * @brief 直接加载xml自定义ribbon配置文件用于ribbon的自定义显示
 * @param filePath xml配置文件路径
 * @param bar SARibbonBar指针
 * @param mgr action管理器
 * @return 成功返回true
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次
 * \endif
 */
bool SA_RIBBON_EXPORT sa_apply_customize_from_xml_file(const QString& filePath, SARibbonBar* bar, SARibbonActionsManager* mgr);

#endif  // SARIBBONCUSTOMIZEWIDGET_H
