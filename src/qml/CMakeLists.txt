# SARibbonQml placeholder: the real module is implemented in plan 04
# (imperative registration, no qt_add_qml_module). No target is created here
# because an add_library without sources is not buildable, and a dummy target
# would add an install artifact that 2.9.5 never had (plan 01 S6.5 decision).
message(STATUS "SARibbonQml: placeholder, implemented in plan 04")
