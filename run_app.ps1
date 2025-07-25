# PowerShell script to run Silver Clinic application
# Compatible with pwsh (PowerShell 7+) and Windows PowerShell 5.1

param(
    [string]$Configuration = "Release",
    [switch]$Help
)

if ($Help) {
    Write-Host "Silver Clinic Application Runner" -ForegroundColor Cyan
    Write-Host "Usage: .\run_app.ps1 [-Configuration <Debug|Release>] [-Help]" -ForegroundColor Green
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -Configuration  Build configuration (Debug or Release, default: Release)"
    Write-Host "  -Help           Show this help message"
    exit 0
}

Write-Host "=== Silver Clinic - Application Runner ===" -ForegroundColor Cyan
Write-Host "Configuration: $Configuration" -ForegroundColor Yellow

# Detect operating system
$isWindowsOS = $PSVersionTable.Platform -eq "Win32NT" -or $env:OS -eq "Windows_NT"

# Check if executable exists
if ($isWindowsOS) {
    $appPath = "build\$Configuration\SilverClinic.exe"
    $debugPath = "build\Debug\SilverClinic.exe"
    $releasePath = "build\Release\SilverClinic.exe"
} else {
    $appPath = "build/SilverClinic"
    $debugPath = "build/SilverClinic"
    $releasePath = "build/SilverClinic"
}

if (-not (Test-Path $appPath)) {
    Write-Host "[ERROR] Application not found: $appPath" -ForegroundColor Red
    Write-Host "Please run .\build_windows.ps1 first to build the application" -ForegroundColor Yellow
    
    # Check if other configurations exist
    
    if ((Test-Path $debugPath) -or (Test-Path $releasePath)) {
        Write-Host ""
        Write-Host "Available configurations:" -ForegroundColor Cyan
        if (Test-Path $debugPath) { Write-Host "  Debug: $debugPath" -ForegroundColor Gray }
        if (Test-Path $releasePath) { Write-Host "  Release: $releasePath" -ForegroundColor Gray }
        
        # For Linux, just use the available executable
        if (-not $isWindowsOS -and (Test-Path $debugPath)) {
            Write-Host "[INFO] Using available executable: $debugPath" -ForegroundColor Green
            $appPath = $debugPath
        } else {
            exit 1
        }
    } else {
        exit 1
    }
}

Write-Host "[INFO] Starting Silver Clinic..." -ForegroundColor Green
Write-Host "Executable: $appPath" -ForegroundColor Gray
Write-Host ""

# Ensure we're in the correct working directory for database paths
$originalLocation = Get-Location
try {
    # Run from project root to ensure correct database paths
    & $appPath
    $exitCode = $LASTEXITCODE
    
    if ($exitCode -ne 0) {
        Write-Host ""
        Write-Host "[WARNING] Application exited with code: $exitCode" -ForegroundColor Yellow
    } else {
        Write-Host ""
        Write-Host "[INFO] Application completed successfully" -ForegroundColor Green
    }
} catch {
    Write-Host "[ERROR] Failed to run application: $_" -ForegroundColor Red
    exit 1
} finally {
    Set-Location $originalLocation
}

Write-Host ""
Write-Host "Database location: data\clinic.db" -ForegroundColor Cyan
Write-Host "Generated PDFs: data\pdfs\" -ForegroundColor Cyan
