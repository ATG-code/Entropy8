# Entropy8 – run in dev mode: build engine if needed, then run CLI.
# Usage: scripts/run.ps1 [create|list|extract] [arg ...]
# Example: .\scripts\run.ps1 create archive.e8 file1.txt
#          .\scripts\run.ps1 list archive.e8

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $ScriptDir
Set-Location $Root

$BindingsLib = Join-Path $Root "engine\bindings\python\entropy8_engine"
$CoreBuild = Join-Path $Root "engine\core\build"
$CoreSrc = Join-Path $Root "engine\core"

function Test-HasLib {
    $names = @("libentropy8.dll", "libentropy8.so", "libentropy8.so.1", "entropy8.dll")
    foreach ($n in $names) {
        $p = Join-Path $BindingsLib $n
        if (Test-Path $p -PathType Leaf) { return $true }
    }
    return $false
}

if (-not (Test-HasLib)) {
    Write-Host "Engine library not found, building..."
    if (-not (Test-Path $CoreBuild)) { New-Item -ItemType Directory -Path $CoreBuild -Force | Out-Null }
    Push-Location $CoreBuild
    try {
        cmake .. -DCMAKE_BUILD_TYPE=Release
        if ($LASTEXITCODE -ne 0) { throw "cmake failed" }
        cmake --build .
        if ($LASTEXITCODE -ne 0) { throw "cmake --build failed" }
        $dll = Join-Path $CoreBuild "libentropy8.dll"
        $so = Join-Path $CoreBuild "libentropy8.so"
        $so1 = Join-Path $CoreBuild "libentropy8.so.1"
        if (Test-Path $dll) { Copy-Item -Force $dll $BindingsLib }
        elseif (Test-Path $so) { Copy-Item -Force (Join-Path $CoreBuild "libentropy8.so*") $BindingsLib }
        elseif (Test-Path $so1) { Copy-Item -Force (Join-Path $CoreBuild "libentropy8.so*") $BindingsLib }
        else { throw "Build finished but library was not found." }
    } finally {
        Pop-Location
    }
    Write-Host "Engine built."
}

$env:PYTHONPATH = "$Root\engine\bindings\python"

$mainPy = Join-Path $Root "apps\cli\entropy8_cli\main.py"
& python $mainPy @args
