/*
 * MIT License
 *
 * Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if (defined(WIN32) || defined(_WIN32))

#include "sysapiloader_p.h"
#include "winverhelper_p.h"
#include "framelesshelper_windows.h"

FRAMELESSHELPER_STRING_CONSTANT(user32)
FRAMELESSHELPER_STRING_CONSTANT(uxtheme)
FRAMELESSHELPER_STRING_CONSTANT(win32u)

FRAMELESSHELPER_STRING_CONSTANT(GetWindowCompositionAttribute)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowCompositionAttribute)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowThemeAttribute)
FRAMELESSHELPER_STRING_CONSTANT(NtUserEnableChildWindowDpiMessage)
FRAMELESSHELPER_STRING_CONSTANT(EnableChildWindowDpiMessage)
FRAMELESSHELPER_STRING_CONSTANT(EnablePerMonitorDialogScaling)
FRAMELESSHELPER_STRING_CONSTANT(GetDpiForWindow)
FRAMELESSHELPER_STRING_CONSTANT(GetWindowDPI)
FRAMELESSHELPER_STRING_CONSTANT(GetSystemMetricsForDpi)
FRAMELESSHELPER_STRING_CONSTANT(GetDpiMetrics)
FRAMELESSHELPER_STRING_CONSTANT(AdjustWindowRectExForDpi)

EXTERN_C BOOL WINAPI
_GetWindowCompositionAttribute(const HWND hWnd, PWINDOWCOMPOSITIONATTRIBDATA pvData)
{
    Q_ASSERT(hWnd);
    Q_ASSERT(pvData);
    if (!hWnd || !pvData) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (!API_USER_AVAILABLE(GetWindowCompositionAttribute)) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return API_CALL_FUNCTION4(user32, GetWindowCompositionAttribute, hWnd, pvData);
}

EXTERN_C BOOL WINAPI
_SetWindowCompositionAttribute(const HWND hWnd, PWINDOWCOMPOSITIONATTRIBDATA pvData)
{
    Q_ASSERT(hWnd);
    Q_ASSERT(pvData);
    if (!hWnd || !pvData) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (!API_USER_AVAILABLE(SetWindowCompositionAttribute)) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return API_CALL_FUNCTION4(user32, SetWindowCompositionAttribute, hWnd, pvData);
}

EXTERN_C HRESULT WINAPI
_SetWindowThemeAttribute(const HWND hWnd, const _WINDOWTHEMEATTRIBUTETYPE attrib,
                         PVOID pvData, const DWORD cbData
)
{
    Q_ASSERT(hWnd);
    Q_ASSERT(pvData);
    Q_ASSERT(cbData != 0);
    if (!hWnd || !pvData || (cbData == 0)) {
        return E_INVALIDARG;
    }
    if (!API_THEME_AVAILABLE(SetWindowThemeAttribute)) {
        return E_NOTIMPL;
    }
    return API_CALL_FUNCTION4(uxtheme, SetWindowThemeAttribute, hWnd, attrib, pvData, cbData);
}

EXTERN_C HRESULT WINAPI
_SetWindowThemeNonClientAttributes(const HWND hWnd, const DWORD dwMask, const DWORD dwAttributes)
{
    Q_ASSERT(hWnd);
    if (!hWnd) {
        return E_INVALIDARG;
    }
    WTA_OPTIONS2 options = {};
    options.dwFlags = dwAttributes;
    options.dwMask = dwMask;
    return _SetWindowThemeAttribute(hWnd, _WTA_NONCLIENT, &options, sizeof(options));
}

EXTERN_C BOOL WINAPI
_ShouldAppsUseDarkMode(VOID)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    static const auto pShouldAppsUseDarkMode
        = reinterpret_cast<ShouldAppsUseDarkModePtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(132)));
    if (!pShouldAppsUseDarkMode) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pShouldAppsUseDarkMode();
}

EXTERN_C BOOL WINAPI
_AllowDarkModeForWindow(const HWND hWnd, const BOOL bAllow)
{
    Q_ASSERT(hWnd);
    if (!hWnd) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    static const auto pAllowDarkModeForWindow
        = reinterpret_cast<AllowDarkModeForWindowPtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(133)));
    if (!pAllowDarkModeForWindow) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pAllowDarkModeForWindow(hWnd, bAllow);
}

EXTERN_C BOOL WINAPI
_AllowDarkModeForApp(const BOOL bAllow)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    static const auto pAllowDarkModeForApp
        = reinterpret_cast<AllowDarkModeForAppPtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(135)));
    if (!pAllowDarkModeForApp) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pAllowDarkModeForApp(bAllow);
}

EXTERN_C VOID WINAPI
_FlushMenuThemes(VOID)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return;
    }
    static const auto pFlushMenuThemes
        = reinterpret_cast<FlushMenuThemesPtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(136)));
    if (!pFlushMenuThemes) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return;
    }
    pFlushMenuThemes();
}

EXTERN_C VOID WINAPI
_RefreshImmersiveColorPolicyState(VOID)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return;
    }
    static const auto pRefreshImmersiveColorPolicyState
        = reinterpret_cast<RefreshImmersiveColorPolicyStatePtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(104)));
    if (!pRefreshImmersiveColorPolicyState) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return;
    }
    pRefreshImmersiveColorPolicyState();
}

EXTERN_C BOOL WINAPI
_IsDarkModeAllowedForWindow(const HWND hWnd)
{
    Q_ASSERT(hWnd);
    if (!hWnd) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    static const auto pIsDarkModeAllowedForWindow
        = reinterpret_cast<IsDarkModeAllowedForWindowPtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(137)));
    if (!pIsDarkModeAllowedForWindow) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pIsDarkModeAllowedForWindow(hWnd);
}

EXTERN_C BOOL WINAPI
_GetIsImmersiveColorUsingHighContrast(const IMMERSIVE_HC_CACHE_MODE mode)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    static const auto pGetIsImmersiveColorUsingHighContrast
        = reinterpret_cast<GetIsImmersiveColorUsingHighContrastPtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(106)));
    if (!pGetIsImmersiveColorUsingHighContrast) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pGetIsImmersiveColorUsingHighContrast(mode);
}

EXTERN_C HTHEME WINAPI
_OpenNcThemeData(const HWND hWnd, LPCWSTR pszClassList)
{
    Q_ASSERT(hWnd);
    Q_ASSERT(pszClassList);
    if (!hWnd || !pszClassList) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
    }
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return nullptr;
    }
    static const auto pOpenNcThemeData
        = reinterpret_cast<OpenNcThemeDataPtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(49)));
    if (!pOpenNcThemeData) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return nullptr;
    }
    return pOpenNcThemeData(hWnd, pszClassList);
}

EXTERN_C BOOL WINAPI
_ShouldSystemUseDarkMode(VOID)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    static const auto pShouldSystemUseDarkMode
        = reinterpret_cast<ShouldSystemUseDarkModePtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(138)));
    if (!pShouldSystemUseDarkMode) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pShouldSystemUseDarkMode();
}

EXTERN_C PREFERRED_APP_MODE WINAPI
_SetPreferredAppMode(const PREFERRED_APP_MODE mode)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return PAM_MAX;
    }
    static const auto pSetPreferredAppMode
        = reinterpret_cast<SetPreferredAppModePtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(135)));
    if (!pSetPreferredAppMode) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return PAM_MAX;
    }
    return pSetPreferredAppMode(mode);
}

EXTERN_C BOOL WINAPI
_IsDarkModeAllowedForApp(VOID)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    static const auto pIsDarkModeAllowedForApp
        = reinterpret_cast<IsDarkModeAllowedForAppPtr>(
            SysApiLoader::resolve(kuxtheme, MAKEINTRESOURCEA(139)));
    if (!pIsDarkModeAllowedForApp) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pIsDarkModeAllowedForApp();
}

EXTERN_C BOOL WINAPI
_EnableChildWindowDpiMessage2(const HWND hWnd, const BOOL fEnable)
{
    Q_ASSERT(hWnd);
    if (!hWnd) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    using EnableChildWindowDpiMessagePtr = decltype(&::_EnableChildWindowDpiMessage);
    static const auto pEnableChildWindowDpiMessage = []() -> EnableChildWindowDpiMessagePtr {
        // EnableChildWindowDpiMessage() was moved to "win32u.dll" and renamed to
        // "NtUserEnableChildWindowDpiMessage" since Win10 1607.
        if (const auto pFunc = reinterpret_cast<EnableChildWindowDpiMessagePtr>(
            SysApiLoader::resolve(kwin32u, kNtUserEnableChildWindowDpiMessage))) {
            return pFunc;
        }
        // EnableChildWindowDpiMessage() was once a public API, so we can load it by name,
        // but it got removed in Win10 1607, so we can't link to it directly.
        if (const auto pFunc = reinterpret_cast<EnableChildWindowDpiMessagePtr>(
            SysApiLoader::resolve(kuser32, kEnableChildWindowDpiMessage))) {
            return pFunc;
        }
        // EnableChildWindowDpiMessage() was made private since Win10 1607.
        if (const auto pFunc = reinterpret_cast<EnableChildWindowDpiMessagePtr>(
            SysApiLoader::resolve(kuser32, MAKEINTRESOURCEA(2704)))) {
            return pFunc;
        }
        return nullptr;
    }();
    if (!pEnableChildWindowDpiMessage) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pEnableChildWindowDpiMessage(hWnd, fEnable);
}

EXTERN_C BOOL WINAPI
_EnablePerMonitorDialogScaling2(VOID)
{
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    using EnablePerMonitorDialogScalingPtr = decltype(&::_EnablePerMonitorDialogScaling);
    static const auto pEnablePerMonitorDialogScaling = []() -> EnablePerMonitorDialogScalingPtr {
        // EnablePerMonitorDialogScaling() was once a public API, so we can load it by name,
        // but it got removed in Win10 1607, so we can't link to it directly.
        if (const auto pFunc = reinterpret_cast<EnablePerMonitorDialogScalingPtr>(
            SysApiLoader::resolve(kuser32, kEnablePerMonitorDialogScaling))) {
            return pFunc;
        }
        // EnablePerMonitorDialogScaling() was made private since Win10 1607.
        if (const auto pFunc = reinterpret_cast<EnablePerMonitorDialogScalingPtr>(
            SysApiLoader::resolve(kuser32, MAKEINTRESOURCEA(2577)))) {
            return pFunc;
        }
        return nullptr;
    }();
    if (!pEnablePerMonitorDialogScaling) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pEnablePerMonitorDialogScaling();
}

EXTERN_C UINT WINAPI
_GetDpiForWindow2(const HWND hWnd)
{
    Q_ASSERT(hWnd);
    if (!hWnd) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
    using GetDpiForWindowPtr = decltype(&::_GetDpiForWindow);
    static const auto pGetDpiForWindow = []() -> GetDpiForWindowPtr {
        // GetDpiForWindow() was made public since Win10 1607.
        if (const auto pFunc = reinterpret_cast<GetDpiForWindowPtr>(
            SysApiLoader::resolve(kuser32, kGetDpiForWindow))) {
            return pFunc;
        }
        // GetDpiForWindow() was named "GetWindowDPI" before made public.
        if (const auto pFunc = reinterpret_cast<GetDpiForWindowPtr>(
            SysApiLoader::resolve(kuser32, kGetWindowDPI))) {
            return pFunc;
        }
        // GetWindowDPI() was made private since Win10 1607.
        if (const auto pFunc = reinterpret_cast<GetDpiForWindowPtr>(
            SysApiLoader::resolve(kuser32, MAKEINTRESOURCEA(2707)))) {
            return pFunc;
        }
        return nullptr;
    }();
    if (!pGetDpiForWindow) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
    return pGetDpiForWindow(hWnd);
}

EXTERN_C int WINAPI
_GetSystemMetricsForDpi2(const int nIndex, const UINT dpi)
{
    Q_ASSERT(nIndex >= 0);
    Q_ASSERT(dpi != 0);
    if ((nIndex < 0) || (dpi == 0)) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
    using GetSystemMetricsForDpiPtr = decltype(&::_GetSystemMetricsForDpi);
    static const auto pGetSystemMetricsForDpi = []() -> GetSystemMetricsForDpiPtr {
        // GetSystemMetricsForDpi() was made public since Win10 1607.
        if (const auto pFunc = reinterpret_cast<GetSystemMetricsForDpiPtr>(
            SysApiLoader::resolve(kuser32, kGetSystemMetricsForDpi))) {
            return pFunc;
        }
        // GetSystemMetricsForDpi() was named "GetDpiMetrics" before made public,
        // that is, when in Win10 1507 & 1511.
        if (const auto pFunc = reinterpret_cast<GetSystemMetricsForDpiPtr>(
            SysApiLoader::resolve(kuser32, kGetDpiMetrics))) {
            return pFunc;
        }
        return nullptr;
    }();
    if (!pGetSystemMetricsForDpi) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
    return pGetSystemMetricsForDpi(nIndex, dpi);
}

EXTERN_C BOOL WINAPI
_AdjustWindowRectExForDpi2(LPRECT lpRect, const DWORD dwStyle,
    const BOOL bMenu, const DWORD dwExStyle, const UINT dpi)
{
    Q_ASSERT(lpRect);
    Q_ASSERT(dpi != 0);
    if (!lpRect || (dpi == 0)) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    FRAMELESSHELPER_USE_NAMESPACE
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    using AdjustWindowRectExForDpiPtr = decltype(&::_AdjustWindowRectExForDpi);
    static const auto pAdjustWindowRectExForDpi = []() -> AdjustWindowRectExForDpiPtr {
        // AdjustWindowRectExForDpi() was made public since Win10 1607.
        if (const auto pFunc = reinterpret_cast<AdjustWindowRectExForDpiPtr>(
            SysApiLoader::resolve(kuser32, kAdjustWindowRectExForDpi))) {
            return pFunc;
        }
        // AdjustWindowRectExForDpi() was made private in Win10 1507 & 1511.
        if (const auto pFunc = reinterpret_cast<AdjustWindowRectExForDpiPtr>(
            SysApiLoader::resolve(kuser32, MAKEINTRESOURCEA(2580)))) {
            return pFunc;
        }
        return nullptr;
    }();
    if (!pAdjustWindowRectExForDpi) {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return pAdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);
}

#endif // Q_OS_WINDOWS
