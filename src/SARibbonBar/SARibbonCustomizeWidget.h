#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonActionsManager.h"
#include "SARibbonPannel.h"
#include "SARibbonCustomizeData.h"


//SARibbonCustomizeWidget 特有
class SARibbonCustomizeWidgetUi;
class SARibbonCustomizeWidgetPrivate;
class SARibbonMainWindow;
class QStandardItemModel;
class QStandardItem;
class QAbstractButton;
//
class QXmlStreamWriter;
class QXmlStreamReader;

/**
 * @brief 自定义界面窗口
 *
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 * 那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeWidget::fromXml 同步配置文件，这样再次修改后的配置文件就一致
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
public:
    SARibbonCustomizeWidget(SARibbonMainWindow *ribbonWindow, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~SARibbonCustomizeWidget();

    /**
     * @brief 定义ribbon树的显示类型
     */
    enum RibbonTreeShowType {
        ShowAllCategory         ///< 显示所有Category，包括contextcategory
        , ShowMainCategory      ///< 显示主要的category，不包含上下文
    };

    /**
     * @brief QStandardItem对应的role
     */
    enum ItemRole {
        LevelRole		= Qt::UserRole + 1      ///< 代表这是层级，有0：category 1：pannel 2：item
        , PointerRole		= Qt::UserRole + 2      ///< 代表这是存放指针。根据LevelRole来进行转
        , CanCustomizeRole	= Qt::UserRole + 3      ///< 代表个item是可以自定义的.bool
        , CustomizeRole		= Qt::UserRole + 4      ///< 代表这个是自定义的item,bool,主要用于那些自己添加的标签和pannel，有此角色必有CanCustomizeRole
        , CustomizeObjNameRole	= Qt::UserRole + 5      ///< 记录了临时的自定义内容的obj名 QString
    };



    //设置action管理器
    void setupActionsManager(SARibbonActionsManager *mgr);

    //判断用户是否有改动内容
    bool isChanged() const;

    //获取model
    const QStandardItemModel *model() const;

    //根据当前的radiobutton选项来更新model
    void updateModel();

    //更新model
    void updateModel(RibbonTreeShowType type);

    //应用所有的设定
    bool applys();

    //转换为xml
    bool toXml(QXmlStreamWriter *xml) const;
    bool toXml(const QString& xmlpath) const;

    //从xml中加载QList<SARibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString& xmlpath);

    //应用xml配置，可以结合customize_datas_from_xml和customize_datas_apply函数
    static bool fromXml(QXmlStreamReader *xml, SARibbonMainWindow *w, SARibbonActionsManager *mgr);

    //清除所有动作，在执行applys函数后，如果要继续调用，应该clear，否则会导致异常
    void clear();


protected:
    //把QList<SARibbonCustomizeData>进行裁剪,把一些动作合并
    void simplify();

    SARibbonPannelItem::RowProportion selectedRowProportion() const;

    QAction *selectedAction() const;
    QAction *itemToAction(QStandardItem *item) const;

    QStandardItem *selectedItem() const;


    //获取选中的ribbon tree 的level
    int selectedRibbonLevel() const;

    //根据选中的item判断
    int itemLevel(QStandardItem *item) const;

    //设置某个item被选中
    void setSelectItem(QStandardItem *item, bool ensureVisible = true);

    //判断itemn能否改动，可以改动返回true
    bool isItemCanCustomize(QStandardItem *item) const;
    bool isSelectedItemCanCustomize() const;

    //判断item是否是自定义的item
    bool isCustomizeItem(QStandardItem *item) const;
    bool isSelectedItemIsCustomize() const;

    //删除一个item
    void removeItem(QStandardItem *item);

private slots:
    void onComboBoxActionIndexCurrentIndexChanged(int index);
    void onRadioButtonGroupButtonClicked(QAbstractButton *b);
    void onPushButtonNewCategoryClicked();
    void onPushButtonNewPannelClicked();
    void onPushButtonRenameClicked();
    void onPushButtonAddClicked();
    void onPushButtonDeleteClicked();
    void onListViewSelectClicked(const QModelIndex& index);
    void onTreeViewResultClicked(const QModelIndex& index);
    void onToolButtonUpClicked();
    void onToolButtonDownClicked();
    void onItemChanged(QStandardItem *item);
    void onLineEditSearchActionTextEdited(const QString& text);
    void onPushButtonResetClicked();

private:
    void initConnection();

private:
    SARibbonCustomizeWidgetUi *ui;
    SARibbonCustomizeWidgetPrivate *m_d;
};


/**
 * @brief 转换为xml
 *
 * 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 * 需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 * @param xml QXmlStreamWriter指针
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 * 因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造，如：
 * @param cds 基于QList<SARibbonCustomizeData>生成的步骤
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 */
bool SA_RIBBON_EXPORT sa_customize_datas_to_xml(QXmlStreamWriter *xml, const QList<SARibbonCustomizeData>& cds);

/**
 * @brief 通过xml获取QList<SARibbonCustomizeData>
 * @param xml
 * @return QList<SARibbonCustomizeData>
 */
QList<SARibbonCustomizeData> SA_RIBBON_EXPORT sa_customize_datas_from_xml(QXmlStreamReader *xml, SARibbonActionsManager *mgr);

/**
 * @brief 应用QList<SARibbonCustomizeData>
 * @param cds
 * @param w SARibbonMainWindow指针
 * @return 成功应用的个数
 */
int SA_RIBBON_EXPORT sa_customize_datas_apply(const QList<SARibbonCustomizeData>& cds, SARibbonMainWindow *w);

/**
 * @brief 直接加载xml自定义ribbon配置文件用于ribbon的自定义显示
 * @param filePath xml配置文件
 * @param w 主窗体
 * @param mgr action管理器
 * @return 成功返回true
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次，如：
 * @code
 * static bool has_call = false;
 * if (!has_call) {
 *     has_call = sa_apply_customize_from_xml_file("customize.xml", this, m_actMgr);
 * }
 * @endcode
 */
bool SA_RIBBON_EXPORT sa_apply_customize_from_xml_file(const QString& filePath, SARibbonMainWindow *w, SARibbonActionsManager *mgr);

#endif // SARIBBONCUSTOMIZEWIDGET_H
