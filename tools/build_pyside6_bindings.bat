@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM  build_pyside6_bindings.bat
REM
REM  Build PySideSARibbon (PySide6 / Shiboken6) bindings.
REM
REM  Usage:
REM    tools\build_pyside6_bindings.bat
REM    tools\build_pyside6_bindings.bat "C:\Python311" "D:\Qt\6.7.3\msvc2019_64"
REM
REM  Prerequisites:
REM    pip install PySide6 shiboken6-generator
REM    Qt6 must be installed (e.g. via Qt Online Installer)
REM ============================================================

set "PYTHON_DIR="
set "QT_DIR="
set "BUILD_DIR=build-pyside6"

REM Parse command-line arguments
if not "%~1"=="" set "PYTHON_DIR=%~1"
if not "%~2"=="" set "QT_DIR=%~2"

echo ============================================================
echo  Building PySideSARibbon (PySide6 / Shiboken6) bindings
echo ============================================================

REM --- Detect Python ---
if defined PYTHON_DIR (
    set "PYTHON_EXE=%PYTHON_DIR%\python.exe"
) else (
    set "PYTHON_EXE=python"
)

echo [1/5] Checking Python...
"%PYTHON_EXE%" --version 2>nul
if errorlevel 1 (
    echo ERROR: Python not found. Please specify Python directory.
    echo Usage: %0 "C:\Python311" "D:\Qt\6.7.3\msvc2019_64"
    exit /b 1
)

REM --- Verify PySide6 and shiboken6-generator ---
echo [2/5] Checking PySide6 and Shiboken6...
"%PYTHON_EXE%" -c "import PySide6; print('PySide6:', PySide6.__version__)" 2>nul
if errorlevel 1 (
    echo ERROR: PySide6 not installed. Run: pip install PySide6
    exit /b 1
)

"%PYTHON_EXE%" -c "import shiboken6_generator; print('shiboken6-generator OK')" 2>nul
if errorlevel 1 (
    echo ERROR: shiboken6-generator not installed. Run: pip install shiboken6-generator
    exit /b 1
)

REM --- Get site-packages path for CMAKE_PREFIX_PATH ---
for /f "delims=" %%I in ('"%PYTHON_EXE%" -c "import site; print(next(p for p in site.getsitepackages() if '"'"'site-packages'"'"' in p))"') do set "SITE_PACKAGES=%%I"

REM --- Initialize MSVC environment ---
echo [3/5] Initializing MSVC environment...
for %%Y in (2019 2022) do (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\%%Y\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\%%Y\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
        goto :msvc_done
    )
    if exist "C:\Program Files\Microsoft Visual Studio\%%Y\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\%%Y\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
        goto :msvc_done
    )
)
:msvc_done

REM --- Determine Qt6 path ---
set "QT6_CMAKE="
if defined QT_DIR (
    set "QT6_CMAKE=%QT_DIR%"
) else (
    REM Try to auto-detect from qmake on PATH
    for /f "delims=" %%I in ('qmake -query QT_INSTALL_PREFIX 2^>nul') do set "QT6_CMAKE=%%I"
)

if not defined QT6_CMAKE (
    echo ERROR: Qt6 not found. Please install Qt6 or specify Qt6 path.
    echo Usage: %0 "C:\Python311" "D:\Qt\6.7.3\msvc2019_64"
    exit /b 1
)

REM --- Configure CMake ---
echo [4/5] Configuring CMake...

set "PROJECT_ROOT=%~dp0.."
pushd "%PROJECT_ROOT%"

set "CMAKE_PREFIX_PATH=%QT6_CMAKE%;%SITE_PACKAGES%\shiboken6_generator\lib\cmake;%SITE_PACKAGES%\shiboken6\lib\cmake;%SITE_PACKAGES%\PySide6\lib\cmake"

cmake -S pyside6 -B %BUILD_DIR% -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="!CMAKE_PREFIX_PATH!"
if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    popd
    exit /b 1
)

REM --- Build ---
echo [5/5] Building...
cmake --build %BUILD_DIR% --parallel
if errorlevel 1 (
    echo ERROR: Build failed.
    popd
    exit /b 1
)

REM --- Install ---
echo.
echo Installing PySideSARibbon to site-packages...
cmake --install %BUILD_DIR%
if errorlevel 1 (
    echo WARNING: Automatic install failed. You can manually copy the built .pyd file.
) else (
    echo.
    echo ============================================================
    echo  Build and install successful!
    echo ============================================================
    echo  Verify: python -c "from PySideSARibbon import saribbon; print('OK')"
    echo  Run demo: python pyexamples\pyside6\ribbon_demo.py
)

popd
endlocal
