// Consumer TU using the core-only include path (<SARibbonCore/...>)
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonMetrics.h>

int main()
{
    SARibbon::Core::SARibbonMetrics m;
    m.setStylePixelMetrics(0, 0, 0, 0);
    SARibbonTheme theme = SARibbonTheme::RibbonThemeDark;
    return (theme == SARibbonTheme::RibbonThemeDark) ? 0 : 1;
}
