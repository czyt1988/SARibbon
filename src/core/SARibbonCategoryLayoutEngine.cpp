#include <SARibbonCore/SARibbonCategoryLayoutEngine.h>

// 计划 02 S6 Step B：函数体自 src/widgets/SARibbonCategoryLayout.cpp 的
// updateGeometryArr() 纯 move。机械替换表（其余一字不改）：
//   SARibbonCategoryLayoutItem* -> SARibbonAbstractCategoryItem*
//   item->isEmpty()             -> item->isHidden()
//   item->mWillSetGeometry      -> item->resultGeometry
//   item->mWillSetSeparatorGeometry -> item->resultSeparatorGeometry
//   item->separatorWidget->hide()（widget 操作）-> 输出 isSeparatorHidden（适配器执行）
//   SARibbonCategoryLayout 私有字段（mXBase/mTotalWidth/mIs*ScrollBtnShow/
//   mCachedSizeHint/mCachedMinSizeHint）-> Result 带回（引擎不直写适配器状态）
//   SA::saIsRTL()               -> input.isRTL
//   SA::saMirrorX(...)          -> 内联镜像算术（w - x - width）
//   滚动按钮标志双实现           -> scrollButtonFlags() 单一纯函数（NOTES B12-1/S6-3）
//   调试打印宏块不随迁
namespace SARibbon
{
namespace Core
{

SARibbonCategoryLayoutEngine::SARibbonCategoryLayoutEngine()
{
}

SARibbonCategoryLayoutEngine::Result SARibbonCategoryLayoutEngine::layout(QVector< SARibbonAbstractCategoryItem* > items,
                                                                         const Input& input)
{
    Result result;

    int categoryWidth = input.categoryWidth;
    QMargins mag      = input.margins;
    int height        = input.height;
    int y             = input.y;

    if (!mag.isNull()) {
        y = mag.top();
        height -= (mag.top() + mag.bottom());
        // categoryWidth不能把mag减去，减去后会导致categoryWidth不是实际的categoryWidth
        // categoryWidth -= (mag.right() + mag.left());
    }
    // total 是总宽，不是x坐标系，x才是坐标系
    // 单次遍历收集 sizeHint，避免后续重复调用
    const SARibbonCategorySizeHints& hints = input.sizeHints;
    int total = hints.totalWidth;

    // 扩展的宽度
    int expandWidth = 0;

    // 判断是否需要滚动，总长度超过宽度就需要滚动
    bool needsScrolling = (total > categoryWidth);

    result.scrollFlags = scrollButtonFlags(total, categoryWidth, input.xBase, input.isRTL);

    if (!needsScrolling) {
        // 说明total 小于 categoryWidth
        // 这个是避免一开始totalWidth > categorySize.width()，通过滚动按钮调整了m_d->mBaseX
        // 随之调整了窗体尺寸，调整后totalWidth < categorySize.width()导致category在原来位置
        // 无法显示，必须这里把mBaseX设置为0
        result.newXBase = 0;

        // 计算可扩展的宽度，canExpandingCount 已由 collectSizeHints 收集
        if (hints.canExpandingCount > 0) {
            expandWidth = (categoryWidth - total) / hints.canExpandingCount;
        } else {
            expandWidth = 0;
        }
    }
    int x = input.xBase + mag.left();
    if (!needsScrolling && (0 == expandWidth)) {
        // Alignment offset when no scrolling needed and no expanding panels
        SARibbonAlignment align = input.alignment;
        if (align == SARibbonAlignment::AlignCenter) {
            // Center alignment: panels centered within category width
            x = (categoryWidth - total) / 2;
        } else if (align == SARibbonAlignment::AlignRight) {
            // Right alignment: panels start from right edge
            x = categoryWidth - total;
        }
        // AlignLeft: x = input.xBase (default, starts from left edge)
    }
    total = 0;  // total重新计算
    // 先按照sizeHint设置所有的尺寸，使用 collectSizeHints 收集的结果避免重复调用
    for (int i = 0; i < items.size(); ++i) {
        SARibbonAbstractCategoryItem* item = items[ i ];
        if (item->isHidden()) {
            // 如果是hide就直接跳过
            // panel hide分割线也要hide（widget 操作经输出标志交适配器执行）
            item->isSeparatorHidden = true;
            item->resultGeometry          = QRect(0, 0, 0, 0);
            item->resultSeparatorGeometry = QRect(0, 0, 0, 0);
            continue;
        }
        // 使用 collectSizeHints 收集的 sizeHint，避免重复调用
        QSize panelSize     = hints.panelSizes[ i ];
        QSize SeparatorSize = hints.separatorSizes[ i ];
        if (item->expandingDirections() & Qt::Horizontal) {
            // 可扩展，就把panel扩展到最大（2.x 经 SARibbonPanel::isExpanding 判定，
            // 经契约 expandingDirections 泛化——仅 widgets 适配器实现该语义）
            panelSize.setWidth(panelSize.width() + expandWidth);
        }
        int w = panelSize.width();

        item->resultGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
        w                               = SeparatorSize.width();
        item->resultSeparatorGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
    }
    result.totalWidth  = total;
    result.sizeHint    = QSize(total, height);
    result.minSizeHint = QSize(categoryWidth, height);

    // RTL mirroring: mirror panel and separator x-coordinates
    if (input.isRTL) {
        for (SARibbonAbstractCategoryItem* item : items) {
            if (!item->isHidden()) {
                // Mirror panel geometry
                QRect panelGeo           = item->resultGeometry;
                int mirroredX            = categoryWidth - panelGeo.x() - panelGeo.width();
                item->resultGeometry = QRect(mirroredX, panelGeo.y(), panelGeo.width(), panelGeo.height());

                // Mirror separator geometry
                QRect sepGeo             = item->resultSeparatorGeometry;
                int mirroredSepX         = categoryWidth - sepGeo.x() - sepGeo.width();
                item->resultSeparatorGeometry = QRect(mirroredSepX, sepGeo.y(), sepGeo.width(), sepGeo.height());
            }
        }
    }
    return result;
}

}
}
