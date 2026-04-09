#ifndef SARIBBONBUTTONLAYOUTSTRATEGY_H
#define SARIBBONBUTTONLAYOUTSTRATEGY_H

#include "SARibbonGlobal.h"
#include <QStyleOptionToolButton>
#include <QSize>
#include <QRect>
#include <memory>

class QFontMetrics;
class QWidget;
class SARibbonToolButton;  // 前向声明

/**
 * @brief 按钮类型枚举 (独立定义，避免循环依赖)
 */
enum class SARibbonButtonType
{
    LargeButton,
    SmallButton
};

/**
 * @brief 按钮布局矩形集合
 *
 * 封装按钮布局中各个区域的矩形位置
 */
struct SA_RIBBON_EXPORT SARibbonButtonLayoutRects
{
    QRect iconRect;       ///< 图标绘制区域
    QRect textRect;       ///< 文本绘制区域
    QRect indicatorRect;  ///< 指示器(下拉箭头)绘制区域

    void clear()
    {
        iconRect      = QRect();
        textRect      = QRect();
        indicatorRect = QRect();
    }

    bool isValid() const
    {
        return iconRect.isValid() || textRect.isValid() || indicatorRect.isValid();
    }
};

/**
 * @brief 布局上下文
 *
 * 包含布局计算所需的各种参数
 */
struct SA_RIBBON_EXPORT SARibbonButtonLayoutContext
{
    const QWidget* buttonWidget { nullptr };  ///< 目标按钮部件
    int spacing { 1 };                        ///< 元素间距
    int indicatorLength { 8 };                ///< 指示器长度
    QSize largeIconSize { 32, 32 };           ///< 大按钮图标尺寸
    QSize smallIconSize { 16, 16 };           ///< 小按钮图标尺寸
    qreal twoLineHeightFactor { 2.05 };       ///< 两行文本高度系数
    qreal oneLineHeightFactor { 1.2 };        ///< 单行文本高度系数
    qreal buttonMaximumAspectRatio { 1.4 };   ///< 按钮最大宽高比
    bool enableWordWrap { true };             ///< 是否启用自动换行
    mutable bool isTextNeedWrap { false };    ///< 标记文本是否需要换行
    int maximumWidth { QWIDGETSIZE_MAX };     ///< 按钮最大宽度

    SARibbonButtonLayoutContext() = default;
    explicit SARibbonButtonLayoutContext(const SARibbonToolButton* btn);
};

/**
 * @brief Ribbon按钮布局策略抽象基类
 *
 * 定义按钮布局计算的接口，不同的按钮类型(大按钮/小按钮)可以实现不同的布局策略
 */
class SA_RIBBON_EXPORT SARibbonButtonLayoutStrategy
{
public:
    virtual ~SARibbonButtonLayoutStrategy() = default;

    /**
     * @brief 计算绘制区域矩形
     * @param opt 样式选项
     * @param rects 输出的矩形集合
     * @param ctx 布局上下文
     */
    virtual void calculateDrawRects(const QStyleOptionToolButton& opt,
                                    SARibbonButtonLayoutRects& rects,
                                    const SARibbonButtonLayoutContext& ctx) const = 0;

    /**
     * @brief 计算按钮的size hint
     * @param opt 样式选项
     * @param ctx 布局上下文
     * @return 推荐的按钮尺寸
     */
    virtual QSize calculateSizeHint(const QStyleOptionToolButton& opt, const SARibbonButtonLayoutContext& ctx) const = 0;

    /**
     * @brief 计算文本绘制区域高度
     * @param opt 样式选项
     * @param ctx 布局上下文
     * @return 文本区域高度
     */
    virtual int calculateTextHeight(const QStyleOptionToolButton& opt, const SARibbonButtonLayoutContext& ctx) const = 0;

    /**
     * @brief 估算大按钮的最优文本宽度
     * @param buttonHeight 按钮高度
     * @param textHeight 文本区域高度
     * @param text 按钮文本
     * @param fm 字体度量
     * @param ctx 布局上下文
     * @return 估算的文本宽度
     */
    virtual int estimateTextWidth(int buttonHeight,
                                  int textHeight,
                                  const QString& text,
                                  const QFontMetrics& fm,
                                  SARibbonButtonLayoutContext& ctx) const;  // 非const因为可能修改ctx.isTextNeedWrap

protected:
    /**
     * @brief 判断是否有指示器(下拉菜单)
     */
    static bool hasIndicator(const QStyleOptionToolButton& opt);

    /**
     * @brief 调整图标尺寸以适应按钮区域
     */
    static QSize adjustIconSize(const QRect& buttonRect, const QSize& originIconSize);

    /**
     * @brief 简化文本(移除换行符)
     */
    static QString simplifiedText(const QString& str);
};

/**
 * @brief 大按钮布局策略
 */
class SA_RIBBON_EXPORT SARibbonLargeButtonLayoutStrategy : public SARibbonButtonLayoutStrategy
{
public:
    void calculateDrawRects(const QStyleOptionToolButton& opt,
                            SARibbonButtonLayoutRects& rects,
                            const SARibbonButtonLayoutContext& ctx) const override;

    QSize calculateSizeHint(const QStyleOptionToolButton& opt, const SARibbonButtonLayoutContext& ctx) const override;

    int calculateTextHeight(const QStyleOptionToolButton& opt, const SARibbonButtonLayoutContext& ctx) const override;

private:
    void calculateIconOnlyRects(const QStyleOptionToolButton& opt,
                                SARibbonButtonLayoutRects& rects,
                                const SARibbonButtonLayoutContext& ctx) const;

    void calculateTextOnlyRects(const QStyleOptionToolButton& opt,
                                SARibbonButtonLayoutRects& rects,
                                const SARibbonButtonLayoutContext& ctx) const;

    void calculateIconAndTextRects(const QStyleOptionToolButton& opt,
                                   SARibbonButtonLayoutRects& rects,
                                   const SARibbonButtonLayoutContext& ctx) const;
};

/**
 * @brief 小按钮布局策略
 */
class SA_RIBBON_EXPORT SARibbonSmallButtonLayoutStrategy : public SARibbonButtonLayoutStrategy
{
public:
    void calculateDrawRects(const QStyleOptionToolButton& opt,
                            SARibbonButtonLayoutRects& rects,
                            const SARibbonButtonLayoutContext& ctx) const override;

    QSize calculateSizeHint(const QStyleOptionToolButton& opt, const SARibbonButtonLayoutContext& ctx) const override;

    int calculateTextHeight(const QStyleOptionToolButton& opt, const SARibbonButtonLayoutContext& ctx) const override;

private:
    void calculateIconOnlyRects(const QStyleOptionToolButton& opt,
                                SARibbonButtonLayoutRects& rects,
                                const SARibbonButtonLayoutContext& ctx) const;

    void calculateTextOnlyRects(const QStyleOptionToolButton& opt,
                                SARibbonButtonLayoutRects& rects,
                                const SARibbonButtonLayoutContext& ctx) const;

    void calculateIconAndTextRects(const QStyleOptionToolButton& opt,
                                   SARibbonButtonLayoutRects& rects,
                                   const SARibbonButtonLayoutContext& ctx) const;
};

/**
 * @brief 布局策略工厂
 */
class SA_RIBBON_EXPORT SARibbonButtonLayoutStrategyFactory
{
public:
    static std::unique_ptr< SARibbonButtonLayoutStrategy > createStrategy(SARibbonButtonType type);
};

#endif  // SARIBBONBUTTONLAYOUTSTRATEGY_H
