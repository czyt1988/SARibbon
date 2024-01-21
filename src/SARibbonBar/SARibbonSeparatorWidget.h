﻿#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include <QStyleOption>

///
/// \brief 用于显示分割线
///
class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QFrame
{
    Q_OBJECT
public:
    SARibbonSeparatorWidget(QWidget* parent = nullptr);
    virtual QSize sizeHint() const override;
};

#endif  // SARIBBONSEPARATORWIDGET_H
