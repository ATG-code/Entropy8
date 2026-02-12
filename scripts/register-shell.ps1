<#
.SYNOPSIS
    Registers Entropy8 in the Windows right-click context menu for ALL files and folders.
    Run this script as Administrator.

.DESCRIPTION
    Adds "Compress with Entropy8" to the right-click context menu for all files
    and folders. Archive files are opened for viewing/extraction, other files
    are compressed. Also adds "Compress with Entropy8" for folders.

.PARAMETER ExePath
    Full path to entropy8_gui.exe. If not specified, auto-detects from the
    build output directory relative to this script.

.PARAMETER Uninstall
    Remove all Entropy8 shell integration entries from the registry.

.EXAMPLE
    # Install (run as Administrator)
    .\register-shell.ps1

    # Install with custom exe path
    .\register-shell.ps1 -ExePath "C:\Program Files\Entropy8\entropy8_gui.exe"

    # Uninstall
    .\register-shell.ps1 -Uninstall
#>
param(
    [string]$ExePath,
    [switch]$Uninstall
)

$ErrorActionPreference = "Stop"

# ── Determine exe path ─────────────────────────────────────────────────────
if (-not $ExePath) {
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    $projectDir = Split-Path -Parent $scriptDir
    $ExePath = Join-Path $projectDir "build\bin\entropy8_gui.exe"
}

if (-not $Uninstall) {
    if (-not (Test-Path $ExePath)) {
        Write-Error "entropy8_gui.exe not found at: $ExePath`nBuild the project first or specify -ExePath."
        exit 1
    }
    $ExePath = (Resolve-Path $ExePath).Path
    Write-Host "Using: $ExePath" -ForegroundColor Cyan
}

# ── Check admin privileges ──────────────────────────────────────────────────
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(
    [Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Warning "This script requires Administrator privileges."
    Write-Host "Restarting as Administrator..." -ForegroundColor Yellow
    Start-Process powershell.exe -Verb RunAs -ArgumentList (
        "-ExecutionPolicy Bypass -File `"$($MyInvocation.MyCommand.Path)`" " +
        $(if ($Uninstall) { "-Uninstall " } else { "" }) +
        "-ExePath `"$ExePath`""
    )
    exit 0
}

# ── Registry paths ──────────────────────────────────────────────────────────
# All files: *\shell\Entropy8
# Folders:   Directory\shell\Entropy8
# Background (right-click in empty area of folder): Directory\Background\shell\Entropy8
$allFilesKey     = "Registry::HKEY_CLASSES_ROOT\*\shell\Entropy8"
$directoryKey    = "Registry::HKEY_CLASSES_ROOT\Directory\shell\Entropy8"
$appKey          = "Registry::HKEY_CLASSES_ROOT\Applications\entropy8_gui.exe"
$appPathKey      = "Registry::HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\entropy8_gui.exe"

# ═══════════════════════════════════════════════════════════════════════════════
# UNINSTALL
# ═══════════════════════════════════════════════════════════════════════════════
if ($Uninstall) {
    Write-Host "`n  Removing Entropy8 shell integration...`n" -ForegroundColor Yellow

    $keysToRemove = @($allFilesKey, $directoryKey, $appKey, $appPathKey)

    foreach ($key in $keysToRemove) {
        if (Test-Path $key) {
            Remove-Item -Path $key -Recurse -Force
            $name = $key -replace "Registry::HKEY_CLASSES_ROOT\\", "" -replace "Registry::HKEY_LOCAL_MACHINE\\", ""
            Write-Host "  Removed: $name" -ForegroundColor Gray
        }
    }

    Write-Host "`n  Entropy8 shell integration removed.`n" -ForegroundColor Green
    Read-Host "Press Enter to close"
    exit 0
}

# ═══════════════════════════════════════════════════════════════════════════════
# INSTALL
# ═══════════════════════════════════════════════════════════════════════════════
Write-Host "`n  Registering Entropy8 shell integration...`n" -ForegroundColor Cyan

# ── 1. Context menu for ALL files ──────────────────────────────────────────
#    Archives will be opened for viewing, other files will be compressed.
New-Item -Path $allFilesKey -Force | Out-Null
Set-ItemProperty -Path $allFilesKey -Name "(Default)" -Value "Open / Compress with Entropy8"
Set-ItemProperty -Path $allFilesKey -Name "Icon" -Value "`"$ExePath`",0"

New-Item -Path "$allFilesKey\command" -Force | Out-Null
Set-ItemProperty -Path "$allFilesKey\command" -Name "(Default)" -Value "`"$ExePath`" `"%1`""

Write-Host "  Registered: All files (*)" -ForegroundColor Gray

# ── 2. Context menu for folders ────────────────────────────────────────────
#    Compresses the entire folder.
New-Item -Path $directoryKey -Force | Out-Null
Set-ItemProperty -Path $directoryKey -Name "(Default)" -Value "Compress with Entropy8"
Set-ItemProperty -Path $directoryKey -Name "Icon" -Value "`"$ExePath`",0"

New-Item -Path "$directoryKey\command" -Force | Out-Null
Set-ItemProperty -Path "$directoryKey\command" -Name "(Default)" -Value "`"$ExePath`" `"%1`""

Write-Host "  Registered: Directories" -ForegroundColor Gray

# ── 3. Register application in "Open with" ─────────────────────────────────
New-Item -Path $appKey -Force | Out-Null
Set-ItemProperty -Path $appKey -Name "FriendlyAppName" -Value "Entropy8"

$appShell = "$appKey\shell\open\command"
New-Item -Path $appShell -Force | Out-Null
Set-ItemProperty -Path $appShell -Name "(Default)" -Value "`"$ExePath`" `"%1`""

Write-Host "  Registered: Open with dialog" -ForegroundColor Gray

# ── 4. Register App Paths ──────────────────────────────────────────────────
New-Item -Path $appPathKey -Force | Out-Null
Set-ItemProperty -Path $appPathKey -Name "(Default)" -Value $ExePath
Set-ItemProperty -Path $appPathKey -Name "Path" -Value (Split-Path $ExePath)

Write-Host "  Registered: App Paths" -ForegroundColor Gray

# ── Done ────────────────────────────────────────────────────────────────────
Write-Host "`n  Entropy8 shell integration installed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "  Right-click any file    -> 'Open / Compress with Entropy8'" -ForegroundColor White
Write-Host "  Right-click any folder  -> 'Compress with Entropy8'" -ForegroundColor White
Write-Host ""

Read-Host "Press Enter to close"
