#ifndef SARIBBONCONTROLBUTTON_H
#define SARIBBONCONTROLBUTTON_H
#include <QToolButton>
#include "SARibbonGlobal.h"
#include "SARibbonToolButton.h"

/**
 * @brief 用于SARibbonGallery的control button
 *
 * 为了防止外部qss的影响，单独继承一个类
 */
class SA_RIBBON_EXPORT SARibbonControlButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SARibbonControlButton(QWidget* parent = nullptr);
    ~SARibbonControlButton();
};

/**
 * @brief 用于SARibbonButtonGroupWidget的control button
 *
 * 为了防止SARibbonToolButton qss的影响，单独继承一个类
 */
class SA_RIBBON_EXPORT SARibbonControlToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SARibbonControlToolButton(QWidget* parent = nullptr);
    ~SARibbonControlToolButton();
};

#endif  // SARIBBONPANNELTOOLBUTTON_H
