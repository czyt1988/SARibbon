@echo off
setlocal enabledelayedexpansion

REM ============================================================
REM  SARibbon Python Bindings - Build & Install Script
REM  Usage: tools\build_python_bindings.bat [PYTHON_DIR] [QMAKE_PATH] [--pyqt6]
REM
REM  By default, builds PyQt5 bindings.
REM  Pass --pyqt6 to build PyQt6 bindings instead.
REM ============================================================

REM --- Parse --pyqt6 flag from arguments ---
set "USE_PYQT6=0"
set "ARGS="
for %%A in (%*) do (
    if /i "%%A"=="--pyqt6" (
        set "USE_PYQT6=1"
    ) else (
        set "ARGS=!ARGS! %%A"
    )
)

REM --- Re-parse positional args (PYTHON_DIR QMAKE_PATH) from filtered list ---
set "PYTHON_DIR="
set "QMAKE_PATH="
set "ARG_IDX=0"
for %%A in (!ARGS!) do (
    set /a ARG_IDX+=1
    if !ARG_IDX! equ 1 set "PYTHON_DIR=%%A"
    if !ARG_IDX! equ 2 set "QMAKE_PATH=%%A"
)

if "!USE_PYQT6!"=="1" (
    echo === Building PyQt6 bindings ===
    set "BUILD_DIR=build-python6"
    set "PYQT_VERSION=PyQt6"
) else (
    echo === Building PyQt5 bindings ===
    set "BUILD_DIR=build-python"
    set "PYQT_VERSION=PyQt5"
)

REM --- Resolve project root (parent of tools\ directory) ---
set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%SCRIPT_DIR:~0,-1%"
cd /d "%SRC_DIR%"
for %%I in ("%SRC_DIR%\..") do set "SRC_DIR=%%~fI"

REM --- Python directory (override via arg or auto-detect) ---
if "%PYTHON_DIR%"=="" (
    where python >nul 2>nul
    if !errorlevel! equ 0 (
        for /f "delims=" %%P in ('python -c "import sys,os;print(os.path.dirname(sys.executable))"') do set "PYTHON_DIR=%%P"
    )
)
if "%PYTHON_DIR%"=="" (
    echo ERROR: Cannot locate Python. Please pass PYTHON_DIR as argument.
    echo        Example: tools\build_python_bindings.bat "C:\Python311" --pyqt6
    exit /b 1
)
echo === Python: %PYTHON_DIR% ===

REM --- qmake path (override via arg or auto-detect) ---
if "%QMAKE_PATH%"=="" (
    where qmake >nul 2>nul
    if !errorlevel! equ 0 (
        for /f "delims=" %%Q in ('where qmake') do set "QMAKE_PATH=%%Q"
    )
)
if "%QMAKE_PATH%"=="" (
    echo ERROR: Cannot locate qmake. Please pass QMAKE_PATH as argument.
    echo        Example: tools\build_python_bindings.bat "C:\Python311" "D:\Qt\6.7.3\msvc2019_64\bin\qmake.exe" --pyqt6
    exit /b 1
)
echo === qmake: %QMAKE_PATH% ===

REM --- Qt bin directory (derived from qmake path) ---
for %%I in ("%QMAKE_PATH%") do set "QT_BIN_DIR=%%~dpI"
set "QT_BIN_DIR=%QT_BIN_DIR:~0,-1%"

REM --- MSVC environment (auto-detect vcvarsall.bat) ---
set "VCVARSALL="
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
)

if "%VCVARSALL%"=="" (
    echo ERROR: Cannot find vcvarsall.bat. Please open a "x64 Native Tools Command Prompt" and re-run.
    exit /b 1
)

echo === Initializing MSVC environment ===
call "%VCVARSALL%" x64
if errorlevel 1 (
    echo ERROR: vcvarsall.bat failed
    exit /b 1
)

echo === Checking cl.exe ===
where cl >nul 2>nul
if errorlevel 1 (
    echo ERROR: cl.exe not found after vcvarsall
    exit /b 1
)

echo === Setting Qt PATH ===
set "PATH=%QT_BIN_DIR%;%PATH%"

REM --- Site-packages directory (derive from Python dir) ---
set "SITE_PACKAGES=%PYTHON_DIR%\Lib\site-packages"
if not exist "%SITE_PACKAGES%" (
    echo ERROR: site-packages not found at %SITE_PACKAGES%
    exit /b 1
)
set "INSTALL_DIR=%SITE_PACKAGES%\PyQtSARibbon"

echo === Cleaning old build ===
cd /d "%SRC_DIR%"
if exist "!BUILD_DIR!" rmdir /s /q "!BUILD_DIR!"
if exist "%INSTALL_DIR%" rmdir /s /q "%INSTALL_DIR%"

REM --- Swap pyproject.toml for PyQt6 if needed ---
if "!USE_PYQT6!"=="1" (
    if not exist "pyproject-pyqt6.toml" (
        echo ERROR: pyproject-pyqt6.toml not found in project root
        exit /b 1
    )
    echo === Swapping pyproject.toml with pyproject-pyqt6.toml ===
    copy /y "pyproject.toml" "pyproject.toml.bak" >nul
    copy /y "pyproject-pyqt6.toml" "pyproject.toml" >nul
)

echo === Starting sip-build ===
"%PYTHON_DIR%\Scripts\sip-build.exe" --build-dir "!BUILD_DIR!" --qmake "%QMAKE_PATH%" --verbose
set "BUILD_RESULT=!errorlevel!"

REM --- Restore original pyproject.toml if swapped ---
if "!USE_PYQT6!"=="1" (
    echo === Restoring original pyproject.toml ===
    copy /y "pyproject.toml.bak" "pyproject.toml" >nul
    del /q "pyproject.toml.bak" >nul
)

if !BUILD_RESULT! neq 0 (
    echo ERROR: sip-build failed
    exit /b 1
)

echo === Installing to %INSTALL_DIR% ===
mkdir "%INSTALL_DIR%"
copy /y "!BUILD_DIR!\py.typed" "%INSTALL_DIR%\py.typed" >nul
copy /y "!BUILD_DIR!\sip.pyi" "%INSTALL_DIR%\sip.pyi" >nul
copy /y "!BUILD_DIR!\saribbon\saribbon.pyd" "%INSTALL_DIR%\saribbon.pyd" >nul
copy /y "!BUILD_DIR!\saribbon\saribbon.pyi" "%INSTALL_DIR%\saribbon.pyi" >nul

REM --- Write __init__.py with PyQt DLL path fix ---
if "!USE_PYQT6!"=="1" (
    > "%INSTALL_DIR%\__init__.py" echo import PyQt6.QtCore  # noqa: F401
) else (
    > "%INSTALL_DIR%\__init__.py" echo import PyQt5.QtCore  # noqa: F401
)

echo === Build and install complete ===
"%PYTHON_DIR%\python.exe" -c "from PyQtSARibbon import saribbon; print('Import OK:', saribbon.SARibbonMainWindow)"
