%Module(name=PyQt6SARibbon.saribbon, call_super_init=True, keyword_arguments="Optional", use_limited_api=True)

%Import QtCore/QtCoremod.sip
%Import QtWidgets/QtWidgetsmod.sip
%Import QtGui/QtGuimod.sip

%DefaultSupertype sip.simplewrapper

%Include SARibbonTabBar.sip
%Include SARibbonButtonGroupWidget.sip
%Include SARibbonBar.sip
%Include SARibbonMainWindow.sip
%Include SARibbonCategory.sip
%Include SARibbonPanel.sip
%Include SARibbonPanelItem.sip
%Include SARibbonToolButton.sip
%Include SARibbonContextCategory.sip
%Include SARibbonGallery.sip
%Include SARibbonGalleryGroup.sip
%Include SARibbonGalleryItem.sip
