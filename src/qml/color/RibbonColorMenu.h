#ifndef RIBBONCOLORMENU_H
#define RIBBONCOLORMENU_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonQuickHost.h"
#include <QColor>
#include <QList>
#include <QSize>
#include <QString>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Color menu host: theme palette, custom colors and a "no color" entry
 * @details The QML counterpart of the widgets SAColorMenu. The host owns the
 *          data and the derived numbers — the standard color row, the shade
 *          rows (core SA::colorPaletteShades, so the loop order and the factors
 *          are the widgets contract), the recorded custom colors and every
 *          caption — while the leaf only arranges three RibbonColorGrid
 *          instances plus two text rows inside a Popup. Cell geometry therefore
 *          stays where WS-D2 put it: each grid derives it from core
 *          SA::colorGridCellSize, and this host never re-derives a pixel.
 *          Two deliberate departures from the widgets class:
 *          - there is no QColorDialog. Opening a dialog is a widgets/QtQuick
 *            Dialogs concern and would drag a second toolkit into this module,
 *            so activating the custom color row emits customColorRequested()
 *            and the QML side feeds the picked color back through
 *            addCustomColor(); recordCustomColor() alone reproduces the widgets
 *            recording rule (append, then shift left once full).
 *          - the outer menu geometry is QML-native (Popup + Column). QMenu's
 *            sizeHint comes from QMenuPrivate and its widget-action sizing,
 *            which is not reproducible without widgets; the swatch grids inside
 *            are pixel-identical, the surrounding chrome is not.
 *          The Popup opens at the bottom-left corner of the host item, so
 *          sizing the host to the control that owns the menu drops the popup
 *          right below it.
 * \endif
 *
 * \if CHINESE
 * @brief 颜色菜单宿主：主题色板、自定义颜色与"无颜色"项
 * @details 对应 widgets 侧的 SAColorMenu。宿主掌握数据与推导出来的数字——标准色
 *          一行、深浅行（core 的 SA::colorPaletteShades，因此循环次序与因子就是
 *          widgets 的契约）、已记录的自定义颜色以及全部文案——叶子只负责在 Popup
 *          里摆三个 RibbonColorGrid 加两行文本。单元几何因此仍留在 WS-D2 放它的
 *          地方：每个网格由 core 的 SA::colorGridCellSize 推导，本宿主一个像素也
 *          不重算。相对 widgets 版本有两处刻意的不同：
 *          - 不含 QColorDialog。弹对话框属于 widgets / QtQuick Dialogs 的职责，
 *            搬进来等于给本模块拖进第二套工具包，因此点击"自定义颜色"行发
 *            customColorRequested()，由 QML 侧把选到的颜色经 addCustomColor()
 *            回灌；单独调用 recordCustomColor() 即可复现 widgets 的记录规则
 *            （先追加，满了再整体左移）。
 *          - 菜单外框几何是 QML 原生的（Popup + Column）。QMenu 的 sizeHint 来自
 *            QMenuPrivate 与其 widget-action 尺寸协商，脱离 widgets 无法复现；
 *            内部的色块网格逐像素一致，外框不一致。
 *          Popup 在宿主项的左下角弹出，因此把宿主撑到拥有该菜单的控件大小，弹窗
 *          就正好落在它下方。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonColorMenu : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(QList< QColor > standardColors READ standardColors WRITE setStandardColors NOTIFY standardColorsChanged)
    Q_PROPERTY(QList< int > paletteFactors READ paletteFactors WRITE setPaletteFactors NOTIFY paletteFactorsChanged)
    Q_PROPERTY(QList< QColor > paletteColors READ paletteColors NOTIFY paletteColorsChanged)
    Q_PROPERTY(int paletteColumns READ paletteColumns NOTIFY paletteColorsChanged)
    Q_PROPERTY(QList< QColor > customColors READ customColors WRITE setCustomColors NOTIFY customColorsChanged)
    Q_PROPERTY(int maxCustomColorCount READ maxCustomColorCount WRITE setMaxCustomColorCount NOTIFY maxCustomColorCountChanged)
    Q_PROPERTY(QSize colorIconSize READ colorIconSize WRITE setColorIconSize NOTIFY colorIconSizeChanged)
    Q_PROPERTY(bool noneColorEnabled READ isNoneColorEnabled WRITE setNoneColorEnabled NOTIFY noneColorEnabledChanged)
    Q_PROPERTY(int noneMarkSide READ noneMarkSide WRITE setNoneMarkSide NOTIFY noneMarkSideChanged)
    Q_PROPERTY(int noneMarkSlashInset READ noneMarkSlashInset NOTIFY noneMarkSideChanged)
    Q_PROPERTY(QString themeColorsTitle READ themeColorsTitle WRITE setThemeColorsTitle NOTIFY themeColorsTitleChanged)
    Q_PROPERTY(QString customColorText READ customColorText WRITE setCustomColorText NOTIFY customColorTextChanged)
    Q_PROPERTY(QString noneColorText READ noneColorText WRITE setNoneColorText NOTIFY noneColorTextChanged)
    Q_PROPERTY(int actionRowHeight READ actionRowHeight WRITE setActionRowHeight NOTIFY actionRowHeightChanged)
    Q_PROPERTY(int paletteSpacing READ paletteSpacing WRITE setPaletteSpacing NOTIFY paletteSpacingChanged)
    Q_PROPERTY(bool menuVisible READ isMenuVisible WRITE setMenuVisible NOTIFY menuVisibleChanged)
public:
    explicit RibbonColorMenu(QQuickItem* parent = nullptr);
    ~RibbonColorMenu() override;

    // Standard color row; defaults to core SA::getStandardColorList()
    QList< QColor > standardColors() const;
    void setStandardColors(const QList< QColor >& colors);

    // One factor per shade row; defaults to core SA::defaultColorPaletteFactors()
    QList< int > paletteFactors() const;
    void setPaletteFactors(const QList< int >& factors);

    // Shade rows derived from the two above, row-major (factor outer loop)
    QList< QColor > paletteColors() const;
    int paletteColumns() const;

    QList< QColor > customColors() const;
    void setCustomColors(const QList< QColor >& colors);

    // Capacity of the custom color record (widgets mMaxCustomColorSize, 10)
    int maxCustomColorCount() const;
    void setMaxCustomColorCount(int n);

    // Swatch box of all three grids (widgets SAColorPaletteGridWidget, 10x10)
    QSize colorIconSize() const;
    void setColorIconSize(const QSize& s);

    // "No color" row (widgets enableNoneColorAction); off by default
    bool isNoneColorEnabled() const;
    void setNoneColorEnabled(bool on);

    // Side of the "no color" mark box; the slash inset core derives from the box
    // after the 1px inset the widgets icon painter uses (createNoneColorIcon)
    int noneMarkSide() const;
    void setNoneMarkSide(int side);
    // Slash inset relative to the mark box (core SA::noneColorSlashLine)
    int noneMarkSlashInset() const;

    QString themeColorsTitle() const;
    void setThemeColorsTitle(const QString& t);
    QString customColorText() const;
    void setCustomColorText(const QString& t);
    QString noneColorText() const;
    void setNoneColorText(const QString& t);

    // Height of one text row (custom color / no color)
    int actionRowHeight() const;
    void setActionRowHeight(int h);

    // Gap between the standard row and the shade rows (widgets layout spacing 8)
    int paletteSpacing() const;
    void setPaletteSpacing(int v);

    bool isMenuVisible() const;
    void setMenuVisible(bool on);

    // Popup control: the leaf renders the popup, the host tracks visibility
    Q_INVOKABLE void openMenu();
    Q_INVOKABLE void closeMenu();

    // Close the menu and report a picked color (widgets emitSelectedColor)
    Q_INVOKABLE void emitSelectedColor(const QColor& c);
    // Report the "no color" choice (widgets onNoneColorActionTriggered)
    Q_INVOKABLE void selectNoneColor();
    // Ask the QML side to pick a color, i.e. the widgets custom color action
    // minus its QColorDialog: emits customColorRequested()
    Q_INVOKABLE void requestCustomColor();

    // Append to the custom color record, shifting left once full
    Q_INVOKABLE void recordCustomColor(const QColor& c);
    // Record a color and report it: the dialog-accepted path of the widgets menu
    Q_INVOKABLE void addCustomColor(const QColor& c);
    Q_INVOKABLE void clearCustomColors();

Q_SIGNALS:
    void standardColorsChanged();
    void paletteFactorsChanged();
    void paletteColorsChanged();
    void customColorsChanged();
    void maxCustomColorCountChanged();
    void colorIconSizeChanged();
    void noneColorEnabledChanged();
    void noneMarkSideChanged();
    void themeColorsTitleChanged();
    void customColorTextChanged();
    void noneColorTextChanged();
    void actionRowHeightChanged();
    void paletteSpacingChanged();
    void menuVisibleChanged();

    /**
     * \if ENGLISH
     * @brief A color was picked anywhere in the menu
     * @param c The picked color; invalid for the "no color" entry
     * \endif
     *
     * \if CHINESE
     * @brief 菜单里任意位置选中了颜色
     * @param c 选中的颜色；"无颜色"项为无效色
     * \endif
     */
    void selectedColor(const QColor& c);

    /**
     * \if ENGLISH
     * @brief The custom color row was activated
     * @details Stands in for the widgets QColorDialog: connect a color dialog to
     *          this signal and feed the result back through addCustomColor()
     * \endif
     *
     * \if CHINESE
     * @brief "自定义颜色"行被激活
     * @details 代替 widgets 侧的 QColorDialog：把一个取色对话框接到该信号上，再把
     *          结果经 addCustomColor() 回灌
     * \endif
     */
    void customColorRequested();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;

private:
    // Republish paletteColors (shades + column count) after either input changes
    void updatePaletteColors();
    void updateNoneMarkInset();

    QList< QColor > mStandardColors;
    QList< int > mPaletteFactors;
    QList< QColor > mPaletteColors;
    QList< QColor > mCustomColors;
    int mMaxCustomColorCount = 10;  ///< widgets SAColorMenu::PrivateData::mMaxCustomColorSize
    QSize mColorIconSize { 10, 10 };  ///< widgets SAColorPaletteGridWidget::init
    bool mNoneColorEnabled = false;
    int mNoneMarkSide      = 16;
    int mNoneMarkSlashInset = 0;
    QString mThemeColorsTitle;
    QString mCustomColorText;
    QString mNoneColorText;
    int mActionRowHeight = 24;
    int mPaletteSpacing  = 8;  ///< widgets SAColorPaletteGridWidget layout spacing
    bool mMenuVisible    = false;
};

}

#endif  // RIBBONCOLORMENU_H
