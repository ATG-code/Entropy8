# Entropy8 – run in dev mode via Docker. No local Python or C++ toolchain needed.
# Usage: scripts/run.ps1 [create|list|extract] [arg ...]
# Example: .\scripts\run.ps1 create archive.e8 file1.txt
#          .\scripts\run.ps1 list archive.e8
#          .\scripts\run.ps1 extract archive.e8 .\out

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $ScriptDir
Set-Location $Root

$ImageName = "entropy8:dev"
$DockerfilePath = Join-Path $Root "docker\Dockerfile"

# ── Check Docker ──────────────────────────────────────────────────────────────
$dockerCmd = Get-Command docker -ErrorAction SilentlyContinue
if (-not $dockerCmd) {
    Write-Host ""
    Write-Host "ERROR: Docker not found." -ForegroundColor Red
    Write-Host "Install Docker Desktop from https://www.docker.com/products/docker-desktop/"
    Write-Host ""
    exit 1
}

# ── Check Docker daemon is running ────────────────────────────────────────────
$dockerInfo = docker info 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: Docker daemon is not running." -ForegroundColor Red
    Write-Host "Start Docker Desktop, then try again."
    Write-Host ""
    exit 1
}

# ── Build image if it doesn't exist ──────────────────────────────────────────
$imageExists = docker images -q $ImageName 2>$null
if (-not $imageExists) {
    Write-Host "Building Docker image ($ImageName)..." -ForegroundColor Cyan
    docker build -f $DockerfilePath -t $ImageName $Root
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Docker build failed." -ForegroundColor Red
        exit 1
    }
    Write-Host "Image built." -ForegroundColor Green
}

# ── Prepare workspace volume ─────────────────────────────────────────────────
# Mount current directory as /workspace so files are read/written locally.
$WorkDir = (Get-Location).Path

# ── Run ──────────────────────────────────────────────────────────────────────
# Docker Desktop on Windows accepts native paths in -v (e.g. C:\Users\...).
docker run --rm -v "${WorkDir}:/workspace" -w /workspace $ImageName @args
exit $LASTEXITCODE
