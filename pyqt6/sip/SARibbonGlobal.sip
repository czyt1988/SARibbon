%ModuleHeaderCode
#include <SARibbonGlobal.h>
%End

%MappedType SARibbonAlignment
{
%TypeHeaderCode
#include <SARibbonGlobal.h>
%End

%ConvertFromTypeCode
    return PyLong_FromLong((int)*sipCpp);
%End

%ConvertToTypeCode
    if (PyLong_Check(sipPy)) {
        *sipCppPtr = (SARibbonAlignment)PyLong_AsLong(sipPy);
        return 1;
    }
    return 0;
%End
};

%MappedType SARibbonTheme
{
%TypeHeaderCode
#include <SARibbonGlobal.h>
%End

%ConvertFromTypeCode
    return PyLong_FromLong((int)*sipCpp);
%End

%ConvertToTypeCode
    if (PyLong_Check(sipPy)) {
        *sipCppPtr = (SARibbonTheme)PyLong_AsLong(sipPy);
        return 1;
    }
    return 0;
%End
};

%MappedType SARibbonMainWindowStyleFlag
{
%TypeHeaderCode
#include <SARibbonGlobal.h>
%End

%ConvertFromTypeCode
    return PyLong_FromLong((int)*sipCpp);
%End

%ConvertToTypeCode
    if (PyLong_Check(sipPy)) {
        *sipCppPtr = (SARibbonMainWindowStyleFlag)PyLong_AsLong(sipPy);
        return 1;
    }
    return 0;
%End
};

typedef QFlags<SARibbonMainWindowStyleFlag> SARibbonMainWindowStyles;