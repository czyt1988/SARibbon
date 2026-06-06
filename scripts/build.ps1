# SARibbon Build Script — Auto-detect Qt, VS, CMake
# Usage: .\scripts\build.ps1 [action] [options]
#   Actions: configure | build | install | clean | rebuild | full | help
#   Options: -QtPath <path>  -VSVersion <2019|2022>  -Config <Release|Debug>
#            -Examples <ON|OFF>  -Tests <ON|OFF>  -StaticLibs <ON|OFF>
#            -Frameless <ON|OFF>  -SnapLayout <ON|OFF>
# Examples:
#   .\scripts\build.ps1              # full: configure + build + install (Release)
#   .\scripts\build.ps1 build        # incremental build only
#   .\scripts\build.ps1 rebuild      # clean + configure + build + install
#   .\scripts\build.ps1 configure -Examples OFF -StaticLibs ON  # static lib only
#   .\scripts\build.ps1 install      # install after build

param(
    [Parameter(Position=0)]
    [ValidateSet('configure','build','install','clean','rebuild','full','help')]
    [string]$Action = 'full',

    [string]$QtPath = '',
    [string]$VSVersion = '',
    [ValidateSet('Release','Debug','RelWithDebInfo','MinSizeRel')]
    [string]$Config = 'Release',

    [ValidateSet('ON','OFF')]
    [string]$Examples = 'ON',
    [ValidateSet('ON','OFF')]
    [string]$Tests = 'OFF',
    [ValidateSet('ON','OFF')]
    [string]$StaticLibs = 'OFF',
    [ValidateSet('ON','OFF')]
    [string]$Frameless = 'OFF',
    [ValidateSet('ON','OFF')]
    [string]$SnapLayout = 'OFF'
)

$ErrorActionPreference = 'Stop'

# ── Project root (always resolve relative to script location) ────────────────
$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot '..')

# ============================================================
# Auto-detect Qt installation
# ============================================================
function Find-Qt {
    param([string]$HintPath)

    if ($HintPath -and (Test-Path $HintPath)) {
        $cmakeDir = Join-Path $HintPath 'lib/cmake'
        if (Test-Path $cmakeDir) {
            Write-Host "[OK] Qt path (user-specified): $HintPath" -ForegroundColor Green
            return $HintPath
        }
        Write-Host "[WARN] User-specified Qt path exists but has no lib/cmake — will search anyway" -ForegroundColor Yellow
    }

    $searchRoots = @(
        'D:\Qt',
        'C:\Qt',
        "${env:USERPROFILE}\Qt",
        "${env:ProgramFiles}\Qt",
        "${env:ProgramFiles(x86)}\Qt"
    )

    foreach ($root in $searchRoots) {
        if (-not (Test-Path $root)) { continue }

        $qtDirs = Get-ChildItem -Path $root -Directory -Recurse -Depth 3 -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match '^msvc2019_64$|^msvc2022_64$' } |
            Sort-Object -Property { $_.Parent.Name } -Descending

        foreach ($dir in $qtDirs) {
            $candidate = $dir.FullName
            $cmakeDir = Join-Path $candidate 'lib/cmake'
            if (Test-Path $cmakeDir) {
                $qt6VersionFile = Join-Path $cmakeDir 'Qt6/Qt6Config.cmake'
                $qt5VersionFile = Join-Path $cmakeDir 'Qt5/Qt5Config.cmake'
                $qtVer = ''
                if (Test-Path $qt6VersionFile) { $qtVer = 'Qt6' }
                elseif (Test-Path $qt5VersionFile) { $qtVer = 'Qt5' }

                Write-Host "[OK] Qt path (auto-detected): $candidate ($qtVer)" -ForegroundColor Green
                return $candidate
            }
        }
    }

    Write-Host "[ERROR] Cannot find Qt installation!" -ForegroundColor Red
    Write-Host "  Searched: $($searchRoots -join ', ')" -ForegroundColor Red
    Write-Host "  Please install Qt or specify -QtPath explicitly" -ForegroundColor Red
    exit 1
}

# ============================================================
# Auto-detect Visual Studio & CMake generator
# ============================================================
function Find-VSGenerator {
    param([string]$HintVersion)

    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

    $vsNum = $HintVersion
    if (-not $vsNum) {
        if (Test-Path $vsWhere) {
            $installs = & $vsWhere -all -property installationVersion -format value 2>$null | Sort-Object -Descending | Select-Object -First 1
            if ($installs) {
                $major = $installs.Split('.')[0]
                switch ($major) {
                    '16' { $vsNum = '2019' }
                    '17' { $vsNum = '2022' }
                    default { $vsNum = '2019' }
                }
            }
        }
        if (-not $vsNum) { $vsNum = '2019' }
    }

    switch ($vsNum) {
        '2019' {
            $genName = 'Visual Studio 16 2019'
            $vsPath = 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community'
            if (-not (Test-Path $vsPath)) {
                $vsPath = 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional'
            }
            if (-not (Test-Path $vsPath)) {
                $vsPath = 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise'
            }
        }
        '2022' {
            $genName = 'Visual Studio 17 2022'
            $vsPath = 'C:\Program Files\Microsoft Visual Studio\2022\Community'
            if (-not (Test-Path $vsPath)) {
                $vsPath = 'C:\Program Files\Microsoft Visual Studio\2022\Professional'
            }
            if (-not (Test-Path $vsPath)) {
                $vsPath = 'C:\Program Files\Microsoft Visual Studio\2022\Enterprise'
            }
        }
        default {
            Write-Host "[ERROR] Unsupported VS version: $vsNum" -ForegroundColor Red
            exit 1
        }
    }

    if (Test-Path $vsPath) {
        Write-Host "[OK] Visual Studio $vsNum found at: $vsPath" -ForegroundColor Green
    } else {
        Write-Host "[WARN] VS $vsNum not found at expected path, CMake may still find it" -ForegroundColor Yellow
    }

    Write-Host "[OK] CMake generator: $genName, architecture: x64" -ForegroundColor Green
    return @{ Generator = $genName; VSVersion = $vsNum }
}

# ============================================================
# Auto-detect CMake executable
# ============================================================
function Find-CMake {
    $pathCmake = Get-Command cmake -ErrorAction SilentlyContinue
    if ($pathCmake) {
        Write-Host "[OK] CMake (PATH): $($pathCmake.Source)" -ForegroundColor Green
        return 'cmake'
    }

    $vsSearchPaths = @(
        'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe',
        'C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe',
        'C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe',
        'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe',
        'C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe',
        'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
    )

    foreach ($p in $vsSearchPaths) {
        if (Test-Path $p) {
            Write-Host "[OK] CMake (VS-embedded): $p" -ForegroundColor Green
            return $p
        }
    }

    $standalonePaths = @(
        'C:\Program Files\CMake\bin\cmake.exe',
        'C:\Program Files (x86)\CMake\bin\cmake.exe'
    )
    foreach ($p in $standalonePaths) {
        if (Test-Path $p) {
            Write-Host "[OK] CMake (standalone): $p" -ForegroundColor Green
            return $p
        }
    }

    Write-Host "[ERROR] Cannot find CMake!" -ForegroundColor Red
    Write-Host "  Install CMake or add it to PATH" -ForegroundColor Red
    exit 1
}

# ============================================================
# Help
# ============================================================
if ($Action -eq 'help') {
    Write-Host @'
SARibbon Build Script
=====================

Usage: .\scripts\build.ps1 [action] [options]

Actions:
  configure  — Run cmake configure only
  build      — Run cmake build only (incremental)
  install    — Run cmake install (after build)
  clean      — Remove build directory
  rebuild    — clean + configure + build + install
  full       — configure + build + install (default)
  help       — Show this message

Options:
  -QtPath <path>         Qt installation path (auto-detect if omitted)
  -VSVersion <2019|2022> Visual Studio version (auto-detect if omitted)
  -Config <Release|Debug|RelWithDebInfo|MinSizeRel>  Build configuration
  -Examples <ON|OFF>     Build examples (default: ON)
  -Tests <ON|OFF>        Build tests (default: OFF)
  -StaticLibs <ON|OFF>   Build static library (default: OFF = shared DLL)
  -Frameless <ON|OFF>    Enable QWindowKit frameless (default: OFF)
  -SnapLayout <ON|OFF>   Enable Windows 11 Snap Layout, requires Frameless=ON (default: OFF)

Examples:
  .\scripts\build.ps1                                  # Full build (configure + build + install Release)
  .\scripts\build.ps1 build                            # Incremental build only
  .\scripts\build.ps1 rebuild                          # Clean rebuild
  .\scripts\build.ps1 configure -Examples OFF          # Configure only, no examples
  .\scripts\build.ps1 configure -StaticLibs ON         # Configure static library
  .\scripts\build.ps1 full -Frameless ON -SnapLayout ON # Full build with frameless + snap layout
  .\scripts\build.ps1 full -QtPath "D:\Qt\Qt5.15.16\5.15.16\msvc2019_64"

Auto-detection:
  Qt:     Searches D:\Qt, C:\Qt, ~\Qt, Program Files\Qt for msvc*_64 dirs
  VS:     Uses vswhere.exe to find installed Visual Studio
  CMake:  Checks PATH, then VS-embedded, then standalone locations

Important:
  - MUST use Visual Studio generator (not Ninja) — vcvars64.bat does not
    work in PowerShell, causing MSVC environment injection failure
  - Qt version must match VS compiler version (e.g. Qt msvc2019 with VS2019)
  - Install output goes to: bin_qt<version>_<compiler>_x64/ (when SARIBBON_INSTALL_IN_CURRENT_DIR=ON)
'@
    exit 0
}

# ============================================================
# Detect environment
# ============================================================
Write-Host "`n=== SARibbon Build Environment Detection ===" -ForegroundColor Cyan

$qtDir = Find-Qt -HintPath $QtPath
$vsInfo = Find-VSGenerator -HintVersion $VSVersion
$cmakeExe = Find-CMake

$qt6Cmake = Join-Path $qtDir 'lib/cmake/Qt6/Qt6Config.cmake'
$isQt6 = Test-Path $qt6Cmake
$qtMajor = if ($isQt6) { '6' } else { '5' }

Write-Host "`n=== Build Configuration ===" -ForegroundColor Cyan
Write-Host "  Qt:          $qtDir (Qt$qtMajor)"
Write-Host "  VS:          $($vsInfo.VSVersion)"
Write-Host "  Generator:   $($vsInfo.Generator)"
Write-Host "  CMake:       $cmakeExe"
Write-Host "  Config:      $Config"
Write-Host "  Examples:    $Examples"
Write-Host "  Tests:       $Tests"
Write-Host "  StaticLibs:  $StaticLibs"
Write-Host "  Frameless:   $Frameless"
Write-Host "  SnapLayout:  $SnapLayout"
Write-Host ""

# ============================================================
# Execute actions
# ============================================================
$buildDir = Join-Path $ProjectRoot 'build'

# --- CLEAN ---
if ($Action -eq 'clean' -or $Action -eq 'rebuild') {
    if (Test-Path $buildDir) {
        $binDir = Join-Path $buildDir 'bin'
        if (Test-Path $binDir) {
            $runningExes = Get-ChildItem -Path $binDir -Filter '*.exe' -Recurse -ErrorAction SilentlyContinue |
                Where-Object { try { [System.IO.File]::Open($_.FullName, 'Open', 'ReadWrite').Close(); $false } catch { $true } }
            if ($runningExes) {
                Write-Host "[WARN] Locked executables detected (processes still running):" -ForegroundColor Yellow
                $runningExes | ForEach-Object { Write-Host "  $($_.Name)" -ForegroundColor Yellow }
                Write-Host "  Kill them first, or skip clean" -ForegroundColor Yellow
                $cont = Read-Host "Continue with clean? (Y/N)"
                if ($cont -ne 'Y') {
                    Write-Host "Clean skipped." -ForegroundColor Yellow
                    if ($Action -eq 'clean') { exit 0 }
                }
            }
        }
        Write-Host "[CLEAN] Removing build directory..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $buildDir
        Write-Host "[OK] Build directory removed" -ForegroundColor Green
    } else {
        Write-Host "[OK] No build directory to clean" -ForegroundColor Green
    }
    if ($Action -eq 'clean') { exit 0 }
}

# --- CONFIGURE ---
if ($Action -eq 'configure' -or $Action -eq 'rebuild' -or $Action -eq 'full') {
    # Check if existing build dir uses a different generator — auto-clean to avoid CMake errors
    $cacheFile = Join-Path $buildDir 'CMakeCache.txt'
    if (Test-Path $cacheFile) {
        $cachedGen = Select-String -Path $cacheFile -Pattern 'CMAKE_GENERATOR:INTERNAL=(.*)' | Select-Object -First 1
        if ($cachedGen -and $cachedGen.Matches[0].Groups[1].Value -ne $vsInfo.Generator) {
            Write-Host "[WARN] Generator mismatch: cached='$($cachedGen.Matches[0].Groups[1].Value)' vs target='$($vsInfo.Generator)'" -ForegroundColor Yellow
            Write-Host "[CLEAN] Removing stale build directory for generator reconfigure..." -ForegroundColor Yellow
            Remove-Item -Recurse -Force $buildDir
            Write-Host "[OK] Build directory removed" -ForegroundColor Green
        }
    }

    Write-Host "[CONFIGURE] Running cmake configure..." -ForegroundColor Cyan

    $cmakeArgs = @(
        '-S', $ProjectRoot,
        '-B', $buildDir,
        '-G', $vsInfo.Generator,
        '-A', 'x64',
        "-DCMAKE_PREFIX_PATH=$qtDir",
        "-DSARIBBON_BUILD_EXAMPLES=$Examples",
        "-DSARIBBON_BUILD_STATIC_LIBS=$StaticLibs",
        "-DSARIBBON_USE_FRAMELESS_LIB=$Frameless",
        "-DSARIBBON_ENABLE_SNAPLAYOUT=$SnapLayout",
        "-DBUILD_TESTS=$Tests"
    )

    Write-Host "  Command: $cmakeExe $($cmakeArgs -join ' ')" -ForegroundColor Gray

    & $cmakeExe $cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[ERROR] CMake configure failed (exit code $LASTEXITCODE)" -ForegroundColor Red
        Write-Host "  Common causes:" -ForegroundColor Red
        Write-Host "  - Qt path wrong: check -QtPath, current=$qtDir" -ForegroundColor Red
        Write-Host "  - Qt compiler mismatch: Qt msvc version must match VS version" -ForegroundColor Red
        Write-Host "  - Missing Qt modules: need Core, Gui, Widgets, Svg" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    Write-Host "[OK] CMake configure succeeded" -ForegroundColor Green

    if ($Action -eq 'configure') { exit 0 }
}

# --- BUILD ---
if ($Action -eq 'build' -or $Action -eq 'rebuild' -or $Action -eq 'full') {
    Write-Host "[BUILD] Running cmake build ($Config)..." -ForegroundColor Cyan

    & $cmakeExe --build $buildDir --config $Config
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[ERROR] Build failed (exit code $LASTEXITCODE)" -ForegroundColor Red
        Write-Host "  Try: .\scripts\build.ps1 rebuild (clean + configure + build)" -ForegroundColor Red
        exit $LASTEXITCODE
    }

    Write-Host "[OK] Build succeeded" -ForegroundColor Green

    $binOutDir = Join-Path $buildDir "bin/$Config"
    if (Test-Path $binOutDir) {
        Write-Host "`n=== Build Artifacts ===" -ForegroundColor Cyan
        Get-ChildItem -Path $binOutDir -Filter '*.dll' -ErrorAction SilentlyContinue |
            ForEach-Object { Write-Host "  DLL: $($_.Name)" }
        Get-ChildItem -Path $binOutDir -Filter '*.exe' -ErrorAction SilentlyContinue |
            ForEach-Object { Write-Host "  EXE: $($_.Name)" }
    }
}

# --- INSTALL ---
if ($Action -eq 'install' -or $Action -eq 'rebuild' -or $Action -eq 'full') {
    Write-Host "[INSTALL] Running cmake install ($Config)..." -ForegroundColor Cyan

    & $cmakeExe --install $buildDir --config $Config
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[ERROR] Install failed (exit code $LASTEXITCODE)" -ForegroundColor Red
        exit $LASTEXITCODE
    }

    Write-Host "[OK] Install succeeded" -ForegroundColor Green

    # Show install directory
    $installDirs = Get-ChildItem -Path $ProjectRoot -Directory -Filter 'bin_qt*' -ErrorAction SilentlyContinue
    if ($installDirs) {
        Write-Host "`n=== Install Directory ===" -ForegroundColor Cyan
        $installDirs | ForEach-Object { Write-Host "  $($_.Name)/" }
    }
}

Write-Host "`n=== Done ===" -ForegroundColor Green
