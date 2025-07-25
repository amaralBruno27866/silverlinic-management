# SilverClinic Development Configuration
# PowerShell Profile for this project

# Set execution policy for development
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force

# Project paths
$ProjectRoot = $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build_test"
$DataDir = Join-Path $ProjectRoot "data"

# Useful aliases for development
Set-Alias -Name "dev" -Value "$ProjectRoot/dev_setup.ps1"
Set-Alias -Name "build" -Value "$ProjectRoot/build_windows.ps1"
Set-Alias -Name "test" -Value "$ProjectRoot/run_tests_windows.ps1"
Set-Alias -Name "run" -Value "$ProjectRoot/run_app.ps1"

# Functions for quick development tasks
function Quick-Build { 
    & pwsh "$ProjectRoot/dev_setup.ps1" -Task build -Configuration Debug 
}

function Quick-Test { 
    & pwsh "$ProjectRoot/dev_setup.ps1" -Task test 
}

function Quick-Run { 
    & pwsh "$ProjectRoot/dev_setup.ps1" -Task run 
}

function Quick-Clean { 
    & pwsh "$ProjectRoot/dev_setup.ps1" -Task clean 
}

function Show-ProjectInfo {
    Write-Host "=== SilverClinic Development Environment ===" -ForegroundColor Cyan
    Write-Host "Project Root: $ProjectRoot" -ForegroundColor Yellow
    Write-Host "Build Dir: $BuildDir" -ForegroundColor Yellow
    Write-Host "Data Dir: $DataDir" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Quick Commands:" -ForegroundColor Green
    Write-Host "  Quick-Build  - Build project" -ForegroundColor White
    Write-Host "  Quick-Test   - Run tests" -ForegroundColor White
    Write-Host "  Quick-Run    - Build and run" -ForegroundColor White
    Write-Host "  Quick-Clean  - Clean build dirs" -ForegroundColor White
    Write-Host ""
    Write-Host "Full Commands:" -ForegroundColor Green
    Write-Host "  pwsh ./dev_setup.ps1 -Task help" -ForegroundColor White
}

# Display info when profile loads
Show-ProjectInfo
