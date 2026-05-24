<#
.SYNOPSIS
    Build script for SARibbon project (Visual Studio generator only).

.DESCRIPTION
    Auto-detects Qt and VS versions from C:\Qt\ filesystem, configures and builds
    the project using CMake with the Visual Studio generator. Supports both
    MSVC 2019 and 2022 — VS generator is automatically inferred from the Qt
    installation path (msvc2019_64 → VS16, msvc2022_64 → VS17).

.EXAMPLE
    .\scripts\build.ps1
    .\scripts\build.ps1 -Config Debug
    .\scripts\build.ps1 -Clean
    .\scripts\build.ps1 -Config Release -EnableTests
    .\scripts\build.ps1 -MsvcVersion 2022
#>

[CmdletBinding()]
param(
    [ValidateSet('Release','Debug')]
    [string]$Config = 'Release',

    [string]$QtPath = '',

    [ValidateSet('2019','2022','')]
    [string]$MsvcVersion = '',

    [switch]$Clean,

    [switch]$EnableTests,

    [switch]$EnableExamples,

    [switch]$Parallel = $true
)

$ErrorActionPreference = 'Stop'

# ── Project root (always resolve relative to script location) ────────────────
$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot '..')

# ── Step 1: Auto-detect Qt ───────────────────────────────────────────────────
function Find-QtPath {
    param([string]$HintPath)

    # If user provided a path, validate and return it
    if ($HintPath) {
        if (-not (Test-Path $HintPath)) {
            Write-Error "Provided Qt path does not exist: $HintPath"
            return $null
        }
        $cmakePath = $HintPath -replace '\\', '/'
        Write-Host "[Qt]     Using provided path: $cmakePath"
        return $cmakePath
    }

    # Scan C:\Qt\ for msvc*_64 directories, prefer highest version
    if (-not (Test-Path 'C:\Qt')) {
        Write-Error "Qt not found at C:\Qt. Install Qt 6.7+ with MSVC binaries."
        return $null
    }

    # Look for msvc2019_64 and msvc2022_64 across all Qt versions
    $candidates = @()
    foreach ($verDir in (Get-ChildItem 'C:\Qt' -Directory -ErrorAction SilentlyContinue)) {
        foreach ($msvcDir in (Get-ChildItem $verDir.FullName -Directory -ErrorAction SilentlyContinue |
                              Where-Object { $_.Name -match '^msvc(2019|2022)_64$' })) {
            $candidates += @{
                Path        = $msvcDir.FullName
                QtVersion   = $verDir.Name
                MsvcVersion = $msvcDir.Name -replace '^msvc(\d+)_(\d+)$', '$1'
                Arch        = $msvcDir.Name -replace '^msvc\d+_(\d+)$', '$1'
            }
        }
    }

    if ($candidates.Count -eq 0) {
        Write-Error "No Qt MSVC installation found under C:\Qt. Expected pattern: C:\Qt\<version>\msvc<year>_64"
        return $null
    }

    # Sort: prefer msvc2022 over msvc2019, then highest Qt version
    $sorted = $candidates | Sort-Object -Property MsvcVersion, QtVersion -Descending
    $best = $sorted[0]

    # Use forward slashes for CMake compatibility
    $cmakePath = $best.Path -replace '\\', '/'
    Write-Host "[Qt]     $($best.QtVersion) / $($best.MsvcVersion)_$($best.Arch) => $cmakePath"
    return $cmakePath
}

# ── Step 2: Determine VS generator from Qt MSVC version ──────────────────────
function Get-VsGenerator {
    param([string]$MsvcVersion)

    switch ($MsvcVersion) {
        '2022' { return 'Visual Studio 17 2022' }
        '2019' { return 'Visual Studio 16 2019' }
        default {
            Write-Error "Unknown MSVC version: $MsvcVersion. Only 2019 and 2022 are supported."
            return $null
        }
    }
}

# ── Detect ────────────────────────────────────────────────────────────────────
$DetectedQtPath = Find-QtPath -HintPath $QtPath
if (-not $DetectedQtPath) { exit 1 }

# Extract MSVC version: prefer user override, then infer from Qt path
if ($MsvcVersion) {
    # User explicitly specified MSVC version
} else {
    # Infer from detected Qt path (e.g. "msvc2019_64" => "2019")
    $msvcMatch = [regex]::Match($DetectedQtPath, 'msvc(\d+)_64')
    if (-not $msvcMatch.Success) {
        Write-Error "Could not parse MSVC version from Qt path: $DetectedQtPath. Use -MsvcVersion to specify manually."
        exit 1
    }
    $MsvcVersion = $msvcMatch.Groups[1].Value
}

$VsGenerator = Get-VsGenerator -MsvcVersion $MsvcVersion
if (-not $VsGenerator) { exit 1 }

Write-Host "[VS]     $VsGenerator"

# ── Step 3: Clean if requested ────────────────────────────────────────────────
$BuildDir = Join-Path $ProjectRoot 'build'

if ($Clean) {
    if (Test-Path $BuildDir) {
        Write-Host "[Clean]  Removing build directory..."
        Remove-Item -Recurse -Force $BuildDir
    } else {
        Write-Host "[Clean]  No build directory to remove."
    }
}

# ── Step 4: Configure (run if build dir missing or after clean) ──────────────
if (-not (Test-Path $BuildDir)) {
    $cmakeArgs = @(
        '-S', $ProjectRoot,
        '-B', $BuildDir,
        '-G', $VsGenerator,
        '-A', 'x64',
        "-DCMAKE_PREFIX_PATH=$DetectedQtPath"
    )

    # Add optional build flags
    if ($EnableTests) {
        $cmakeArgs += '-DBUILD_TESTS=ON'
        Write-Host "[Config] Tests enabled"
    } else {
        $cmakeArgs += '-DBUILD_TESTS=OFF'
    }

    if ($EnableExamples) {
        $cmakeArgs += '-DSARIBBON_BUILD_EXAMPLES=ON'
        Write-Host "[Config] Examples enabled"
    } else {
        $cmakeArgs += '-DSARIBBON_BUILD_EXAMPLES=OFF'
    }

    Write-Host "[Config] cmake $($cmakeArgs -join ' ')"

    & cmake @cmakeArgs

    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configure failed (exit code $LASTEXITCODE)."
        exit $LASTEXITCODE
    }
    Write-Host "[Config] Done."
} else {
    Write-Host "[Config] Build directory exists, skipping configure."
}

# ── Step 5: Build ────────────────────────────────────────────────────────────
$buildArgs = @('--build', $BuildDir, '--config', $Config)

if ($Parallel) {
    $buildArgs += '--parallel'
}

Write-Host "[Build]  Config: $Config"

& cmake @buildArgs
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed (exit code $LASTEXITCODE)."
    exit $LASTEXITCODE
}
Write-Host "[Build]  Done."

# ── Step 6: Run tests if enabled ─────────────────────────────────────────────
if ($EnableTests) {
    Write-Host "[Test]   Running ctest..."
    Push-Location $BuildDir
    try {
        & ctest --output-on-failure -C $Config

        if ($LASTEXITCODE -ne 0) {
            Write-Error "Tests FAILED (exit code $LASTEXITCODE)."
            exit $LASTEXITCODE
        }

        Write-Host "[Test]   All tests PASSED."
    }
    finally {
        Pop-Location
    }
}

# ── Done ──────────────────────────────────────────────────────────────────────
Write-Host "[Done]   Build completed successfully."
exit 0