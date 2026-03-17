#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include <QStyleOption>

/**
 * \if ENGLISH
 * @brief Used to display separator lines
 * \endif
 *
 * \if CHINESE
 * @brief 用于显示分割线
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QFrame
{
    Q_OBJECT
public:
    /// Constructor for SARibbonSeparatorWidget
    explicit SARibbonSeparatorWidget(QWidget* parent = nullptr);
    /// Destructor for SARibbonSeparatorWidget
    ~SARibbonSeparatorWidget();
};

#endif  // SARIBBONSEPARATORWIDGET_H
