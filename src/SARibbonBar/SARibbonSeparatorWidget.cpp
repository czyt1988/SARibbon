#include "SARibbonSeparatorWidget.h"
#include <QApplication>
#include <QScreen>
#include <QStylePainter>
#include <QPainter>
#include <QDebug>

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonSeparatorWidget
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonSeparatorWidget 构造函数
 * @param parent 父窗口
 * \endif
 */
SARibbonSeparatorWidget::SARibbonSeparatorWidget(QWidget* parent) : QFrame(parent)
{
    setFrameShape(QFrame::VLine);
    setFrameShadow(QFrame::Plain);

    if (QScreen* screen = QApplication::primaryScreen()) {
        qreal dpr           = screen->physicalDotsPerInch() / screen->logicalDotsPerInch();
        int scaledLineWidth = qRound(1.0 * dpr);  // 假设基础 lineWidth 是 1
        if (scaledLineWidth < 1) {
            scaledLineWidth = 1;
        }
        setLineWidth(scaledLineWidth);
        //    qDebug() << "SARibbonSeparatorWidget:" << scaledLineWidth;
    } else {
        setLineWidth(1);
    }
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonSeparatorWidget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonSeparatorWidget 析构函数
 * \endif
 */
SARibbonSeparatorWidget::~SARibbonSeparatorWidget()
{
}
