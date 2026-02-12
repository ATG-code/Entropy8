# Entropy8 GUI – Native Build Script (Windows / MinGW)
# Requires: CMake, MinGW g++, Qt6 (Quick, QuickControls2)
#
# Usage:
#   .\scripts\build-gui.ps1              # Build Release
#   .\scripts\build-gui.ps1 -Debug       # Build Debug
#   .\scripts\build-gui.ps1 -Run         # Build + Run
#   .\scripts\build-gui.ps1 -Clean       # Clean + Rebuild
#   .\scripts\build-gui.ps1 -Clean -Run  # Clean + Build + Run
#
# If Qt6 is not found automatically, set the QT_DIR environment variable:
#   $env:QT_DIR = "C:\Qt\6.x.x\mingw_64"
#   .\scripts\build-gui.ps1

param(
    [switch]$Debug,
    [switch]$Run,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $Root "build"
$BuildType = if ($Debug) { "Debug" } else { "Release" }

Set-Location $Root

# ── Ensure MinGW is in PATH ──────────────────────────────────────────────────
$MinGWBin = "C:\Users\aytug\Documents\C++ Derleyici\mingw64\bin"
if (Test-Path $MinGWBin) {
    if ($env:PATH -notlike "*$MinGWBin*") {
        $env:PATH = "$MinGWBin;$env:PATH"
    }
} else {
    # Try to find MinGW in PATH
    $gcc = Get-Command g++ -ErrorAction SilentlyContinue
    if (-not $gcc) {
        Write-Host "ERROR: MinGW g++ not found. Set MinGWBin path or add to PATH." -ForegroundColor Red
        exit 1
    }
}

# ── Check for CMake ──────────────────────────────────────────────────────────
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Host "ERROR: CMake not found." -ForegroundColor Red
    exit 1
}

# ── Clean ────────────────────────────────────────────────────────────────────
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item $BuildDir -Recurse -Force
}

# ── Configure ────────────────────────────────────────────────────────────────
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Build cmake args
$cmakeArgs = @(
    "-S", $Root,
    "-B", $BuildDir,
    "-G", "Ninja",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_C_COMPILER=gcc",
    "-DCMAKE_CXX_COMPILER=g++"
)

# Pass Qt prefix path if QT_DIR is set
if ($env:QT_DIR) {
    Write-Host "Using Qt from: $env:QT_DIR" -ForegroundColor Cyan
    $cmakeArgs += "-DCMAKE_PREFIX_PATH=$env:QT_DIR"
}

Write-Host "Configuring ($BuildType) with Ninja + MinGW..." -ForegroundColor Cyan
cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Configure FAILED." -ForegroundColor Red
    exit 1
}

# ── Build ────────────────────────────────────────────────────────────────────
Write-Host "Building..." -ForegroundColor Cyan
cmake --build $BuildDir --config $BuildType
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build FAILED." -ForegroundColor Red
    exit 1
}

Write-Host "Build successful!" -ForegroundColor Green

# ── Run ──────────────────────────────────────────────────────────────────────
if ($Run) {
    $exe = Join-Path $BuildDir "bin\entropy8_gui.exe"
    if (Test-Path $exe) {
        Write-Host "Running $exe..." -ForegroundColor Cyan
        & $exe
    } else {
        Write-Host "entropy8_gui.exe not found at $exe" -ForegroundColor Red
    }
}
