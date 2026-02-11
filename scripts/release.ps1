# Entropy8 – Build release package (Windows)
# Creates a distributable folder with all required files.
#
# Usage:
#   .\scripts\release.ps1          # Build Release and package
#   .\scripts\release.ps1 -Clean   # Clean build first

param(
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $Root "build"
$ReleaseDir = Join-Path $Root "release\Entropy8"
$MinGWBin = "C:\Users\aytug\Documents\C++ Derleyici\mingw64\bin"

Set-Location $Root

# ── Ensure MinGW in PATH ─────────────────────────────────────────────────────
if (Test-Path $MinGWBin) {
    if ($env:PATH -notlike "*$MinGWBin*") {
        $env:PATH = "$MinGWBin;$env:PATH"
    }
}

# ── Clean ─────────────────────────────────────────────────────────────────────
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "Cleaning..." -ForegroundColor Yellow
    Remove-Item $BuildDir -Recurse -Force
}

# ── Configure (static runtime) ───────────────────────────────────────────────
Write-Host "Configuring (Release, static runtime)..." -ForegroundColor Cyan
$linkerFlags = "-static-libgcc -static-libstdc++ -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic"
cmake -S $Root -B $BuildDir -G Ninja `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_C_COMPILER=gcc `
    -DCMAKE_CXX_COMPILER=g++ `
    "-DCMAKE_EXE_LINKER_FLAGS=$linkerFlags" `
    "-DCMAKE_SHARED_LINKER_FLAGS=$linkerFlags"
if ($LASTEXITCODE -ne 0) { Write-Host "Configure failed." -ForegroundColor Red; exit 1 }

# ── Build ─────────────────────────────────────────────────────────────────────
Write-Host "Building..." -ForegroundColor Cyan
cmake --build $BuildDir --config Release
if ($LASTEXITCODE -ne 0) { Write-Host "Build failed." -ForegroundColor Red; exit 1 }

# ── Package ───────────────────────────────────────────────────────────────────
Write-Host "Packaging..." -ForegroundColor Cyan

if (Test-Path $ReleaseDir) { Remove-Item $ReleaseDir -Recurse -Force }
New-Item -ItemType Directory -Path $ReleaseDir -Force | Out-Null

$buildBin = Join-Path $BuildDir "bin"
Copy-Item "$buildBin\entropy8_gui.exe" $ReleaseDir
Copy-Item "$buildBin\libentropy8.dll" $ReleaseDir

# MinGW runtime (only libwinpthread if still needed)
if (Test-Path "$MinGWBin\libwinpthread-1.dll") {
    Copy-Item "$MinGWBin\libwinpthread-1.dll" $ReleaseDir
}

# Strip debug symbols
strip "$ReleaseDir\entropy8_gui.exe"
strip "$ReleaseDir\libentropy8.dll"

# ── Summary ───────────────────────────────────────────────────────────────────
Write-Host ""
Write-Host "Release package:" -ForegroundColor Green
Get-ChildItem $ReleaseDir | Format-Table Name, @{N='Size';E={
    if ($_.Length -ge 1MB) { "{0:N1} MB" -f ($_.Length / 1MB) }
    else { "{0:N0} KB" -f ($_.Length / 1KB) }
}} -AutoSize

$total = (Get-ChildItem $ReleaseDir | Measure-Object -Property Length -Sum).Sum
Write-Host "Total: $([math]::Round($total / 1MB, 2)) MB" -ForegroundColor Cyan
Write-Host "Output: $ReleaseDir" -ForegroundColor Cyan
