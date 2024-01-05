﻿#ifndef SARIBBONCATEGORYLAYOUT_H
#define SARIBBONCATEGORYLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include <QList>
#include <QMap>
#include "SARibbonCategory.h"

class SARibbonPannel;
class SARibbonCategoryLayoutItem;
class SARibbonSeparatorWidget;

/**
 * @brief The SARibbonCategoryLayout class
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCategoryLayout)
public:
    SARibbonCategoryLayout(SARibbonCategory* parent);
    ~SARibbonCategoryLayout();

    SARibbonCategory* ribbonCategory() const;

    virtual void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;
    virtual QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;
    virtual QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;
    SARibbonCategoryLayoutItem* takePannelItem(int index);
    SARibbonCategoryLayoutItem* takePannelItem(SARibbonPannel* pannel);
    bool takePannel(SARibbonPannel* pannel);
    virtual int count() const Q_DECL_OVERRIDE;
    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
    // 追加一个pannel
    void addPannel(SARibbonPannel* pannel);
    // 插入一个pannel
    void insertPannel(int index, SARibbonPannel* pannel);
    //
    QSize categoryContentSize() const;
    // 更新尺寸
    void updateGeometryArr();

    // 执行位置调整
    void doLayout();

    // 返回所有pannels
    QList< SARibbonPannel* > pannels() const;

    // 通过obj name获取pannel
    SARibbonPannel* pannelByObjectName(const QString& objname) const;
    // 通过pannel name获取pannel
    SARibbonPannel* pannelByName(const QString& pannelname) const;
    // 通过索引找到pannel，如果超过索引范围，会返回nullptr
    SARibbonPannel* pannelByIndex(int i) const;
    // 移动pannel
    void movePannel(int from, int to);
    // pannel的数量
    int pannelCount() const;
    // pannel的索引
    int pannelIndex(SARibbonPannel* p) const;
    // 获取所有的pannel
    QList< SARibbonPannel* > pannelList() const;
    // 执行滚轮事件
    void scroll(int px);
    // 判断是否有滚动过
    bool isScrolled() const;
    // category的总宽度
    int categoryTotalWidth() const;
    // 设置Category的对齐方式
    void setCategoryAlignment(SARibbonAlignment al);
    SARibbonAlignment categoryAlignment() const;
private slots:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();
};

/**
 * @brief SARibbonCategoryLayoutItem，用于标识SARibbonCategoryLayout的item
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayoutItem : public QWidgetItem
{
public:
    SARibbonCategoryLayoutItem(SARibbonPannel* w);
    SARibbonSeparatorWidget* separatorWidget;
    // 把内部的widget转换为pannel
    SARibbonPannel* toPannelWidget();
    QRect mWillSetGeometry;           ///< pannel将要设置的Geometry
    QRect mWillSetSeparatorGeometry;  ///< pannel将要设置的Separator的Geometry
};
#endif  // SARIBBONCATEGORYLAYOUT_H
